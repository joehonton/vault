//=============================================================================
//
// FILE NAME:       vdbPtr.cpp
//
// CONTENTS:        Smart pointer template
//
//=============================================================================

#ifdef USE_PRE_HDR
	#include "vdbHeader.h"
#else
	#include <vdbAssert.h>
	#include "boolean.h"
	#include "vdbPtr.h"
#endif


//=============================================================================
//  essential class functions
//=============================================================================

//--------------------------------------------
// basic constructor
template<class T>
vdbPtr<T>::vdbPtr()
	: _pObject( 0 )
{
}


//--------------------------------------------
// constructor with auto-adopt
template<class T>
vdbPtr<T>::vdbPtr( T* pObject )
{
   _pObject = pObject;
}


//--------------------------------------------
// constructor with database lookup and auto-adopt
#ifndef NO_OID_CONSTRUCTOR
	template<class T>
	vdbPtr<T>::vdbPtr( OID oid )
		: _pObject( 0 )
	{
		if ( oid == 0 )
			return;

		try
		{
			_pObject = new T( oid );
		}
		catch( ... )
		{
		}
	}
#endif


//--------------------------------------------
// destructor
template<class T>
vdbPtr<T>::~vdbPtr()
{
	delete _pObject;
	_pObject = 0;
}


//--------------------------------------------
// copy constructor
template<class T>
vdbPtr<T>::vdbPtr( const vdbPtr<T>& rhs )
{
	assert( "not allowed to copy vdbPtr" );
}


//--------------------------------------------
// assignment operator
template<class T>
vdbPtr<T>& vdbPtr<T>::operator=( const vdbPtr<T>& rhs )
{
	assert( "not allowed to assign vdbPtr" );
	return *this;
}


//--------------------------------------------
// adoption of pointer
template<class T>
void vdbPtr<T>::Adopt( T* pObject )
{
	if ( _pObject != 0 )
		Release();

	_pObject = pObject;
}




