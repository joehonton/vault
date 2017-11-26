//=============================================================================
//
// CLASS:           vdbUtility
//
// OVERVIEW:		The vdbUtility class of static member functions principally
//                  for manipulating character arrays.
//
// FILE NAME:       vdbUtility.cpp
//
// INITIAL DATE:    8/23/95
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbUtility.h"
#include "vdbAssert.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// warning C4996: '_vsnprintf': This function or variable may be unsafe. Consider using _vsnprintf_s instead.
#pragma warning( disable : 4996 )

//=============================================================================
//  String manipulation
//=============================================================================

//------------------------------------------
//^ The SafeCopy function is just like strncpy except that it puts the null
//  terminating byte at the end.
//> dest is the address of the destination string.
//> source is the address of the string to copy.
//> sizeofDest is the size of the destination string.
//
void vdbUtility::SafeCopy( TCHAR* dest, const TCHAR* source, int sizeofDest )
{
	assert( dest != 0 );
	assert( source != 0 );

	TCHAR* p = (TCHAR*) source;
	int i = 0;
	
	while ( (*p != 0x0) && ( i < sizeofDest-1 ) )
	{
		*dest = *p;
		dest++;
		p++;
		i++;
	}
	*dest = 0x0;
}


//------------------------------------------
//^ The SafeCat function is just like strncat except that it puts the null
//  terminating byte at the end.
//> dest is the address of the destination string.
//> source is the address of the string to concatenate.
//> sizeofDest is the size of the destination string.
//
void vdbUtility::SafeCat( TCHAR* dest, const TCHAR* source, int sizeofDest )
{
	assert( dest != 0 );
	assert( source != 0 );

	const TCHAR* p = dest;
	int i = 0;
	while ( (*p != 0x0) && ( i < sizeofDest-1 ) )
	{
		dest++;
		p++;
		i++;
	}

	p = (TCHAR*) source;
	while ( (*p != 0x0) && ( i < sizeofDest-1 ) )
	{
		*dest = *p;
		dest++;
		p++;
		i++;
	}
	*dest = 0x0;
}


//------------------------------------------
//^ The SafeCatWithNullTerminator function concatenates the first string with
//  the second string, keeping the null-terminating byte of both strings in
//  place, and adding a second null-terminating byte at the end.
//^ The final string looks like:  First\0Second\0\0.
//> dest is the address of the destination string.  If the first byte of dest
//  is NULL, nothing is saved.  Otherwise dest must be a string of characters
/// terminated with two consecutive NULL bytes before calling this function.
//> source is the address of the string to concatenate.
//> sizeofDest is the maximum size of the destination string.
//
void vdbUtility::SafeCatWithNullTerminator( TCHAR* dest, const TCHAR* source, int sizeofDest )
{
	assert( dest != 0 );
	assert( source != 0 );

	const TCHAR* p = dest;
	int i = 0;
	
	// if the first byte is a null, then dest has nothing that needs to be kept
	if ( *p != 0x0 )
	{
		while ( i < sizeofDest-2 ) 
		{
			if ( *p == 0x0 )
			{
				dest++;
				p++;
				i++;
				if ( *p == 0x0 )
					break;			
			}
			dest++;
			p++;
			i++;
		}
	}

	p = (TCHAR*) source;
	while ( (*p != 0x0) && ( i < sizeofDest-2 ) )
	{
		*dest = *p;
		dest++;
		p++;
		i++;
	}
	*dest = 0x0;
	dest++;
	*dest = 0x0;
}


//------------------------------------------
//^ The SafeFormat function uses the fprintf format commands to format
//  a variable number of arguments into a destination string of limited length
//> dest is the output string
//> sizeofDest is the maximum size of the output including the NULL terminator
//> szFormat is the sprintf format string
//
void vdbUtility::SafeFormat( TCHAR* dest, int sizeofDest, const TCHAR* szFormat, ... )
{
	va_list argList;
	va_start( argList, szFormat );

	assert( dest != 0 );
	assert( szFormat != 0 );

	int count = _vsntprintf( dest, sizeofDest - 1, szFormat, argList );
	
	// add null-terminating byte
	if ( count < 0 )
		dest[sizeofDest - 1] = 0x0;
	else
		dest[count] = 0x0;

	va_end( argList );
}


