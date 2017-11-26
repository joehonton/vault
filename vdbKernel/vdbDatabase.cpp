//=============================================================================
//
// CLASS:           vdbDatabase
//
// OVERVIEW:		Class implementation for vdbDatabase
//
// FILE NAME:       vdbDatabase.cpp
//
// INITIAL DATE:    01/14/96 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifdef USE_PRE_HDR
	#include "vdbHeader.h"
#else
	#include <strstream>
	#include <vdbAssert.h>
	#include "vdbException.h"
#endif

//{{AllTableHeaders
#include "vdbDatabase.h"
#include "vdbUniqueTable.h"
#include "vdbVersionTable.h"
//}}

#include "vdbServer.h"
#include "vdbMicrosoftSQLServer.h"
#include "vdbOmniscienceORDBMS.h"
#include "vdbWatcomSQLServer.h"
#include "vdbStatement.h"
#include "vdbQuery.h"
#include "vdbSetOfVersions.h"
#include "vdbVersion.h"
#include "vdbUnique.h"
#include "vdbUtility.h"
#include "vdbSafeString.h"
#include "vdbPtr.h"
#include "vdbTroubleshootLoginDialog.h"
#include "vdbProfileString.h"
#include "vdbRegistrationKey.h"
#include <ODBCinst.h>

//warning C4996: 'strcpy': This function or variable may be unsafe. Consider using strcpy_s instead.
#pragma warning( disable : 4996 )


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------------------------
//^^ The vdbDatabase constructor
//
vdbDatabase::vdbDatabase()
	: _connected( false ),
	  _hasSystemTables( false ),
	  _pServer( 0 ),
	  _pUnique( 0 ),
	  _quenchErrors( false ),
	  _transactionLevel( 0 ),
	  _bSaveAutoCommit( true )
{
	//  Allocate the environment
	_henv = SQL_NULL_HENV;
	_rc = ::SQLAllocEnv( &_henv );
	dbException ( SQL_API_SQLALLOCENV );

	//  Allocate the connection
	_hdbc = SQL_NULL_HDBC;
	_rc = ::SQLAllocConnect( _henv, &_hdbc );
	dbException ( SQL_API_SQLALLOCCONNECT );

	// Initialize critical section for vdbStatements
	InitializeCriticalSection( &_statementGuard );

	_pvdbUniqueTable  = new vdbUniqueTable( this );
	if ( _pvdbUniqueTable == 0 ) throw vdbMemoryException();
	_pvdbVersionTable = new vdbVersionTable( this );
	if ( _pvdbVersionTable == 0 ) throw vdbMemoryException();
}


//-------------------------------------------------------
//^^ The vdbDatabase destructor.
//
vdbDatabase::~vdbDatabase()
{
	// Disconnect from data source
	Disconnect();

	// _pUnique should be deleted by Disconnect
	assert( _pUnique == 0 );

	// System tables
	delete _pvdbUniqueTable; _pvdbUniqueTable = 0;
	delete _pvdbVersionTable; _pvdbVersionTable = 0;

	// Delete critical section for vdbStatements
	DeleteCriticalSection( &_statementGuard );
	
	// Free the connection
	if (_hdbc != SQL_NULL_HDBC)
	{
		_rc = ::SQLFreeConnect(_hdbc);
		dbException ( SQL_API_SQLFREECONNECT );
 	}
	
	// Free the environment
	if (_henv != SQL_NULL_HENV)
	{
		_rc = ::SQLFreeEnv(_henv);
		dbException ( SQL_API_SQLFREEENV );
 	}
}


//--------------------------------------------
//^^ vdbDatabase copy constructor
vdbDatabase::vdbDatabase( const vdbDatabase& rhs )
{
	assert( false );  // copying vdbDatabase objects is not allowed
}


//--------------------------------------------
//^^ vdbDatabase assignment operator
vdbDatabase& vdbDatabase::operator=( const vdbDatabase& rhs )
{
	assert( false );  // assignment of vdbDatabase objects is not allowed

	return *this;
}


//=============================================================================
// Table maintenance functions
//=============================================================================

//-------------------------------------------------
//^ The AddTable function adds a table to the internal
//  list of tables maintained by the class.  Call this
//  function from your derived class constructor once
//  for each table in your schema.
//> pTable is a pointer to a vdbTable instance.  The system
//  adopts this pointer and manages its deletion upon destruction
//  of the vdbDatabase instance.
//
void vdbDatabase::AddTable( vdbTable* pTable )
{
	_tables.Add( pTable );
}


//-------------------------------------------------
//^ The CreateSystemTables function may be called from
//  your application.  This function should be called
//  after your database file has been created by your DBMS
//  utilities, and before you create any user tables.
//^ vdbVersion and vdbUnique are the "system" tables.
//  Call this function after Connecting to the database.
//
void vdbDatabase::CreateSystemTables()
{
	_pvdbUniqueTable->CreateTable();
	_pvdbUniqueTable->CreateIndexes();
	
	// create the special unique instance.  This first instance will always
	// remain with the database, and will always have a _nextOID value which
	// is the next available starting point for the next group of unique OID's.
	vdbUnique* pUnique = new vdbUnique( 1, this );	// oid of 1 causes Initialize to return before attempting retrieval
	if ( pUnique == 0 ) throw vdbMemoryException();
	pUnique->SetMasterOID();						// SetMasterOID sets oid to 2
	pUnique->SetNextOID( 3 );
	pUnique->SetLastOID( 0 );
	pUnique->SetCacheIndicatorActive();
	delete pUnique; pUnique = 0;

	_pvdbVersionTable->CreateTable();
	_pvdbVersionTable->CreateIndexes();
	_hasSystemTables = true;
	
	_pvdbUniqueTable->AddToVersionTable();
	_pvdbVersionTable->AddToVersionTable();
}


//-------------------------------------------------
//^ The DropSystemTables function should only be called
//  if you are starting everything again from scratch.
//  Otherwise keep them around and just drop the individual
//  user tables.  This function will drop vdbVersion and vdbUnique.
//
void vdbDatabase::DropSystemTables()
{
	if ( HasSystemTables() )
	{
		delete _pUnique; _pUnique = 0;
		_pvdbUniqueTable->DropTable();
		_pvdbVersionTable->DropTable();
		_hasSystemTables = false;
	}

	// clear the version cache
	_versionCache.ClearAll();
}


