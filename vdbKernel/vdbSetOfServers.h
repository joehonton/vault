//=============================================================================
//
// FILE NAME:       vdbSetOfServers.h
//
// ESSENCE:         Server Information to be kept on disk
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    1/19/96
//
// COPYRIGHT:       Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __vdbSetOfServers_h
#define __vdbSetOfServers_h

#include "vdbString.h"
#include "vdbFilename.h"
#include "boolean.h"
#include "vdbSet.h"
#include "vdbServerInfo.h"

class vdbServerInfo;


class vdbSetOfServers : public vdbSet<vdbServerInfo>
{
protected:
	vdbSetOfServers();									// protected, use Instance() to instantiate
	virtual ~vdbSetOfServers();							// protected, use Destroy() to delete

public:
	bool ReadFromArchive( bool openForEdit = false );
	bool WriteToArchive();
	vdbServerInfo* Find( vdbString& szDriverName );

public:
	// public access to this class
	static vdbSetOfServers* Instance();					// use this function for all access to the class
	static void Destroy();								// use this function before exiting the program

public:
	vdbFilename _ServerMapFilename;

private:
	static vdbSetOfServers* _instance;					// the one and only database instance
};


#endif  // __vdbSetOfServers_h