//------------------------------------------
//^ The GetNextEmbeddedString function retrieves the next null terminated
//  string embedded within the source character array.  The source array
//  must be terminated with double null-terminating bytes.
//> source is the address of the character array to search.
//< the return value is a pointer to the beginning of the next string after
//  the first null byte.  This pointer can be used in expressions requiring
//  a single null-terminated string.  It is also suitable for use in calling
//  this function again.  When the end of the character array is reached, as
//  signaled by the consecutive null bytes, the function returns a pointer to
//  these two null bytes.
//
TCHAR* vdbUtility::GetNextEmbeddedString( TCHAR* source )
{
	TCHAR* p = source;

	while ( *p != 0x0 )
		++p;

	return ++p;
}


//---------------------------------------
//^ The StripTrailingSpaces function removes all spaces at the
//  end of a null-terminated string.
//> szField is the null-terminated string to edit.
//> szField is also the destination for the edited field; ie. editing is done in-place.
//< The return value is a pointer to the string
//
TCHAR* vdbUtility::StripTrailingSpaces( TCHAR* szField )
{
	TCHAR *pfield = szField;

    // go to end of field
	int count = 0;
	while ( *pfield )
	{
		pfield++;
		count++;
	}

    // search from end for a non-blank
    pfield--;
	count--;
	while ( *pfield == ' ' && count >= 0 )
	{
		pfield--;
		count--;
	}

	pfield++;
	*pfield = 0x0;
	return szField;
}


//---------------------------------------
//^ The StripPrecedingSpaces function removes all spaces at the
//  beginning of a null-terminated string.
//> szField is the null-terminated string to edit.
//> szField is also the destination for the edited field; ie. editing is done in-place.
//< The return value is a pointer to the string
//
TCHAR* vdbUtility::StripPrecedingSpaces( TCHAR* szField )
{
	TCHAR *pfield;
	TCHAR *copy;

    copy = _tcsdup( szField );
    pfield = copy;
	while ( *pfield == ' ')
        pfield++;

	_tcscpy( szField, pfield );
    free( copy );
	
	return szField;
}


//---------------------------------------
//^ The ReplaceNonAlphanumeric function replaces all non-alphanumeric characters with
//  the underscore character.
//> szField is the null-terminated string to edit.
//> szField is also the destination for the edited field; ie. editing is done in-place.
//< The return value is a pointer to the modified string.
//
TCHAR* vdbUtility::ReplaceNonAlphanumeric( TCHAR* szField )
{
	TCHAR *pfield;
	pfield = szField;

	while ( *pfield != 0x0 )
	{
		if ( (*pfield >= 'a' && *pfield <= 'z') ||
		     (*pfield >= 'A' && *pfield <= 'Z') ||
			 (*pfield >= '0' && *pfield <= '9') )
			pfield++;
		else
       	{
			*pfield = '_';
			pfield++;
        }
    }
	
	return szField;
}


//---------------------------------------
//^ The ReplaceEmbeddedSpaces function replaces all spaces in the field with
//  the underscore character.
//> szField is the null-terminated string to edit.
//> szField is also the destination for the edited field; ie. editing is done in-place.
//< The return value is a pointer to the modified string.
//
TCHAR* vdbUtility::ReplaceEmbeddedSpaces( TCHAR* szField )
{
	TCHAR *pfield;
	pfield = szField;

	while ( *pfield != 0x0 )
	{
		if ( *pfield == ' ' )
			*pfield = '_';

		pfield++;
	}

	return szField;
}


//---------------------------------------
//^ The Replace function replaces all instances of a character with a different
//  character.  This function does not work if the oldChar is 0x0.
//> szField is the null-terminated string to edit.
//> szField is also the destination for the edited field; ie. editing is done in-place.
//> oldChar is the character to replace
//> newChar is the replacement value
//< The return value is a pointer to the modified string.
//
TCHAR* vdbUtility::Replace( TCHAR* szField, TCHAR oldChar, TCHAR newChar )
{
	TCHAR *p = szField;

    // loop and replace
	while ( *p != 0 )
	{
		if ( *p == oldChar ) 
			*p = newChar;
		p++;
	}

	return szField;
}


