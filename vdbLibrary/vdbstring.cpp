//=============================================================================
//
// FILE NAME:       vdbString.cpp
//
// ESSENCE:         dynamically resizeable string class
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    1/31/96
//
// COPYRIGHT:       Copyright © 1996 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbString.h"
#include "vdbException.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "vdbAssert.h"
#include <istream>
#include <ostream>

//warning C4996: '_vsnprintf': This function or variable may be unsafe. Consider using _vsnprintf_s instead.
#pragma warning( disable : 4996 )


//=============================================================================
//  Constructors and destructor
//=============================================================================

//------------------------------------------
//^^ default constructor
vdbString::vdbString()
	: _s(0),
	  _length(0)
{
}


//------------------------------------------
//^^ copy constructor
vdbString::vdbString( const vdbString& rhs )
	: _s(0),
	  _length(0)
{
	// nothing to do?
	if ( rhs._length == 0 )
		return; 

	// allocate new string
	_s = new TCHAR[rhs._length + 1];
	if ( _s == 0 ) vdbThrowMemoryException();
	
	// copy bytes
	memcpy( _s, rhs._s, (rhs._length + 1) * sizeof(TCHAR) );
	_length = rhs._length;
}


//------------------------------------------
//^^ const TCHAR* constructor
vdbString::vdbString( const TCHAR* sz )
	: _s(0),
	  _length(0)
{
	// nothing to do?
	if ( sz == 0 )
		return;
	int len = _tcslen( sz );
	if ( len == 0 )
		return;

	// allocate new string
	_s = new TCHAR[len + 1];
	if ( _s == 0 ) vdbThrowMemoryException();
	
	// copy bytes
	memcpy( _s, sz, (len + 1) * sizeof(TCHAR) );
	_length = len;
}


//------------------------------------------
//^^ char constructor
vdbString::vdbString( TCHAR ch )
	: _s(0),
	  _length(0)
{
	// nothing to do?
	if ( ch == 0 )
		return;
			
	// allocate new string
	_s = new TCHAR[2];
	if ( _s == 0 ) vdbThrowMemoryException();
	
	// copy bytes
	_s[0] = ch;
	_s[1] = 0;
	_length = 1;
}


//------------------------------------------
//^^ destructor
vdbString::~vdbString()
{
	delete[] _s;
	_s = 0;
}


//=============================================================================
//  Assignment operators
//=============================================================================

//------------------------------------------
//^^ const vdbString& assignment operator
const vdbString& vdbString::operator= ( const vdbString& rhs )
{
	// nothing to do?
	if ( this == &rhs )
		return *this;

	// delete existing string
	delete[] _s; _s = 0;

	// special case
	if ( rhs._length == 0 )
	{
		_length = 0;
		return *this;
	}

	// reallocate new string
	_s = new TCHAR[rhs._length + 1];
	if ( _s == 0 ) vdbThrowMemoryException();
	
	// copy bytes
	memcpy( _s, rhs._s, (rhs._length + 1) * sizeof(TCHAR) );
	_length = rhs._length;

	return *this;
}


//------------------------------------------
//^^ const TCHAR* assignment operator
const vdbString& vdbString::operator= ( const TCHAR* sz )
{
	// delete existing string
	delete[] _s; _s = 0;

	// empty new string?
	if ( sz == 0 )
	{
		_length = 0;
		return *this;
	}
	int len = _tcslen( sz );		
	if ( len == 0 )
	{
		_length = 0;
		return *this;
	}

	// reallocate new string
	_s = new TCHAR[len + 1];
	if ( _s == 0 ) vdbThrowMemoryException();
	
	// copy bytes
	memcpy( _s, sz, (len + 1) * sizeof(TCHAR) );
	_length = len;

	return *this;
}


//------------------------------------------
//^^ char assignment operator
const vdbString& vdbString::operator= ( TCHAR ch )
{
	// delete existing string
	delete[] _s; _s = 0;

	// reallocate new string
	_s = new TCHAR[2];
	if ( _s == 0 ) vdbThrowMemoryException();
	
	// copy bytes
	_s[0] = ch;
	_s[1] = 0;
	_length = 1;

	return *this;
}


//=============================================================================
// Concatenation operators
//=============================================================================

//------------------------------------------
//^^ const vdbString& concatenation operator
const vdbString& vdbString::operator+= ( const vdbString& rhs )
{
	// nothing to do?
	if ( rhs._length == 0 )
		return *this;

	// very little to do?
	if ( _length == 0 )
		return operator=( rhs );

	// reallocate new string
	int newLength = rhs._length + _length;
	TCHAR* szTemp = new TCHAR[newLength + 1];
	if ( szTemp == 0 ) vdbThrowMemoryException();

	// copy bytes
	memcpy( &szTemp[0], _s, (_length + 1) * sizeof(TCHAR) );
	memcpy( &szTemp[_length], rhs._s, (rhs._length + 1) * sizeof(TCHAR) );

	// delete existing string
	delete[] _s; _s = 0;

	// repositition pointer
	_s = szTemp;
	_length = newLength;

	return *this;
}


