//=============================================================================
// CLASS:           vdbLinkSet
//
// ESSENCE:         A template proxy class for managing a set of related
//                  vdbObject-derived objects.
//
// FILE NAME:       vdbLinkSet.cpp
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
//^ The players:  vdbLinkSet<T> is the surrogate object.  Any vdbObject-derived
//  class may be the "client".  Any vdbObject-derived class may be the "subjects".
//  Each subject must have a data member to hold an object identifier for the client.
//
//^ Usage rules:  Place a vdbLinkSet<T> object as a data member in a
//  vdbObject-derived class.  This class is the "client".  During construction
//  of the client, the vdbLinkSet<T> surrogate will be instantiated.  Inside 
//  the call to the client's Initialize function, call the surrogate's Initialize
//  function to establish the linkage between the client and the surrogate.
//  All access to the subject is through the surrogate's overridden operators.
//  The first such access attempt will create a vdbSet to hold all subjects and
//  will retreive all related subjects from the DBMS.  Subsequent accesses will
//  work with this snapshot view.
// 
//^ Optional:  If no attempt to access any subject ever occurs, the vdbSet is not created.
//
//^ Load on first use:  When instantiated, the set of subjects is not loaded.
//  The	set is created and the subjects are loaded during the client's first
//  attempt to access any subject.
//
//^ Multiple:  This surrogate class is for managing a set of object.
//
//^ Break link:  If the client is deleted, the subject is removed from memory, 
//  but remains in persistent store.  Cascade delete may be implemented by
//  overriding the client's Delete function to call the subject's Delete function.
//
//^ Shallow copy:  When the client is copied through its copy constructor, the
//  surrogate's copy constructor is called.  The surrogate's copy constructor
//  creates a new vdbLinkSet<T> instance with a null _pClient and _pSubject.
//  The pointer to the child is not assigned and will therefore cause the first
//  usage of This() to retrieve a new empty set of T.
//
//^	Shallow save:  When the client is saved, the subject is not implicitly saved.
//  Implict save can be accomplished by overridding the client's Insert and
//  Update functions to call the subject's Insert and Update.
//
//^ Unidirectional:  (I don't think so 8/1/96) The client holds a persistent pointer to the subject.  This
//  is the client's data member which holds the subject's OID.  The subject does
//  not have any persistent pointer to the client and cannot find it's client(s).
//
//^ Direct access:  (I don't think so 8/1/96) The client holds the subject's OID which is the DBMS's
//  primary key to the subject's table.  Thus all access is direct access
//  through this key.
//  
//^ Orphans allowed:  If all client's are deleted, the subject lives on.  The 
//  inverse of this "auto-delete other side if last one" may be implemented 
//  by overridding the client's Delete function like this:  search the DBMS for
//  all rows in the client table which have a "subjectID" equal to the subject's
//  OID.  If no other such clients are found, delete the subject.
//
//^ Snapshot view:  The first access to any subject will retrieve all subjects
//  from the DBMS.  This is the snapshot.  Subsequent accesses will not attempt
//  to synchronize with the DBMS.  To implement "monitor changes" use a different
//  strategy completely -- one that uses DynamicQuery is closer but may still fall
//  short of complete synchronicity.
//
//
//=============================================================================

#ifdef USE_PRE_HDR
	#include "vdbHeader.h"
#else
	#include <vdbAssert.h>
	#include "boolean.h"
	#include "vdbLinkSet.h"
	#include "vdbQuery.h"
	#include "vdbDatabase.h"
	#include "vdbException.h"
	#include "vdbPool.h"
#endif


//=============================================================================
//  essential class functions
//=============================================================================

//--------------------------------------------
//^^ The basic constructor
template<class T>
vdbLinkSet<T>::vdbLinkSet( HANDLE_DATABASE hDB )
	: _pDatabase( vdbPool::Instance()->GetDatabase( hDB ) ),
	  _hDB( hDB ),
	  _pSubjects( 0 ),
	  _pClient( 0 ),
	  _szSortColumn( 0 )
{
}


//--------------------------------------------
//^^ The destructor
//^ Delete the subject being managed from memory.
//
template<class T>
vdbLinkSet<T>::~vdbLinkSet()
{
	delete _pSubjects; _pSubjects = 0;
	delete[] _szSortColumn; _szSortColumn = 0;
}


//--------------------------------------------
//^^ The copy constructor.
//  This will be invoked by any vdbObject-derived class that contains a
//  vdbLinkSet<T> object.  The semantics are:  The new vdbLinkSet<T>
//  instance is created without a _pClient; the client must therefore
//  call Initialize to set this _pClient variable to itself.  The pointer
//  to _pSubjects is not assigned;  which will therefore cause the first
//  usage of This() to retrieve a new empty set of T.
//^ This is a shallow copy.
//
template<class T>
vdbLinkSet<T>::vdbLinkSet( const vdbLinkSet<T>& rhs )
	: _pSubjects( 0 ),
	  _pClient( 0 )
{
	delete[] _szSortColumn; _szSortColumn = 0;
	if ( rhs._szSortColumn != 0 )
		SortCriteria( rhs._szSortColumn );
}


//--------------------------------------------
//^^ The assignment operator.
//  The same semantics as the copy constructor.
//
template<class T>
vdbLinkSet<T>& vdbLinkSet<T>::operator=( const vdbLinkSet<T>& rhs )
{
	_pSubjects = 0;
	_pClient = 0;
	delete[] _szSortColumn; _szSortColumn = 0;
	if ( rhs._szSortColumn != 0 )
		SortCriteria( rhs._szSortColumn );

	return *this;
}


