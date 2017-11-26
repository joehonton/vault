//=============================================================================
//
// CLASS:           vdbIndex
//
// ESSENCE:         database index base class 
//
// FILE NAME:       vdbIndex.cpp
//
// INITIAL DATE:    2/1/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifdef USE_PRE_HDR
	#include "vdbHeader.h"
#else
	#include "boolean.h"
	#include "vdbIndex.h"
	#include "vdbException.h"
#endif

#include <string.h>

//warning C4996: 'strcpy': This function or variable may be unsafe. Consider using strcpy_s instead.
#pragma warning( disable : 4996 )


//------------------------------------------
// basic constructor for indexes using 1 column
vdbIndex::vdbIndex( const char* szIndexName, const char* szColumnName1, bool isUnique )
	: _isUnique( isUnique ),
	  _columnCount( 1 )
{
	_szIndexName = new char[strlen( szIndexName ) + 1];
	if ( _szIndexName == 0 ) throw vdbMemoryException();
	strcpy( _szIndexName, szIndexName );
	
	_szColumnName[0] = new char[strlen( szColumnName1 ) + 1];
	if ( _szColumnName[0] == 0 ) throw vdbMemoryException();
	strcpy( _szColumnName[0], szColumnName1 );
	
	_szColumnName[1] = new char[1];
	if ( _szColumnName[1] == 0 ) throw vdbMemoryException();
	_szColumnName[1][0] = 0;

	_szColumnName[2] = new char[1];
	if ( _szColumnName[2] == 0 ) throw vdbMemoryException();
	_szColumnName[2][0] = 0;
}


//------------------------------------------
// basic constructor for indexes using 2 columns
vdbIndex::vdbIndex( const char* szIndexName, const char* szColumnName1, const char* szColumnName2, bool isUnique )
	: _isUnique( isUnique ),
	  _columnCount( 2 )
{
	_szIndexName = new char[strlen( szIndexName ) + 1];
	if ( _szIndexName == 0 ) throw vdbMemoryException();
	strcpy( _szIndexName, szIndexName );

	_szColumnName[0] = new char[strlen( szColumnName1 ) + 1];
	if ( _szColumnName[0] == 0 ) throw vdbMemoryException();
	strcpy( _szColumnName[0], szColumnName1 );

	_szColumnName[1] = new char[strlen( szColumnName2 ) + 1];
	if ( _szColumnName[1] == 0 ) throw vdbMemoryException();
	strcpy( _szColumnName[1], szColumnName2 );

	_szColumnName[2] = new char[1];
	if ( _szColumnName[2] == 0 ) throw vdbMemoryException();
	_szColumnName[2][0] = 0;
}


//------------------------------------------
// basic constructor for indexes using 3 columns
vdbIndex::vdbIndex( const char* szIndexName, const char* szColumnName1, const char* szColumnName2, const char* szColumnName3, bool isUnique )
	: _isUnique( isUnique ),
	  _columnCount( 3 )
{
	_szIndexName = new char[strlen( szIndexName ) + 1];
	if ( _szIndexName == 0 ) throw vdbMemoryException();
	strcpy( _szIndexName, szIndexName );

	_szColumnName[0] = new char[strlen( szColumnName1 ) + 1];
	if ( _szColumnName[0] == 0 ) throw vdbMemoryException();
	strcpy( _szColumnName[0], szColumnName1 );

	_szColumnName[1] = new char[strlen( szColumnName2 ) + 1];
	if ( _szColumnName[1] == 0 ) throw vdbMemoryException();
	strcpy( _szColumnName[1], szColumnName2 );

	_szColumnName[2] = new char[strlen( szColumnName3 ) + 1];
	if ( _szColumnName[2] == 0 ) throw vdbMemoryException();
	strcpy( _szColumnName[2], szColumnName3 );
}


//------------------------------------------
// destructor
vdbIndex::~vdbIndex()
{
	delete[] _szIndexName;
	delete[] _szColumnName[0];
	delete[] _szColumnName[1];
	delete[] _szColumnName[2];
}


//------------------------------------------
// copy constructor 
vdbIndex::vdbIndex( const vdbIndex& rhs )
	: _isUnique( rhs._isUnique ),
	  _columnCount( rhs._columnCount )
{
	_szIndexName = new char[strlen( rhs._szIndexName ) + 1];
	if ( _szIndexName == 0 ) throw vdbMemoryException();
	strcpy( _szIndexName, rhs._szIndexName );

	for ( int i = 0; i < 3; i++ )
	{
		_szColumnName[i] = new char[strlen( rhs._szColumnName[i] ) + 1];
		if ( _szColumnName[i] == 0 ) throw vdbMemoryException();
		strcpy( _szColumnName[i], rhs._szColumnName[i] );
	}
}


//------------------------------------------
// assignment operator
vdbIndex& vdbIndex::operator=( const vdbIndex& rhs )
{
	if ( this == &rhs )
		return *this;
	
	delete[] _szIndexName;
	_szIndexName = 0;
	_szIndexName = new char[strlen( rhs._szIndexName ) + 1];
	if ( _szIndexName == 0 ) throw vdbMemoryException();
	strcpy( _szIndexName, rhs._szIndexName );

	for ( int i = 0; i < 3; i++ )
	{
		delete[] _szColumnName[i];
		_szColumnName[i] = 0;
		_szColumnName[i] = new char[strlen( rhs._szColumnName[i] ) + 1];
		if ( _szColumnName[i] == 0 ) throw vdbMemoryException();
		strcpy( _szColumnName[i], rhs._szColumnName[i] );
	}

	return *this;
}