//------------------------------------------
//^^ const TCHAR* concatenation operator
const vdbString& vdbString::operator+= ( const TCHAR* sz )
{
	// nothing to do?
	if ( sz == 0 )
		return *this;
	int szLength = _tcslen( sz );
	if ( szLength == 0 )
		return *this;

	// very little to do?
	if ( _length == 0 )
		return operator=( sz );

	// reallocate new string
	int newLength = szLength + _length;
	TCHAR* szTemp = new TCHAR[newLength + 1];
	if ( szTemp == 0 ) vdbThrowMemoryException();

	// copy bytes
	memcpy( &szTemp[0], _s, (_length + 1) * sizeof(TCHAR) );
	memcpy( &szTemp[_length], sz, (szLength + 1) * sizeof(TCHAR) );

	// delete existing string
	delete[] _s; _s = 0;

	// repositition pointer
	_s = szTemp;
	_length = newLength;

	return *this;
}


//------------------------------------------
//^^ char concatenation operator
const vdbString& vdbString::operator+= ( TCHAR ch )
{
	// very little to do?
	if ( _length == 0 )
		return operator=(ch);

	// reallocate new string
	int newLength = 1 + _length;
	TCHAR* szTemp = new TCHAR[newLength + 1];
	if ( szTemp == 0 ) vdbThrowMemoryException();

	// copy bytes
	memcpy( szTemp, _s, (_length + 1) * sizeof(TCHAR) );
	szTemp[_length] = ch;
	szTemp[_length + 1] = 0;

	// delete existing string
	delete[] _s; _s = 0;

	// repositition pointer
	_s = szTemp;
	_length = newLength;

	return *this;
}
	

//=============================================================================
// Non-member concatenation operators
//=============================================================================

//------------------------------------------
//^^ const vdbString& + const vdbString& operator
vdbString operator+ ( const vdbString& string1, const vdbString& string2 )
{
	// very little to do?
	if ( string1.Length() == 0 )
		return string2;
	if ( string2.Length() == 0 )
		return string1;

	// copy first string
	vdbString newString( string1 );

	// concatenate second string
	newString += string2;

	return newString;
}


//------------------------------------------
//^^ const vdbString& + const TCHAR* operator
vdbString operator+ ( const vdbString& string, const TCHAR* sz )
{
	// very little to do?
	if ( string.Length() == 0 )
		return vdbString( sz );
	if ( sz == 0 )
		return string;
	if ( _tcslen(sz) == 0 )
		return string;

	// copy first string
	vdbString newString( string );

	// concatenate second string
	newString += sz;

	return newString;
}
	
	
//------------------------------------------
//^^ const TCHAR* + const vdbString& operator
vdbString operator+ ( const TCHAR* sz, const vdbString& string )
{
	// very little to do?
	if ( sz == 0 )
		return string;
	if ( _tcslen(sz) == 0 )
		return string;
	if ( string.Length() == 0 )
		return vdbString( sz );

	// copy first string
	vdbString newString( sz );

	// concatenate second string
	newString += string;

	return newString;
}


//------------------------------------------
//^^ const vdbString& + char operator
vdbString operator+ ( const vdbString& string, TCHAR ch )
{
	// very little to do?
	if ( string.Length() == 0 )
		return vdbString( ch );

	// copy first string
	vdbString newString( string );

	// concatenate second string
	newString += ch;

	return newString;
}


//------------------------------------------
//^^ char + const vdbString& operator
vdbString operator+ ( TCHAR ch, const vdbString& string )
{
	// very little to do?
	if ( string.Length() == 0 )
		return vdbString( ch );

	// copy first string
	vdbString newString( ch );

	// concatenate second string
	newString += string;

	return newString;
}


//=============================================================================
// Character access
//=============================================================================

//------------------------------------------
//^^ const TCHAR* cast operator
vdbString::operator const TCHAR*() const
{
	if ( _s == 0 )
		return _T("");
	else
		return _s;
}


//------------------------------------------
//^^ character access operator[]
//^ This overloaded operator[] is suitable for lefthand-side use.
//> index is the zero-based index into the string of the character to access.
//< returns the reference to the character at the zero-based index location
//  specified.
//
TCHAR& vdbString::operator[] ( int index )
{
	assert( index < _length );
	assert( index >= 0 );

	return _s[index];
}


//------------------------------------------
//^^ character access operator[]
//> index is the zero-based index into the string of the character to return.
//< returns the character at the zero-based index location specified.
//
const TCHAR& vdbString::operator[] ( int index ) const
{
	assert( index < _length );
	assert( index >= 0 );

	return _s[index];
}


//------------------------------------------
//^ The Get function returns the character at the specified index position.
//> index is the zero-based index into the string of the character to return.
//< returns the character at the zero-based index location specified.
//
TCHAR vdbString::Get( int index ) const
{
	assert( index < _length );
	assert( index >= 0 );

	return _s[index];
}


