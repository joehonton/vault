//=============================================================================
//
// CLASS:           vdbTable
//
// ESSENCE:         database table base class 
//
// FILE NAME:       vdbTable.cpp
//
// INITIAL DATE:    2/1/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifdef USE_PRE_HDR
	#include "vdbHeader.h"
#else
	#include <vdbMessageBox.h>
	#include <vdbAssert.h>
	#include <vdbString.h>
	#include "vdbDatabase.h"
	#include "vdbStatement.h"
	#include "vdbTable.h"
	#include "vdbSet.h"
	#include "vdbServer.h"
	#include "vdbException.h"
#endif

#include <strstream>
#include "vdbSetOfVersions.h"
#include "vdbVersion.h"


//==========================================================================================
//  Essential class functions
//==========================================================================================

//------------------------------------------
// basic constructor
vdbTable::vdbTable( vdbDatabase* pDatabase )
	: _pDatabase( pDatabase ),
	  _columns( 10 ),						  // pre-allocate for 10 columns
	  _indexes( 3 )							  // pre-allocate for 3 indexes
{
	_szTableName[0] = 0;
	_szFullyQualifiedTableName[0] = 0;
}


//------------------------------------------
// destructor
vdbTable::~vdbTable()
{
}


//------------------------------------------
// copy constructor 
vdbTable::vdbTable( const vdbTable& rhs )
	: _pDatabase( rhs._pDatabase ),
	  _columns( rhs._columns ),
	  _indexes( rhs._indexes )
{
	vdbUtility::SafeCopy( _szTableName, rhs._szTableName, sizeof(_szTableName) );
	vdbUtility::SafeCopy( _szFullyQualifiedTableName, rhs._szFullyQualifiedTableName, sizeof(_szFullyQualifiedTableName) );
}


//------------------------------------------
// assignment operator
vdbTable& vdbTable::operator=( vdbTable& rhs )
{
	if ( this == &rhs )
		return *this;
	
	_pDatabase = rhs._pDatabase;

	int i;
	_columns.ClearAll();
	for ( i = 0; i < rhs._columns.Count(); i++ )
	{
		vdbColumn* lhsColumn = new vdbColumn( rhs._columns[i] );
		if ( lhsColumn == 0 ) throw vdbMemoryException();
		_columns.Add( lhsColumn );
	}

	_indexes.ClearAll();
	for ( i = 0; i < rhs._indexes.Count(); i++ )
	{
		vdbIndex* lhsIndex = new vdbIndex( rhs._indexes[i] );
		if ( lhsIndex == 0 ) throw vdbMemoryException();
		_indexes.Add( lhsIndex );
	}

	vdbUtility::SafeCopy( _szTableName, rhs._szTableName, sizeof(_szTableName) );
	vdbUtility::SafeCopy( _szFullyQualifiedTableName, rhs._szFullyQualifiedTableName, sizeof(_szFullyQualifiedTableName) );

	return *this;
}



//==========================================================================================
// required virtual functions
//==========================================================================================

//------------------------------------------
//  The case-senstive name of the database table.  This function
//  must be supplied by each derived class.  The string
//  may be in upper and lower case; proper capitalization
//  is handled by vdbServer class. 
//
const char* vdbTable::TableNameCS()
{
	return "";
}


//------------------------------------------
int vdbTable::GetCodeVersion()
{
	return 1;
}


//-----------------------------------------------
//^ The ClientIDColumn function is used by vdbLinkSet to obtain the name of
//  the DBMS column which this object uses to hold the object identifier of
//  its client owner.
//> pClient is the object which is the client of this.
//< The return value is the character string containing the column name which
//  is suitable for use in a call to vdbQuery::Selection.
//< The return value is an empty string if the client passed as the argument
//  is not a valid owner of this.
//
const char* vdbTable::ClientIDColumn( vdbObject* pClient )
{
	vdbString s;
	s.Format( "This client is not a valid client for %s", (const char*) TableNameCS() ),
	GetDatabase()->Trouble( s, pClient->TableNameCS() );
	return "";
}


//==========================================================================================
// SQL table create and drop functions
//==========================================================================================

//------------------------------------------
//^ The TableName function returns the properly capitalized	table name,
//  without the owner prefix.
//
const char* vdbTable::TableName()
{
	// Only ask the server to capitalize this name the first time the function is called
	if ( _szTableName[0] == 0 )
	{
		vdbUtility::SafeCopy( _szTableName, TableNameCS(), sizeof(_szTableName) );
		GetDatabase()->Server()->CapitalizeNames( _szTableName );
	}

	return _szTableName;
}


