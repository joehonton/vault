//=============================================================================
//
// CLASS:           vdbPool
//
// OVERVIEW:		The vdbPool class is an array of pointers to vdbDatabase
//                  instances.  One and only one vdbPool must exist per
//                  application
//
// FILE NAME:       vdbPool.h
//
// INITIAL DATE:    10/7/99
//
// COPYRIGHT:		Copyright © 1999 by Joe Honton
//
//=============================================================================

#ifndef __vdbPool_h
#define __vdbPool_h

#include "vdbSet.h"
#include "vdbDatabase.h"

class vdbPool : public vdbSet<vdbDatabase>
{
protected:
	vdbPool();									// protected, use Instance() to instantiate
	virtual ~vdbPool();							// protected, use Destroy() to delete

public:
	// public access to this class
	static vdbPool* Instance();					// use this function for all access to the class
	static void Destroy();						// use this function before exiting the program

public:
	HANDLE_DATABASE AdoptDatabase( vdbDatabase* pDB );
	bool RemoveDatabase( HANDLE_DATABASE hDB );
	HANDLE_DATABASE GetHandle( vdbDatabase* pDB );
	vdbDatabase* GetDatabase( HANDLE_DATABASE which = 0 );
	void DisconnectAll();
	int CountConnections() const;

private:
	static vdbPool* _instance;					// the one and only database instance
};


#endif  // __vdbPool_h
