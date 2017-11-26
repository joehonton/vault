//=============================================================================
//
// CLASS:           vdbColumn
//
// ESSENCE:         database column base class 
//
// FILE NAME:       vdbColumn.cpp
//
// INITIAL DATE:    2/1/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#include <string.h>
#include "vdbServer.h"
#include "vdbDatabase.h"
#include "vdbColumn.h"
#include "vdbException.h"

//warning C4996: 'strcpy': This function or variable may be unsafe. Consider using strcpy_s instead.
#pragma warning( disable : 4996 )

//=============================================================================
// Essential class functions
//=============================================================================

//------------------------------------------
//^^ vdbColumn basic constructor
//> szColumnName is the name of the column as it will appear in the DBMS.
//> szColumnType is the DBMS independant data type, for example, DBINT, DBFLOAT, DBTIMESTAMP, etc.
//
vdbColumn::vdbColumn( vdbDatabase* pDatabase, const char* szColumnName, const char* szColumnType )
	: _pDatabase( pDatabase ),
	  _bDataAtExecution( false )
{
	_szColumnName = new char[strlen( szColumnName ) + 1];
	if ( _szColumnName == 0 ) throw vdbMemoryException();
	strcpy( _szColumnName, szColumnName );
	_szColumnType = new char[strlen( szColumnType ) + 1];
	if ( _szColumnType == 0 ) throw vdbMemoryException();
	strcpy( _szColumnType, szColumnType );

	if ( !strncmp( szColumnType, "longvar", 7 ) )
		_bDataAtExecution = true;
}


//------------------------------------------
//^^ vdbColumn destructor
vdbColumn::~vdbColumn()
{
	delete[] _szColumnName;
	delete[] _szColumnType;
}


//------------------------------------------
//^^ vdbColumn copy constructor
//^ The copy constructor copies the column name and column type
//  of the right hand side to the new object.
//> rhs is the right hand side column to copy.
//
vdbColumn::vdbColumn( const vdbColumn& rhs )
	: _bDataAtExecution( rhs._bDataAtExecution )
{
	_szColumnName = new char[strlen( rhs._szColumnName ) + 1];
	if ( _szColumnName == 0 ) throw vdbMemoryException();
	strcpy( _szColumnName, rhs._szColumnName );

	_szColumnType = new char[strlen( rhs._szColumnType ) + 1];
	if ( _szColumnType == 0 ) throw vdbMemoryException();
	strcpy( _szColumnType, rhs._szColumnType );
}


//------------------------------------------
//^^ vdbColumn assignment operator
//^ The assignment operator copies the column name and column type
//  of the right hand side of the assignment.
//> rhs is the right hand side.
//< the return value is a reference to this object.
//
vdbColumn& vdbColumn::operator=( const vdbColumn& rhs )
{
	if ( this == &rhs )
		return *this;
	
	delete[] _szColumnName;
	_szColumnName = 0;
	_szColumnName = new char[strlen( rhs._szColumnName ) + 1];
	if ( _szColumnName == 0 ) throw vdbMemoryException();
	strcpy( _szColumnName, rhs._szColumnName );

	delete[] _szColumnType;
	_szColumnType = 0;
	_szColumnType = new char[strlen( rhs._szColumnType ) + 1];
	if ( _szColumnType == 0 ) throw vdbMemoryException();
	strcpy( _szColumnType, rhs._szColumnType );

	_bDataAtExecution = rhs._bDataAtExecution;

	return *this;
}


//=============================================================================
// Utility functions
//=============================================================================

//------------------------------------------
//^ The Type function returns the datatype of the column 
//  for this object.  This function returns a DBMS-specific
//  string such as int, tinyint, smallint, etc.
//< returns a constant character pointer.
//
const char* vdbColumn::Type()
{
	assert( _pDatabase != 0 );

	return _pDatabase->Server()->ColumnType( _szColumnType );
}
