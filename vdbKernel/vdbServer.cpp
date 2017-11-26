//=============================================================================
//
// CLASS:           vdbServer
//
// ESSENCE:         Server specific database functionality available to
//                  vdbDatabase via GetServer()
//
// FILE NAME:       vdbServer.cpp
//
// INITIAL DATE:    5/6/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifdef USE_PRE_HDR
	#include "vdbHeader.h"
#else
	#include <vdbPool.h>
	#include "vdbServer.h"
	#include "vdbDatabase.h"
	#include "vdbStatement.h"
	#include "vdbUtility.h"
	#include "vdbException.h"
#endif

#include <strstream>


//=============================================================================
// essential class functions
//=============================================================================

//-------------------------------------
vdbServer::vdbServer( HANDLE_DATABASE hDB )
	: _pDatabase( vdbPool::Instance()->GetDatabase( hDB ) )
{
}


//-------------------------------------
vdbServer::~vdbServer()
{
}


//=============================================================================
//  virtual functions
//=============================================================================

//-------------------------------------
// Returns column types suitable for use in table creation
//
const char* vdbServer::ColumnType( const char* szType )
{
	if ( !strcmp( szType, "OID" ) )
		return "int";
	else if ( !strcmp( szType, "DBBIT" ) )			// default is a 2 byte int
		return "smallint";
	else if ( !strcmp( szType, "DBBYTE" ) )			// default is a 2 byte int
		return "smallint";
	else if ( !strcmp( szType, "DBSMALL" ) )
		return "smallint";
	else if ( !strcmp( szType, "DBINT" ) )
		return "int";
	else if ( !strcmp( szType, "DBFLOAT" ) )
		return "real";
	else if ( !strcmp( szType, "DBDOUBLE" ) )
		return "float";
	else if ( !strcmp( szType, "DBTIMESTAMP" ) )
		return "timestamp";
	else if ( !strncmp( szType, "decimal", 7 ) )
		return szType;
	else if ( !strncmp( szType, "char", 4 ) )
		return szType;
	else if ( !strncmp( szType, "varchar", 7 ) )
		return szType;
	else if ( !strncmp( szType, "longvarchar", 11 ) )
		return szType;
	else if ( !strncmp( szType, "binary", 6 ) )
		return szType;
	else if ( !strncmp( szType, "varbinary", 9 ) )
		return szType;
	else if ( !strncmp( szType, "longvarbinary", 13 ) )
		return szType;
	else
	{		
		GetDatabase()->Trouble( "unidentified column type in vdbServer::ColumnType()" ); 
		return "";
	}

}


//-------------------------------------
// Drops an index, using no dot prequalifiers
//
RETCODE vdbServer::DropIndex( vdbTable* pTable, int which )
{
    // assemble the SQL statement
	int size = 11 + strlen( pTable->Index(which).IndexName() ) + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
 	os << "DROP INDEX " << pTable->Index(which).IndexName() << std::ends;
	
	// bind the parameter values and execute																					
	vdbStatement stmt( GetDatabase() );
	RETCODE rc = stmt.Execute( sql );														

	delete[] sql; sql = 0;
	return rc;
}


//-------------------------------------
// Initialize a new database after creating it.
// For example, grant connect, grant resource, etc.
//
RETCODE vdbServer::InitializeAfterCreate()
{
	return 0;
}



//-------------------------------------
// If a server needs to have table and column names
// in all upper or all lower case, this routine can
// be overridden to do that.
char* vdbServer::CapitalizeNames( char* szString )
{
	return szString;
}


//-------------------------------------
//^ The AdminAuthentication function obtains the default administrator userID and password.
//  Each server should provide this function if any application needs to use the
//  vdbDatabase::ConnectAsAdministrator function.
//
//> szUID is the default administrator ID.
//> sizeUID is the maximum size of the string used for szUID.
//> szAuthStr is the password of the default administrator ID
//> sizeAuthStr is the maximum size of the string used for szAuthStr.
//
void vdbServer::AdminAuthentication( char* szUID, int sizeUID, char* szAuthStr, int sizeAuthStr )
{
	vdbUtility::SafeCopy( szUID, "id", sizeUID );
	vdbUtility::SafeCopy( szAuthStr, "password", sizeAuthStr );
}


//-------------------------------------
bool vdbServer::IsEngineRunning()
{
	return true;
}


//-------------------------------------
bool vdbServer::StartEngine()
{
	return true;
}


//-------------------------------------
bool vdbServer::StopEngine()
{
	return true;
}

