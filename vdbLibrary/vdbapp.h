//=============================================================================
//
// CLASS:           vdbApp
//
// OVERVIEW:		The vdbApp class of static member functions for accessing
//                  application level global variables.
//
// FILE NAME:       vdbApp.h
//
// INITIAL DATE:    6/13/00
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __vdbApp_h
#define __vdbApp_h

#ifndef __vdbWindows_h
	#include <vdbWindows.h>
#endif


class vdbApp
{
public:
	// set globals
	static void SetMessageParent( HWND hwnd );
	static void SetMessageTitle( const char* sz );
	static void SetRegistrationRoot( const char* sz );
	static void SetDatabaseRegistrationRoot( const char* sz );

public:
	// get globals
	static HWND GetMessageParent();
	static const char* GetMessageTitle();
	static const char* GetRegistrationRoot();
	static const char* GetDatabaseRegistrationRoot();
};

#endif // __vdbApp_h

