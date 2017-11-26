//=============================================================================
//
// FILE NAME:       vdbNoDialogs.h
//
// ESSENCE:         Message box
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    3/2/2002
//
// COPYRIGHT:       Copyright © 2002 by Joe Honton
//
//=============================================================================

#ifndef __vdbNoDialogs_h
#define __vdbNoDialogs_h

#ifndef _INC_TCHAR
	#include <tchar.h>
#endif

extern "C" int vdbMessageBox( int hwnd, const TCHAR* szMessage, const TCHAR* szTitle, unsigned int buttons );
extern "C" int vdbTextBox( const TCHAR* ssText, unsigned int buttons );

#endif // __vdbNoDialogs_h
