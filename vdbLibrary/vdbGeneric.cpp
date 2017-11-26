//=============================================================================
//
// CLASS:           vdbGeneric
//
// OVERVIEW:		The vdbGeneric class of static member functions is used
//                  to conveniently convert variables to and from string
//                  representations.
//
// FILE NAME:       vdbGeneric.cpp
//
// INITIAL DATE:    2/3/01
//
// COPYRIGHT:		Copyright © 2001 by Joe Honton
//
//=============================================================================

#include "vdbGeneric.h"
#include "vdbAssert.h"
#include "vdbString.h"
#include "vdbTimeStampDB.h"
#include "vdbBinaryVar.h"
#include "vdbBinaryFixed.h"
#include "vdbBinaryLong.h"
#include "vdbVarCharLong.h"
#include "vdbUtility.h"

// warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
#pragma warning( disable : 4800 )

//warning C4996: 'sscanf': This function or variable may be unsafe. Consider using sscanf_s instead. 
#pragma warning( disable : 4996 )

//=============================================================================
// convert from null-terminated character array to specific type
//=============================================================================

//-------------------------------------
void vdbGeneric::Set( char* szDest, int sizeofDest, const char* szSource )
{
	vdbUtility::SafeCopy( szDest, szSource, sizeofDest );
}


//-------------------------------------
void vdbGeneric::Set( bool& dest, const char* szSource )
{
	dest = (bool) atoi( szSource );
}


//-------------------------------------
void vdbGeneric::Set( OID& dest, const char* szSource )
{
	dest = atoi( szSource );
}


//-------------------------------------
void vdbGeneric::Set( DBBIT& dest, const char* szSource )
{
	dest = atoi( szSource );
}


//-------------------------------------
void vdbGeneric::Set( DBBYTE& dest, const char* szSource )
{
	dest = atoi( szSource );
}


//-------------------------------------
void vdbGeneric::Set( DBSMALL& dest, const char* szSource )
{
	dest = atoi( szSource );
}


//-------------------------------------
void vdbGeneric::Set( DBINT& dest, const char* szSource )
{
	dest = atoi( szSource );
}


//-------------------------------------
void vdbGeneric::Set( DBLONG& dest, const char* szSource )
{
	dest = atoi( szSource );
}


//-------------------------------------
void vdbGeneric::Set( DBFLOAT& dest, const char* szSource )
{
	dest = static_cast<DBFLOAT>( atof( szSource ) );
}


//-------------------------------------
void vdbGeneric::Set( DBDOUBLE& dest, const char* szSource )
{
	dest = static_cast<DBFLOAT>( atof( szSource ) );
}


//-------------------------------------
void vdbGeneric::Set( vdbString& sDest, const char* szSource )
{
	sDest = szSource;
}


//-------------------------------------
//> szSource should be in the form: "%Y-%m-%d %H:%M:%S"
void vdbGeneric::Set( vdbTimeStampDB& dest, const char* szSource )
{
	dest.SetFromDatabaseString( szSource );
}


//-------------------------------------
//> szSource should be a string of Hex characters where each two characters
//  is translated into a single byte in the destination.  The entire string
//  is null terminated.
//> If the source does not have enough hex characters, pad the trailing bytes
//  of the fixed length string with zeroes.  If the source has too many hex
//  characters, truncate it to the length of the destination.
//
void vdbGeneric::Set( vdbBinaryFixed& dest, const char* szSource )
{
	int binaryLength = strlen( szSource ) / 2;
	int destLength = dest.MaxLength();
	int minLength = min( binaryLength, destLength );
	
	// convert from hex
	char sz[3];
	int c;
	int offset;
	int i;
	for ( i = 0; i < minLength; i++ )
	{
		offset = i*2;
		sz[0] = szSource[offset];
		sz[1] = szSource[offset+1];
		sscanf( sz, "%x", &c );
		dest[i] = c;
	}
	
	// pad any remainder with zeros
	for ( ; i < destLength; i++ )
		dest[i] = 0x0;
}


//-------------------------------------
//> szSource should be a string of Hex characters where each two characters
//  is translated into a single byte in the destination.  The entire string
//  is null terminated.
//> The length of the destination is reset to be the length of the source.
void vdbGeneric::Set( vdbBinaryVar& dest, const char* szSource )
{
	int binaryLength = strlen( szSource ) / 2;
	dest.MaxLength( binaryLength );

	// convert from hex
	char sz[3];
	int c;
	int offset;
	for ( int i = 0; i < binaryLength; i++ )
	{
		offset = i*2;
		sz[0] = szSource[offset];
		sz[1] = szSource[offset+1];
		sscanf( sz, "%x", &c );
		dest[i] = c;
	}
}


