//=============================================================================
//
// CLASS:           vdbWatcomSQLServer
//
// ESSENCE:         vdbDatabase mix in functions specific to Watcom SQL 10.0
//
// FILE NAME:       vdbWatcomSQLServer.cpp
//
// INITIAL DATE:    5/7/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#include <strstream>
#include "vdbWatcomSQLServer.h"
#include "vdbStatement.h"
#include "vdbUtility.h"
#include "vdbException.h"


//=============================================================================
// essential class functions
//=============================================================================

//--------------------------------
vdbWatcomSQLServer::vdbWatcomSQLServer( HANDLE_DATABASE hDB )
	: vdbServer( hDB )
{
}


//--------------------------------
vdbWatcomSQLServer::~vdbWatcomSQLServer()
{
}


//=============================================================================
//  required virtual functions
//=============================================================================

//--------------------------------
// Returns column types suitable for
// use in table creation
//
const char* vdbWatcomSQLServer::ColumnType( const char* szType )
{
	if ( !strcmp( szType, "OID" ) )
		return "INT";
	else if ( !strcmp( szType, "DBBIT" ) )			// no support for 1 bit integers
		return "SMALLINT";
	else if ( !strcmp( szType, "DBBYTE" ) )			// no support for 1 byte integers
		return "SMALLINT";
	else if ( !strcmp( szType, "DBSMALL" ) )
		return "SMALLINT";
	else if ( !strcmp( szType, "DBINT" ) )
		return "INT";
	else if ( !strcmp( szType, "DBFLOAT" ) )
		return "FLOAT";
	else if ( !strcmp( szType, "DBDOUBLE" ) )
		return "DOUBLE";
	else if ( !strcmp( szType, "DBTIMESTAMP" ) )
		return "TIMESTAMP";
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
		GetDatabase()->Trouble( "unidentified column type in vdbWatcomSQLServer::ColumnType()" ); 
		return "";
	}

}


//--------------------------------
// 	Using the database administrator's
//  login, grant a new connection ID and password
//  and allow this userID to create tables
//
//  Must be logged in as database administrator
//
RETCODE vdbWatcomSQLServer::InitializeAfterCreate()
{
	RETCODE rc;

	rc = GrantConnect( "qsar", "carbon" );
	if ( rc == DBS_OK )
		rc = GrantResource( "qsar" );

	return rc;
}


//--------------------------------
//^ The AdminAuthentication function obtains the default administrator userID and password.
//
//> szUID is the default administrator ID.
//> sizeUID is the maximum size of the string used for szUID.
//> szAuthStr is the password of the default administrator ID
//> sizeAuthStr is the maximum size of the string used for szAuthStr.
//
void vdbWatcomSQLServer::AdminAuthentication( char* szUID, int sizeUID, char* szAuthStr, int sizeAuthStr )
{
	vdbUtility::SafeCopy( szUID, "DBA", sizeUID );
	vdbUtility::SafeCopy( szAuthStr, "SQL", sizeAuthStr );
}


//=============================================================================
//  public functions
//=============================================================================

//--------------------------------
// "grant connect to qsar identified by carbon"
//
RETCODE vdbWatcomSQLServer::GrantConnect( const char* szLoginID, const char* szPassword )
{
    // assemble the SQL statement
	int size = 17 + strlen(szLoginID) + 15 + strlen(szPassword) + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "GRANT CONNECT TO " << szLoginID;
	os << " IDENTIFIED BY " << szPassword;
	os << std::ends;
    
	// execute statement
	vdbStatement stmt( GetDatabase() );																
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;
	return rc;
}


//--------------------------------
// "grant resource to qsar"
//
RETCODE vdbWatcomSQLServer::GrantResource( const char* szLoginID )
{
    // assemble the SQL statement
	int size = 18 + strlen(szLoginID) + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "GRANT RESOURCE TO " << szLoginID;
	os << std::ends;
    
	// execute statement
	vdbStatement stmt( GetDatabase() );																
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;
	return rc;
}


