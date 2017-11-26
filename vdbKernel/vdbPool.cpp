//=============================================================================
//
// FILE NAME:       vdbPool.cpp
//
// OVERVIEW:		The vdbPool class is an array of pointers to vdbDatabase
//                  instances.  One and only one vdbPool must exist per
//                  application
//
// FILE NAME:       vdbPool.cpp
//
// INITIAL DATE:    10/7/99
//
// COPYRIGHT:		Copyright © 1999 by Joe Honton
//
//=============================================================================

#include "vdbPool.h"
#include "vdbException.h"


//=============================================================================
// Construction -- Singleton
//=============================================================================

//-------------------------------------------------------
//^^ The vdbPool constructor
//
vdbPool::vdbPool() : vdbSet<vdbDatabase>()
{
}


//-------------------------------------------------------
//^^ The vdbPool destructor.
//^ Every application must call delete on the vdbPool instance
//  created through the Instance function.
//
vdbPool::~vdbPool()
{
	_instance = 0;
}


//-------------------------------------------------------
// The static member pointer
vdbPool* vdbPool::_instance = 0;


//-------------------------------------
//^ The Instance funcion returns a pointer to the vdbPool
//  instance. All access to the set is through this member function.
//  The first call to the function instantiates the class.  Subsequent
//  calls return a pointer to the same instance.  This mechanism ensures
//  that one and only one instance per application exists.
//
//^ Every application must also call Destroy on the instance created through
//  Instance.
//
vdbPool* vdbPool::Instance()
{
	if ( _instance == 0 )
		_instance = new vdbPool;
	if ( _instance == 0 ) throw vdbMemoryException();

	return _instance;
}


//-------------------------------------
void vdbPool::Destroy()
{
	if ( _instance != 0 )
	{
		delete _instance; _instance = 0;
	}
}


//=============================================================================
//
//=============================================================================

//-------------------------------------
//^ The Adopt function is called with a vdbDatabase-derived object.  This object
//  must have been created on the heap, it will be managed by the connection pool
//  and deleted when the connection pool is destroyed.
//
HANDLE_DATABASE vdbPool::AdoptDatabase( vdbDatabase* pDB )
{
	return Add( pDB );
}


//-------------------------------------
//^ The RemoveDatabase function is called when an auxilliary database is no longer needed
//  and can be removed from memory
bool vdbPool::RemoveDatabase( HANDLE_DATABASE hDB )
{
	vdbDatabase* pDB = &Get( hDB );
	if ( pDB->Disconnect() != DBS_OK )
		return false;

	Replace( hDB, NULL );	// this replaces the entry in the table with a null pointer
	delete pDB; pDB = 0;
	return true;
}


//-------------------------------------
//^ The GetHandle function returns the database handle given an existing database pointer.
//< returns -1 on failure
//< return 0 or higher on success
//
HANDLE_DATABASE vdbPool::GetHandle( vdbDatabase* pDB )
{
	for ( int i = 0; i < Count(); i++ )
		if ( &Get( i ) == pDB )
			return i;

	return -1;
}


//-------------------------------------
//^ The GetDatabase function returns a pointer to a vdbDatabase-derived object
//  previously created and adopted by this class.  Cast the returned pointer
//  into its derived type if needed.
//> handle is the value returned from the call to Adopt.  As a convenience
//  you may call with a handle of 0	to get the default database instance
//
//< Returns a valid pointer if the database corresponding to the handle exists
//< Returns NULL if the handle is not valid
//< May return a NULL pointer if the given handle has been removed via the
//  RemoveDatabase function.
vdbDatabase* vdbPool::GetDatabase( HANDLE_DATABASE handle )
{
	if ( handle >= Count() )
		return NULL;
	if ( handle < 0 )
		return NULL;

	return &Get( handle );
}


//-------------------------------------
//^ The DisconnectAll function disconnects from the DBMS across all instances
//  under pool management
void vdbPool::DisconnectAll()
{
	for ( int i = 0; i < Count(); i++ )
	{
		vdbDatabase* pDB = &Get(i);
		if ( pDB != NULL )
			pDB->Disconnect();
	}
}


//-------------------------------------
//^ The CountConnections function returns the number of database connections
//  currently under pool management
int vdbPool::CountConnections() const
{
	int count = 0;
	for ( int i = 0; i < Count(); i++ )
	{
		vdbDatabase* pDB = &Get(i);
		if ( pDB != NULL )
//			if ( pDB->IsConnected() )	// some type of synchronization needs to be implemented to use this successfully
				count++;
	}
	
	return count;
}