//-------------------------------------------------
//^ The CreateUserTables function creates all user tables
//  added to the system's internal list of tables via the
//  AddTable function.
//
void vdbDatabase::CreateUserTables()
{
	for ( int i = 0; i < _tables.Count(); i++ )
		CreateUserTable( _tables[i] );
}


//-------------------------------------------------
//^ The CreateUserTable function creates one table and
//  its indexes and inserts a corresponding record in the
//  vdbVersion table.
//^ This action always causes a commit before and after the
//  CREATE TABLE statement regardless of the Autocommit state.
//> pTable is a reference to the table to create.
//
void vdbDatabase::CreateUserTable( vdbTable& pTable )
{
	if ( pTable.CreateTable() != SQL_SUCCESS )
	{
		vdbString s;
		s.Format( "Unable to create the table %s", (const char*) pTable.TableNameCS() );
		vdbMessageBox( NULL, s, "Create Database", MB_OK );
		return;
	}
		
	// Every table must have at least one index.
	// The first index is assumed to be the Object ID ( _oid )
	assert( pTable.IndexCount() > 0 );
	pTable.CreateIndexes();
	
	// Add this table to the vdbVersion table
	pTable.AddToVersionTable();
}


//-------------------------------------------------
//^ The DropUserTables function drops all user tables
//  from the DBMS.  This function will also remove
//  all version info from the vdbVersion table.
//
void vdbDatabase::DropUserTables()
{
	for ( int i = 0; i < _tables.Count(); i++ )
		DropUserTable( _tables[i] );
}


//-------------------------------------------------
//^ The DropUserTable function drops one user table,
//  its indexes and its entry in the vdbVersion table.
//^ This action always causes a commit before and after the
//  DROP statement regardless of the Autocommit state.
//> pTable is a reference to the table to drop.
//
void vdbDatabase::DropUserTable( vdbTable& table )
{
	if ( TableExists( table.TableName() ) )
	{
		table.DropTable();
		table.RemoveFromVersionTable();
	}
}


//=============================================================================
//  Overrideable functions
//=============================================================================

//-------------------------------------------------
RETCODE vdbDatabase::dbException ( int functionNumber )
{
	// throw an exception or return rc
	if ( _rc )
		Error( functionNumber );

	if ( _rc == SQL_SUCCESS_WITH_INFO )
		return _rc = SQL_SUCCESS;
	else
		return _rc;
}


#ifdef BORLAND
	#define MB_SETFOREGROUND	0
#endif

//-------------------------------------------------
//^ The Error function displays an error message box when an
//  ODBC error occurs during processing.  The default handling
//  of this error is to display a message box with details of the
//  error and to ask the user to continue or cancel the application.
//  Override this function to provide your own custom error processing.
//> functionNumber is the number of the function which caused this
//  error.  These numbers correspond to the #defines for SQLGetFunctions
//  as listed in SQLEXT.H.
// 
void vdbDatabase::Error( int functionNumber )
{
	// If we were told to ignore all errors, just return
	if ( QuenchErrors() )
		return;

	char szSqlState[50];
	char szErrorMsg[SQL_MAX_MESSAGE_LENGTH];
	SDWORD pfNativeError;
	SWORD pcbErrorMsg;
	Error( szSqlState, &pfNativeError, szErrorMsg, SQL_MAX_MESSAGE_LENGTH - 1, &pcbErrorMsg);

	// Mask "Changed database context to 'master'" message from Microsoft SQL Server
	if ( pfNativeError == 5701 )
	{
		_rc = SQL_SUCCESS;
		return;
	}

	std::ostrstream os;
	os << "A problem occured while accessing the database.\n\n";
//	os << "Sql State: " << szSqlState << "\n\nfunction number: " << functionNumber << "\n\nNative Error:  " << pfNativeError << "\n\nError Message: " << szErrorMsg;
	os << "Error Message: " << szErrorMsg;
	os << "\n\nPress OK to continue anyway or CANCEL to quit the application." << std::ends;
    vdbSafeString msg( os.str() );
	if ( vdbMessageBox( NULL, msg, "Database Error", MB_SETFOREGROUND | MB_OKCANCEL ) == IDCANCEL )
	{
		QuenchErrors( true );
		throw vdbException( "Application cancelled" );
	}
}


//=============================================================================
// Public error recovery functions
//=============================================================================

//-------------------------------------------------
//^ The Trouble function is called when something is
//  not quite right and the programmer should be alerted.
//  This function displays a message box with the alert
//  condition allowing the user to continue or cancel the application.
//> desc is a description of the problem.
//> table is the name of the table which is associated with
//  the problem.
//< If the user requests to cancel the application a vdbException
//  is thrown.  If the user requests to continue the application
//  the return code is DBS_TROUBLE.
//
RETCODE vdbDatabase::Trouble( const char* desc, const char* table )
{
	std::ostrstream os;
	os << desc << "\n\nTable: " << table << std::ends;
    char* msg = os.str();
	RETCODE rc = Trouble( msg );
	delete[] msg;
	return rc;
}


//-------------------------------------------------
//^ The Trouble function is called when something is
//  not quite right and the programmer should be alerted.
//  This function displays a message box with the alert
//  condition allowing the user to continue or cancel the application.
//> desc is a description of the problem.
//> OID is the object ID if the vdbObject-derived instance
//  which is associated with the problem.
//< If the user requests to cancel the application a vdbException
//  is thrown.  If the user requests to continue the application
//  the return code is DBS_TROUBLE.
//
RETCODE vdbDatabase::Trouble( const char* desc, OID oid )
{
	std::ostrstream os;
	os << desc << oid << std::ends;
    char* msg = os.str();
	RETCODE rc = Trouble( msg );
	delete[] msg;
	return rc;
}

//--------------------------------------------------
//^ The QuitApplication function is called is called
//  only in an emergency to shutdown after trouble.
//< rc is the return code passed back to the operating
//  system through the exit function.
//
void vdbDatabase::QuitApplication( int rc )
{
	Disconnect();
	delete this;
	exit( rc );
}



