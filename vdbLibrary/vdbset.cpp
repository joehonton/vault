//=============================================================================
//
// CLASS:           vdbSet
//
// OVERVIEW:		The vdbSet class is used to manage a variable length array
//                  of object pointers.  The pointed-to objects are adopted by
//                  this object when the Add function is called.  All objects
//                  of type T must be created on the heap.  The vdbSet
//                  destructor deletes the pointed-to objects held by this
//                  object.
//
// FILE NAME:       vdbSet.cpp
//
// INITIAL DATE:    11/30/94
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbSet.h"
#include "vdbAssert.h"
#include "vdbMessageBox.h"
#include "vdbException.h"
#include "vdbStreamHeader.h"
#include <malloc.h>
#include <istream>
#include <ostream>


//=============================================================================
// Essential class functions
//=============================================================================

//--------------------------------------------
//^^ The constructor
//> initialSize is the number of slots to allocate for holding object pointers
//> The intialSize must be at least 1.
//
template<class T>
vdbSet<T>::vdbSet( int initialSize )
	: _next( 0 ),
	  _size( initialSize )
{
	assert( initialSize > 0 );
	_pObjects = (T**) malloc( sizeof(T*) * _size );
	for ( int i = 0; i < _size; i++ )
		_pObjects[i] = 0;
}
	

//--------------------------------------------
//^^ The destructor
template<class T>
vdbSet<T>::~vdbSet()
{
	ClearAll();
	free( _pObjects );
}



//--------------------------------------------
//^^ The copy constructor
//^ Copies the contained objects.  Deletion of the rhs set will not effect the
//  lhs set.
//
template<class T>
vdbSet<T>::vdbSet( const vdbSet<T>& rhs )
	: _next( rhs._next ),
	  _size( rhs._size )
{
	_pObjects = (T**) malloc( sizeof(T*) * _size );
	for ( int i = 0; i < _next; i++ )
	{
		T* p = (T*) rhs._pObjects[i];

		if ( p == NULL )
			_pObjects[i] = NULL;
		else
		{
			_pObjects[i] = new T( *p );
			if ( _pObjects[i] == 0 ) vdbThrowMemoryException();
		}
	}
}


//--------------------------------------------
//^^ The assignment operator
//^ Copies the contained objects.  Deletion of the rhs set will not effect the
//  lhs set.  (Same as copy constructor.)
//
template<class T>
vdbSet<T>& vdbSet<T>::operator=( const vdbSet<T>& rhs )
{
	if ( this == &rhs )
		return *this;
	
	ClearAll();
	free( _pObjects );

	_next = rhs._next;
	_size = rhs._size;
	
	_pObjects = (T**) malloc( sizeof(T*) * _size );
	for ( int i = 0; i < _next; i++ )
	{
		T* p = (T*) rhs._pObjects[i];
		if ( p == NULL )
			_pObjects[i] = NULL;
		else
		{
			_pObjects[i] = new T( *p );
			if ( _pObjects[i] == 0 ) vdbThrowMemoryException();
		}
	}

	return *this;
}


//--------------------------------------------
//^ The concatenation operator copies each member of the right hand side and
//  adds the copied objects to the left hand side
//
template<class T>
const vdbSet<T>& vdbSet<T>::operator+= ( const vdbSet<T>& rhs )
{
	for ( int i = 0; i < rhs.Count(); i++ )
	{
		T* pItem = &rhs[i];
		if ( pItem == NULL )
			Add( NULL );
		else
			Add( new T( rhs[i] ) );
	}

	return *this;
}
	

//=============================================================================
// Public member functions
//=============================================================================

//--------------------------------------------
//^ The Add function adopts the object passed in the parameter list.  If there
//  is insufficient space in the array to hold this pointer, more space is
//  allocated.
//> pItem is the object to add to the array.  This object must have been
//  created on the heap.  The object may be a NULL pointer.
//< returns the 0-based index of the position in the array at which this object
//  was added.  Returns -1 and displays an error message if the array expansion
//  failed due to lack of memory.
//
template<class T>
int vdbSet<T>::Add( T* pItem )
{
	if ( _next == _size )
	{
		int newSize = static_cast<int>( _size * 1.5 );
		T** newPointer = 0;
		newPointer = (T**) realloc( _pObjects, sizeof(T*) * newSize );
		if ( newPointer == 0 )
		{
			vdbMessageBox( NULL, _T("Out of memory while expanding set"), _T("vdbSet"), MB_OK );
			vdbThrowMemoryException();
		}
		_pObjects = newPointer;
		_size = newSize;

		// zero fill the newly allocated slots
		for ( int i = _next; i < _size; i++ )
			_pObjects[i] = 0;
	}

	// adopt
	_pObjects[_next] = pItem;
	_next++;
	return _next - 1;
}