//------------------------------------------
//^ The Set function replaces the character at the specified index position.
//> index is the zero-based index into the string of the character to replace.
//
void vdbString::Set( int index, TCHAR ch )
{
	assert( index < _length );
	assert( index >= 0 );

	_s[index] = ch;
}

//------------------------------------------
//^ The Set function replaces the entire string with the contents of the passed buffer
//  Note that the buffer does not have to be null-terminated.
//> pBuffer is the address of the character string to copy from
//> length is the number of bytes to copy
//
void vdbString::Set( const TCHAR* pBuffer, int length )
{
	assert( length >= 0 );

	// delete existing string
	delete[] _s; _s = 0;

	// empty new string?
	if ( pBuffer == 0 )
	{
		_length = 0;
		return;
	}

	// reallocate new string
	_s = new TCHAR[length + 1];
	if ( _s == 0 ) vdbThrowMemoryException();

	// copy bytes
	memcpy( _s, pBuffer, length * sizeof(TCHAR) );
	_s[length] = 0x0;

	_length = length;
}


//------------------------------------------
//^ The First function returns the first part of the string up to
//  the maximum number of characters requested.
//> numChars is the maximum number of characters to copy.  If the source
//  string has fewer characters, the shorter string is returned.
//< returns the sub-string as a vdbString.
//
vdbString vdbString::First( int numChars ) const
{
	// nothing to do?
	if ( numChars <= 0 )
		return vdbString();
	
	// make a temporary copy
	int subStringLen = (numChars > _length) ? _length : numChars;
	TCHAR* szTemp = new TCHAR[subStringLen + 1];
	if ( szTemp == 0 ) vdbThrowMemoryException();
	memcpy( szTemp, _s, subStringLen * sizeof(TCHAR) );
	szTemp[subStringLen] = 0;
	
	// make it a vdbString
	vdbString subString( szTemp );
	delete[] szTemp;
	return subString;
}


//------------------------------------------
//^ The Middle function returns the middle part of the string starting at
//  the specified zero-based index position up to the maximum number of
//  characters requested.
//> index is the zero-based index of the first character to copy.
//> numChars is the maximum number of characters to copy.  If the source
//  string has fewer characters, the shorter string is returned.
//< returns the sub-string as a vdbString.
//
vdbString vdbString::Middle( int index, int numChars ) const
{
	// nothing to do?
	if ( index < 0 || index >= _length || numChars <= 0 )
		return vdbString();
	
	// make a temporary copy
	int subStringLen = ( index + numChars > _length) ? (_length - index) : numChars;
	TCHAR* szTemp = new TCHAR[subStringLen + 1];
	if ( szTemp == 0 ) vdbThrowMemoryException();
	memcpy( szTemp, &_s[index], subStringLen * sizeof(TCHAR) );
	szTemp[subStringLen] = 0;
	
	// make it a vdbString
	vdbString subString( szTemp );
	delete[] szTemp;
	return subString;
}


//------------------------------------------
//^ The Middle function returns the middle part of the string starting at
//  the specified zero-based index position up the end of the string.
//> index is the zero-based index of the first character to copy.
//< returns the sub-string as a vdbString.
//
vdbString vdbString::Middle( int index ) const
{
	// nothing to do?
	if ( index < 0 || index >= _length )
		return vdbString();

	// make a temporary copy
	int subStringLen = _length - index;
	TCHAR* szTemp = new TCHAR[subStringLen + 1];
	if ( szTemp == 0 ) vdbThrowMemoryException();
	memcpy( szTemp, &_s[index], subStringLen * sizeof(TCHAR) );
	szTemp[subStringLen] = 0;
	
	// make it a vdbString
	vdbString subString( szTemp );
	delete[] szTemp;
	return subString;
}


//------------------------------------------
//^ The Last function returns the last part of the string up to
//  the maximum number of characters requested.
//> numChars is the maximum number of characters to copy.  If the source
//  string has fewer characters, the shorter string is returned.
//< returns the sub-string as a vdbString.
//
vdbString vdbString::Last( int numChars ) const
{
	// nothing to do?
	if ( numChars <= 0 )
		return vdbString();
	
	// make a temporary copy
	int subStringLen = ( numChars > _length) ? _length : numChars;
	TCHAR* szTemp = new TCHAR[subStringLen + 1];
	if ( szTemp == 0 ) vdbThrowMemoryException();
	memcpy( szTemp, &_s[_length - subStringLen], subStringLen * sizeof(TCHAR) );
	szTemp[subStringLen] = 0;
	
	// make it a vdbString
	vdbString subString( szTemp );
	delete[] szTemp;
	return subString;
}


//=============================================================================
// Comparison operators
//=============================================================================

//------------------------------------------
//^^ const vdbString operator==
bool vdbString::operator== ( const vdbString& rhs ) const
{
	// nothing to do?
	if ( this == &rhs )
		return true;

	if( _length == 0 && rhs._length == 0 )
		return true;

	if( _length == 0  )
		return false;

	if( rhs._length == 0 )
		return false;

	return ( _tcscmp( _s, rhs._s ) ? false : true );
}


