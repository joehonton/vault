//=============================================================================
//
// CLASS:           vdbSetSort
//
// FILE NAME:       vdbSetSort.h
//
// PURPOSE:         Sort a vdbSet
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    09/06/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#ifndef __vdbSetSort_h
#define __vdbSetSort_h

#ifndef __vdbSet_h
	#include <vdbSet.h>
#endif


template <class T> class vdbSetSort
{
public:
	// essential class functions
	vdbSetSort();
	vdbSetSort( vdbSet<T>& dataset );
	~vdbSetSort();										// destructor
	vdbSetSort( const vdbSetSort& rhs );				// copy constructor
	vdbSetSort& operator=( const vdbSetSort& rhs );		// assignment operator

public:
	void SetData( vdbSet<T>& dataset );
	int Count() const;
	int operator[]( int which ) const;
	int Get( int which ) const;

public:
	// operations
	void ShellSort();
	void SelectionSort();
	void Reverse();

private:
	// members
	vdbSet<T>* _pDataset;	// The collection to be sorted
	int _numPoints;			// size of the set
	int* _indexes;			// The array of zero-based indexes containing the results of the sort
};

#endif  // __vdbSetSort_h