//--------------------------------------------------
//^ The QuenchErrors function changes the state of
//  the quench errors flag.  If true no messages will
//  be displayed to the user.  If false, normal message
//  processing occurs.
//  tf is the value true or false.
void vdbDatabase::QuenchErrors( bool tf )
{
	_quenchErrors = tf;
}


//--------------------------------------------------
//^ The QuenchErrors function obtains the state of
//  the quench errors flag.
//< returns true or false
//
bool vdbDatabase::QuenchErrors()
{
	return _quenchErrors;
}


//=============================================================================
// Public overridables
//=============================================================================

//------------------------------------------
//^ The GetNextOID function assigns a unique
//  object identifier to new objects.
//< OID is the next available OID to be used by the
//  requesting object.
//
OID vdbDatabase::GetNextOID()
{
	// just to make sure ...
	if ( _pUnique == 0 )
		GetCacheOfUniqueIDs();

	// if all unique ID's have been used up from the current cache, get another cache
	else if ( _pUnique->GetNextOID() > _pUnique->GetLastOID() )
		GetCacheOfUniqueIDs();

	OID next = _pUnique->GetNextOID();
	_pUnique->SetNextOID( next + 1 );		
	return next; 
}


//-------------------------------------------------
//^ The Trouble function is called when something is
//  not quite right and the programmer should be alerted.
//  This function displays a message box with the alert
//  condition allowing the user to continue or cancel the application.
//> desc is a description of the problem.
//< If the user requests to cancel the application a vdbException
//  is thrown.  If the user requests to continue the application
//  the return code is DBS_TROUBLE.
//
RETCODE vdbDatabase::Trouble( const char* desc )
{
	// If we were told to ignore all errors, just return
	if ( QuenchErrors() )
		return DBS_TROUBLE;

	std::ostrstream os;
	os << "A problem occured while accessing the database\n\n";
	os << "Trouble: " << desc;
	os << "\n\nPress OK to continue anyway or CANCEL to quit the application." << std::ends;
    vdbSafeString msg( os.str() );
	
	if ( vdbMessageBox( NULL, msg, "Trouble", MB_SETFOREGROUND | MB_OKCANCEL ) == IDCANCEL )
	{
		QuenchErrors( true );
		throw vdbException( "Application cancelled" );
	}
	return DBS_TROUBLE;
}


//=============================================================================
//  Value added public functions
//=============================================================================

//-------------------------------------------------
//^ The BeginTransaction function starts a new transaction.  Transactions are 
//  completed by calling either Commit or Rollback.  AutoCommitMode is turned
//  off by this function.  AutoCommit is turned back on by any Rollback and by
//  the final outer nested Commit.
//^ BeginTransaction can safely be omitted when no nesting of transactions is
//  required.  Calls to Commit and Rollback are successful even in the absence of
//  BeginTransaction.
//
void vdbDatabase::BeginTransaction()
{
	// Save autocommit so that rollback and commit can restore it.
	// (Check autocommit first.  Just blindly changing autocommit to false may 
	// have the side effect of commiting any outstanding transactions thus
	// defeating the nesting effort.)
	if ( _transactionLevel == 0 )
	{
		_bSaveAutoCommit = GetAutoCommit();
		if ( _bSaveAutoCommit == true )
			SetAutoCommit( false );
	}
	
	_transactionLevel++;
}


//-------------------------------------------------
//^ The Commit function causes all outstanding inserts, updates and deletes to
//  be committed to the DBMS.  If this is an inner commit of a nested set of
//  transactions, it merely decrements the transaction counter.
//< Returns DBS_OK if successful or the return code from Transact on failure.
//< Commit succeeds even when there is no outstanding transaction.
//
RETCODE vdbDatabase::Commit()
{
	_transactionLevel--;

	if ( _transactionLevel <= 0 )
	{
		_transactionLevel = 0;
		RETCODE rc = Transact( SQL_COMMIT );

		if ( _bSaveAutoCommit == true )
			SetAutoCommit( true );
		return rc;
	}
	else
		return DBS_OK;
}


//-------------------------------------------------
//^ The Rollback function causes all outstanding inserts, updates and deletes
//  to be rolled back from the DBMS.  If this is a nested transaction, all
//  outer level transactions are also rolled back.  The first Rollback therfore
//  cancels all outstanding transactions and subsequent calls to Commit and
//  Rollback have no effect.
//< Returns DBS_OK if successful or the return code from Transact on failure.
//
RETCODE vdbDatabase::Rollback()
{
	_transactionLevel = 0;
	RETCODE rc = Transact( SQL_ROLLBACK );

	if ( _bSaveAutoCommit == true )
		SetAutoCommit( true );
	return rc;
}


//-------------------------------------------------
//^ The SetAutoCommit function turns on and off the automatic commit of DBMS
//  changes on a record-by-record basis.
//> commit is true to turn autocommit on.  Records will be automatically
//  committed immediately.
//> commit is false to turn autocommit off.  Records will only be commited
//  when a Commit or Rollback is encountered.
//< Returns DBS_OK on success or the return code from SetConnectOption
//  on failure.
//
RETCODE vdbDatabase::SetAutoCommit( bool commit )
{
	RETCODE rc;
	EnterCriticalSection( &_statementGuard );
	
	rc = SetConnectOption( SQL_AUTOCOMMIT, ( commit ? SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF ) );
	
	LeaveCriticalSection( &_statementGuard );
	return rc;
}


//-------------------------------------------------
//^ The GetAutoCommit function queries the DBMS to find the current
//  autocommit state.
//< Returns true if autocommit is on.  Records will be
//  automatically committed immediately.
//< Returns false if autocommit is off.  Records will only
//  be commited when a Commit or Rollback is encountered.
//
bool vdbDatabase::GetAutoCommit()
{
	int commit;
	GetConnectOption( SQL_AUTOCOMMIT, &commit );
	return ( commit == SQL_AUTOCOMMIT_ON ? true : false ); 
}


//-------------------------------------------------
//^ The IsTransactionActive function determines wether any transaction is
//  outstanding.
//< Returns true if a transaction is outstanding.
//< Returns false if no transaction is outstanding.
//
bool vdbDatabase::IsTransactionActive()
{
	return ( _transactionLevel > 0 ? true : false );
}


