//=============================================================================
//
// CLASS:           vdbProperty
//
// FILE NAME:       vdbProperty.cpp
//
// PURPOSE:         Napier device.  See "C++ Report" Nov-Dec 1995, pp29-33.
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    2/9/96
//
// COPYRIGHT:       Copyright © 1996 by Joe Honton
//
//=============================================================================

#include <vdbProperty.h>



//=============================================================================
//  essential class functions
//=============================================================================

//--------------------------------------------
// default constructor
template<class T>
vdbProperty<T>::vdbProperty()
{
}


//--------------------------------------------
// destructor
template<class T>
vdbProperty<T>::~vdbProperty()
{
}


//--------------------------------------------
// copy constructor
template<class T>
vdbProperty<T>::vdbProperty( const vdbProperty<T>& rhs )
{
	_object = rhs._object;
}


//--------------------------------------------
// assignment operator
template<class T>
vdbProperty<T>& vdbProperty<T>::operator=( const vdbProperty<T>& rhs )
{
	if ( this == &rhs )
		return *this;
	
	_object = rhs._object;
	return *this;
}


//--------------------------------------------
// conversion from T to Property<T>
template<class T>
vdbProperty<T>::vdbProperty( const T &t )
	: _object( t )
{
}


//--------------------------------------------
// get value
template<class T>
T vdbProperty<T>::operator()() const
{
	return _object;
}


//--------------------------------------------
// set value
template<class T>
void vdbProperty<T>::operator()( const T& value )
{
	_object = value;
}

