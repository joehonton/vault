//=============================================================================
//
// FILE NAME:       vdbWideChar.cpp
//
// ESSENCE:         Conversion between ANSI and UNICODE
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    2/13/2002
//
// COPYRIGHT:       Copyright © 2002 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbWideChar.h"
#include <vdbException.h>

//warning C4996: 'strcpy': This function or variable may be unsafe. Consider using strcpy_s instead.
#pragma warning( disable : 4996 )

//=============================================================================
// Essential class functions
//=============================================================================

//---------------------------------------
//^^ constructor
vdbWideChar::vdbWideChar()
	: _pszAnsi( 0 ),
	  _pszUnicode( 0 ),
	  _bIsAnsiValid( false ),
	  _bIsUnicodeValid( false )
{
}


//---------------------------------------
vdbWideChar::vdbWideChar( const char* sz, int length )
	: _pszAnsi( 0 ),
	  _pszUnicode( 0 ),
	  _bIsAnsiValid( false ),
	  _bIsUnicodeValid( false )
{
	SetAnsi( sz, length );
}


//---------------------------------------
vdbWideChar::vdbWideChar( const wchar_t* sz, int length )
	: _pszAnsi( 0 ),
	  _pszUnicode( 0 ),
	  _bIsAnsiValid( false ),
	  _bIsUnicodeValid( false )
{
	SetUnicode( sz, length );
}


//---------------------------------------
//^^ destructor
vdbWideChar::~vdbWideChar()
{
	delete[] _pszAnsi; _pszAnsi = 0;
	delete[] _pszUnicode; _pszUnicode = 0;
}


//---------------------------------------
//^^ copy constructor
vdbWideChar::vdbWideChar( const vdbWideChar& rhs )
{
	delete[] _pszAnsi; _pszAnsi = 0;
	int len = ( rhs._pszAnsi == 0 ? 0 : strlen( rhs._pszAnsi ) );
	if ( len > 0 )
	{
		_pszAnsi = new char[len+1];
		if ( _pszAnsi == 0 )
			throw vdbMemoryException();
		strcpy( _pszAnsi, rhs._pszAnsi );
	}

	delete[] _pszUnicode; _pszUnicode = 0;
	len = ( rhs._pszUnicode == 0 ? 0 : wcslen( rhs._pszUnicode ) );
	if ( len > 0 )
	{
		_pszUnicode = new wchar_t[len+1];
		if ( _pszUnicode == 0 )
			throw vdbMemoryException();
		wcscpy( _pszUnicode, rhs._pszUnicode );
	}

	_bIsAnsiValid = rhs._bIsAnsiValid;
	_bIsUnicodeValid = rhs._bIsUnicodeValid;
}


//---------------------------------------
//^^ assignment 
const vdbWideChar& vdbWideChar::operator= ( const vdbWideChar& rhs )
{
	if ( &rhs == this )
		return *this;

	delete[] _pszAnsi; _pszAnsi = 0;
	int len = ( rhs._pszAnsi == 0 ? 0 : strlen( rhs._pszAnsi ) );
	if ( len > 0 )
	{
		_pszAnsi = new char[len+1];
		if ( _pszAnsi == 0 )
			throw vdbMemoryException();
		strcpy( _pszAnsi, rhs._pszAnsi );
	}

	delete[] _pszUnicode; _pszUnicode = 0;
	len = ( rhs._pszUnicode == 0 ? 0 : wcslen( rhs._pszUnicode ) );
	if ( len > 0 )
	{
		_pszUnicode = new wchar_t[len+1];
		if ( _pszUnicode == 0 )
			throw vdbMemoryException();
		wcscpy( _pszUnicode, rhs._pszUnicode );
	}

	_bIsAnsiValid = rhs._bIsAnsiValid;
	_bIsUnicodeValid = rhs._bIsUnicodeValid;

	return *this;
}


//=============================================================================
// Accessor functions
//=============================================================================

//------------------------------------------
//^^ const char* cast operator
vdbWideChar::operator const char*()
{
	return GetAnsi();
}


