//=============================================================================
//
// CLASS:           VaultResourceInstance
//
// OVERVIEW:		On-demand loading of resource DLL
//
// FILE NAME:       VaultResourceInstance.cpp
//
// INITIAL DATE:    02/28/96 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "VaultResourceInstance.h"
#include "vdbAssert.h"
#include "vdbMessageBox.h"
#include <vdbException.h>


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------------------------
//^^ The VaultResourceInstance constructor
//
VaultResourceInstance::VaultResourceInstance()
{
	_hInstDLL = LoadLibrary( "VaultResources.dll" );
	_hCommanderDLL = LoadLibrary( "Commander.dll" );
}


//-------------------------------------------------------
//^^ The VaultResourceInstance destructor.
//
VaultResourceInstance::~VaultResourceInstance()
{
	FreeLibrary( _hInstDLL ); _hInstDLL = 0;
	FreeLibrary( _hCommanderDLL ); _hCommanderDLL = 0;
}


//-------------------------------------------------------
// The static member pointer
VaultResourceInstance* VaultResourceInstance::_instance = 0;



//-------------------------------------------------------
//^ The Handle function returns the handle to the DLL instance.
//  The first call to the function instantiates the class.  Subsequent calls
//  return a pointer to the same instance.  This mechanism ensures that one
//  and only one instance per application exists.
//^ Every application must also call VaultResourceInstance::UnloadDLL on the
//  instance created through Handle.
//
HINSTANCE VaultResourceInstance::Handle()
{
	if ( _instance == 0 )
		_instance = new VaultResourceInstance;
	if ( _instance == 0 ) throw vdbMemoryException();

	assert( _instance != 0 );
	if ( _instance->_hInstDLL == 0 )
	{
		vdbTextBox( "The file \"VaultResources.dll\" is needed but could not be located.\n\nTry finding this file and copying it to the same location as your application file", MB_OK );
		return 0;
	}
	if ( _instance->_hCommanderDLL == 0 )
	{
		vdbTextBox( "The file \"Commander.dll\" is needed but could not be located.\n\nTry finding this file and copying it to the same location as your application file", MB_OK );
		return 0;
	}
	
	return _instance->_hInstDLL;
}


//-------------------------------------------------------
//^ The UnloadDLL function unloads the DLL and deletes the single instance.
//
void VaultResourceInstance::UnloadDLL()
{
	if ( _instance == 0 )
		return;

	delete _instance; _instance = 0;
}