//--------------------------------------------
//^ The InsertAt function adopts the object passed in the parameter list inserting
//  it at the requested zero-based index position and moving all existing items
//  in the list down.
//  If there is insufficient space in the array to hold this pointer, more space is
//  allocated.
//> index is the zero-based index of the desired position.  It must be not be
//  less than zero or greater than 1 plus the last item in the list
//> pItem is the object to add to the array.  This object must have been
//  created on the heap.  The object may be a NULL pointer.
//< returns the 0-based index of the position in the array at which this object
//  was added.  Returns -1 if the requested index position is invalid or if out of memory.
//
template<class T>
int vdbSet<T>::InsertAt( int index, T* pItem )
{
	if ( index < 0 )
		return -1;
	if ( index > _next )
		return -1;

	if ( _next == _size )
	{
		int newSize = static_cast<int>( _size * 1.5 );
		T** newPointer = 0;
		newPointer = (T**) realloc( _pObjects, sizeof(T*) * newSize );
		if ( newPointer == 0 )
		{
			vdbMessageBox( NULL, _T("Out of memory while expanding set"), _T("vdbSet"), MB_OK );
			vdbThrowMemoryException();
		}
		_pObjects = newPointer;
		_size = newSize;

		// zero fill the newly allocated slots
		for ( int i = _next; i < _size; i++ )
			_pObjects[i] = 0;
	}

	// move everything down by one
	for ( int i = _next; i >= index; i-- )
		_pObjects[i+1] = _pObjects[i];

	// adopt
	_pObjects[index] = pItem;
	_next++;
	return index;
}


//--------------------------------------------
//^^ The operator[] function
//^ Returns a reference to the object at the specified index number.
//> index is the zero-based item in the array to retrieve.
//< returns a reference to the object at the specified index number
//
template<class T>
T& vdbSet<T>::operator[]( int index ) const
{
	RangeCheck( index, _next, this );
	T* p = (T*) _pObjects[index];
	return *p;
}


//--------------------------------------------
//^ The Get function return a reference to the object at the specified
// index number. Same as operator[].
//> index is the zero-based item in the array to retrieve.
//< returns a reference to the object at the specified index number
//
template<class T>
T& vdbSet<T>::Get( int index ) const
{
	RangeCheck( index, _next, this );
	T* p = (T*) _pObjects[index];
	return *p;
}


//--------------------------------------------
//^ The Replace function replaces the object at the specified index with a new
//  object to be managed by the set.
//> index is the zero-based item in the array to replace
//> pReplacement is a pointer to new object to be managed by the set.  The
//  caller must relinquish ownership of this pointer to the set.  This may
//  be a NULL pointer
//< returns a pointer to the old object at the index position.  The caller is
/// responsible for deleting the old object
//
template<class T>
T* vdbSet<T>::Replace( int index, T* pReplacement )
{
	RangeCheck( index, _next, this );
	T* p = (T*) _pObjects[index];

	_pObjects[index] = pReplacement;

	return p;
}


//--------------------------------------------
//^ The Remove function removes a single object from the set, deletes its
//  memory, and shifts all items above it down.	 Note that external saved
//  pointers to objects in the set may now point to the wrong object.  Be
//  sure to use Get or operator[] again to refetch these pointers.
//> index is the zero-based item of the object to remove.
//
template<class T>
void vdbSet<T>::Remove( int index )
{
	RangeCheck( index, _next, this );
	
	if ( _pObjects[index] != NULL )
		delete _pObjects[index];

	for ( int i = index; i < (_next - 1); i++ )
		_pObjects[i] = _pObjects[i+1];

	_pObjects[_next - 1] = 0;
	_next = _next - 1;
}
	

//--------------------------------------------
//^ The Orphan function removes a single object from the set and shifts all
//  items above it down.  Note that external saved pointers to objects in the
//  set may now point to the wrong object.  Be sure to use Get or operator[]
//  again to refetch these pointers.
//> index is the zero-based item of the object to orphan.
//< returns the pointer to the object that was removed from the set.  Ownership
//  of the object is given to the caller; thus it is the caller's responsibility
//  to delete the item.
//
template<class T>
T* vdbSet<T>::Orphan( int index )
{
	RangeCheck( index, _next, this );
	T* p = (T*) _pObjects[index];

	for ( int i = index; i < (_next - 1); i++ )
		_pObjects[i] = _pObjects[i+1];

	_pObjects[_next - 1] = 0;
	_next = _next - 1;

	return p;	
}


//--------------------------------------------
//^ The Swap function exchanges the objects at index1 and index2 by exchanging
//  their pointers in the set.
//> index1 is the zero-based item of the first object to exchange.
//> index2 is the zero-based item of the second object to exchange.
//
template<class T>
void vdbSet<T>::Swap( int index1, int index2 )
{
	RangeCheck( index1, _next, this );
	RangeCheck( index2, _next, this );

	// nothing to do?
	if ( index1 == index2 )
		return;

	T* p1 = (T*) _pObjects[index1];
	T* p2 = (T*) _pObjects[index2];
	_pObjects[index1] = p2;
	_pObjects[index2] = p1;
}


//--------------------------------------------
//^ The ClearAll function deletes all objects from the set.
//
template<class T>
void vdbSet<T>::ClearAll()
{
	for ( int i = 0; i < Count(); i++ )
	{
		if ( _pObjects[i] != NULL )
			{ delete _pObjects[i]; _pObjects[i] = 0; }
	}
	_next = 0;
}
	

//--------------------------------------------
//^ The Find function finds the object pointed to and returns its index position
//  or -1 if not found.
//
template<class T>
int vdbSet<T>::Find( T* pItem )
{
	for ( int i = 0; i < Count(); i++ )
		if ( _pObjects[i] == pItem )
			return i;
	return -1;
}


