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

#include "vdbApp.h"
#include <vdbString.h>


//=============================================================================
//  Global application level variables
//=============================================================================

// the window handle to use as the parent for message windows
HWND g_hMessageParent = NULL;

// the text to show at the top of all message box windows
vdbString g_sMessageTitle = "";

// the root key to the application's registry entries
vdbString g_sRegistrationRoot = "";				// SOFTWARE\\CrescentBloom\\CompleatBotanica\\1.0

// the root key to the application's database related registry entries
vdbString g_sDatabaseRegistrationRoot = "";		// SOFTWARE\\CrescentBloom\\CompleatBotanica\\1.0\\Database


//=============================================================================
//  Set functions
//=============================================================================

//------------------------------------------
void vdbApp::SetMessageParent( HWND hwnd )
{
	g_hMessageParent = hwnd;
}


//------------------------------------------
void vdbApp::SetMessageTitle( const char* sz )
{
	g_sMessageTitle = sz;
}


//------------------------------------------
void vdbApp::SetRegistrationRoot( const char* sz )
{
	g_sRegistrationRoot = sz;
}


//------------------------------------------
void vdbApp::SetDatabaseRegistrationRoot( const char* sz )
{
	g_sDatabaseRegistrationRoot = sz;
}


//=============================================================================
//  Get functions
//=============================================================================

//------------------------------------------
HWND vdbApp::GetMessageParent()
{
	return g_hMessageParent;
}


//------------------------------------------
const char* vdbApp::GetMessageTitle()
{
	return g_sMessageTitle;
}


//------------------------------------------
const char* vdbApp::GetRegistrationRoot()
{
	return g_sRegistrationRoot;
}


//------------------------------------------
const char* vdbApp::GetDatabaseRegistrationRoot()
{
	return g_sDatabaseRegistrationRoot;
}