//==========================================================================================
// Memory initialization functions
//==========================================================================================

//------------------------------------------
//^ The Initialize function initializes all bytes of
//  an object to 0xcd.  This is a flag to indicate
//  an invalid/uninitialize object.
//> p is the address of the object to initialize.
//> size is the size of the object in bytes.
//
void vdbUtility::Initialize( void* p, size_t size )
{
	memset( p, 0xcd, size ); 
}


//------------------------------------------
//^ The MemCopy function copies bytes from one location to another.  If the
//  destination is smaller the trailing bytes are not transferred.  If the
//  source is smaller the extra bytes in the destination are set to zero.
//> pDest is a pointer to the destination
//> pSource is a pointer to the source
//> sizeofDest is the size of the destination
//> sizeofSource is the size of the source
//< returns the number of bytes transfered
//
int vdbUtility::MemCopy( void* pDest, void* pSource, int sizeofDest, int sizeofSource )
{
	assert( pDest != 0 );
	assert( pSource != 0 );

	// nothing to copy?
	if ( sizeofSource <= 0 )
	{
		if ( sizeofDest > 0 )
			memset( pDest, 0x00, sizeofDest );
		return 0;
	}
	
	// nothing to copy?
	if ( sizeofDest <= 0 )
		return 0;

	if ( sizeofDest == sizeofSource )
	{
		memcpy( pDest, pSource, sizeofDest );
		return sizeofDest;
	}
	else if ( sizeofDest < sizeofSource )
	{
		memcpy( pDest, pSource, sizeofDest );
		memset( (TCHAR*) pDest + sizeofSource, 0x00, sizeofSource - sizeofDest );
		return sizeofDest;
	}
	else // ( sizeofDest > sizeofSource )
	{
		memcpy( pDest, pSource, sizeofSource );
		return sizeofSource;
	}
}



//==========================================================================================
// Numeric test function
//==========================================================================================

//-------------------------------------
//^ The IsNumber function returns true if the character string is a number
//> sz is a null terminated character string with no leading zeros  
//< Returns true if sz is a numerical string of the form 
//        [+|-][digits][.digits][{d|D|e|E}[-|+]digits]
//< Returns false if not a number
//
bool vdbUtility::IsNumber (const TCHAR* sz)
{
	// s indicates which part of the field is being searched
	// i is which character in the string
	int s = 0, i = 0;
	while (1)
	{
		switch (s)
		{
			// looking for the {+|-] or a digit or a decimal point
			case 0:
			   if (sz[i] == '-' || sz[i] == '+')
					s = 1;
			   else if (isdigit (sz[i]))
					s = 2;
			   else if (sz[i] == '.')
					s = 3;
			   else
					return false;
			   break;
     		
			// looking for a digit or a decimal point
			case 1:
			   if (isdigit (sz[i]))
					s = 2;
			   else if (sz[i] == '.')
					s = 3;
			   else
					return false;
			   break;
     
			 // looking for a digit or a decimal point or the end of string
			 case 2:
			   if (isdigit (sz[i]))
					break;
			   else if (sz[i] == '.')
					s = 3;
			   else if (sz[i] == '\0')
					return true;
			   else
					return false;
			   break;
			 
			 // looking for a digit or {d|D|e|E} or the end of string  
			 case 3:
			   if (isdigit (sz[i]))
					break;
			   else if (sz[i] == 'd' || sz[i] == 'D' || sz[i] == 'e' || sz[i] == 'E')
					s = 4;
			   else if (sz[i] == '\0')
					return true;
			   else
					return false;
			   break;
			 
			 // looking for a digit or [+|-]
			 case 4:
			   if (sz[i] == '-' || sz[i] == '+')
					s = 5;
			   else if (isdigit (sz[i]))
					s = 6;
			   else
					return false;
			   break;
			 
			 // looking for a digit
			 case 5:
			   if (isdigit (sz[i]))
					s = 6;
			   else
					return false;
			   break;
			 
			 // looking for a digit or the end of string
			 case 6:
			   if (isdigit (sz[i]))
					break;
			   else if (sz[i] == '\0')
					return true;
			   else
					return false;
			   break;
		}
		i++;
	}
}  
