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

#ifndef __vdbProfileString_h
#define __vdbProfileString_h

extern "C" unsigned long vdbGetPrivateProfileString( const char* lpAppName, const char* lpKeyName, const char* lpDefault, char* lpReturnedString, unsigned long nSize, const char* lpFileName );
extern "C" int vdbWritePrivateProfileString( const char* lpAppName, const char* lpKeyName, const char* lpString, const char* lpFileName );

#endif // __vdbProfileString_h
