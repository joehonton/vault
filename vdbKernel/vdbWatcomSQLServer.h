//=============================================================================
//
// CLASS:           vdbWatcomSQLServer
//
// ESSENCE:         vdbDatabase mix in functions specific to Watcom SQL 10.0
//
// FILE NAME:       vdbWatcomSQLServer.h
//
// INITIAL DATE:    5/7/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef __vdbWatcomSQLServer_h
#define __vdbWatcomSQLServer_h

#include "vdbServer.h"
#include "vdbDatabase.h"

class vdbTable;

//=============================================================================
class vdbWatcomSQLServer : public vdbServer 
{
public:
	// essential class functions
	vdbWatcomSQLServer( HANDLE_DATABASE hDB = 0 );					// basic constructor
	virtual ~vdbWatcomSQLServer();									// destructor

private:
	vdbWatcomSQLServer( const vdbWatcomSQLServer& rhs );			// copy constructor disabled
	vdbWatcomSQLServer& operator=( const vdbWatcomSQLServer& rhs );	// assignment operator disabled

public:
	// virtuals
	virtual const char* ColumnType( const char* szType );
	virtual RETCODE InitializeAfterCreate();
	virtual void AdminAuthentication( char* szUID, int sizeUID, char* szAuthStr, int sizeAuthStr );

public:
	RETCODE GrantConnect( const char* szLoginID, const char* szPassword );
	RETCODE GrantResource( const char* szLoginID );
};

#endif // __vdbWatcomSQLServer_h
