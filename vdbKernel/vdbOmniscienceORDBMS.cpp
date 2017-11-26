//=============================================================================
//
// CLASS:           vdbOmniscienceORDBMS
//
// ESSENCE:         vdbDatabase mix in functions specific to Omniscience ORDBMS 2.0
//
// FILE NAME:       vdbOmniscienceORDBMS.cpp
//
// INITIAL DATE:    8/10/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifdef USE_PRE_HDR
	#include "vdbHeader.h"
#else
	#include "vdbStatement.h"
#endif

#include "vdbOmniscienceORDBMS.h"


//=============================================================================
// essential class functions
//=============================================================================

//-------------------------------------------------
vdbOmniscienceORDBMS::vdbOmniscienceORDBMS( HANDLE_DATABASE hDB )
	: vdbServer( hDB )
{
}


//-------------------------------------------------
vdbOmniscienceORDBMS::~vdbOmniscienceORDBMS()
{
}


//=============================================================================
//  required virtual functions
//=============================================================================

//-------------------------------------------------
// Returns column types suitable for use in table creation
//
const char* vdbOmniscienceORDBMS::ColumnType( const char* szType )
{
	if ( !strcmp( szType, "OID" ) )
		return "INT";
	else if ( !strcmp( szType, "DBBIT" ) )			// no support for 1 bit values
		return "SMALLINT";
	else if ( !strcmp( szType, "DBBYTE" ) )			// no support for 1 byte integers
		return "SMALLINT";
	else if ( !strcmp( szType, "DBSMALL" ) )
		return "SMALLINT";
	else if ( !strcmp( szType, "DBINT" ) )
		return "INT";
	else if ( !strcmp( szType, "DBFLOAT" ) )
		return "REAL";
	else if ( !strcmp( szType, "DBDOUBLE" ) )
		return "FLOAT";
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
		GetDatabase()->Trouble( "unidentified column type in vdbOmniscienceORDBMS::ColumnType()" ); 
		return "";
	}

}


//=============================================================================
//  public functions
//=============================================================================

//--------------------------------
// Omniscience needs to have table and column names
// in all upper case.
char* vdbOmniscienceORDBMS::CapitalizeNames( char* szString )
{
	//return strupr( szString );
	char* p = szString;
	while ( *p != 0 )
	{
		*p = toupper( *p );
		p++;
	} 
	return szString;
}
