//=============================================================================
//
// FILE NAME:       vdbVarCharLong.cpp
//
// ESSENCE:         A variably-sized string object with no pre-set maximum size
//                  for use by a vdbObject-derived class containing a column
//                  of type "long varchar".
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    12/6/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#include <vdbWindows.h>
#include "vdbVarCharLong.h"
#include "vdbException.h"


//=============================================================================
//  Essential class functions
//=============================================================================

//------------------------------------------
//^^ default constructor
vdbVarCharLong::vdbVarCharLong()
	: vdbString(),
	  _bModified ( false ),
	  _bInMemory( false )
{
}


//------------------------------------------
//^^ simple constructor
vdbVarCharLong::vdbVarCharLong( long maxLength )
	: vdbString(),
	  _bModified ( false ),
	  _bInMemory( false )
{
	//TODO maxLength?
}


//------------------------------------------
//^^ constructor with initialization
vdbVarCharLong::vdbVarCharLong( void* pData, long dataLength, long maxLength )
	: vdbString(),
	  _bModified ( false ),
	  _bInMemory( false )
{
	//TODO args?
}


//------------------------------------------
//^^ destructor
vdbVarCharLong::~vdbVarCharLong()
{
}


//------------------------------------------
//^^ copy constructor
vdbVarCharLong::vdbVarCharLong( const vdbVarCharLong& rhs )
	: vdbString( (vdbString&) rhs ),
	  _bModified ( rhs._bModified ),
	  _bInMemory( rhs._bInMemory )
{
}


//------------------------------------------
//^^ const vdbVarCharLong& assignment operator
const vdbVarCharLong& vdbVarCharLong::operator= ( const vdbVarCharLong& rhs )
{
	// nothing to do?
	if ( this == &rhs )
		return *this;

	vdbString::operator=( rhs );
	_bModified  = rhs._bModified;
	_bInMemory = rhs._bInMemory;

	return *this;
}


//------------------------------------------
//^^ const char* cast operator
vdbVarCharLong::operator const char*() const
{
	if ( _s == 0 )
		return "";
	else
		return _s;
}


//------------------------------------------
//^ The GetDataPointer function returns a pointer to the actual storage location
//  of the string data.
//^ Caution this will return NULL if the actual length is 0
//
char* vdbVarCharLong::GetDataPointer()
{
	return _s;
}


//------------------------------------------
void vdbVarCharLong::SetData( const char* sz )
{
	vdbString::operator=( sz );
	_bModified = true;
	_bInMemory = true;
}


//------------------------------------------
//^ The Append function adds the character data to the end of
// the existing string.
//> sz is the pointer to the character data.  It does not have to be NULL-terminated.
//> len is the actual length of the character data not including any NULL-termination byte.
//
void vdbVarCharLong::Append( const char* sz, int len )
{
	// nothing to do?
	if ( sz == NULL || len == 0 )
		return;

	// very little to do
	if ( _length == 0 && _s == 0 )
	{
		_s = new char[len + 1];
		if ( _s == 0 ) throw vdbMemoryException();
		memcpy( _s, sz, len );
		_s[len] = 0x0;
		_length = len;

		_bModified = true;
		_bInMemory = true;
		return;
	}

	// allocate buffer for old plus new
	int newLength = _length + len;
	char* szTemp = new char[newLength + 1];
	if ( szTemp == 0 ) throw vdbMemoryException();

	// copy bytes
	memcpy( &szTemp[0], _s, _length );
	memcpy( &szTemp[_length], sz, len );
	szTemp[newLength] = 0x0;

	// delete existing string
	delete[] _s; _s = 0;

	// repositition pointer
	_s = szTemp;
	_length = newLength;

	_bModified = true;
	_bInMemory = true;
}


//------------------------------------------
int vdbVarCharLong::Length() const
{
	return _length;
}
