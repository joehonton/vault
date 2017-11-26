//=============================================================================
//
// FILE NAME:       vdbWindows.h
//
// ESSENCE:         Include either MFC or windows but not both.
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    2/2/96
//
// COPYRIGHT:       Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __vdbWindows_h
#define __vdbWindows_h


#if defined(USE_AFX) && defined(USE_WIN)
	#error Please define either USE_AFX or USE_WIN but not both
#endif


#ifdef USE_AFX
	#include <afxwin.h>
	#include <afxext.h>
	#include <afxcmn.h>
#endif

#ifdef USE_WIN
	#include <windows.h>
#endif


// __AFX_H__ is defined in <afx.h>
// _WINDOWS_ is defined in <windows.h>
#if !defined(__AFX_H__) && !defined(_WINDOWS_)
	#error Please define either USE_AFX or USE_WIN if not including <afx.h> or <windows.h>
#endif


#endif  // __vdbWindows_h
