//=============================================================================
//
// CLASS:           vdbArray
//
// OVERVIEW:		The vdbArray class is used to manage an array which can
//                  automatically expand in size.
//
// FILE NAME:       vdbArray.cpp
//
// INITIAL DATE:    09/13/2003
//
// COPYRIGHT:		Copyright © 2003 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbArray.h"
#include "vdbAssert.h"
#include "vdbMessageBox.h"
#include "vdbException.h"
#include "vdbStreamHeader.h"
#include <malloc.h>


//=============================================================================
// Essential class functions
//=============================================================================

//--------------------------------------------
//^^ The constructor
//> initialSize is the number of slots to allocate for holding object pointers
//> The intialSize must be at least 1.
//
template<class T>
vdbArray<T>::vdbArray( int initialSize )
	: _count( 0 ),
	  _size( initialSize )
{
	assert( initialSize > 0 );
	_pObjects = (T*) malloc( sizeof(T) * _size );
	memset( _pObjects, 0x0, sizeof(T) * _size );
}
	

//--------------------------------------------
//^^ The destructor
template<class T>
vdbArray<T>::~vdbArray()
{
	free( _pObjects );
	_pObjects = 0;
}


//--------------------------------------------
//^^ The copy constructor
//^ Copies the values of the objects.
//
template<class T>
vdbArray<T>::vdbArray( const vdbArray<T>& rhs )
	: _count( rhs._count ),
	  _size( rhs._size )
{
	_pObjects = (T*) malloc( sizeof(T) * _size );
	for ( int i = 0; i < _count; i++ )
		_pObjects[i] = rhs._pObjects[i];
}


//--------------------------------------------
//^^ The assignment operator
//^ Copies the values of the objects.
//
template<class T>
vdbArray<T>& vdbArray<T>::operator=( const vdbArray<T>& rhs )
{
	if ( this == &rhs )
		return *this;
	
	free( _pObjects );

	_count = rhs._count;
	_size = rhs._size;
	
	_pObjects = (T*) malloc( sizeof(T) * _size );
	for ( int i = 0; i < _count; i++ )
		_pObjects[i] = rhs._pObjects[i];

	return *this;
}


//=============================================================================
// Public member functions
//=============================================================================

//--------------------------------------------
//^^ The operator[] function
//^ Sets the internal array element to be equal to the rhs value
//> index is the zero-based item in the array to set.
//> uses the class T assignment operator to assign the value of rhs
//  to the array element.
template<class T>
T& vdbArray<T>::operator[]( int index )
{
	if ( index > _size - 1 )
		Resize(); 

	// _count hold the number of valid elements
	if ( index > _count - 1 )
		_count = index + 1;

	return _pObjects[index];
}


//--------------------------------------------
//^^ The operator[] const function
//^ Sets the internal array element to be equal to the rhs value
//> index is the zero-based item in the array to set.
//> uses the class T assignment operator to assign the value of rhs
//  to the array element.
template<class T>
T& vdbArray<T>::operator[]( int index ) const
{
	assert( index < _size );

	return _pObjects[index];
}


//--------------------------------------------
//^ The Get function return a reference to the object at the specified
// index number. Same as operator[].
//> index is the zero-based item in the array to retrieve.
//< returns a reference to the object at the specified index number
//
template<class T>
T& vdbArray<T>::Get( int index )
{
	if ( index > _size - 1 )
		Resize(); 
	
	// _count hold the number of valid elements
	if ( index > _count - 1 )
		_count = index + 1;

	return _pObjects[index];
}


//--------------------------------------------
//^ The Set function sets the internal array element to be equal to the rhs value
//> index is the zero-based item in the array to set.
//> uses the class T assignment operator to assign the value of rhs
//  to the array element.
template<class T>
void vdbArray<T>::Set( int index, const T& rhs )
{
	if ( index > _size - 1 )
		Resize(); 

	// _count hold the number of valid elements
	if ( index > _count - 1 )
		_count = index + 1;

	_pObjects[index] = rhs;
}


//--------------------------------------------
//^ The Add function add the given item of type T to the end of the array of elements
//> rhs is the item to assign
//< returns the index to the array element to which this item was assigned
//
template<class T>
int vdbArray<T>::Add( const T& rhs )
{
	int index = _count;

	if ( index > _size - 1 )
		Resize(); 

	_pObjects[index] = rhs;
	_count++;

	return index;
}


//--------------------------------------------
//^ The Remove function delete the object at the specified slot and moves
//  everything up.
template<class T>
void vdbArray<T>::Remove( int index )
{
	// if this is not the last item in the array, shift everything up one
	if ( index < _count )
		memcpy( &_pObjects[index], &_pObjects[index+1], sizeof(T) * ((_count-1) - index) );

	// zero fill the last slot
	memset( &_pObjects[(_count-1)], 0x0, sizeof(T) );
	
	// decrease total valid array size
	_count--;
}


//--------------------------------------------
//^ The Resize function changes the size of the array to hold more items
//< returns the actual new size of the allocated array or -1 if memory
//  could not be allocated
//
template<class T>
int vdbArray<T>::Resize( int newSize )
{
	// nothing to do
	if ( newSize <= _size )
		return _size;

	T* newPointer = 0;
	newPointer = (T*) realloc( _pObjects, sizeof(T) * newSize );
	if ( newPointer == 0 )
	{
		vdbMessageBox( NULL, _T("Out of memory while expanding set"), _T("vdbArray"), MB_OK );
		vdbThrowMemoryException();
		return -1;
	}
	_pObjects = newPointer;

	// zero fill the newly allocated slots
	memset( &_pObjects[_size], 0x0, sizeof(T) * (newSize - _size) );

	_size = newSize;
	return _size;
}


//--------------------------------------------
//^ The Resize function changes the size of the array to hold more items
//< returns the actual new size of the allocated array or -1 if memory
//  could not be allocated
//
template<class T>
int vdbArray<T>::Resize()
{
	// grow to at least 32 or by 50%
	int actualNewSize = _size < 32 ? 32 : static_cast<int>( _size * 1.5 );

	T* newPointer = 0;
	newPointer = (T*) realloc( _pObjects, sizeof(T) * actualNewSize );
	if ( newPointer == 0 )
	{
		vdbMessageBox( NULL, _T("Out of memory while expanding set"), _T("vdbArray"), MB_OK );
		vdbThrowMemoryException();
		return -1;
	}
	_pObjects = newPointer;

	// zero fill the newly allocated slots
	memset( &_pObjects[_size], 0x0, sizeof(T) * (actualNewSize - _size) );

	_size = actualNewSize;
	return _size;
}

