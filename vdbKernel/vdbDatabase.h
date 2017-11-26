//=============================================================================
//
// CLASS:           vdbDatabase
//
// OVERVIEW:		Class declaration for vdbDatabase
//
// FILE NAME:       vdbDatabase.h
//
// INITIAL DATE:    01/14/96 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __vdbDatabase_h
#define __vdbDatabase_h

#include <vdbMessageBox.h>
#include "vdbSQL.h"
#include "vdbTypedef.h"
#include <vdbAssert.h>
#include <vdbString.h>
#include <boolean.h>
#include "vdbSet.h"
#include "vdbTable.h"
#include "vdbSetOfVersions.h"
#include "vdbMicrosoftSQLServer.h"
#include "vdbPool.h"

//{{ForwardTableDeclarations
class vdbUniqueTable;
class vdbVersionTable;
//}}
class vdbServer;
class vdbUnique;


//-------------------------------------
class vdbDefaultCreationParameters
{
public:
	vdbString _sDataSourceName;
	vdbString _sDescription;
	vdbString _sDesktopInstance;		// SQL Server 2000 Instance name
	vdbString _sDatabaseName;
	vdbString _sDirectoryLocation;
	int _dataSize;						// in Megabytes
	int _logSize;
	vdbString _sAuthorizationID;
	vdbString _sAuthorizationPassword;
	bool _bEnableAuthorization;			// false to prevent users from changing ID and password on creation
};


//-------------------------------------
class vdbDatabase
{
protected:
	// essential class functions
	vdbDatabase();										// basic constructor
	virtual ~vdbDatabase();								// destructor
	vdbDatabase( const vdbDatabase& rhs );				// copy constructor disabled
	vdbDatabase& operator=( const vdbDatabase& rhs );	// assignment operator disabled

public:
	// member access
	inline vdbServer* Server();
	inline RETCODE ReturnCode();
	inline HDBC GetHdbc();
	inline vdbUniqueTable* UniqueTable();
	inline vdbVersionTable* VersionTable();
	inline vdbSetOfVersions& VersionCache();
	inline bool IsConnected();
	inline bool HasSystemTables();
	inline const char* Owner();
	inline vdbSet<vdbTable>& Tables();
	inline CRITICAL_SECTION* StatementGuard();

public:
	// table maintenance	
	void AddTable( vdbTable* p );
	virtual void CreateSystemTables();
	virtual void CreateUserTables();
	void CreateUserTable( vdbTable& pTable );
	virtual void DropSystemTables();
	virtual void DropUserTables();
	void DropUserTable( vdbTable& pTable );

protected:
	// protected overrideables
	virtual void Error( int functionNumber );
	virtual RETCODE dbException( int functionNumber );

public:
	// public error recovery functions
	RETCODE Trouble( const char* desc, const char* table );
	RETCODE Trouble( const char* desc, OID oid );
	void QuitApplication( int rc );
	void QuenchErrors( bool tf );
	bool QuenchErrors();

public:
	// public overrideables
	virtual RETCODE Trouble( const char* desc );
	virtual OID GetNextOID();

public:
	// transaction management
	void BeginTransaction();
	RETCODE Commit();
	RETCODE Rollback();
	RETCODE SetAutoCommit( bool commit = true );
	bool GetAutoCommit();
	bool IsTransactionActive();

public:
	// value added
	bool TableExists( const char* szTableName );
	bool TableExists( const char* szOwner, const char* szTableName );
	RETCODE ServerNameFromConnection( char* szServerName, size_t size );
	RETCODE ServerNameFromDatasource( const char* szDataSource, char* szServerName, size_t size );
	bool IsValidDataSource( const char* szDataSource ) const;
	RETCODE DriverName( char* szDriverName, size_t size );
	inline const char* DataSource() const;
	inline const char* AuthenticationID() const;
	inline const char* AuthenticationPassword() const;

protected:
	// protected initialization functions
	virtual RETCODE InitializeAfterConnect();
	virtual RETCODE CleanupBeforeDisconnect();
	virtual vdbServer* CreateServerInstance( const char* szServerName );
	virtual OID FindHighestOID( OID previousValue );

public:
	RETCODE InitializeAfterCreate();

public:
	// SQL Server Desktop Engine
	bool IsEngineRunning( const char* szServerName );
	bool StartEngine( const char* szServerName );
	bool StopEngine( const char* szServerName );