//------------------------------------------
//^^ const TCHAR* operator==
bool vdbString::operator== ( const TCHAR* sz ) const
{
	if( _length == 0 )
	{
		if ( sz == 0 )
			return true;
		else if ( sz[0] == 0x0 )
			return true;
		else
			return false;
	}

	if ( sz == 0 )
		return false;

	return ( _tcscmp( _s, sz ) ? false : true );
}



//------------------------------------------
//^^ char operator==
bool vdbString::operator== ( TCHAR ch ) const
{
	if( _length == 0 && ch == 0 )
		return true;

	if( _length == 0  )
		return false;

	if ( ch == 0 )
		return false;

	return ( ( _length == 1 && _s[0] == ch ) ? true : false );
}


//------------------------------------------
//^^ const vdbString operator!=
bool vdbString::operator!= ( const vdbString& rhs ) const
{
	// nothing to do?
	if ( this == &rhs )
		return false;

	if( _length == 0 && rhs._length == 0 )
		return false;

	if( _length == 0  )
		return true;

	if( rhs._length == 0 )
		return true;

	return ( _tcscmp( _s, rhs._s ) ? true : false );
}


//------------------------------------------
//^^ const TCHAR* operator!=
bool vdbString::operator!= ( const TCHAR* sz ) const
{
	if( _length == 0 )
	{
		if ( sz == 0 )
			return false;
		else if ( sz[0] == 0x0 )
			return false;
		else
			return true;
	}

	if ( sz == 0 )
		return true;

	return ( _tcscmp( _s, sz ) ? true : false );
}



//------------------------------------------
//^^ char operator!=
bool vdbString::operator!= ( TCHAR ch ) const
{
	if( _length == 0 && ch == 0 )
		return false;

	if( _length == 0  )
		return true;

	if ( ch == 0 )
		return true;

	return ( ( _length == 1 && _s[0] == ch ) ? false : true );
}


//------------------------------------------
//^^ vdbString operator<
bool vdbString::operator< ( const vdbString& rhs )
{
	// nothing to do?
	if ( this == &rhs )
		return false;

	if( _length == 0 && rhs._length == 0 )
		return false;

	if( _length == 0  )
		return true;

	if( rhs._length == 0 )
		return false;

	return ( ( _tcscmp( _s, rhs._s ) < 0 ) ? true : false );
}


//------------------------------------------
//^^ const TCHAR* operator<
bool vdbString::operator< ( const TCHAR* sz ) const
{
	if( _length == 0 && sz == 0 )
		return false;

	if( _length == 0  )
		return true;

	if ( sz == 0 )
		return false;

	return ( ( _tcscmp( _s, sz ) < 0 ) ? true : false );
}


//------------------------------------------
//^^ char operator<
bool vdbString::operator< ( TCHAR ch ) const
{
	if( _length == 0 && ch == 0 )
		return false;

	if( _length == 0  )
		return true;

	if ( ch == 0 )
		return false;

	TCHAR temp[2];
	temp[0] = ch;
	temp[1] = 0;
	return ( ( _tcscmp( _s, temp ) < 0 ) ? true : false );
}


//------------------------------------------
//^^ vdbString operator>
bool vdbString::operator> ( const vdbString& rhs )
{
	// nothing to do?
	if ( this == &rhs )
		return false;

	if( _length == 0 && rhs._length == 0 )
		return false;

	if( _length == 0  )
		return false;

	if( rhs._length == 0 )
		return true;

	return ( ( _tcscmp( _s, rhs._s ) > 0 ) ? true : false );
}


//------------------------------------------
//^^ const TCHAR* operator>
bool vdbString::operator> ( const TCHAR* sz ) const
{
	if( _length == 0 && sz == 0 )
		return false;

	if( _length == 0  )
		return false;

	if ( sz == 0 )
		return true;

	return ( ( _tcscmp( _s, sz ) > 0 ) ? true : false );
}


//------------------------------------------
//^^ char operator>
bool vdbString::operator> ( TCHAR ch ) const
{
	if( _length == 0 && ch == 0 )
		return false;

	if( _length == 0  )
		return false;

	if ( ch == 0 )
		return true;

	TCHAR temp[2];
	temp[0] = ch;
	temp[1] = 0;
	return ( ( _tcscmp( _s, temp ) > 0 ) ? true : false );
}


//------------------------------------------
//^^ Compare const vdbString
//> numChars is the maximum number of characters to compare.  If this value
//  is zero, the entire string is compared.
bool vdbString::Compare( const vdbString& rhs, int numChars ) const
{
	assert( numChars >= 0 );

	// nothing to do?
	if ( this == &rhs )
		return true;

	if( _length == 0 && rhs._length == 0 )
		return true;

	if( _length == 0  )
		return false;

	if ( numChars == 0 )
		return ( _tcscmp( _s, rhs._s ) ? false : true );
	else
		return ( _tcsncmp( _s, rhs._s, numChars ) ? false : true );
}


