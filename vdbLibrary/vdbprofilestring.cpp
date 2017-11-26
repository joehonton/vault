//=============================================================================
//
// FILE NAME:       vdbProfileString.h
//
// ESSENCE:         wrapper for GetPrivateProfileString, WritePrivateProfileString
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    2/3/96
//
// COPYRIGHT:       Copyright © 1996 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbProfileString.h"


unsigned long vdbGetPrivateProfileString( const char* lpAppName, const char* lpKeyName, const char* lpDefault, char* lpReturnedString, unsigned long nSize, const char* lpFileName )
{
	return ::GetPrivateProfileString( lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName );
}


int vdbWritePrivateProfileString( const char* lpAppName, const char* lpKeyName, const char* lpString, const char* lpFileName )
{
	return ::WritePrivateProfileString( lpAppName, lpKeyName, lpString, lpFileName );
}

