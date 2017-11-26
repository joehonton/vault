//=============================================================================
//
// CLASS:           DLL Startup
//
// OVERVIEW:		DLL initialization and shutdown routines
//
// FILE NAME:       Startup.cpp
//
// INITIAL DATE:    01/20/96 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#include <windows.h>

// function prototypes for DLL entry/exit
BOOL StartupCode(HINSTANCE hInstDLL);
BOOL ShutdownCode(HINSTANCE hInstDLL);

// globals
HINSTANCE g_hVaultResources = NULL;  


//=============================================================================
//  Entry/exit point for the 32-bit DLL
//  register a new window class with first process
//  unregister as the last process detaches
//=============================================================================
#ifdef WIN32

BOOL WINAPI DllMain( HINSTANCE hInstDLL, DWORD reason, LPVOID situation )
{
	switch (reason)
	{
		case DLL_PROCESS_ATTACH:
			return StartupCode( hInstDLL );


		case DLL_PROCESS_DETACH:
			return ShutdownCode( hInstDLL );


		case DLL_THREAD_ATTACH:
			return TRUE;


		case DLL_THREAD_DETACH:
			return TRUE;


		default:
			return FALSE;
	}
}

#endif  //WIN32


//=============================================================================
//  Entry/exit point for the 16-bit DLL
//  register a new window class with first process
//  unregister as the last process detaches
//=============================================================================
#ifndef WIN32

extern "C" int CALLBACK LibMain( HINSTANCE hInstDLL, UINT uiDataSegment, UINT uiHeapSize, LPSTR lpszCmdLine )
{
	if ( g_hVaultResources == NULL )
		return StartupCode( hInstDLL );
	return 1;
}
                                         

int CALLBACK WEP(int)
{
    ShutdownCode( g_hVaultResources );
	return 1;
}

#endif	//WIN32


//---------------------------------------------------------------------------------------------
BOOL StartupCode(HINSTANCE hInstDLL)
{
	g_hVaultResources = hInstDLL;
	return TRUE;
}



//---------------------------------------------------------------------------------------------
BOOL ShutdownCode(HINSTANCE hInstDLL)
{
	g_hVaultResources = NULL;
	return TRUE;
}