//-------------------------------------
//^ The TableExists function checks the DBMS to see whether a table with the
//  given name exists.  This function will first convert the tablename to
//  uppercase according to the Server CapitalizeNames rule.
//^ NOTE:  This function obtains and sends the table owner in a separte
//  parameter.  Do not use the fully qualified table name when calling this function.
//> szTableName is the name of the table to check (without the owner).
//< returns true if the table exists in this datasource
//< returns false if the table does not exist in this datasource.
//
bool vdbDatabase::TableExists( const char* szTableName )
{
	return TableExists( Owner(), szTableName );
}


//-------------------------------------
//^ The TableExists function checks the DBMS to see whether a
//  table with the given name exists.
//> szOwner is the table qualifier
//> szTableName is the name of the table to check
//< returns true if the table exists in this datasource
//< returns false if the table does not exist in this datasource.
//
bool vdbDatabase::TableExists( const char* szOwner, const char* szTableName )
{
	int len = strlen(szTableName) + 1;
	char* szTemp = new char[len];
	if ( szTemp == 0 ) throw vdbMemoryException();
	vdbUtility::SafeCopy( szTemp, szTableName, len );
	
	vdbStatement stmt( this );
	RETCODE rc = stmt.Tables( NULL, 0, szOwner, SQL_NTS, Server()->CapitalizeNames( szTemp ), SQL_NTS, "TABLE", SQL_NTS );
	if ( rc != SQL_SUCCESS )
		return false;
	delete[] szTemp; szTemp = 0;
		
	// If the fetch was successful, then there is such a table
	rc = stmt.Fetch();
	if ( rc != SQL_SUCCESS )
		return false;
	else
		return true;
}


//-------------------------------------------------
//^ The ServerNameFromConnection function gets the name of the DBMS from the 
//  currently established connection.
//  This function must be called only after establishing a connection.
//  The server name obtained from this function is suitable for use in
//  determining which derived vdbServer class to instantiate.
//> szServerName is the name returned by the function.
//> size is the size of the server string.
//< The return code is DBS_OK on success or the return code from GetInfo on
//  failure.
//
RETCODE vdbDatabase::ServerNameFromConnection( char* szServerName, size_t size )
{
	SWORD pcbInfoValue = size; 
	return GetInfo( SQL_DBMS_NAME, szServerName, size, &pcbInfoValue );
}


//-------------------------------------------------
//^ The ServerNameFromDatasource function gets the name of the DBMS from the 
//  ODBC data source entry in the registry.
//  This function may be called before establishing a connection.
//  The server name obtained from this function is suitable for use in
//  determining which derived vdbServer class to instantiate.
//> szDataSource is the name of the ODBC data source
//> szServerName is the name returned by the function.
//> size is the size of the server string.
//< The return code is DBS_OK on success
//
RETCODE vdbDatabase::ServerNameFromDatasource( const char* szDataSource, char* szServerName, size_t size )
{
	// if we got here with a blank data source, just assume SQL Server
	if ( szDataSource[0] == 0x0 )
	{
		strcpy( szServerName, "SQL Server" );
		return DBS_OK;
	}
	
	vdbRegistrationKey key1( HKEY_CURRENT_USER, "SOFTWARE\\ODBC\\ODBC.INI\\ODBC Data Sources", szDataSource );
	if ( key1.GetValue( szServerName, size ) == false )
	{
		vdbRegistrationKey key2( HKEY_LOCAL_MACHINE, "SOFTWARE\\ODBC\\ODBC.INI\\ODBC Data Sources", szDataSource );
		if ( key2.GetValue( szServerName, size ) == false )
		{
			// if we got here then the datasource has been deleted, just return SQL Server
			strcpy( szServerName, "SQL Server" );
			return DBS_OK;
		}
	}
	return DBS_OK;
}


//-------------------------------------------------
//^ The IsValidDataSource function determines whether or not the ODBC data source
//  name is in the system registry.  Call this function when attempting to
//  reconnect to the MRU datasource upon application startup.
bool vdbDatabase::IsValidDataSource( const char* szDataSource ) const
{
	vdbRegistrationKey key1( HKEY_CURRENT_USER, "SOFTWARE\\ODBC\\ODBC.INI\\ODBC Data Sources", szDataSource );
	if ( key1.LeafExists() == false )
	{
		vdbRegistrationKey key2( HKEY_LOCAL_MACHINE, "SOFTWARE\\ODBC\\ODBC.INI\\ODBC Data Sources", szDataSource );
		if ( key2.LeafExists() == false )
			return false;
	}
	return true;
}


//-------------------------------------------------
//^ The DriverName function gets the name of the driver used to access the
//  current data source.  This function must be called only after establishing
//  a connection.
//> szDriverName is the name returned by the function.
//> size is the size of the server string.
//< The return code is DBS_OK on success or the return code from GetInfo on
//  failure.
//
RETCODE vdbDatabase::DriverName( char* szDriverName, size_t size )
{
	SWORD pcbInfoValue = size; 
	return GetInfo( SQL_DRIVER_NAME, szDriverName, size, &pcbInfoValue );
}


//=============================================================================
// Initialization functions
//=============================================================================

//-------------------------------------------------
//^ The InitializeAfterConnect function is called by
//  Connect after a valid DBMS connection is established.
//  This function first creates a vdbServer instance for
//  Server-specific functions.  Then it determines whether or
//  not the necessary system tables exist.  If they do exist
//  the _nextOID is retrieved and the version information is
//  cached.  If these system tables do not exist, no caching
//  occurs, but the connection proceeds nevertheless.
//< Always returns DBS_OK.
//
RETCODE vdbDatabase::InitializeAfterConnect()
{
	// Create a vdbServer instance for Server-specific functions for this source.
	char szServerName[30];
	ServerNameFromConnection( szServerName, sizeof(szServerName) );
	_pServer = CreateServerInstance( szServerName );

	// Does this datasource have the necessary system tables?
	if ( TableExists( "vdbVersion" ) && TableExists( "vdbUnique" ) )
	{
		_hasSystemTables = true;

		// Fill the version cache
		vdbQuery<vdbVersion> q( this );
		q.Execute( _versionCache );

		// Get a cache of unique OID's
		GetCacheOfUniqueIDs();

		// release locks
		Commit();
	}

	// set autocommit to true so that Commit and Rollback are not necessary.
	SetAutoCommit( true );

	// set the connection status
	_connected = true;
		
	return DBS_OK;
}


