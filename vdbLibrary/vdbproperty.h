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

#ifndef __vdbProperty_h
#define __vdbProperty_h


//=============================================================================
// template declaration
//=============================================================================
template <class T> class vdbProperty
{
public:
	// essential class functions	
	vdbProperty();										// basic constructor
	~vdbProperty();										// destructor
	vdbProperty( const vdbProperty<T>& rhs );			// copy constructor
	vdbProperty<T>& operator=( const vdbProperty<T>& rhs );	// assignment operator 

public:
	vdbProperty<T> ( const T& t );						// conversion from T to Property<T>
	T operator()() const;								// get value
	void operator()( const T& value );					// set value

private:
	T _object;
};

#endif  // vdbProperty_h