//=============================================================================
//  Initialization functions
//=============================================================================
//--------------------------------------------
//^ The SortCriteria function registers a column name to be used as the sort
//  column when the first retrieval occurs.  Note that subsequent Inserts will
//  not maintain this sort order.
//^ This function can only be called once.  To be effective it must be called
//  before the first implicit call to This.  The best place to call this function
//  is in the client's constructor after the call to Initialize.
//> szColumnName is any valid DBMS column name for class T
//
template<class T>
void vdbLinkSet<T>::SortCriteria( const char* szColumnName )
{
	assert ( _szSortColumn == 0 );

	int len = strlen(szColumnName) + 1;
	_szSortColumn = new char[len];
	if ( _szSortColumn == 0 ) throw vdbMemoryException();
	vdbUtility::SafeCopy( _szSortColumn, szColumnName, len );
}


//=============================================================================
// vdbSet cover functions
//=============================================================================

//--------------------------------------------
//^ The This function return a pointer to the set of subjects of this class.
//  If the set has already been loaded from the DBMS, just return
//  the pointer to the set.
//
//< Always returns a pointer to the set of subjects being managed.
//
template<class T>
vdbSet<T>* vdbLinkSet<T>::This()
{
	// if already loaded just return the pointer
	if ( IsValid() )
		return _pSubjects;

	else
	{
		// if this is the first access create the set
		assert( _pClient != 0 );				// did you forget to call Initialize?
		OID clientOID = _pClient->GetOID();
		assert( clientOID > 2 );				// not a valid object ID
		_pSubjects = new vdbSet<T>;
		if ( _pSubjects == 0 ) throw vdbMemoryException();

		// now retrieve all subjects
		vdbQuery<T> q( _hDB );
		q.SelectionCriteria( q.ClientIDColumn( _pClient ), "=", clientOID );
		if ( _szSortColumn != 0 )
			q.SortCriteria( _szSortColumn );
		q.Execute( *_pSubjects );
		return _pSubjects;
	}
}


//--------------------------------------------
//^ The Insert function inserts a subject of type T into the set
//  of subjects being managed.  Its clientID is filled with the
//  client's OID.  The client is marked as Modified to insure a trigger
//  to it's Insert.
//^ The subject is adopted by the set.  Therefore all subjects to
//  be added must be created on the heap.  The set then owns the
//  subject and is responsible for saving it to the DBMS and destroying it.
//  
//< Returns the location within the set at which the object was added.
//  Be careful with storing this value for future use as calls to Delete
//  will change this index number.
//< Returns -1 if unsuccessful.
//
template<class T>
int vdbLinkSet<T>::Insert( T* pSubject )
{
	vdbSet<T>* pThis = This();

	assert( _pClient != 0 );				// did you forget to call Initialize?
	OID* pID = pSubject->ClientID( _pClient );
	if ( pID == 0 )
		return -1;
	
	// assign the client's OID to the subject's "clientID" data member
	OID clientOID = _pClient->GetOID();
	assert( clientOID > 2 );				// not a valid object ID
	*pID = clientOID;

	// If the client is not yet persistent mark the client as modified.
	// This will insure that subjects aren't born as orphans.
	if ( !_pClient->IsPersistent() )
		_pClient->Modified();

	// add the new subject
	return pThis->Add( pSubject );
}


//--------------------------------------------
//^ The Save function saves each of the members of the link set to the DBMS.
//  If all saves are successful returns DBS_OK
//  
template<class T>
RETCODE vdbLinkSet<T>::Save()
{
	RETCODE rc;
	int count = Count();

	for ( int index = 0; index < count; index++ )
	{	
		rc = Get(index).Save();
		if ( rc != DBS_OK )
			return rc;
	}

	return DBS_OK;
}


//--------------------------------------------
//^ The Delete function removes the object at the indexed offset from the set
//  and deletes it from the DBMS.
//> index is the zero-based index into the set of the object to delete.
//< If the call to the DBMS to delete the record fails, the return code
//  from vdbObject::Delete is passed back here and the object is not removed
//  from the set.
//  If successful returns DBS_OK
//  
template<class T>
RETCODE vdbLinkSet<T>::Delete( int index )
{
	vdbSet<T>* pThis = This();

	assert( index >= 0 );
	assert( index < _pSubjects->Count() );
	
	// delete from DBMS
	RETCODE rc = (*pThis)[index].Delete();
	if ( rc != DBS_OK )
		return rc;

	// remove from set
	pThis->Remove( index );
	return DBS_OK;
}


//--------------------------------------------
//^ The DeleteAll function removes all objects from the set and deletes them
//  from the DBMS.
//> bAllOrNothing is a flag to indicate whether or not all deletions should 
//  occur in a group transaction.  True means use rollback and commit.  False
//  means delete each record individually without transaction grouping.
//> Note that in all cases, the first unsuccessful deletion causes the function
//  to return.
//< If any call to the DBMS to delete a record fails, the return code
//  from vdbObject::Delete is passed back here and remaining objects are not
//  removed from the set.  If everything was successfully deleted returns DBS_OK.
//  
template<class T>
RETCODE vdbLinkSet<T>::DeleteAll( bool bAllOrNothing )
{
	RETCODE rc;

	vdbDatabase* pDB = GetDatabase();
	if ( bAllOrNothing )
		pDB->BeginTransaction();
		 
	// delete from DBMS
	int count = Count();
	for ( int index = 0; index < count; index++ )
	{	
		rc = Get(index).Delete();
		if ( rc != DBS_OK  &&  bAllOrNothing == true )
		{
			pDB->Rollback();
			return rc;
		}
		else if ( rc != DBS_OK  &&  bAllOrNothing == false )
			return rc;
	}

	// commit
	if ( bAllOrNothing )
		pDB->Commit();

	// remove all objects from the set
	ClearAll();
	return DBS_OK;
}