//------------------------------------------
// The FullyQualifiedTableName funtion returns the table name including the
// owner prefix.
//
const char* vdbTable::FullyQualifiedTableName()
{
	// Only assemble the fully qualified name the first time this function is called
	if ( _szFullyQualifiedTableName[0] == 0 )
	{
		if ( strlen( GetDatabase()->Owner() ) > 0 )
		{ 
			vdbUtility::SafeCopy( _szFullyQualifiedTableName, GetDatabase()->Owner(), sizeof(_szFullyQualifiedTableName) );
			vdbUtility::SafeCat( _szFullyQualifiedTableName, ".", sizeof(_szFullyQualifiedTableName) );
			vdbUtility::SafeCat( _szFullyQualifiedTableName, TableName(), sizeof(_szFullyQualifiedTableName) );
		}
		else
			vdbUtility::SafeCopy( _szFullyQualifiedTableName, TableName(), sizeof(_szFullyQualifiedTableName) );
	}

	return _szFullyQualifiedTableName;
}


//------------------------------------------
//^ The ResetFullyQualifiedTableName function is called during database opening to
//  clear the _szFullyQualifiedTableName variable in case the owner of the new database
//  is different. 
void vdbTable::ResetFullyQualifiedTableName()
{
	// The first call to FullyQualifiedTableName() will refresh the variable
	_szFullyQualifiedTableName[0] = 0x0;
}


//------------------------------------------
// CreateTable() creates a new table
//^ This action always causes a commit before and after the
//  CREATE TABLE statement regardless of the Autocommit state.
//
RETCODE vdbTable::CreateTable()
{						
    GetDatabase()->Commit();
	
	// assemble the SQL statement
	int size = 13 + strlen( FullyQualifiedTableName() ) + 3 + ( ColumnCount() * (2 + 32 /* max sizeof column name */ + 1 + 15 /* max sizeof type */ ) ) + 2 + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "CREATE TABLE " << FullyQualifiedTableName() << " ( ";
	for ( int i = 0; i < ColumnCount(); i++ )
	{
		if ( i != 0 )
			os << ", ";
		os << Column(i).Name() << " " << Column(i).Type();
	}
    os << " )" << std::ends;

	vdbStatement stmt( GetDatabase() );
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;

    GetDatabase()->Commit();

	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}
	
	
//------------------------------------------
// CreateIndexes() creates all indexes for the table
//
void vdbTable::CreateIndexes()
{						
	for( int i = 0; i < _indexes.Count(); i++ )
		CreateIndex(i);	
}


//------------------------------------------
// CreateIndex() creates a new index
//
RETCODE vdbTable::CreateIndex( int which )
{						
    assert( which < IndexCount() );

    // assemble the SQL statement
	int size = 6 + 7 + 7 + strlen( Index(which).IndexName() ) + 4 + strlen( FullyQualifiedTableName() ) + 2 + ( Index(which).ColumnCount() * (32 /* max sizeof index column name */ + 2) ) + 1 + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "CREATE";
	if ( Index(which).IsUnique() )
		os << " UNIQUE";
	os << " INDEX " << Index(which).IndexName() << " ON " << FullyQualifiedTableName() << " (";
	for ( int j = 0; j < Index(which).ColumnCount(); j++ )
	{
		if ( j != 0 )
			os << ", ";
		os << Index(which).ColumnName(j);
	}
    os << ")" << std::ends;
	
	vdbStatement stmt( GetDatabase() );
	RETCODE rc = stmt.Execute( sql );														

	delete[] sql; sql = 0;
	return rc;
}


//------------------------------------------
//^ The Drop function drops the table and all its rows from the DBMS.
//^ This action always causes a commit before and after the
//  DROP statement regardless of the Autocommit state.
//
RETCODE vdbTable::DropTable()
{						
	GetDatabase()->Commit();

    // assemble the SQL statement
	int size = 11 + strlen( FullyQualifiedTableName() ) + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "DROP TABLE " << FullyQualifiedTableName() << std::ends;
	
	vdbStatement stmt( GetDatabase() );
	RETCODE rc = stmt.Execute( sql );														

	GetDatabase()->Commit();

	delete[] sql; sql = 0;
	return rc;
}


//------------------------------------------
// DropIndexes() drops all indexes for the table
//
void vdbTable::DropIndexes()
{						
	for( int i = 0; i < _indexes.Count(); i++ )
		DropIndex(i);	
}


//------------------------------------------
// Drop() drops the table and all its rows
// Due to server variations in syntax this is
// referred to the vdbServer class to handle.
//
RETCODE vdbTable::DropIndex( int which )
{						
    assert( which < IndexCount() );
	return GetDatabase()->Server()->DropIndex( this, which );
}


//-------------------------------------------------
// returns true if the table exists in this datasource
//
bool vdbTable::TableExists()
{
	vdbStatement stmt( GetDatabase() );
	RETCODE rc = stmt.Tables( NULL, 0, GetDatabase()->Owner(), SQL_NTS, TableName(), SQL_NTS, "TABLE", SQL_NTS );
	if ( rc != SQL_SUCCESS )
		return false;
	
	// If the fetch was successful, then there is such a table
	rc = stmt.Fetch();
	if ( rc != SQL_SUCCESS )
		return false;
	else
		return true;
}