//------------------------------------------
//^^ Compare const TCHAR*
//> numChars is the maximum number of characters to compare.  If this value
//  is zero, the entire string is compared.
bool vdbString::Compare( const TCHAR* sz, int numChars ) const
{
	assert( numChars >= 0 );

	if( _length == 0 && sz == 0 )
		return true;

	if( _length == 0  )
		return false;

	if ( sz == 0 )
		return false;

	if ( numChars == 0 )
		return ( _tcscmp( _s, sz ) ? false : true );
	else
		return ( _tcsncmp( _s, sz, numChars ) ? false : true );
}



//------------------------------------------
//^^ CompareNoCase const vdbString
//> numChars is the maximum number of characters to compare.  If this value
//  is zero, the entire string is compared.
bool vdbString::CompareNoCase( const vdbString& rhs, int numChars ) const
{
	assert( numChars >= 0 );

	// nothing to do?
	if ( this == &rhs )
		return true;

	if( _length == 0 && rhs._length == 0 )
		return true;

	if( _length == 0  )
		return false;

	if( rhs._length == 0  )
		return false;

	if ( numChars == 0 )
		return ( _tcsicmp( _s, rhs._s ) ? false : true );
	else
		return ( _tcsnicmp( _s, rhs._s, numChars ) ? false : true );
}


//------------------------------------------
//^^ CompareNoCase const TCHAR*
//> numChars is the maximum number of characters to compare.  If this value
//  is zero, the entire string is compared.
bool vdbString::CompareNoCase( const TCHAR* sz, int numChars ) const
{
	assert( numChars >= 0 );

	if( _length == 0 && sz == 0 )
		return true;

	if( _length == 0  )
		return false;

	if ( sz == 0 )
		return false;

	if ( numChars == 0 )
		return ( _tcsicmp( _s, sz ) ? false : true );
	else
		return ( _tcsnicmp( _s, sz, numChars ) ? false : true );
}



//------------------------------------------
//^^ CompareNoCase char
bool vdbString::CompareNoCase( TCHAR ch ) const
{
	if ( ch == 0 )
		return false;
	if ( _length != 1 )
		return false;

	char c1 = tolower( _s[0] );
	char c2 = tolower(ch);
	return ( ( c1 == c2 ) ? true : false );
}


//-------------------------------------
//^ The TypeOfCapitalization function determines the type of capitalization for the string
vdbString::capitalization vdbString::TypeOfCapitalization()
{
	int countUpper = 0;
	int countLower = 0;
	int countOther = 0;

	if ( _length == 0 )
		return IS_NULL;

	for ( int i = 0; i < _length; i++ )
	{
		if ( _s[i] >= 'A' && _s[i] <= 'Z' )
			countUpper++;
		else if ( _s[i] >= 'a' && _s[i] <= 'z' )
			countLower++;
		else
			countOther++;
	}


	if ( _s[0] >= 'A' && _s[0] <= 'Z' )
	{
		if ( countUpper == 1 )
			return FIRST_LETTER_ONLY_CAPS;		// <-- Samplecase or Sample-case
		
		else if ( countUpper + countOther == _length )
			return ALL_UPPERCASE;				// <-- SAMPLECASE or SAMPLE-CASE
		
		else
			return MIXED_CAPS;					// <-- SampleCase or Sample-Case 
	}

	else if ( _s[0] >= 'a' && _s[0] <= 'z' ) 
	{
		if ( countLower + countOther == _length )
			return ALL_LOWERCASE;				// <-- samplecase or sample-case
		else
			return MIXED_CAPS;					// <-- sampleCase or Sample-Case 
	}

	else
	{
		if ( countLower + countOther == _length )
			return ALL_LOWERCASE;				// <-- -samplecase or -sample-case
		else
			return MIXED_CAPS;					// <-- -sampleCase or -Sample-Case 
	}
}


//=============================================================================
// Information functions
//=============================================================================

//------------------------------------------
//^ The Length function returns the length of the string not counting the
//  null terminating byte.
//
int vdbString::Length() const
{
	return _length;
}


//------------------------------------------
//^ The IsNull function returns true if this object contains no string.  It returns
//  false if this object does contain a string.
//
bool vdbString::IsNull() const
{
	return _length == 0 ? true : false;
}


//------------------------------------------
//^ The IsNotNull function returns true if this object contains a string.  It returns
//  false if this object does not contain a string.
//
bool vdbString::IsNotNull() const
{
	return _length != 0 ? true : false;
}


//=============================================================================
// String modification routines
//=============================================================================

//------------------------------------------
//^ The ToUpper function converts all characters to their uppercase equivalent.
void vdbString::ToUpper()
{
	for ( int i = 0; i < _length; i++ )
		_s[i] = toupper( _s[i] );
}


//------------------------------------------
//^ The ToLower function converts all characters to their lowercase equivalent.
void vdbString::ToLower()
{
	for ( int i = 0; i < _length; i++ )
		_s[i] = tolower( _s[i] );
}


