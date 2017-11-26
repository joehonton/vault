//=============================================================================
//
// CLASS:           vdbLinkSet
//
// ESSENCE:         A template class for managing a set of related objects
//
// FILE NAME:       vdbLinkSet.h
//
// INITIAL DATE:    11/14/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef __vdbLinkSet_h
#define __vdbLinkSet_h

#ifndef __boolean_h
	#include "boolean.h"
#endif

#ifndef __vdbTypedef_h
	#include "vdbTypedef.h"
#endif

#ifndef __vdbSet_h
	#include "vdbSet.h"
#endif

#ifndef __vdbObject_h
	#include "vdbObject.h"
#endif

class vdbDatabase;


//=============================================================================
// template declaration
//=============================================================================
template <class T> class vdbLinkSet
{
public:
	// essential class functions	
	vdbLinkSet( HANDLE_DATABASE hDB = 0 );					// basic constructor
	virtual ~vdbLinkSet();									// destructor
	vdbLinkSet( const vdbLinkSet<T>& rhs );					// copy constructor
	vdbLinkSet<T>& operator=( const vdbLinkSet<T>& rhs );	// assignment operator

public:
	// member access
	inline vdbDatabase* GetDatabase();

public:
	// initialization
	inline void Initialize( vdbObject* pClient );
	void SortCriteria( const char* szColumnName );

public:
	// vdbSet cover functions
	inline void ClearAll();
	inline T& operator[]( int index );
	inline T& Get( int index );
	int Insert( T* pSubject );
	RETCODE Save();
	RETCODE Delete( int index );
	RETCODE DeleteAll( bool bAllOrNothing );
	inline int Count();

public:
/*
	// member access fuctions
	inline vdbSet<T>* operator->();		// indirection operator
	inline vdbSet<T>& operator*();		// dereference operator
	inline operator vdbSet<T>*();		// conversion to vdbSet<T>* operator
	inline operator vdbSet<T>&();		// conversion to vdbSet<T>& operator
*/
public:
	// status functions
	inline bool IsValid();
	inline bool IsInvalid();

private:
	// subject access
	vdbSet<T>* This();

private:
	// data members
	HANDLE_DATABASE _hDB;		// database handle
	vdbDatabase* _pDatabase;	// the ODBC environment and database
	vdbSet<T>* _pSubjects;		// pointer to the subjects managed by this class
	vdbObject* _pClient;		// pointer to the client object
	char* _szSortColumn;		// char pointer to the column to sort on
};


//==========================================================================================
// inlines
//==========================================================================================

//-------------------------------------
template<class T>
inline vdbDatabase* vdbLinkSet<T>::GetDatabase()
{
	return _pDatabase;
}


//----------------------------
//^ The Initialize function obtains the address of the client.  This must be
//  called after the constructor and before the first attempt to access any subject.
//
template<class T>
inline
void vdbLinkSet<T>::Initialize( vdbObject* pClient )
{
	assert( pClient != 0 );
	_pClient = pClient;
}

/*

//----------------------------
//^^ The indirection operator.
//  Returns a pointer to the set of subjects being managed.
//
template<class T>
inline
vdbSet<T>* vdbLinkSet<T>::operator->()
{
	return This();
}


//--------------------------------------------
//^^ The dereference operator.
//  Returns a reference to the set of subjects being managed.
//
template<class T>
inline
vdbSet<T>& vdbLinkSet<T>::operator*()
{
	return *This();
}

//--------------------------------------------
//^^ The conversion to vdbSet<T>* operator.
//  Returns a pointer to the set of subjects being managed.
//
template<class T>
inline
vdbLinkSet<T>::operator vdbSet<T>*()
{
	return This();
}


//--------------------------------------------
//^^ The conversion to vdbSet<T>& operator.
//  Returns a reference to the set of subjects being managed.
//
template<class T>
inline
vdbLinkSet<T>::operator vdbSet<T>&()
{
	return *This();
}
*/

//--------------------------------------------
//^ The IsValid function is called to determine whether or not
//  the subjects being managed have been loaded into memory yet.
//
//< Returns true if they have been loaded into memory.
//< Returns false if they have not been loaded into memory.  Do not
//  use _pSubjects if this returns false.
//
template<class T>
inline
bool vdbLinkSet<T>::IsValid()
{
	return ( _pSubjects != 0 ? true : false ); 
}


//--------------------------------------------
//^ The IsInvalid function is called to determine whether or not
//  the subjects being managed have been loaded into memory yet.
//
//< Returns true if they have not been loaded into memory.  Do not
//  use _pSubjects if this returns true.
//< Returns false if they have been loaded into memory.
//
template<class T>
inline
bool vdbLinkSet<T>::IsInvalid()
{
	return ( _pSubjects == 0 ? true : false ); 
}

/*
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
void vdbLinkSet<T>::CreateSubject()
{
	assert( _pSubjects == 0 );

	_pSubject = new T();

	assert( _pSubjects != 0 );
}

*/


//--------------------------------------------
//^ The ClearAll function clears the set of subjects
//  and saves them to the DBMS.  Subsequent accesses
//  will cause This() to reretrieve them.
//
template<class T>
inline
void vdbLinkSet<T>::ClearAll()
{
	if ( _pSubjects == 0 )
		return;

	This()->ClearAll();
			
}

//--------------------------------------------
//^ The operator[] accesses the subject at the 
//  zero-based index into the set of subjects.
//< The return value is a reference to the subject
//  at that index.
//
template<class T>
inline
T& vdbLinkSet<T>::operator[]( int index )
{
	vdbSet<T>* pThis = This();
	return (*pThis)[index];
			
}


//--------------------------------------------
//^ The Get function returns a reference to the object at the specified
// index number. Same as operator[].
//
template<class T>
T& vdbLinkSet<T>::Get( int index )
{
	return This()->Get( index );
}


//--------------------------------------------
//^ The Count function returns a count of the number of
//  subjects that are associated with this client.
//< the return value is the count
//  
template<class T>
inline
int vdbLinkSet<T>::Count()
{
	return This()->Count();
}

#endif  // vdbLinkSet_h