//------------------------------------------
//^ The AlterTable function adds/modifies/deletes columns in an existing table.  Use this
//  function only in conversion routines.  The column added/modified/deleted is only added to
//  the underlying database table.  It is not added to the in-memory
//  representation of the schema.
//> newColumn is the new column to add/modify/delete.
//> szHow is "ADD" to add the column, "ALTER" to alter it, or "DROP" to delete it
//
RETCODE vdbTable::AlterTable( vdbColumn& newColumn, const char* szHow )
{						
    // assemble the SQL statement
	int size = 12 + strlen( FullyQualifiedTableName() ) + 14
	              + strlen( newColumn.Name() ) + 1
	              + strlen( newColumn.Type() ) + 5 + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	if ( !strcmp( szHow, "ADD" ) )
	{
		os << "ALTER TABLE " << FullyQualifiedTableName() << " ADD ";
		os << newColumn.Name() << " ";
		os << newColumn.Type() << " NULL" << std::ends;
	}
	else if ( !strcmp( szHow, "ALTER" ) )
	{
		os << "ALTER TABLE " << FullyQualifiedTableName() << " ALTER COLUMN ";
		os << newColumn.Name() << " ";
		os << newColumn.Type() << " NULL" << std::ends;
	}
	else if ( !strcmp( szHow, "DROP" ) )
	{
		os << "ALTER TABLE " << FullyQualifiedTableName() << " DROP COLUMN ";
		os << newColumn.Name() << std::ends;
	}
	else
		assert( false );

	vdbStatement stmt( GetDatabase() );
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;    
	
	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}


//------------------------------------------
//^ The RenameColumn function changes the name of an existing column
//
RETCODE vdbTable::RenameColumn( const char* szOldName, const char* szNewName )
{						
    // assemble the SQL statement
	int size = 11 + strlen( FullyQualifiedTableName() ) + 1
	              + strlen( szOldName ) + 4
	              + strlen( szNewName ) + 11 + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );

	os << "sp_rename '";
	os << FullyQualifiedTableName() << ".";
	os << szOldName << "', '";
	os << szNewName << "', 'COLUMN'" << std::ends;

	vdbStatement stmt( GetDatabase() );
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;    
	
	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}


//==========================================================================================
//  Information functions
//==========================================================================================

//------------------------------------------
//^ The IsValidColumn function is a debugging routine to verify
//  that the given column exists in the database.  This check is
//  done through this object's associated vdbTable-derived class,
//  not through the DBMS.
//> szColumnName is the name of the column to validate.
//< Returns true if the column name exists in the table.
//< Returns false if not in the table.
//
bool vdbTable::IsValidColumn( const char* szColumnName )
{
	for ( int i = 0; i < ColumnCount(); i++ )
	{
		if ( !strcmp( Column(i).Name(), szColumnName ) )
			return true;
	}
	return false;
}
		

//==========================================================================================
// Support functions
//==========================================================================================

//------------------------------------------
// AddToVersionTable() adds this table
// to the vdbVersion table
//
RETCODE vdbTable::AddToVersionTable()
{						
	// now add this table to the Version table and to the global vdbSetOfVersions
	vdbVersion* pVer = new vdbVersion;
	if ( pVer == 0 ) throw vdbMemoryException();
	pVer->SetTableID( 0 );					// TableID is not set and not used anywhere.
	pVer->SetTableName( TableNameCS() );	// modified 9/3/96
	pVer->SetVersion( GetCodeVersion() );
	RETCODE rc = pVer->Insert();

	if ( pVer->IsPersistent() )
		GetDatabase()->VersionCache().Add( pVer );
		
	GetDatabase()->Commit();
	return rc;
}


//------------------------------------------
// RemoveFromVersionTable() removes this table
// from the vdbVersion table in memory and in the DBMS.
//
RETCODE vdbTable::RemoveFromVersionTable()
{
	return GetDatabase()->VersionCache().Remove( TableNameCS() );	 // modified 9/3/96
}


//==========================================================================================
// public version and identification utility functions
//==========================================================================================

//------------------------------------------
//  GetDiskVersion() returns the version number for 'this' derived class.
//  Use this to verify if the disk version is equivalent to the code version.
//  Returns 0 if table does not exist in database.
//
int vdbTable::GetDiskVersion() 
{
	vdbVersion* pVer = GetDatabase()->VersionCache().FindTable( TableNameCS() );	// modified 9/3/96
    if ( pVer == NULL )
    	return 0;
	return (int) pVer->GetVersion();	
}


//------------------------------------------
//> The TableID function returns the unique ID within the database domain
//  for 'this' derived class.
//> NOTE: The current implementation always returns 0 because this value is never set.
//  Returns 0 if table does not exist in database.
//
int vdbTable::TableID() 
{
	vdbVersion* pVer = GetDatabase()->VersionCache().FindTable( TableNameCS() );	 // modified 9/3/96
    if ( pVer == NULL )
    	return 0;
	return (int) pVer->GetTableID();	
}