//------------------------------------------
//^ The StripTrailingSpaces function removes spaces from the end of the string.
void vdbString::StripTrailingSpaces()
{
    // nothing to do?
    if ( _length == 0 )
		return;

    // go to end of string
	TCHAR* p = &_s[_length - 1];

    // search from end for a non-blank
	int position = _length - 1;
	while ( *p == ' ' && position > 0 )
	{
		p--;
		position--;
	}

	// add null terminator just after the last non-space
	p++;
	*p = 0x0;
	
	_length = position + 1;

	// leave over-allocation at end of string for destructor to take care of
}


//------------------------------------------
//^ The StripLeadingSpaces function removes spaces from the beginning of the
//  string.
void vdbString::StripLeadingSpaces()
{
    // nothing to do?
    if ( _length == 0 )
		return;

    // start at beginning of string
	TCHAR* p = _s;

    // search from beginning for a non-blank
	int position = 0;
	while ( *p == ' ' && position < _length )
	{
		p++;
		position++;
	}

    // nothing to do?
	if ( position == 0 )
		return;

	// shift the characters to left
	_length = _length - position;
	memmove( _s, &_s[position], _length + 1 );
}


//------------------------------------------
//^ The StripTrailingWhitespace function removes spaces, tabs, and newlines
//  from the end of the string.
void vdbString::StripTrailingWhitespace()
{
    // nothing to do?
    if ( _length == 0 )
		return;

    // go to end of string
	TCHAR* p = &_s[_length - 1];

    // search from end for a non-blank
	int position = _length - 1;
	while ( ( *p == ' ' || *p == '\t' || *p == '\n' ) && position > 0 )
	{
		p--;
		position--;
	}

	// add null terminator just after the last non-space
	p++;
	*p = 0x0;
	
	_length = position + 1;

	// leave over-allocation at end of string for destructor to take care of
}


//------------------------------------------
//^ The StripLeadingWhitespace function removes spaces, tabs and newlines
//  from the beginning of the string.
void vdbString::StripLeadingWhitespace()
{
    // nothing to do?
    if ( _length == 0 )
		return;

    // start at beginning of string
	TCHAR* p = _s;

    // search from beginning for a non-blank
	int position = 0;
	while ( ( *p == ' ' || *p == '\t' || *p == '\n' ) && position < _length )
	{
		p++;
		position++;
	}

    // nothing to do?
	if ( position == 0 )
		return;

	// shift the characters to left
	_length = _length - position;
	memmove( _s, &_s[position], _length + 1 );
}


//------------------------------------------
//^ The Replace function replaces every occurance of the first character 
//  with the second character.
//^ Caution:  Visual C++ 5.0 debug builds cause an assertion upon deletion
//  of this string when the replacement value is '\0';  
//> oldValue is the character to replace.
//> newValue is the character to replace it with.
//
void vdbString::Replace( TCHAR oldValue, TCHAR newValue )
{
	for ( int i = 0; i < _length; i++ )
		if ( _s[i] == oldValue )
			_s[i] = newValue;
}


//=============================================================================
// Sub-string matching
//=============================================================================

//------------------------------------------
//^ The Find function finds the first character in the string matching the
//  specified character.
//> ch is the character to search for.
//< The return value is the zero-based index of the first character found,
//  or -1 if no such character was found.
//
int vdbString::Find( TCHAR ch ) const
{
    // nothing to do?
    if ( _length == 0 )
		return - 1;

	// find first single character
	TCHAR* p = _tcschr( _s, ch );

	if ( p == NULL )
		return -1;

	return (int)(p - _s);
}


//------------------------------------------
//^ The FindNext function finds the first matching character after the starting
//  position.
//> ch is the character to search for.
//> startingPosition is the zero-based index of the first character to begin
//  searching with.
//< The return value is the zero-based index of the first character found,
//  or -1 if no such character was found.
//
int vdbString::FindNext( TCHAR ch, int startingPosition ) const
{
    // nothing to do?
    if ( _length == 0 )
		return - 1;

    if ( startingPosition >= _length )
		return - 1;

    if ( startingPosition < 0 )
		return - 1;

	// find first single character
	TCHAR* p = _tcschr( &_s[startingPosition], ch );

	if ( p == NULL )
		return -1;

	return (int)(p - _s);
}


//------------------------------------------
//^ The Find function finds the first occurence of the given substring within
//  this string.
//> sz is the substring to search for
//< The return value is the zero-based index of the substring's position
//  within this string or -1 if no such substring was found.
//
int vdbString::Find( const TCHAR* sz ) const
{
    // nothing to do?
    if ( _length == 0 )
		return - 1;

	TCHAR* p = _tcsstr( _s, sz );

	if ( p == NULL )
		return -1;

	return (int)(p - _s);
}


//------------------------------------------
//^ The FindNext function finds the next occurence of the given substring within
//  this string starting at the given position.
//> sz is the substring to search for
//< The return value is the zero-based index of the substring's position
//  within this string or -1 if no such substring was found.
//
int vdbString::FindNext( const TCHAR* sz, int startingPosition ) const
{
    // nothing to do?
    if ( _length == 0 )
		return - 1;

    if ( startingPosition >= _length )
		return - 1;

    if ( startingPosition < 0 )
		return - 1;

	TCHAR* p = _tcsstr( &_s[startingPosition], sz );

	if ( p == NULL )
		return -1;

	return (int)(p - _s);
}