//-------------------------------------
//> szSource should be a string of Hex characters where each two characters
//  is translated into a single byte in the destination.  The entire string
//  is null terminated.
//> The length of the destination is reset to be the length of the source.
void vdbGeneric::Set( vdbBinaryLong& dest, const char* szSource )
{
	int binaryLength = strlen( szSource ) / 2;
	dest.Initialize( binaryLength );

	// convert from hex
	char sz[3];
	int c;
	int offset;
	for ( int i = 0; i < binaryLength; i++ )
	{
		offset = i*2;
		sz[0] = szSource[offset];
		sz[1] = szSource[offset+1];
		sscanf( sz, "%x", &c );
		dest[i] = c;
	}
}


//-------------------------------------
void vdbGeneric::Set( vdbVarCharLong& dest, const char* szSource )
{
	dest.SetData( szSource );
}


//=============================================================================
// convert from specific variable type to null-terminated character array
//=============================================================================

//-------------------------------------
void vdbGeneric::Set( char* szDest, int sizeofDest, bool& source )
{
	_snprintf( szDest, sizeofDest - 1, "%d", (int) source );
}


//-------------------------------------
void vdbGeneric::Set( char* szDest, int sizeofDest, OID& source )
{
	_snprintf( szDest, sizeofDest - 1, "%d", source );
}


//-------------------------------------
void vdbGeneric::Set( char* szDest, int sizeofDest, DBBIT& source )
{
	_snprintf( szDest, sizeofDest - 1, "%d", source );
}


//-------------------------------------
void vdbGeneric::Set( char* szDest, int sizeofDest, DBBYTE& source )
{
	_snprintf( szDest, sizeofDest - 1, "%d", source );
}


//-------------------------------------
void vdbGeneric::Set( char* szDest, int sizeofDest, DBSMALL& source )
{
	_snprintf( szDest, sizeofDest - 1, "%d", source );
}


//-------------------------------------
void vdbGeneric::Set( char* szDest, int sizeofDest, DBINT& source )
{
	_snprintf( szDest, sizeofDest - 1, "%d", source );
}


//-------------------------------------
void vdbGeneric::Set( char* szDest, int sizeofDest, DBLONG& source )
{
	_snprintf( szDest, sizeofDest - 1, "%d", source );
}


//-------------------------------------
void vdbGeneric::Set( char* szDest, int sizeofDest, DBFLOAT& source )
{
	_snprintf( szDest, sizeofDest - 1, "%f", source );
}


//-------------------------------------
void vdbGeneric::Set( char* szDest, int sizeofDest, DBDOUBLE& source )
{
	_snprintf( szDest, sizeofDest - 1, "%f", source );
}


//-------------------------------------
void vdbGeneric::Set( char* szDest, int sizeofDest, vdbString& source )
{
	vdbUtility::SafeCopy( szDest, (const char*) source, sizeofDest );
}


//-------------------------------------
void vdbGeneric::Set( char* szDest, int sizeofDest, vdbTimeStampDB& source )
{
	source.Format( szDest, sizeofDest, "%Y-%m-%d %H:%M:%S" );
}


//-------------------------------------
void vdbGeneric::Set( char* szDest, int sizeofDest, vdbBinaryFixed& source )
{
	int limit = min( source.ActualLength(), (sizeofDest - 1) / 2 );
	
	// convert to hex
	for ( int i = 0; i < limit; i++ )
		sprintf( &szDest[i*2], "%x", source[i] );

	szDest[limit*2] = 0x0;
}


//-------------------------------------
void vdbGeneric::Set( char* szDest, int sizeofDest, vdbBinaryVar& source )
{
	int limit = min( source.ActualLength(), (sizeofDest - 1) / 2 );
	
	// convert to hex
	for ( int i = 0; i < limit; i++ )
		sprintf( &szDest[i*2], "%x", source[i] );

	szDest[limit*2] = 0x0;
}


//-------------------------------------
void vdbGeneric::Set( char* szDest, int sizeofDest, vdbBinaryLong& source )
{
	int limit = min( source.ActualLength(), (sizeofDest - 1) / 2 );
	
	// convert to hex
	for ( int i = 0; i < limit; i++ )
		sprintf( &szDest[i*2], "%x", source[i] );

	szDest[limit*2] = 0x0;
}


//-------------------------------------
void vdbGeneric::Set( char* szDest, int sizeofDest, vdbVarCharLong& source )
{
	vdbUtility::SafeCopy( szDest, (const char*) source, sizeofDest );
}
