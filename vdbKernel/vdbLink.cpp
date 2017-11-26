//=============================================================================
//
// CLASS:           vdbLink
//
// ESSENCE:         A template proxy class for managing related vdbObject-derived classes.
//
// FILE NAME:       vdbLink.cpp
//
// INITIAL DATE:    11/14/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//^ This class is a Proxy class.
//  "Design Patterns Elements of Reusable Object-Oriented Software", Erich Gamma, et. al.
//
//^ The players:  vdbLink<T> is the surrogate object.  Any vdbObject-derived
//  class may be the "client".  Any vdbObject-derived class may be the "subject".
//  The client must have a data member to hold an object identifier for the subject.
//
//^ Usage rules:  Place a vdbLink<T> object as a data member in a
//  vdbObject-derived class.  This class is the "client".  During construction
//  of the client, the vdbLink<T> surrogate will be instantiated.  Inside 
//  the call to the client's Initialize function, call the surrogate's Initialize
//  function to establish the linkage between the client and the surrogate.
//  All access to the subject is through the surrogate's overridden operators.
//  The first such access attempt will retreive or create the subject.  Subsequent
//  accesses will return a pointer to the subject.
// 
//^ Optional:  If no attempt to access the subject ever occurs, it is not created.
//
//^ Load on demand:  When instantiated, the subject is not loaded.
//  The subject is loaded during the client's first attempt to access it.
//
//^ One:  This surrogate class is for managing one object only. Not a collection.
//
//^ Break link:  If the client is deleted, the subject is removed from memory, 
//  but remains in persistent store.  Cascade delete may be implemented by
//  overriding the client's Delete function to call the subject's Delete function.
//
//^ Shallow copy:  When the client is copied through its copy constructor, the
//  surrogate's copy constructor is called.  The surrogate's copy constructor
//  creates a new vdbLink<T> instance with the same	_pParent and _pID.  The
//  pointer to the child is not assigned and will therefore cause the next usage
//  of This() to retrieve a new T.
//
//^ Shallow save:  When the client is saved, the subject is not implicitly saved.
//  Implict save can be accomplished by overridding the client's Insert and
//  Update functions to call the subject's Insert and Update.
//
//^ Unidirectional:  The client holds a persistent pointer to the subject.  This
//  is the client's data member which holds the subject's OID.  The subject does
//  not have any persistent pointer to the client and cannot find it's client(s).
//
//^ Direct access:  The client holds the subject's OID which is the DBMS's
//  primary key to the subject's table.  Thus all access is direct access
//  through this key.
//  
//^ Orphans allowed:  If all client's are deleted, the subject lives on.  The 
//  inverse of this "auto-delete other side if last one" may be implemented 
//  by overridding the client's Delete function like this:  search the DBMS for
//  all rows in the client table which have a "subjectID" equal to the subject's
//  OID.  If no other such clients are found, delete the subject.
//
//^ Creation of a child object constititues modification of the parent object,
//  but simply modifying an existing child object does not constitute modification
//  of the parent.
//
//=============================================================================

#ifdef USE_PRE_HDR
	#include "vdbHeader.h"
#else
	#include <vdbAssert.h>
	#include "boolean.h"
#endif

#include "vdbLink.h"
#include "vdbQuery.h"
#include "vdbDatabase.h"

//=============================================================================
//  essential class functions
//=============================================================================

//--------------------------------------------
//^^ The basic constructor
template<class T>
vdbLink<T>::vdbLink()
	: _pSubject( 0 ),
	  _pClient( 0 ),
	  _pID( 0 )
{
}


//--------------------------------------------
//^^ The destructor
//^ Delete the subject being managed from memory.
//
template<class T>
vdbLink<T>::~vdbLink()
{
	delete _pSubject;
	_pSubject = 0;
}


//--------------------------------------------
//^^ The copy constructor.
//  This will be invoked by any vdbObject-derived class that
//  contains a vdbLink<T> object.  The semantics are:
//  The new vdbLink<T> instance is created with the same
// _pParent and _pID.  The pointer to the subject is not
//  assigned; which will therefore cause the next usage
//  of This() to retrieve a new T.
//
template<class T>
vdbLink<T>::vdbLink( const vdbLink<T>& rhs )
	: _pSubject( 0 ),
	  _pClient( rhs._pClient ),
	  _pID ( rhs._pID )
{
}


//--------------------------------------------
//^^ The assignment operator.
// The same semantics as the copy constructor.
//
template<class T>
vdbLink<T>& vdbLink<T>::operator=( const vdbLink<T>& rhs )
{
	_pSubject = 0;
	_pClient = rhs._pClient;
	_pID = rhs._pID;

	return *this;
}


//--------------------------------------------
//^ The This function return a pointer to the subject of this class.
//  If the subject has already been loaded from the DBMS, just return
//  the pointer to the subject.  If the subject has not been
//  retrieved get it's OID from the client, then if the client knew
//  what the subject's OID is, retrieve it.  If the client didn't
//  know the subject's OID, or if the client thought it knew, but it
//  couldn't be found, create a new subject, assign the new subject's
//  OID to the client's "subjectID" variable through _pID,
//  and mark the client as Modified.
//
//< Always returns a pointer to the subject being managed.
//< Side effects:  Marks the client as Modified if the subject
//  was created rather than just retreived.
//
template<class T>
T* vdbLink<T>::This()
{
	// if already loaded just return the pointer
	if ( IsValid() )
		return _pSubject;

	else
	{
		// if the subject hasn't been loaded, get it's OID from the client.
		assert( _pID != 0 );		// did you forget to call Initialize?
		OID thisOID = *_pID;

		// if the client knew what the subject's OID is, retreive it.
		if ( thisOID != 0 )
		{
			vdbQuery<T> q;
			q.SelectionCriteria( "oid", "=", thisOID );
			RETCODE rc;
			_pSubject = q.Execute( rc );
			if ( IsValid() )
				return _pSubject;
			else
				// display trouble message and then fall through
				GetDatabase()->Trouble( "Could not retrieve vdbLink subject with ID = ", thisOID );
		}
	
		// if the client didn't know the subject's OID,
		// OR if the client thought it knew, but it couldn't be found.
		CreateSubject();
		assert( _pSubject != 0 );
		*_pID =  _pSubject->GetOID();
		assert( _pClient != 0 );		// did you forget to call Initialize?
		_pClient->Modified();
		return _pSubject;
	}
}