//-------------------------------------------------
//^ The CreateServerInstance function creates the
//  vdbServer-specific instance to handle SQL dialects and to
//  provide special DBMS capabilities.
//^ Rules for overidding this function: create a new vdbServer-derived
//  instance and assign the pointer to the variable _pServer.
//  The CleanupBeforeDisconnect function will destroy the instance.
//> szServerName is the name obtained from ServerNameFromConnection or ServerNameFromDatasource 
//< returns a pointer to the server; the caller must assign it to _pServer or
//  otherwise manage and delete it.
//
vdbServer* vdbDatabase::CreateServerInstance( const char* szServerName )
{
	vdbServer* pServer = 0;

	// first try matching the name obtained by ServerNameFromConnection
	if ( !strcmp(szServerName, "Microsoft SQL Server" )	)
		pServer = new vdbMicrosoftSQLServer();
	else if ( !strcmp(szServerName, "WATCOM SQL" ) )
		pServer = new vdbWatcomSQLServer();
	else if ( !strcmp(szServerName, "OOT-SQL" ) )
		pServer = new vdbOmniscienceORDBMS();

	// then try matching the name obtained byServerNameFromDatasource
	else if ( !strcmp(szServerName, "SQL Server" )	)
		pServer = new vdbMicrosoftSQLServer();
	else if ( !strcmp(szServerName, "Watcom SQL 4.0 (32 bit)" ) )
		pServer = new vdbWatcomSQLServer();
	else if ( !strcmp(szServerName, "OmniScience ODBC Driver (32 bit)" ) )
		pServer = new vdbOmniscienceORDBMS();
	
	// if all else fails
	else
		pServer = new vdbServer();

	if ( pServer == 0 ) throw vdbMemoryException();
	
	return pServer;
}


//-------------------------------------------------
//^ The FindHighestOID function is called by
//  InitializeAfterConnect when it determines that the
//  database wasn't fully closed.  This function scans
//  all tables and finds the highest OID in the system.
//> previousValue is the previously know highest OID.
//< The return value is an OID one higher than the highest 
//  found by the system.
//
OID vdbDatabase::FindHighestOID( OID previousValue )
{
	OID highest = previousValue;

	// loop through all tables
	for ( int i = 0; i < _tables.Count(); i++ )
	{
		std::ostrstream os;
		os << "SELECT ";
		os << Server()->CapitalizeNames( "oid" );
		os << " FROM ";
		os << _tables[i].FullyQualifiedTableName();
		os << " WHERE ";
		os << Server()->CapitalizeNames( "oid" );
		os << " > ";
		os << highest;
		os << std::ends;
	    char* sql = os.str();

		vdbStatement stmt( this );																
		RETCODE rc = stmt.Execute( sql );														
		OID fromDB;

		// bind the parameter value and fetch until done
		while ( rc == SQL_SUCCESS )															
		{																					
			stmt.Bind( 1, fromDB );
			rc = stmt.Fetch();																
			if ( rc == SQL_NO_DATA_FOUND )													
				break;																		
			else if ( rc != SQL_SUCCESS )
			{													
				stmt.Error(0);	
				return highest;
			}
		
			if ( fromDB > highest )
				highest = fromDB;
		}                                                                                   
		delete[] sql; sql = 0;
	}

	return highest + 1;
}


//-------------------------------------------------
//^ The CleanupBeforeDisconnect function is called by Disconnect to clear the
//  version cache and save the cached _nextOID.  This function deletes the
//  vdbServer specific instance created by CreateServerInstance during Connect.
//< Always returns DBS_OK.
//  
RETCODE vdbDatabase::CleanupBeforeDisconnect()
{
	if ( HasSystemTables() )
	{
		// reliquish the version cache
		_versionCache.ClearAll();

		// Save the nextOID/lastOID for the next session
		SaveCacheOfUniqueIDs();
		Commit();
	}

	// remove the server specific instance
	delete _pServer; _pServer = 0;

	// set the connection status
	_connected = false;

	return DBS_OK;
}


//------------------------------------------
//^ The InitializeAfterCreate function is intended to 
//  be called immediately after first creation, before
//  calling CreateSystemTables but after a valid Connect
//  (typically ConnectAsAdministrator would be used).
//  This function should initialize a new database according
//  to the rules of the specific DBMS; for example, grant connect,
//  grant resource, etc.
//^ Due to server variations this is referred to the
//  vdbServer class to handle.
//< returns the value from vdbServer::InitializeAfterCreate
//
RETCODE vdbDatabase::InitializeAfterCreate()
{						
	return Server()->InitializeAfterCreate();
}


//=============================================================================
// SQL Server Desktop Engine
//=============================================================================

//-------------------------------------
bool vdbDatabase::IsEngineRunning( const char* szServerName )
{
	vdbServer* pServer = CreateServerInstance( szServerName );
	bool b = pServer->IsEngineRunning();
	delete pServer; pServer = 0;
	return b;
}


//-------------------------------------
bool vdbDatabase::StartEngine( const char* szServerName )
{
	vdbServer* pServer = CreateServerInstance( szServerName );
	bool b = pServer->StartEngine();
	delete pServer; pServer = 0;
	return b;
}


//-------------------------------------
bool vdbDatabase::StopEngine( const char* szServerName )
{
	vdbServer* pServer = CreateServerInstance( szServerName );
	bool b = pServer->StopEngine();
	delete pServer; pServer = 0;
	return b;
}


//=============================================================================
// Connection / Disconnection
//=============================================================================


