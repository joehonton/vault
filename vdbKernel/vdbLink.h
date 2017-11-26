//=============================================================================
//
// CLASS:           vdbLink
//
// ESSENCE:         A template class for managing a single related object
//                  with these attributes:
//				       optional
//		               load on demand
//				       one
//				       break link
//				       shallow copy
//
// FILE NAME:       vdbLink.cpp
//
// INITIAL DATE:    9/17/95
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef __vdbLink_h
#define __vdbLink_h

#include "boolean.h"

//=============================================================================
// template declaration
//=============================================================================
template <class T> class vdbLink
{
public:
	// essential class functions	
	vdbLink();										// basic constructor
	virtual ~vdbLink();								// destructor
	vdbLink( const vdbLink<T>& rhs );				// copy constructor
	vdbLink<T>& operator=( const vdbLink<T>& rhs );	// assignment operator

public:
	// initialization
	inline void Initialize( vdbObject* pClient, OID* pID );

public:
	// member access fuctions
	inline T* operator->();		// indirection operator
	inline T& operator*();		// dereference operator
	inline operator T*();		// conversion to T* operator
	inline operator T&();		// conversion to T& operator

public:
	// status functions
	inline bool IsValid();
	inline bool IsInvalid();

private:
	// subject creation
	inline void CreateSubject();

private:
	// subject access
	T* This();

private:
	// data members
	T* _pSubject;				// pointer to the subject managed by this class
	OID* _pID;					// pointer to the variable in the client which is OID of the subject being managed
	vdbObject* _pClient;		// pointer to the client object
};


//=============================================================================
// member access fuctions
//=============================================================================

//----------------------------
//^ The Initialize function obtains the address of the 
//  client and the client's child OID.  This must be called
//  after the constructor and before the first attempt to 
//  access the subject.
//
template<class T>
inline
void vdbLink<T>::Initialize( vdbObject* pClient, OID* pID )
{
	assert( pClient != 0 );
	_pClient = pClient;

	assert( pID != 0 );
	_pID = pID;
}


//----------------------------
//^^ The indirection operator.
//  Returns a pointer to the subject being managed.
//
template<class T>
inline
T* vdbLink<T>::operator->()
{
	return This();
}


//--------------------------------------------
//^^ The dereference operator.
//  Returns a reference to the subject being managed.
//
template<class T>
inline
T& vdbLink<T>::operator*()
{
	return *This();
}


//--------------------------------------------
//^^ The conversion to T* operator.
//  Returns a pointer to the subject being managed.
//
template<class T>
inline
vdbLink<T>::operator T*()
{
	return This();
}


//--------------------------------------------
//^^ The conversion to T& operator.
//  Returns a reference to the subject being managed.
//
template<class T>
inline
vdbLink<T>::operator T&()
{
	return *This();
}


//--------------------------------------------
//^ The IsValid function is called to determine whether or not
//  the subject being managed has been loaded into memory yet.
//
//< Returns true if it has been loaded into memory.
//< Returns false if it has not been loaded into memory.  Do not
//  use _pSubject if this returns false.
//
template<class T>
inline
bool vdbLink<T>::IsValid()
{
	return ( _pSubject != 0 ? true : false ); 
}


//--------------------------------------------
//^ The IsInvalid function is called to determine whether or not
//  the subject being managed has been loaded into memory yet.
//
//< Returns true if it has not been loaded into memory.  Do not
//  use _pSubject if this returns true.
//< Returns false if it has been loaded into memory.
//
template<class T>
inline
bool vdbLink<T>::IsInvalid()
{
	return ( _pSubject == 0 ? true : false ); 
}


//--------------------------------------------
//^ The CreateSubject function constructs a new subject.
//  The "This" function calls NewSubject if a persistent
//  database object can't be retrieved or if the client's
//  "subjectID" is 0.  The standard CreateSubject function
//  creates a new subject of "vdbObject-derived type T" with
//  a unique OID.
//
//^ Create your own specialization of this member function
//  if your constructor takes arguments.
// 
template<class T>
void vdbLink<T>::CreateSubject()
{
	assert( _pSubject == 0 );

	_pSubject = new T();

	assert( _pSubject != 0 );
}


#endif  // vdbLink_h