//------------------------------------------
//^ The ReverseFind function finds the last character in the string matching the
//  specified character.
//> ch is the character to search for.
//< The return value is the zero-based index of the last character found,
//  or -1 if no such character was found.
//
int vdbString::ReverseFind( TCHAR ch ) const
{
    // nothing to do?
    if ( _length == 0 )
		return - 1;

	// find last single character
	TCHAR* p = _tcsrchr( _s, ch );

	if ( p == NULL )
		return -1;

	return (int)(p - _s);
}


//------------------------------------------
//^ The ReverseFindNext function finds the last character in the string, before
//  the ending position, matching the specified character.
//> ch is the character to search for.
//> endingPosition is the zero-based index of the character to begin
//  searching backwards with.
//< The return value is the zero-based index of the last character found,
//  or -1 if no such character was found.
//
int vdbString::ReverseFindNext( TCHAR ch, int endingPosition ) const
{
    // nothing to do?
    if ( _length == 0 )
		return - 1;

    if ( endingPosition >= _length )
		return - 1;

    if ( endingPosition < 0 )
		return - 1;

	// use a temp string
	vdbString s( this->First( endingPosition + 1) );
	return s.ReverseFind( ch );
}


//------------------------------------------
//^ The FindOneOf function finds the index of the first character in the string
//  matching any of the specified characters.
//> szCharSet is the set of characters to search for.
//< The return value is the zero-based index of the first character found,
//  or -1 if none of the characters were found.
//
int vdbString::FindOneOf( const TCHAR* szCharSet ) const
{
    // nothing to do?
    if ( _length == 0 )
		return - 1;

	// find first character in set
	int found = _tcscspn( _s, szCharSet );
	return ( found < _length ) ? found : -1;
}


//------------------------------------------
//^ The FindOneOfNext function finds the index of the first character, after
//  the starting position, matching any of the specified characters.
//> szCharSet is the set of characters to search for.
//> startingPosition is the zero-based index of the first character to begin
//  searching with.
//< The return value is the zero-based index of the first character found,
//  or -1 if no such character was found.
//
int vdbString::FindOneOfNext( const TCHAR* szCharSet, int startingPosition ) const
{
    // nothing to do?
    if ( _length == 0 )
		return - 1;

    if ( startingPosition >= _length )
		return - 1;

    if ( startingPosition < 0 )
		return - 1;

	// find first character in set
	int found = _tcscspn( &_s[startingPosition], szCharSet );

	if ( startingPosition + found >= _length )
		return -1;
	else
		return startingPosition + found;
}


//------------------------------------------
//^ The FindInverseOf function finds the index of the first character in the string
//  not matching any of the specified characters.
//> szCharSet is the set of characters to search for.
//< The return value is the zero-based index of the first character found which is
//  not in the supplied character set.
//  Returns -1 only if the string is empty or the characer set is empty
//
int vdbString::FindInverseOf( const TCHAR* szCharSet ) const
{
    // nothing to do?
    if ( _length == 0 )
		return - 1;

	if ( szCharSet[0] == 0x0 )
		return -1;

	// find first character not in set
	int found = _tcsspn( _s, szCharSet );
	return ( found < _length ) ? found : -1;
}


//------------------------------------------
//^ The FindInverseOfNext function finds the index of the first character, after
//  the starting position, not matching any of the specified characters.
//> szCharSet is the set of characters to search for.
//> startingPosition is the zero-based index of the first character to begin
//  searching with.
//< The return value is the zero-based index of the first character found which is
//  not in the supplied character set.
//  Returns -1 only if the string is empty or the characer set is empty
//
int vdbString::FindInverseOfNext( const TCHAR* szCharSet, int startingPosition ) const
{
    // nothing to do?
    if ( _length == 0 )
		return - 1;

	if ( szCharSet[0] == 0x0 )
		return -1;

    if ( startingPosition >= _length )
		return - 1;

    if ( startingPosition < 0 )
		return - 1;

	// find first character in set
	int found = _tcsspn( &_s[startingPosition], szCharSet );

	if ( startingPosition + found >= _length )
		return -1;
	else
		return startingPosition + found;
}


//=============================================================================
// String formatting
//=============================================================================

//------------------------------------------
//^ The Format function formats the variable list of arguments according to the
//  format string using printf-style formatting commands.
//^ At most 1000 characters can be written.
//
void vdbString::Format( const TCHAR* szFormat, ... )
{
	va_list argList;
	va_start( argList, szFormat );

	// delete existing string
	delete[] _s; _s = 0; _length = 0;

	// nothing to do?
	if ( szFormat == 0 )
		return;
	if ( _tcslen(szFormat) == 0 )
		return;

	TCHAR* szTemp = new TCHAR[1001];
	if ( szTemp == 0 ) vdbThrowMemoryException();

	// format
	int count = _vsntprintf( szTemp, 1000, szFormat, argList );
	
	// add null-terminating byte
	( count == -1 ) ? _length = 1000 : _length = count;
	szTemp[_length] = 0x0;

	// reassign
	_s = szTemp;
	va_end( argList );
}