//------------------------------------------
//^^ const wchar_t* cast operator
vdbWideChar::operator const wchar_t*()
{
	return GetUnicode();
}


//---------------------------------------
const char* vdbWideChar::GetAnsi()
{
	if ( _bIsAnsiValid == false )
		UnicodeToAnsi();
	
	if ( _bIsAnsiValid == true )
		return _pszAnsi;
	else
		return "";
}


//---------------------------------------
const wchar_t* vdbWideChar::GetUnicode()
{
	if ( _bIsUnicodeValid == false )
		AnsiToUnicode();
	
	if ( _bIsUnicodeValid == true )
		return _pszUnicode;
	else
		return L"";
}


//---------------------------------------
//> The SetAnsi function assigns this to be the value of the given ANSI string
//> sz the ANSI value to set
//> length is the maximum number of characters to copy, or -1 to copy everything
//  up to and including the null-terminator
void vdbWideChar::SetAnsi( const char* sz, int length )
{
	delete[] _pszAnsi; _pszAnsi = 0;

	int len;
	if ( length == -1 )
		len = ( sz == 0 ? 0 : strlen( sz ) );
	else
		len = length;

	if ( len > 0 )
	{
		_pszAnsi = new char[len+1];
		if ( _pszAnsi == 0 )
			throw vdbMemoryException();
		strncpy( _pszAnsi, sz, len );
		_pszAnsi[len] = 0x0;
		_bIsAnsiValid = true;
	}
	else
		_bIsAnsiValid = false;

	_bIsUnicodeValid = false;
}


//---------------------------------------
//> The SetUnicode function assigns this to be the value of the given Unicode string
//> sz the Unicode value to set
//> length is the maximum number of characters to copy, or -1 to copy everything
//  up to and including the null-terminator
void vdbWideChar::SetUnicode( const wchar_t* sz, int length )
{
	delete[] _pszUnicode; _pszUnicode = 0;

	int len;
	if ( length == -1 )
		len = ( sz == 0 ? 0 : wcslen( sz ) );
	else
		len = length;

	if ( len > 0 )
	{
		_pszUnicode = new wchar_t[len+1];
		if ( _pszUnicode == 0 )
			throw vdbMemoryException();
		wcsncpy( _pszUnicode, sz, len );
		_pszUnicode[len] = 0x0;
		_bIsUnicodeValid = true;
	}
	else
		_bIsUnicodeValid = false;

	_bIsAnsiValid = false;
}


//---------------------------------------
int vdbWideChar::Length()
{
	if ( _bIsAnsiValid == true )
		return strlen( _pszAnsi );

	if ( _bIsUnicodeValid == true )
		return wcslen( _pszUnicode );
	
	return 0;
}


//=============================================================================
// Conversion functions
//=============================================================================

//---------------------------------------
void vdbWideChar::AnsiToUnicode()
{
	if ( _pszAnsi == NULL )
	{
		_bIsUnicodeValid = false;
		return;
	}

	int lenAnsi = strlen( _pszAnsi );
	
	if ( _pszUnicode != 0 )
		{ delete[] _pszUnicode; _pszUnicode = 0; }

	_pszUnicode = new wchar_t[lenAnsi+1];
	if ( _pszUnicode == 0 )
		throw vdbMemoryException();
	
	MultiByteToWideChar( CP_ACP, 0, _pszAnsi, -1, _pszUnicode, lenAnsi+1 );

	_bIsUnicodeValid = true;
}


//---------------------------------------
void vdbWideChar::UnicodeToAnsi()
{
	if ( _pszUnicode == NULL )
	{
		_bIsAnsiValid = false;
		return;
	}

	int lenUnicode = wcslen( _pszUnicode );

	if ( _pszAnsi != 0 )
		{ delete[] _pszAnsi; _pszAnsi = 0; }

	_pszAnsi = new char[lenUnicode+1];
	if ( _pszAnsi == 0 )
		throw vdbMemoryException();
	
	WideCharToMultiByte( CP_ACP, 0, _pszUnicode, -1, _pszAnsi, lenUnicode+1, NULL, NULL );

	_bIsAnsiValid = true;
}


