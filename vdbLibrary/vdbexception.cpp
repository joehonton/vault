//=============================================================================
//
// CLASS:           vdbException
//
// ESSENCE:         Vault Class Library exception class
//
// FILE NAME:       vdbException.cpp
//
// INITIAL DATE:    11/12/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbException.h"
#include "string.h"

//warning C4996: 'strcpy': This function or variable may be unsafe. Consider using strcpy_s instead.
#pragma warning( disable : 4996 )

//=============================================================================
//  character string exception
//=============================================================================

//-------------------------------------
vdbException::vdbException( const TCHAR* szDesc )
	: _szDescription( 0 )
{
	if ( szDesc != 0 )
	{
		_szDescription = new TCHAR[_tcslen( szDesc ) + 1];
		if ( _szDescription != 0 )
			_tcscpy( _szDescription, szDesc );
	}
}


//-------------------------------------
vdbException::~vdbException()
{
	delete[] _szDescription; _szDescription = 0;
}


//-------------------------------------
vdbException::vdbException( const vdbException& rhs )
	: _szDescription( 0 )
{
	if ( rhs._szDescription != 0 )
	{
		_szDescription = new TCHAR[_tcslen( rhs._szDescription ) + 1];
		if ( _szDescription != 0 )
			_tcscpy( _szDescription, rhs._szDescription );
	}
}


//-------------------------------------
const vdbException& vdbException::operator= ( const vdbException& rhs )
{
	if ( this == &rhs )
		return *this;

	delete[] _szDescription; _szDescription = 0;
	if ( rhs._szDescription != 0 )
	{
		_szDescription = new TCHAR[_tcslen( rhs._szDescription ) + 1];
		if ( _szDescription != 0 )
			_tcscpy( _szDescription, rhs._szDescription );
	}

	return *this;
}


//=============================================================================
//  memory exception
//=============================================================================

//-------------------------------------
vdbMemoryException::vdbMemoryException()
	: vdbException( _T("memory") )
{
}


//-------------------------------------
vdbMemoryException::~vdbMemoryException()
{
}


//-------------------------------------
vdbMemoryException::vdbMemoryException( const vdbMemoryException& rhs )
	: vdbException( (const vdbException&) rhs )
{
}


//-------------------------------------
const vdbMemoryException& vdbMemoryException::operator= ( const vdbMemoryException& rhs )
{
	if ( this == &rhs )
		return *this;

	vdbException::operator=( rhs );

	return *this;
}


//=============================================================================
//  Assert exception
//=============================================================================

//-------------------------------------
vdbAssertException::vdbAssertException( const TCHAR* szAssert, const TCHAR* szFile, int iLine )
	: vdbException( _T("assert") )
{
	if ( szAssert != 0 )
	{
		_szAssertion = new TCHAR[_tcslen( szAssert ) + 1];
		if ( _szAssertion != 0 )
			_tcscpy( _szAssertion, szAssert );
	}

	if ( szFile != 0 )
	{
		_szFile = new TCHAR[_tcslen( szFile ) + 1];
		if ( _szFile != 0 )
			_tcscpy( _szFile, szFile );
	}

	_iLine = iLine;
}


//-------------------------------------
vdbAssertException::~vdbAssertException()
{
	delete[] _szAssertion; _szAssertion = 0;
	delete[] _szFile; _szFile = 0;
}


//-------------------------------------
vdbAssertException::vdbAssertException( const vdbAssertException& rhs )
	: vdbException( (const vdbException&) rhs )
{
	if ( rhs._szAssertion != 0 )
	{
		_szAssertion = new TCHAR[_tcslen( rhs._szAssertion ) + 1];
		if ( _szAssertion != 0 )
			_tcscpy( _szAssertion, rhs._szAssertion );
	}

	if ( rhs._szFile != 0 )
	{
		_szFile = new TCHAR[_tcslen( rhs._szFile ) + 1];
		if ( _szFile != 0 )
			_tcscpy( _szFile, rhs._szFile );
	}

	_iLine = rhs._iLine;
}


//-------------------------------------
const vdbAssertException& vdbAssertException::operator= ( const vdbAssertException& rhs )
{
	if ( this == &rhs )
		return *this;

	vdbException::operator=( rhs );

	delete[] _szAssertion; _szAssertion = 0;
	if ( rhs._szAssertion != 0 )
	{
		_szAssertion = new TCHAR[_tcslen( rhs._szAssertion ) + 1];
		if ( _szAssertion != 0 )
			_tcscpy( _szAssertion, rhs._szAssertion );
	}

	delete[] _szFile; _szFile = 0;
	if ( rhs._szFile != 0 )
	{
		_szFile = new TCHAR[_tcslen( rhs._szFile ) + 1];
		if ( _szFile != 0 )
			_tcscpy( _szFile, rhs._szFile );
	}

	_iLine = rhs._iLine;

	return *this;
}



//=============================================================================
//  Range exception
//=============================================================================

//-------------------------------------
vdbRangeException::vdbRangeException( int index, int maxSubscript, const TCHAR* szClassName )
	: vdbException( _T("range") )
{
	_iIndex = index;
	_iMaxSubscript = maxSubscript;
	
	if ( szClassName != 0 )
	{
		_szClassName = new TCHAR[_tcslen( szClassName ) + 1];
		if ( _szClassName != 0 )
			_tcscpy( _szClassName, szClassName );
	}

}


//-------------------------------------
vdbRangeException::~vdbRangeException()
{
	delete[] _szClassName; _szClassName = 0;
}


//-------------------------------------
vdbRangeException::vdbRangeException( const vdbRangeException& rhs )
	: vdbException( (const vdbException&) rhs )
{
	_iIndex = rhs._iIndex;
	_iMaxSubscript = rhs._iMaxSubscript;

	if ( rhs._szClassName != 0 )
	{
		_szClassName = new TCHAR[_tcslen( rhs._szClassName ) + 1];
		if ( _szClassName != 0 )
			_tcscpy( _szClassName, rhs._szClassName );
	}
}


//-------------------------------------
const vdbRangeException& vdbRangeException::operator= ( const vdbRangeException& rhs )
{
	if ( this == &rhs )
		return *this;

	vdbException::operator=( rhs );

	_iIndex = rhs._iIndex;
	_iMaxSubscript = rhs._iMaxSubscript;

	delete[] _szClassName; _szClassName = 0;
	if ( rhs._szClassName != 0 )
	{
		_szClassName = new TCHAR[_tcslen( rhs._szClassName ) + 1];
		if ( _szClassName != 0 )
			_tcscpy( _szClassName, rhs._szClassName );
	}
	return *this;
}

//=============================================================================
//  File exception
//=============================================================================

//-------------------------------------
vdbFileException::vdbFileException( const TCHAR* szFilename )
	: vdbException( szFilename )
{
}


//-------------------------------------
vdbFileException::~vdbFileException()
{
}


//-------------------------------------
vdbFileException::vdbFileException( const vdbFileException& rhs )
	: vdbException( (const vdbException&) rhs )
{
}


//-------------------------------------
const vdbFileException& vdbFileException::operator= ( const vdbFileException& rhs )
{
	if ( this == &rhs )
		return *this;

	vdbException::operator=( rhs );

	return *this;
}