//-------------------------------------
//^ The Connect function is used to establish a database connection.
//
//> szDSN is the data source name.  Use the DataSources function to obtain one of these.
//> szUID is the user ID for authentication.
//> szAuthStr is the password for authentication
//
//< returns DBS_OK if connection established.  Be sure to check this return code.
//
RETCODE vdbDatabase::Connect( LPCSTR szDSN, LPCSTR szUID, LPCSTR szAuthStr )
{
	assert( _hdbc != SQL_NULL_HDBC );

	// Disconnect if already connected
	if ( IsConnected() )
		Disconnect();

	// start the Server Engine if it isn't already running	
	char szServerName[30];
	ServerNameFromDatasource( szDSN, szServerName, sizeof(szServerName) );
	if ( StartEngine( szServerName ) == false )
	{
		vdbTextBox( "Unable to start Database Server Engine", MB_OK );
		return DBS_FAILED;
	}

	// attempt to connect repeatedly until successful or the user cancels the request
	bool msgbox = true;
	vdbTroubleshootLoginDlgData* pData = new vdbTroubleshootLoginDlgData( szDSN, szUID, szAuthStr );
	if ( pData == 0 ) throw vdbMemoryException();
	do
	{
		_rc = ::SQLConnect( _hdbc, (UCHAR*) pData->szDataSource, SQL_NTS, (UCHAR*) pData->szAuthenticationID, SQL_NTS, (UCHAR*) pData->szAuthenticationPassword, SQL_NTS );
		if ( _rc != SQL_SUCCESS && _rc != SQL_SUCCESS_WITH_INFO )
			msgbox = vdbTroubleshootLoginDialog( *pData );
	}
	while ( msgbox == true && _rc != SQL_SUCCESS && _rc != SQL_SUCCESS_WITH_INFO );
	
	if ( msgbox == false )
		_rc = SQL_ERROR;

	// save the connection parameters
	_sDataSource = pData->szDataSource;
	_sAuthenticationID = pData->szAuthenticationID;
	_sAuthenticationPassword = pData->szAuthenticationPassword;

	// save the loginID to use as the "owner" qualifier for all tables
	Owner( pData->szAuthenticationID );
	delete pData; pData = 0; 			

	// Connecting sometimes produces "Changed database context to 'master'" message from Microsoft SQL Server
	if ( _rc == SQL_SUCCESS_WITH_INFO )
		_rc = SQL_SUCCESS;

	// fill the version cache and the nextOID
	if ( _rc == SQL_SUCCESS )
		_rc = InitializeAfterConnect();

	return _rc;
}


//-----------------------------------------------
//^ The ConnectAsAdministrator function establishes a connection
//  to a database, but does not call InitializeAfterConnect.
//  Therefore the vdbUnique and vdbVersion tables are not
//  available for use.
//  This function is intended for use in granting privelages
//  during database initialization.
//  Use the Connect function for normal connection.
//
//> szDSN is the datasource, if NULL a temporary data source is created for
//  the connection
//> szUID is the id of the Administrator.
//> szAuthStr is the password of the Administrator.
//  if the address of either szUID or szAuthStr are NULL, the server
//  default values will be obtained using vdbServer::AdminAuthentication.
//
//< returns SQL_SUCCESS if database connection was established.
//
RETCODE vdbDatabase::ConnectAsAdministrator( LPCSTR szDSN, LPCSTR szUID, LPCSTR szAuthStr )
{
	assert( _hdbc != SQL_NULL_HDBC );

	// Disconnect if already connected
	if ( IsConnected() )
		Disconnect();

	// start the Server Engine if it isn't already running	
	if ( StartEngine( "SQL Server" ) == false )
	{
		vdbTextBox( "Unable to start Database Server Engine", MB_OK );
		return DBS_FAILED;
	}

	char szDSN2[40];
	if ( szDSN == 0 )
		vdbUtility::SafeCopy( szDSN2, "Vault Administrator", 40 );
	else
		vdbUtility::SafeCopy( szDSN2, szDSN, 40 );

	// if no data source was specified, create a temporary one
	if ( szDSN == 0 )
	{
		char szComputerName[MAX_COMPUTERNAME_LENGTH + 1];
		DWORD len = MAX_COMPUTERNAME_LENGTH + 1;
		GetComputerName( szComputerName, &len );

		vdbString sServerInstance;
		if ( _defaults._sDesktopInstance.IsNull() )
			sServerInstance = szComputerName;
		else
			sServerInstance.Format( "%s\\%s", szComputerName, (const char*) _defaults._sDesktopInstance );

		vdbString sParam;
		sParam.Format( "DSN=Vault Administrator|SERVER=%s|DATABASE=master||", (const char*) sServerInstance );
		sParam.Replace( '|', '\0' );

		if ( SQLConfigDataSource( NULL, ODBC_ADD_DSN, "SQL Server", (const char*) sParam ) == FALSE )
		{
			vdbMessageBox( NULL, "Unable to create temporary ODBC Data source.", "Database", MB_OK );
			return DBS_FAILED;
		}
	}

	// if no ID and password are supplied, look up the defaults from the server
	if ( szUID == 0 || szAuthStr == 0 )
	{
		// Get server name for this Datasource from the Registry
		char szServerName[30];
		ServerNameFromDatasource( szDSN2, szServerName, sizeof(szServerName) );
		_pServer = CreateServerInstance( szServerName );

		// Obtain administrator ID and password
		char szUID2[30];
		char szAuthStr2[30];
		_pServer->AdminAuthentication( szUID2, sizeof(szUID2), szAuthStr2, sizeof(szAuthStr2) );
		
		vdbTroubleshootLoginDlgData* pData = new vdbTroubleshootLoginDlgData( szDSN2, szUID2, szAuthStr2 );
		if ( pData == 0 ) throw vdbMemoryException();

		// first attempt to connect using the strong password supplied by _pServer
		_rc = ::SQLConnect( _hdbc, (UCHAR*) pData->szDataSource, SQL_NTS, (UCHAR*) pData->szAuthenticationID, SQL_NTS, (UCHAR*) pData->szAuthenticationPassword, SQL_NTS );
		
		// if strong password fails, try using a blank password
		if ( _rc != SQL_SUCCESS && _rc != SQL_SUCCESS_WITH_INFO )
			pData->szAuthenticationPassword[0] = 0x0;

		// attempt to connect repeatedly until successful or the user cancels the request
		bool msgbox = true;
		while ( msgbox == true && _rc != SQL_SUCCESS && _rc != SQL_SUCCESS_WITH_INFO )
		{
			_rc = ::SQLConnect( _hdbc, (UCHAR*) pData->szDataSource, SQL_NTS, (UCHAR*) pData->szAuthenticationID, SQL_NTS, (UCHAR*) pData->szAuthenticationPassword, SQL_NTS );
			if ( _rc != SQL_SUCCESS && _rc != SQL_SUCCESS_WITH_INFO )
				msgbox = vdbTroubleshootLoginDialog( *pData );
		}

		if ( msgbox == false )
			_rc = SQL_ERROR;

		// save the connection parameters
		_sDataSource = pData->szDataSource;
		_sAuthenticationID = pData->szAuthenticationID;
		_sAuthenticationPassword = pData->szAuthenticationPassword;

		// save the loginID to use as the "owner" qualifier for all tables
		Owner( pData->szAuthenticationID );
		delete pData; pData = 0; 			

		if ( _rc == SQL_SUCCESS_WITH_INFO )
			_rc = SQL_SUCCESS;

		if ( _rc == SQL_SUCCESS )
			_connected = true;
		else
		{
			delete _pServer; _pServer = 0;
		}
	}

	
	// if the user ID and password were supplied, just connect
	else
	{
		// attempt to connect repeatedly until successful or the user cancels the request
		bool msgbox = true;
		vdbTroubleshootLoginDlgData* pData = new vdbTroubleshootLoginDlgData( szDSN2, szUID, szAuthStr );
		if ( pData == 0 ) throw vdbMemoryException();
		do
		{
			_rc = ::SQLConnect( _hdbc, (UCHAR*) pData->szDataSource, SQL_NTS, (UCHAR*) pData->szAuthenticationID, SQL_NTS, (UCHAR*) pData->szAuthenticationPassword, SQL_NTS );
			if ( _rc != SQL_SUCCESS && _rc != SQL_SUCCESS_WITH_INFO )
				msgbox = vdbTroubleshootLoginDialog( *pData );
		}
		while ( msgbox == true && _rc != SQL_SUCCESS && _rc != SQL_SUCCESS_WITH_INFO );
		delete pData; pData = 0; 			
		
		if ( msgbox == false )
			_rc = SQL_ERROR;

		// save the connection parameters
		_sDataSource = pData->szDataSource;
		_sAuthenticationID = pData->szAuthenticationID;
		_sAuthenticationPassword = pData->szAuthenticationPassword;

		if ( _rc == SQL_SUCCESS_WITH_INFO )
			_rc = SQL_SUCCESS;

		if ( _rc == SQL_SUCCESS )
		{
			char szServerName[30];
			ServerNameFromConnection( szServerName, sizeof(szServerName) );
			_pServer = CreateServerInstance( szServerName );
			_connected = true;
		}
	}
	
	// if no data source was specified, delete the temporary one just created
	if ( szDSN == 0 )
	{
		if ( SQLConfigDataSource( NULL, ODBC_REMOVE_DSN, "SQL Server", "DSN=Vault Administrator\0" ) == FALSE )
			vdbMessageBox( NULL, "Unable to remove temporary ODBC Data source.", "Database", MB_OK );
	}

	return _rc;
}


