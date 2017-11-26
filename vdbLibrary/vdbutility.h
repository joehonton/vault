//=============================================================================
//
// CLASS:           vdbUtility
//
// OVERVIEW:		The vdbUtility class of static member functions principally
//                  for manipulating character arrays.
//
// FILE NAME:       vdbUtility.h
//
// INITIAL DATE:    8/23/95
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef __vdbUtility_h
#define __vdbUtility_h

#define TSIZEOF(exp)	( sizeof(exp)/sizeof(TCHAR) )

#ifndef _INC_TCHAR
	#include <tchar.h>
#endif

class vdbUtility
{
public:
	// string manipulation
	static void SafeCopy( TCHAR* dest, const TCHAR* source, int sizeofDest );
	static void SafeCat( TCHAR* dest, const TCHAR* source, int sizeofDest );
	static void SafeCatWithNullTerminator( TCHAR* dest, const TCHAR* source, int sizeofDest );
	static void SafeFormat( TCHAR* dest, int sizeofDest, const TCHAR* szFormat, ... );
	static TCHAR* GetNextEmbeddedString( TCHAR* source );
	static TCHAR* StripTrailingSpaces( TCHAR* szField );
	static TCHAR* StripPrecedingSpaces( TCHAR* szField );
	static TCHAR* ReplaceNonAlphanumeric( TCHAR* szField );
	static TCHAR* ReplaceEmbeddedSpaces( TCHAR* szField );
	static TCHAR* Replace( TCHAR* szField, TCHAR oldChar, TCHAR newChar );
	
	// memory initialization
	static void Initialize( void* p, size_t size );
	static int MemCopy( void* pDest, void* pSource, int sizeofDest, int sizeofSource );

	// numeric test function
	static bool IsNumber (const TCHAR* sz);
};

#endif // __vdbUtility_h

