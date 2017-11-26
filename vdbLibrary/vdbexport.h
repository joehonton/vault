//=============================================================================
//
// FILE NAME:       vdbExport.h
//
// ESSENCE:         Export declarations for DLL
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    10/14/94
//
// COPYRIGHT:       Copyright © 1994 by Joe Honton
//
//=============================================================================

#ifndef __vdbExport_h
#define __vdbExport_h


//-------------------------------------
// Define CBC_EXPORT_DLL within the compiler settings when building any of the Portfolio DLLs
#ifdef CBC_EXPORT_DLL
	#define EXPORT_CBC __declspec(dllexport)
#else
	#define EXPORT_CBC __declspec(dllimport)
#endif


#endif // __vdbExport_h   
