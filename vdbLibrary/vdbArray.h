//=============================================================================
//
// CLASS:           vdbArray
//
// OVERVIEW:		The vdbArray class is used to manage an array which can
//                  automatically expand in size.
//
// FILE NAME:       vdbArray.h
//
// INITIAL DATE:    09/13/2003
//
// COPYRIGHT:		Copyright © 2003 by Joe Honton
//
//=============================================================================

#ifndef __vdbArray_h
#define __vdbArray_h

#include <istream>
#include <ostream>

#ifndef __vdbStreamHeader_h
	#include "vdbStreamHeader.h"
#endif

#ifndef _INC_MALLOC
	#include <malloc.h>
#endif

#ifndef __vdbAssert_h
	#include "vdbAssert.h"
#endif

template <class T> class vdbArray;


//=============================================================================
template <class T> class vdbArray
{
public:
	// essential class functions
	vdbArray( int initialSize = 32 );					// basic constructor
	~vdbArray();										// destructor
	vdbArray( const vdbArray<T>& rhs );					// copy constructor 
	vdbArray<T>& operator=( const vdbArray<T>& rhs );	// assignment operator 

public:
	// public member functions
	T& operator[]( int index );
	T& operator[]( int index ) const;
	T& Get( int index );
	void Set( int index, const T& rhs );
	int Add( const T& rhs );
	void Remove( int index );
	int Resize();
	int Resize( int newSize );
	inline const int Count() const;
	inline int ResetCount( int newCount );

private:
	T* _pObjects;								// an array of elements of type T

private:
	int _size;									// number of elements allocated
	int _count;									// number of valid array elements (automatically updated whenever Set() references a larger value)

	// io stream
//	friend std::ostream& operator<< <> ( std::ostream& os, const vdbArray<T>& obj );
//	friend std::istream& operator>> <> ( std::istream& is, vdbArray<T>& obj );
};


//=============================================================================
// inline functions
//=============================================================================

//--------------------------------------------
//^ The Count function return a count of the number of elements currently in the
//  set.
//< returns the count.
//
template<class T>
inline
const int vdbArray<T>::Count() const
{
	return _count;
}


//--------------------------------------------
//^ The ResetCount function sets the count of valid elements to a user-specified value
//> newCount is the new number of valid array elements
//< returns the new count
template<class T>
inline
int vdbArray<T>::ResetCount( int newCount )
{
	if ( newCount < 0 )
		_count = 0;
	
	else if ( newCount <= _size )
		_count = newCount;

	return _count;
}


//=============================================================================
// non-member stream functions
//=============================================================================

//------------------------------------------
//^^ ostream << operator
template<class T>
std::ostream& operator<< ( std::ostream& os, const vdbArray<T>& obj )
{
	unsigned int count = obj.Count();
	os << count << std::endl;

	for ( int i = 0; i < count; i++ )
		os << obj[i];

	return os;
}


//------------------------------------------
//^^ istream >> operator
//^  All classes of type T must have a default constructor. 
//
template<class T>
std::istream& operator>> ( std::istream& is, vdbArray<T>& obj )
{
	if ( is.fail() )
		return is;

	unsigned int count;
	is >> count >> std::ws;
	assert( count >= 0 );

	obj.Resize( count );
	for ( int i = 0; i < count; i++ )
		is >> obj[i];

	return is;
}

#endif // __vdbArray_h