#ifndef UNICODE

//=============================================================================
//  iostream functions
//=============================================================================

//------------------------------------------
//^ The Put function writes the string to the output stream with NULL separators
//  between the length and the string itself
//^ This function is suitable for binary files
//^ This was written for vdbSetOfServers::ReadFromArchive
//
void vdbString::Put( std::ostream& os ) const
{
	int len = Length();
	os << '\0';
	os << len;
	os << '\0';
	if ( len > 0 )
		os.write( (const TCHAR*) *this, len );
	os << '\0';
}


//------------------------------------------
//^ The Get function expects a separator character followed by the
//  length of the string followed by a second separator character
//  followed by the string itself and terminated with a final separator.
//^ This function is suitable for binary files
//^ This was written for vdbSetOfServers::ReadFromArchive
//
void vdbString::Get( std::istream& is )
{
	int len;
	char separator;

	if ( is.fail() )
		return;

	is >> separator;
	is >> len;
	is >> separator;
	
	// no more than 1 MB please
	if ( len > 1024 * 1024 * 1024 )
	{
		is.clear(std::ios_base::badbit);		
		return;
	}

	// read em in
	if ( len > 0 )
	{
		TCHAR* szTemp = new TCHAR[len + 1];
		if ( szTemp == 0 ) vdbThrowMemoryException();
		is.read( szTemp, len );
		szTemp[len] = 0;
		*this = szTemp;
		delete[] szTemp; szTemp = 0;
	}
	else
		*this = _T("");

	is >> separator;
}


//------------------------------------------
//^ The write function writes the string to a binary output file
//  by writing the length of the string to the first 1, 2, or 4 byte area
//  followed by the actual string itself, without any null terminator
//> os is the output stream opened with ios::binary
//> sizeofLength is 1, 2 or 4 to specify the number of bytes occupied in
//  representing the length of the string.
//
void vdbString::write( std::ostream& os, const int sizeofLength ) const
{
	if ( sizeofLength == 1 )
	{
		char len = static_cast<char>( _length );
		os.write( &len, 1 );
		os.write( _s, len );
	} 
	else if ( sizeofLength == 2 )
	{
		unsigned short int len = static_cast<unsigned short int>( _length );
		os.write( (const char*) &len, 2 );
		os.write( _s, len );
	} 
	else if ( sizeofLength == 4 )
	{
		unsigned int len = static_cast<unsigned int>( _length );
		os.write( (const char*) &len, 4 );
		os.write( _s, len );
	} 
	else
		assert( false );
}


//------------------------------------------
void vdbString::read( std::istream& is, const int sizeofLength )
{
	if ( sizeofLength == 1 )
	{
		char len;
		is.read( &len, 1 );
		_length = len;
	}
	else if ( sizeofLength == 2 )
	{
		unsigned short int len;
		is.read( (char*) &len, 2 );
		_length = len;
	}
	else if ( sizeofLength == 4 )
	{
		unsigned int len;
		is.read( (char*) &len, 4 );
		_length = len;
	}
	else
		assert( false );
		

	// delete existing string
	delete[] _s; _s = 0;

	// special case
	if ( _length == 0 )
		return;

	// reallocate new string
	_s = new TCHAR[_length + 1];
	if ( _s == 0 ) vdbThrowMemoryException();

	// read 'em in
	is.read( _s, _length );
	_s[_length] = 0x0;
}


//------------------------------------------
//^^ ostream << operator writes the string to the output stream and follows it
//  with an end of line character.  If the string is empty, the special "<empty>"
//  string is output in its place so that the istream::ws function doesn't eat it.
//^ This function is suitable for text files
//^ This was written for rgaLibrary
//
std::ostream& operator<< ( std::ostream& os, const vdbString& obj )
{
	if( obj.Length() == 0 )
		os << "<empty>" << std::endl;
	else
		os << (const TCHAR*) obj << std::endl;
	
	return os;
}


//------------------------------------------
//^^ istream >> operator reads the string and the end of line character from
//  the input stream.  If the special "<empty>" string is found, this object
//  is assigned nothing.
//^ This function is suitable for text files
//^ This was written for rgaLibrary
//
std::istream& operator>> ( std::istream& is, vdbString& obj )
{
	const int bufSize = 80;
	TCHAR szBuffer[bufSize];

	is.get( szBuffer, bufSize, '\n' );
	obj = szBuffer;

	while ( is.gcount() == bufSize - 1 )
	{
		is.get( szBuffer, bufSize, '\n' );
		obj += szBuffer;
	}
	is >> std::ws;

	if( obj == "<empty>" )
		obj = _T("");

	return is;
}

#endif // UNICODE

