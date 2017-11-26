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

#ifndef __vdbGeneric_h
#define __vdbGeneric_h

#ifndef __vdbTypedef_h
	#include "vdbTypedef.h"
#endif

class vdbString;
class vdbTimeStampDB;
class vdbBinaryFixed;
class vdbBinaryVar;
class vdbBinaryLong;
class vdbVarCharLong;


class vdbGeneric
{
public:
	// convert from null-terminated character array to specific type
	static void Set( char* szDest, int sizeofDest, const char* szSource );
	static void Set( bool& dest, const char* szSource );
	static void Set( OID& dest, const char* szSource );
	static void Set( DBBIT& dest, const char* szSource );
	static void Set( DBBYTE& dest, const char* szSource );
	static void Set( DBSMALL& dest, const char* szSource );
	static void Set( DBINT& dest, const char* szSource );
	static void Set( DBLONG& dest, const char* szSource );
	static void Set( DBFLOAT& dest, const char* szSource );
	static void Set( DBDOUBLE& dest, const char* szSource );
	static void Set( vdbString& sDest, const char* szSource );
	static void Set( vdbTimeStampDB& dest, const char* szSource );
	static void Set( vdbBinaryFixed& dest, const char* szSource );
	static void Set( vdbBinaryVar& dest, const char* szSource );
	static void Set( vdbBinaryLong& dest, const char* szSource );
	static void Set( vdbVarCharLong& dest, const char* szSource );

	// convert from specific variable type to null-terminated character array
	static void Set( char* szDest, int sizeofDest, OID& source );
	static void Set( char* szDest, int sizeofDest, bool& source );
	static void Set( char* szDest, int sizeofDest, DBBIT& source );
	static void Set( char* szDest, int sizeofDest, DBBYTE& source );
	static void Set( char* szDest, int sizeofDest, DBSMALL& source );
	static void Set( char* szDest, int sizeofDest, DBINT& source );
	static void Set( char* szDest, int sizeofDest, DBLONG& source );
	static void Set( char* szDest, int sizeofDest, DBFLOAT& source );
	static void Set( char* szDest, int sizeofDest, DBDOUBLE& source );
	static void Set( char* szDest, int sizeofDest, vdbString& source );
	static void Set( char* szDest, int sizeofDest, vdbTimeStampDB& source );
	static void Set( char* szDest, int sizeofDest, vdbBinaryFixed& source );
	static void Set( char* szDest, int sizeofDest, vdbBinaryVar& source );
	static void Set( char* szDest, int sizeofDest, vdbBinaryLong& source );
	static void Set( char* szDest, int sizeofDest, vdbVarCharLong& source );
};

#endif // __vdbGeneric_h