	// connection and disconnection
	RETCODE Connect( LPCSTR szDSN, LPCSTR szUID = NULL, LPCSTR szAuthStr = NULL );
	RETCODE ConnectAsAdministrator( LPCSTR szDSN = NULL, LPCSTR szUID = NULL, LPCSTR szAuthStr = NULL );
	RETCODE Disconnect();

public:
	// ODBC cover functions
	RETCODE DataSources( UWORD fDirection, LPSTR szDSN,	LPSTR szDescription, SWORD cbDescription = 255 );
	RETCODE DataSources( UWORD fDirection, LPSTR szDSN,	SWORD cbDSNMax,	SWORD* pcbDSN, LPSTR szDescription,	SWORD cbDescriptionMax,	SWORD* pcbDescription );
	RETCODE Error( LPSTR szSqlState, SDWORD* pfNativeError, LPSTR szErrorMsg, SWORD cbErrorMsgMax = SQL_MAX_MESSAGE_LENGTH-1, SWORD* pcbErrorMsg = NULL );
	RETCODE GetConnectOption( UWORD fOption, PTR pvParam );
	RETCODE GetFunctions( UWORD fFunction, UWORD* pfExists );
	RETCODE GetInfo( UWORD fInfoType, PTR rgbInfoValue, SWORD cbInfoValueMax = 4, SWORD* pcbInfoValue = NULL );
	RETCODE NativeSql( LPCSTR szSqlStrIn, LPSTR szSqlStr, SDWORD cbSqlStrMax, SDWORD* pcbSqlStr = NULL );
	RETCODE NativeSql( LPCSTR szSqlStrIn, SDWORD cbSqlStrIn, LPSTR szSqlStr, SDWORD cbSqlStrMax, SDWORD* pcbSqlStr = NULL );
	RETCODE SetConnectOption( UWORD fOption, UDWORD vParam );
	RETCODE Transact( UWORD fType );

private:
	// unique ID management
	void GetCacheOfUniqueIDs();
	void SaveCacheOfUniqueIDs();

private:
	//{{TableInstances
	vdbUniqueTable* _pvdbUniqueTable;
	vdbVersionTable* _pvdbVersionTable;
	//}}

private:
	void Owner( const char* owner );

public:
	// used by vdbDialogs::NewDatabase
	vdbDefaultCreationParameters _defaults;

private:
	// member data
	RETCODE _rc;             			// ODBC return code
	HENV    _henv;           			// ODBC environment handle
	HDBC    _hdbc;           			// ODBC connection handle
	CRITICAL_SECTION _statementGuard;	// to prevent calls for vdbStatement::Bind, Parameter, Fetch, Execute, etc. from getting interlaced
	vdbServer* _pServer;				// pointer to the specific type of SQL server for non-generic services
	vdbSet<vdbTable> _tables;			// the tables of this database
	vdbSetOfVersions _versionCache;		// cache of all table versions for use by vdbObject class
	vdbUnique* _pUnique;				// cache of the nextOID and lastOID
	bool	_connected;					// whether or not a connection is currently established
	bool	_hasSystemTables;			// whether or not this datasource has a "vdbVersion" and a "vdbUnique" table
	char	_owner[30];					// the name of the "owner" of all tables in this database
	bool	_quenchErrors;				// should messages be quenched?
	int     _transactionLevel;			// 0 = no transactions begun, 1 = one transaction started, 2 = two nested transactions started, etc.
	bool	_bSaveAutoCommit;			// the autocommit value is saved by BeginTransaction and restored by Commit and Rollback
	vdbString _sDataSource;				// name of the ODBC datasource currently connected to
	vdbString _sAuthenticationID;		// name of the current connection loginID
	vdbString _sAuthenticationPassword;	// password of the current connection

	friend class vdbMicrosoftSQLServer;		// MicrosoftSQLServer::Use() calls GetCacheOfUniqueIDs ans SaveCacheOfUniqueIDs [changes _nextOID and _hasSystemTables].
	friend class vdbSet<vdbDatabase>;		// calls the constructor
	friend class vdbPool;					// RemoveDatabase() calls destructor
};									


// Connect call OpenDialogSQLSCMLocalServiceControl is in Commander.dll
#pragma comment(linker,"/defaultlib:Commander.lib")

//=============================================================================
// inlines
//=============================================================================

inline vdbServer* vdbDatabase::Server()
{
	assert ( _pServer != 0 );
	return _pServer;
}


inline RETCODE vdbDatabase::ReturnCode()
{
	return _rc;
}


inline HDBC vdbDatabase::GetHdbc()
{
	return _hdbc;
}


inline vdbUniqueTable* vdbDatabase::UniqueTable()
{
	return _pvdbUniqueTable;
}


inline vdbVersionTable* vdbDatabase::VersionTable()
{
	return _pvdbVersionTable;
}


inline vdbSetOfVersions& vdbDatabase::VersionCache()
{
	return _versionCache;
}


inline bool vdbDatabase::IsConnected()
{
	return _connected;
}


inline bool vdbDatabase::HasSystemTables()
{
	return _hasSystemTables;
}


inline const char* vdbDatabase::Owner()
{
	return _owner;
}


inline vdbSet<vdbTable>& vdbDatabase::Tables()
{
	return _tables;
}


inline CRITICAL_SECTION* vdbDatabase::StatementGuard()
{
	return &_statementGuard;
}


inline const char* vdbDatabase::DataSource() const
{
	return _sDataSource;
}


inline const char* vdbDatabase::AuthenticationID() const
{
	return _sAuthenticationID;
}


inline const char* vdbDatabase::AuthenticationPassword() const
{
	return _sAuthenticationPassword;
}

#endif // __vdbDatabase_h
