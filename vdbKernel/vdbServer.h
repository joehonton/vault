//=============================================================================
//
// CLASS:           vdbServer
//
// ESSENCE:         Server specific database functionality available to
//                  vdbDatabase via GetServer()
//
// FILE NAME:       vdbServer.h
//
// INITIAL DATE:    5/6/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef __vdbServer_h
#define __vdbServer_h

#include "vdbTypedef.h"

class vdbDatabase;
class vdbTable;

//=============================================================================
class vdbServer 
{
public:
	// essential class functions
	vdbServer( HANDLE_DATABASE hDB = 0 );			// basic constructor
	virtual ~vdbServer();							// destructor

private:
	vdbServer( const vdbServer& rhs );				// copy constructor disabled
	vdbServer& operator=( const vdbServer& rhs );	// assignment operator disabled

public:
	// virtuals
	virtual const char* ColumnType( const char* szType );
	virtual RETCODE DropIndex( vdbTable* pTable, int which );
	virtual RETCODE InitializeAfterCreate();
	virtual char* CapitalizeNames( char* szString );
	virtual void AdminAuthentication( char* szUID, int sizeUID, char* szAuthStr, int sizeAuthStr );
	virtual bool IsEngineRunning();
	virtual bool StartEngine();
	virtual bool StopEngine();

public:
	inline vdbDatabase* GetDatabase();

private:
	vdbDatabase* _pDatabase;
};


//=============================================================================
// inlines
//=============================================================================

inline vdbDatabase* vdbServer::GetDatabase()
{
	return _pDatabase;
}

#endif // __vdbServer_h