//-----------------------------------------------
//^ The Disconnect function calls CleanupBeforeDisconnect
//  prior to disconnection from the DBMS.  No further
//  work should be done with this datasource after Disconnect.
//  Another Connect may be called after disconnection or
//  the vdbDatabase instance may be deleted.
//< Returns DBS_OK on success or a value from SQLDisconnect
//  on failure.
//
RETCODE vdbDatabase::Disconnect()
{
	if ( !IsConnected() )
		return DBS_OK;
	
	if ( IsTransactionActive() )
	{
		if ( vdbMessageBox( NULL, "One or more database transactions are pending, do you want to save these records before disconnecting?",
							"Outstanding Transaction", MB_SETFOREGROUND | MB_YESNO ) == IDYES )
			Commit();
		else
			Rollback();
	}

	// relinquish the version cache and nextOID
	CleanupBeforeDisconnect();

	assert( _hdbc != SQL_NULL_HDBC );
	_rc = ::SQLDisconnect( _hdbc );

	// erase the connection parameters
	_sDataSource = "";
	_sAuthenticationID = "";
	_sAuthenticationPassword = "";

	return dbException ( SQL_API_SQLDISCONNECT );
}


//=============================================================================
// ODBC cover functions
//=============================================================================

//-----------------------------------------------
//^ The DataSources function obtains a list of datasources
//  as maintained by the ODBC Driver Manager.  Each call
//  obtains one datasource; call DataSources repeatedly to
//  obtain all datasources.
//> fDirection is SQL_FETCH_FIRST or SQL_FETCH_NEXT.
//> szDSN is the datasource returned by the function.  It should be 
//  SQL_MAX_DSN_LENGTH + 1 in size.
//> szDescription is the name of the driver returned by the function.
//> cbDescription is the maximum size of the szDescription string.
// 
RETCODE vdbDatabase::DataSources( UWORD fDirection, LPSTR szDSN, LPSTR szDescription, SWORD cbDescription )
{
	return DataSources(fDirection, szDSN, SQL_MAX_DSN_LENGTH-1, NULL, szDescription, cbDescription, NULL);
}


RETCODE vdbDatabase::DataSources(UWORD fDirection, LPSTR szDSN, SWORD cbDSNMax, SWORD* pcbDSN, LPSTR szDescription, SWORD cbDescriptionMax, SWORD* pcbDescription )
{
	assert( _henv != SQL_NULL_HENV );
	_rc = ::SQLDataSources( _henv, fDirection, (UCHAR*) szDSN, cbDSNMax, pcbDSN, (UCHAR*) szDescription, cbDescriptionMax, pcbDescription );
	return dbException( SQL_API_SQLDATASOURCES );
}


RETCODE vdbDatabase::Error( LPSTR szSqlState, SDWORD* pfNativeError, LPSTR szErrorMsg, SWORD cbErrorMsgMax, SWORD* pcbErrorMsg )
{
	assert( _hdbc != SQL_NULL_HDBC );
	return ::SQLError( SQL_NULL_HENV, _hdbc, SQL_NULL_HSTMT, (UCHAR*) szSqlState, pfNativeError, (UCHAR*) szErrorMsg, cbErrorMsgMax, pcbErrorMsg );
}


