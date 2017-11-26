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
// FILE NAME:       vdbSet.h
//
// INITIAL DATE:    11/30/94
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef __vdbSet_h
#define __vdbSet_h

#ifndef __vdbStreamHeader_h
	#include "vdbStreamHeader.h"
#endif

#ifndef _INC_MALLOC
	#include <malloc.h>
#endif

#ifndef __vdbAssert_h
	#include "vdbAssert.h"
#endif

#include <ostream>
#include <istream>

// forward declarations
template <class T> class vdbSet;
template <class T> std::ostream& operator<< ( std::ostream& os, const vdbSet<T>& obj );
template <class T> std::istream& operator>> ( std::istream& is, vdbSet<T>& obj );


//=============================================================================
template <class T> class vdbSet
{
public:
	// essential class functions
	vdbSet( int initialSize = 32 );					// basic constructor
	~vdbSet();										// destructor
	vdbSet( const vdbSet<T>& rhs );					// copy constructor 
	vdbSet<T>& operator=( const vdbSet<T>& rhs );	// assignment operator 

	const vdbSet<T>& operator+= ( const vdbSet<T>& rhs );	// concatenation

public:
	// public member functions
	int Add( T* pItem );
	int InsertAt( int index, T* pItem );
	T& operator[]( int index ) const;
	T& Get( int index ) const;
	T* Replace( int index, T* pReplacement );
	void Remove( int index );
	T* Orphan( int index );
	void Swap( int index1, int index2 );
	void ClearAll();
	int Find( T* pItem );
	inline const int Count() const;

private:
	T** _pObjects;								// a variable length array of pointers to objects

private:
	// member data
	int _size;									// number of slots allocated
	int _next;									// next available slot

	friend std::ostream& operator<< <T> ( std::ostream& os, const vdbSet<T>& obj );
	friend std::istream& operator>> <T> ( std::istream& is, vdbSet<T>& obj );
};


//=============================================================================
// inline functions
//=============================================================================

//--------------------------------------------
//^ The Count function return a count of the number of objects currently in the
//  set.
//< returns the count.
//
template<class T>
inline
const int vdbSet<T>::Count() const
{
	return _next;
}


//=============================================================================
// non-member stream functions
//=============================================================================

//------------------------------------------
//^^ ostream << operator
template<class T>
std::ostream& operator<< ( std::ostream& os, const vdbSet<T>& obj )
{
	os << obj._next << std::endl;
	for ( int i = 0; i < obj.Count(); i++ )
		os << obj.Get(i);

	return os;
}


//------------------------------------------
//^^ istream >> operator
//^  All classes of type T must have a default constructor. 
//^  Any items of type T currently owned by this set are left in the set.
//   Thus the extraction operator can be used to concatenate two sets.
//
template<class T>
std::istream& operator>> ( std::istream& is, vdbSet<T>& obj )
{
	if ( is.fail() )
		return is;

	int inputNext;
	is >> inputNext >> std::ws;
	assert( inputNext >= 0 );

	for ( int i = 0; i < inputNext; i++ )
	{
		T* pItem = new T;
		is >> *pItem;
		if ( is.fail() )
			{ delete pItem; pItem = 0; break; }
		obj.Add( pItem );
	}

	return is;
}

#endif // __vdbSet_h



