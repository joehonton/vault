//=============================================================================
//
// FILE NAME:       vdbNoDialogs.cpp
//
// ESSENCE:         NULL functions for MessageBox.
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    1/22/96
//
// COPYRIGHT:       Copyright © 1996 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbNoDialogs.h"

int vdbMessageBox( int hwnd, const TCHAR* szMessage, const TCHAR* szTitle, unsigned int buttons )
{
	return 1;  // IDOK
}


int vdbTextBox( const TCHAR* szText, unsigned int buttons )
{
	return 1;  // IDOK
}

