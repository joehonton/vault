//=============================================================================
//
// CLASS:           vdbOmniscienceORDBMS
//
// ESSENCE:         vdbDatabase mix in functions specific to Omniscience ORDBMS 2.0
//
// FILE NAME:       vdbOmniscienceORDBMS.h
//
// INITIAL DATE:    8/10/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef vdbOmniscienceORDBMS_h
#define vdbOmniscienceORDBMS_h

#include "vdbServer.h"
#include "vdbDatabase.h"

class vdbTable;


//=============================================================================
class vdbOmniscienceORDBMS : public vdbServer 
{
public:
	// essential class functions
	vdbOmniscienceORDBMS( HANDLE_DATABASE hDB = 0 );					// basic constructor
	virtual ~vdbOmniscienceORDBMS();									// destructor

private:
	vdbOmniscienceORDBMS( const vdbOmniscienceORDBMS& rhs );			// copy constructor disabled
	vdbOmniscienceORDBMS& operator=( const vdbOmniscienceORDBMS& rhs );	// assignment operator disabled

public:
	// virtuals
	virtual const char* ColumnType( const char* szType );
	virtual char* CapitalizeNames( char* szString );
};


#endif // vdbOmniscienceORDBMS_h
