//=============================================================================
//
// CLASS:           VaultResourceInstance
//
// OVERVIEW:		On-demand loading of resource DLL
//
// FILE NAME:       VaultResourceInstance.h
//
// INITIAL DATE:    2/28/96 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __VaultResourceInstance_h
#define __VaultResourceInstance_h

#include <windef.h>


//=============================================================================
class VaultResourceInstance
{
private:
	VaultResourceInstance();					// basic constructor
	~VaultResourceInstance();					// destructor

private:
	VaultResourceInstance( const VaultResourceInstance& rhs );				// copy constructor disabled
	VaultResourceInstance& operator=( const VaultResourceInstance& rhs );	// assignment operator disabled

public:
	// public access to this class
	static HINSTANCE Handle();
	static void UnloadDLL();

private:
	// member data
	static VaultResourceInstance* _instance;			// the one and only instance of this class
	HINSTANCE _hInstDLL;								// the handle to the DLL
	HINSTANCE _hCommanderDLL;
};									


#endif // __VaultResourceInstance_h
