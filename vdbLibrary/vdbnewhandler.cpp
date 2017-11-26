//=============================================================================
//
// CLASS:           vdbNewHandler
//
// ESSENCE:         Function to handle failures in new
//
// FILE NAME:       vdbNewHandler.cpp
//
// INITIAL DATE:    07/18/97 
//
// COPYRIGHT:       Copyright © 1997 by Joe Honton
//
//=============================================================================

#include "vdbNewHandler.h"
#include "vdbException.h"


//=============================================================================
//  set new handler
//=============================================================================

//-------------------------------------
//^ The vdbNewHandler function is installed close to the top of main or WinMain
//  by calling _set_new_handler( vdbNewHandler );  This function will simply
//  throw an exception of type vdbMemoryException.
//
int vdbNewHandler( size_t size )
{
	throw vdbMemoryException();
	return 0;	
}

