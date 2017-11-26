//=============================================================================
//
// CLASS:           vdbRowColumn
//
// OVERVIEW:		The vdbRowColumn class is used to manage a two-dimensional table
//                  of object pointers.  The number of columns and rows must be
//                  set at construction time.  The default pointer for each entry
//                  is NULL.  Use the Adopt function to place an item into the
//                  table.  All objects of type T must be created on the heap.
//                  The vdbRowColumn destructor deletes the adopted objects.
//
// FILE NAME:       vdbRowColumn.cpp
//
// INITIAL DATE:    2/18/00
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#include "vdbRowColumn.h"
#include <vdbAssert.h>
#include "vdbException.h"
#include <vdbStreamHeader.h>
#include <malloc.h>
#include <memory.h>


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
//^^ The constructor( int, int)
//> rowCount is the number of rows to allocate
//> columnCount is the number of columns to allocate
//
template<class T>
vdbRowColumn<T>::vdbRowColumn( int rowCount, int columnCount )
	: _pObjects( 0 ),
	  _rowCount( 0 ),
	  _columnCount( 0 )
{
	// allocate the pointers
	AllocateTable( rowCount, columnCount );
}
	

//-------------------------------------
//^^ This constructor is to be used with istream 
//
template<class T>
vdbRowColumn<T>::vdbRowColumn()
	: _pObjects( 0 ),
	  _rowCount( 0 ),
	  _columnCount( 0 )
{
}


//-------------------------------------
//^^ The destructor
template<class T>
vdbRowColumn<T>::~vdbRowColumn()
{
	// delete the pointers
	DeleteTable();
}


//-------------------------------------
//^^ The copy constructor
//^ Copies the contained objects.  Deletion of the rhs set will not effect the
//  lhs set.
//
template<class T>
vdbRowColumn<T>::vdbRowColumn( const vdbRowColumn<T>& rhs )
	: _pObjects( 0 ),
	  _rowCount( 0 ),
	  _columnCount( 0 )
{
	// allocate the new table
	AllocateTable( rhs._rowCount, rhs._columnCount );

	// copy the objects adopted by the table
    for ( int i = 0; i < rhs._rowCount; i++ )
	{
		for ( int j = 0; j < rhs._columnCount; j++ )
		{
			T* p = (T*) rhs._pObjects[i][j];
			if ( p != 0x0 )
			{
				_pObjects[i][j] = new T( *p );
				if ( _pObjects[i] == 0 ) throw vdbMemoryException();
			}
		}
	}
}


//-------------------------------------
//^^ The assignment operator
//^ Copies the contained objects.  Deletion of the rhs set will not effect the
//  lhs set.  (Same as copy constructor.)
//
template<class T>
vdbRowColumn<T>& vdbRowColumn<T>::operator=( const vdbRowColumn<T>& rhs )
{
	if ( this == &rhs )
		return *this;
	
	// get rid of any existing table
	DeleteTable();

	// allocate the new table
	AllocateTable( rhs._rowCount, rhs._columnCount );

	// copy the objects adopted by the table
    for ( int i = 0; i < rhs._rowCount; i++ )
	{
		for ( int j = 0; j < rhs._columnCount; j++ )
		{
			T* p = (T*) rhs._pObjects[i][j];
			if ( p != 0x0 )
			{
				_pObjects[i][j] = new T( *p );
				if ( _pObjects[i] == 0 ) throw vdbMemoryException();
			}
		}
	}

	return *this;
}



//=============================================================================
// Public member functions
//=============================================================================

//-------------------------------------
//^ The Adopt function adopts the object passed in the parameter list.
//> pItem is the object to add to the array.  This object must have been
//  created on the heap.  This must not be a NULL pointer.
//< returns true if adopted, or false if the (row,column) already has an object
//
template<class T>
bool vdbRowColumn<T>::Adopt( int row, int column, T* pItem )
{
	RangeCheck( row, _rowCount, this );
	RangeCheck( column, _columnCount, this );
	assert( pItem != 0 );

	if ( _pObjects[row][column] != 0x0 )
		return false;
	
	_pObjects[row][column] = pItem;
	return true;
}


//--------------------------------------------
//^ The Get function returns a pointer to the object at the specified
//  (row,column)
//< returns a pointer to the object at the specified location or 0 if there
//  is no object their
//
template<class T>
T* vdbRowColumn<T>::Get( int row, int column ) const
{
	RangeCheck( row, _rowCount, this );
	RangeCheck( column, _columnCount, this );
	T* p = (T*) _pObjects[row][column];
	return p;
}


//--------------------------------------------
//^ The Orphan function removes a single object from the set and sets its pointer
//  to 0 so that another object can be placed there.
//< returns the pointer to the object that was removed from the set.  Ownership
//  of the object is given to the caller; thus it is the caller's responsibility
//  to delete the item.
//
template<class T>
T* vdbRowColumn<T>::Orphan( int row, int column )
{
	RangeCheck( row, _rowCount, this );
	RangeCheck( column, _columnCount, this );
	
	T* p = (T*) _pObjects[row][column];
	_pObjects[row][column] = 0x0;
	
	return p;	
}


//--------------------------------------------
//^ The ClearEntries function deletes all objects adopted by the table
//
template<class T>
void vdbRowColumn<T>::ClearEntries()
{
	for ( int i = 0; i < _rowCount; i++ )
	{
		for ( int j = 0; j < _columnCount; j++ )
		{
			if ( _pObjects[i][j] != 0x0 )
			{
				delete _pObjects[i][j];
				_pObjects[i][j] = 0x0;
			}
		}
	}
}
	

//=============================================================================
// Table pointer functions
//=============================================================================

//-------------------------------------
//^^ The AllocateTable function creates the pointers
template<class T>
void vdbRowColumn<T>::AllocateTable( int rowCount, int columnCount )
{
	assert( rowCount > 0 );
	assert( columnCount > 0 );

	// delete any existing table
	if ( _rowCount > 0 || _columnCount > 0 )
		DeleteTable();

	_rowCount = rowCount ;
	_columnCount = columnCount;

    // allocate pointers to rows.
    _pObjects = (T***) malloc( (_rowCount) * sizeof(T**) );
    if ( _pObjects == 0 )
		throw vdbMemoryException();

    // allocate the rows themselves
    _pObjects[0] = (T**) malloc( _rowCount * _columnCount * sizeof(T*) );
    if ( _pObjects[0] == 0 )
		throw vdbMemoryException();
	memset( _pObjects[0], 0x0, _columnCount * sizeof(T*) );

    for ( int i = 1; i < _rowCount; i++ )
	{
		_pObjects[i] = _pObjects[i-1] + _columnCount;
		memset( _pObjects[i], 0x0, _columnCount * sizeof(T*) );
	}
}


//-------------------------------------
//^^ The DeleteTable function deletes the pointers
template<class T>
void vdbRowColumn<T>::DeleteTable()
{
	// delete the objects adopted by the table so that they aren't orphaned
	ClearEntries();

	// delete the rows
	if ( _pObjects != 0x0 )
	{
		free( _pObjects[0] );
		free( _pObjects ); _pObjects = 0x0;
	}

	_rowCount = 0;
	_columnCount = 0;
}

