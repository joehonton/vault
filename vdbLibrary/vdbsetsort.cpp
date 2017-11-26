//=============================================================================
//
// CLASS:           vdbSetSort
//
// FILE NAME:       vdbSetSort.cpp
//
// PURPOSE:         Sort a vdbSet
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    02/26/97
//
// COPYRIGHT:       Copyright © 1997 by Joe Honton
//
//=============================================================================

#include "vdbSetSort.h"
#include <vdbAssert.h>
#include <math.h>
#include <vdbException.h>

//#pragma warning( disable : 4244 )


//=============================================================================
// Essential Class functions
//=============================================================================

//-------------------------------------
//^^ The default constructor
//> You must call SetData before calling any other operation when using this
//  constructor.
//
template<class T>
vdbSetSort<T>::vdbSetSort()
	: _pDataset( 0 ),
	  _indexes( 0 )
{
	_numPoints = 0;
}


//-------------------------------------
//^^ The vdbSetSort( vdbSet<T>& dataset ) constructor
//> dataset is the vdbSet to be sorted
//
template<class T>
vdbSetSort<T>::vdbSetSort( vdbSet<T>& dataset )
	: _pDataset( &dataset ),
	  _indexes( 0 )
{
	_numPoints = dataset.Count();

	if ( _numPoints > 0 )
	{
		_indexes = new int[_numPoints];
		if ( _indexes == 0 ) throw vdbMemoryException();
		for ( int i = 0; i < _numPoints; i++ )
			_indexes[i] = 0;
	}
}


//--------------------------------------------
// destructor
template<class T>
vdbSetSort<T>::~vdbSetSort()
{
	delete[] _indexes; _indexes = 0;
}


//--------------------------------------------
// copy constructor
template<class T>
vdbSetSort<T>::vdbSetSort( const vdbSetSort& rhs )
	: _pDataset( rhs._pDataset ),
	  _numPoints( rhs._numPoints )
{
	_indexes = new int[_numPoints];
	if ( _indexes == 0 ) throw vdbMemoryException();
}


//--------------------------------------------
// assignment operator
template<class T>
vdbSetSort<T>& vdbSetSort<T>::operator=( const vdbSetSort& rhs )
{
	if ( this == &rhs )
		return *this;
	
	_pDataset = rhs._pDataset;
	_numPoints = rhs._numPoints;

	delete[] _indexes; _indexes = 0;
	_indexes = new int[_numPoints];
	if ( _indexes == 0 ) throw vdbMemoryException();
	for ( int i = 0; i < _numPoints; i++ )
		_indexes[i] = rhs._indexes[i];
	
	return *this;
}


//=============================================================================
// Member access functions
//=============================================================================

//-------------------------------------
template<class T>
void vdbSetSort<T>::SetData( vdbSet<T>& dataset )
{
	_pDataset = &dataset;

	if ( _indexes == 0 )
		{ delete[] _indexes; _indexes = 0; }

	_numPoints = dataset.Count();

	if ( _numPoints > 0 )
	{
		_indexes = new int[_numPoints];
		if ( _indexes == 0 ) throw vdbMemoryException();
		for ( int i = 0; i < _numPoints; i++ )
			_indexes[i] = 0;
	}
}


//-------------------------------------
//^ The Count function is a convenience function to return the size of the
//  dataset.
//< returns the number of points in the dataset.
//
template<class T>
int vdbSetSort<T>::Count() const
{
	return _numPoints;
}


//--------------------------------------------
//^^ The operator[] function
//^ Returns the zero-based index into the dataset at the requested sort
//  position.
//> which is the zero-based item of the sort array to retrieve.
//< returns a zero-based index into the dataset.
//
template<class T>
int vdbSetSort<T>::operator[]( int which ) const
{
	assert ( which >= 0 );
	assert ( which < _numPoints );
	return _indexes[which];
}


//--------------------------------------------
//^ The Get function returns the zero-based index into the dataset at the
//  requested sort position.
//> which is the zero-based item of the sort array to retrieve.
//< returns a zero-based index into the dataset.
//
template<class T>
int vdbSetSort<T>::Get( int which ) const
{
	assert ( which >= 0 );
	assert ( which < _numPoints );
	return _indexes[which];
}


//=============================================================================
// Operations
//=============================================================================

//------------------------------------------
//^ The ShellSort algorithm from Sedgewick
//  This algorithm uses the T::operator> for comparisons
//< Upon completion _indexes contains a sorted list of indexes into _pDataset
//
template<class T>
void vdbSetSort<T>::ShellSort()
{
	int i, j, h, t;
	int v;
	int r = _numPoints - 1;	// the last element in the array

	// set the _indexes to the original unsorted order of the array
	for ( i = 0; i < _numPoints; i++ )
		_indexes[i] = i;

	// adjust t for the size of the dataset
	for (t = 1; 4*t*t <= r; t += t) ;

	// outer sort, ending in a 1-sort
	for ( h = r/4; t > 0; t /= 2, h = t*t - (3*t)/2 + 1)
	{
		for ( i = h; i <= r; i++ )
		{ 
			v = _indexes[i];
			j = i;
			
			while ( j >= h && _pDataset->Get(v) < _pDataset->Get( _indexes[j-h] ) )
			{
				_indexes[j] = _indexes[j-h];
				j -= h;
			}
			
			_indexes[j] = v;
		}
	}
}


//------------------------------------------
//^ The SelectionSort algorithm is taken from "Algorithms in C++", p98, by Robert
//  Sedgewick, 1992. Published by Addison Wesley.
//
template<class T>
void vdbSetSort<T>::SelectionSort()
{
	int i, j, min;
	
	for ( i = 0; i < _numPoints; i++ )
		_indexes[i] = i;

	for ( i = 0; i < _numPoints-1; i++ )
	{
		min = i;
		for ( j = i+1; j <= _numPoints-1; j++ )
			if ( _pDataset->Get( _indexes[j] ) < _pDataset->Get( _indexes[min] ) )
				min = j;
			
		int t = _indexes[min];
		_indexes[min] = _indexes[i];
		_indexes[i] = t;
	}
}


//------------------------------------------
//^ The Reverse function takes the array of sorted indexes and reverses their order.
//
template<class T>
void vdbSetSort<T>::Reverse()
{
	int half = (int) floor((double)_numPoints/2);
	for ( int i = 0; i < half; i++ )
	{
		int t = _indexes[i];
		_indexes[i] = _indexes[_numPoints-i-1];
		_indexes[_numPoints-i-1] = t;
	}
}