RETCODE vdbDatabase::GetConnectOption(UWORD fOption, PTR pvParam)
{
	assert( _hdbc != SQL_NULL_HDBC );
	_rc = ::SQLGetConnectOption( _hdbc, fOption, pvParam );
	return dbException ( SQL_API_SQLGETCONNECTOPTION );
}

RETCODE vdbDatabase::GetFunctions(UWORD fFunction, UWORD* pfExists)
{
	assert( _hdbc != SQL_NULL_HDBC );
	_rc = ::SQLGetFunctions( _hdbc, fFunction, pfExists );
	return dbException ( SQL_API_SQLGETFUNCTIONS );
}

RETCODE vdbDatabase::GetInfo( UWORD fInfoType, PTR rgbInfoValue, SWORD cbInfoValueMax, SWORD* pcbInfoValue )
{
	assert( _hdbc != SQL_NULL_HDBC );
	_rc = ::SQLGetInfo( _hdbc, fInfoType, rgbInfoValue, cbInfoValueMax, pcbInfoValue );
	return dbException ( SQL_API_SQLGETFUNCTIONS );
}


RETCODE vdbDatabase::NativeSql( LPCSTR szSqlStrIn, LPSTR szSqlStr, SDWORD cbSqlStrMax, SDWORD* pcbSqlStr )
{
	return NativeSql( szSqlStrIn, SQL_NTS, szSqlStr, cbSqlStrMax, pcbSqlStr );
}


RETCODE vdbDatabase::NativeSql( LPCSTR szSqlStrIn, SDWORD cbSqlStrIn,	LPSTR szSqlStr, SDWORD cbSqlStrMax,	SDWORD* pcbSqlStr )
{
	assert( _hdbc != SQL_NULL_HDBC );
	_rc = ::SQLNativeSql( _hdbc, (UCHAR*) szSqlStrIn, cbSqlStrIn, (UCHAR*) szSqlStr, cbSqlStrMax, pcbSqlStr );
	return dbException ( SQL_API_SQLNATIVESQL );
}

RETCODE vdbDatabase::SetConnectOption( UWORD fOption, UDWORD vParam )
{
	assert(_hdbc != SQL_NULL_HDBC);
	_rc = ::SQLSetConnectOption( _hdbc, fOption, vParam );
	return dbException ( SQL_API_SQLSETCONNECTOPTION );
}


RETCODE vdbDatabase::Transact( UWORD fType )
{
	assert( _hdbc != SQL_NULL_HDBC );
	_rc = ::SQLTransact( SQL_NULL_HENV, _hdbc, fType );
	return dbException( SQL_API_SQLTRANSACT );
}


//=============================================================================
// Unique ID management
//=============================================================================


const int g_OIDCacheSize = 100;		// the number of OID's to cache

//------------------------------------------
//^ Get a cache of unique OID's whenever opening the database
//  or whenever the existing cache has been depleted.
//
void vdbDatabase::GetCacheOfUniqueIDs()
{
	// find any vdbUnique record which is not being used, the first one will do
	RETCODE rc;
	vdbQuery<vdbUnique> q( this );
	q.SelectionCriteria( "cacheIndicator", "=", (DBSMALL) 0 );
	vdbUnique* pExistingUnique = q.Execute( rc );

	// if found, use it
	if ( pExistingUnique != 0 )
	{
		// if the vdbDatabase already has a vdbUnique instance,
		// it has been depleted, so Delete it from the DBMS and delete it from memory.
		if ( _pUnique != 0 )
		{
			_pUnique->Delete();
			delete _pUnique; _pUnique = 0;
		}

		// make the existing vdbUnique record this vdbDatabase instance's member. 
		pExistingUnique->SetCacheIndicatorActive();
		pExistingUnique->Save();
		_pUnique = pExistingUnique;
		return;
	}

	// if no vdbUnique record is found in the DBMS, create one.
	else
	{
		// if the vdbDatabase already has a vdbUnique instance, reuse it,
		// otherwise the database is just now opening, so create one in memory.
		if ( _pUnique == 0 )
			_pUnique = new vdbUnique( 0, this );
		if ( _pUnique == 0 ) throw vdbMemoryException();

		// get the master vdbUnique record
		vdbPtr<vdbUnique> pMaster;
		pMaster.Adopt( new vdbUnique( 2, this ) );
		if ( pMaster.IsInvalid() )
			throw vdbException( "vdbUnique OID = 2 not found" );
		
		// copy the master record's nextOID to the local user's record
		_pUnique->SetNextOID( pMaster->GetNextOID() );
		_pUnique->SetLastOID( pMaster->GetNextOID() + g_OIDCacheSize - 1 );
		_pUnique->SetCacheIndicatorActive();
		_pUnique->Save();

		// Update the master record's nextOID
		pMaster->SetNextOID( pMaster->GetNextOID() + g_OIDCacheSize );
		pMaster->Save();
		return;
	}
}


//------------------------------------------
//^  Save the nextOID and lastOID to the DBMS for 
//   the next usage.  Called during Disconnect.
//
void vdbDatabase::SaveCacheOfUniqueIDs()
{
	if ( _pUnique != 0 )
	{
		_pUnique->SetCacheIndicatorAvailable();
		_pUnique->Save();
		delete _pUnique; _pUnique = 0;
	}
}


//------------------------------------------------
//^ The Owner function changes the "owner" of all tables to be the supplied
//  value.  FullyQualifiedTableName uses this value to prepend to every 
//  table name.
//^ This function is private and intended to be called only by the ConnectXXX functions.
//> owner is the owner name to assign to all tables.
//
void vdbDatabase::Owner( const char* owner )
{
	vdbUtility::SafeCopy( _owner, owner, sizeof(_owner) );

	// Because FullyQualifiedTableName caches it's value, we must reset all tables
	// to NULL so that the cache is refreshed on the next call to FullyQualifiedTableName
	_pvdbUniqueTable->ResetFullyQualifiedTableName();
	_pvdbVersionTable->ResetFullyQualifiedTableName();

	for ( int i = 0; i < _tables.Count(); i++ )
		_tables[i].ResetFullyQualifiedTableName();
}


