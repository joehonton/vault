//=============================================================================
//
// FILE NAME:       vdbMessageBox.h
//
// ESSENCE:         Message box
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    1/22/96
//
// COPYRIGHT:       Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __vdbMessageBox_h
#define __vdbMessageBox_h

#ifndef _INC_TCHAR
	#include <tchar.h>
#endif

#include "vdbExport.h"

#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L
#define MB_OKCANCELSKIP             0x00000006L		//+

#define MB_ICONHAND                 0x00000010L
#define MB_ICONQUESTION             0x00000020L
#define MB_ICONEXCLAMATION          0x00000030L
#define MB_ICONASTERISK             0x00000040L

#define MB_ICONINFORMATION          MB_ICONASTERISK
#define MB_ICONSTOP                 MB_ICONHAND

#define MB_DEFBUTTON1               0x00000000L
#define MB_DEFBUTTON2               0x00000100L
#define MB_DEFBUTTON3               0x00000200L

#define MB_APPLMODAL                0x00000000L
#define MB_SYSTEMMODAL              0x00001000L
#define MB_TASKMODAL                0x00002000L

#define MB_NOFOCUS                  0x00008000L
#define MB_SETFOREGROUND            0x00010000L
#define MB_DEFAULT_DESKTOP_ONLY     0x00020000L

#define MB_TYPEMASK                 0x0000000FL
#define MB_ICONMASK                 0x000000F0L
#define MB_DEFMASK                  0x00000F00L
#define MB_MODEMASK                 0x00003000L
#define MB_MISCMASK                 0x0000C000L

#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7
#define IDSKIP              8	//+

extern "C" int vdbMessageBox( int hwnd, const TCHAR* szMessage, const TCHAR* szTitle, unsigned int buttons );
extern "C" int vdbTextBox( const TCHAR* ssText, unsigned int buttons );

#endif // __vdbMessageBox_h
