//=============================================================================
//
// FILE NAME:       vdbPtr.h
//
// CONTENTS:        Smart pointer template
//
//=============================================================================

#ifndef __vdbPtr_h
#define __vdbPtr_h

#ifndef NO_OID_CONSTRUCTOR
	#ifndef __vdbTypedef_h
		#include "vdbTypedef.h"
	#endif
#endif


//=============================================================================
// template declaration
//=============================================================================
template <class T> class vdbPtr
{
public:
	// essential class functions	
	vdbPtr();										// basic constructor
	vdbPtr( T* pObject );							// constructor with auto-adopt
	~vdbPtr();										// destructor

	#ifndef NO_OID_CONSTRUCTOR
		vdbPtr( OID oid );							// constructor with database lookup and auto-adopt
	#endif

private:
	vdbPtr( const vdbPtr<T>& rhs );					// copy constructor disabled
	vdbPtr<T>& operator=( const vdbPtr<T>& rhs );	// assignment operator disabled 

public:
	// member access fuctions
	void Adopt( T* pObject );
	inline T* operator->();
	inline operator T*();
	inline T& operator*();
	inline T& operator[]( int i );
	inline bool IsValid();
	inline bool IsInvalid();
	inline void Release();

private:
	T* _pObject;
};


//=============================================================================
// member access fuctions
//=============================================================================

template<class T>
inline
T* vdbPtr<T>::operator->()
{
	return _pObject; 
}


template<class T>
inline
vdbPtr<T>::operator T*()
{
	return _pObject; 
}


template<class T>
inline
T& vdbPtr<T>::operator*()
{
	return *_pObject; 
}


template<class T>
inline
T& vdbPtr<T>::operator[]( int i )
{
	return _pObject[i]; 
}


//--------------------------------------------
// does this point to a non-NULL object?
template<class T>
inline
bool vdbPtr<T>::IsValid()
{
	return ( _pObject != 0 ? true : false ); 
}


//--------------------------------------------
// does this point to a NULL object?
template<class T>
inline
bool vdbPtr<T>::IsInvalid()
{
	return ( _pObject == 0 ? true : false ); 
}


//--------------------------------------------
// Release this object so that a new object can be adopted
template<class T>
inline
void vdbPtr<T>::Release()
{
	delete _pObject;
	_pObject = 0;
}


#endif  // vdbPtr_h

