//=============================================================================
//
// FILE NAME:       vdbMessageBox.cpp
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

#include "vdbWindows.h"
#include "vdbMessageBox.h"
#include <vdbApp.h>


int vdbMessageBox( int hwnd, const TCHAR* szMessage, const TCHAR* szTitle, unsigned int buttons )
{
	#ifdef VDB_NO_DIALOGS
		return 1;
	#else
		return ::MessageBox( (HWND) hwnd, szMessage, szTitle, buttons | MB_SETFOREGROUND );
	#endif
}


int vdbTextBox( const TCHAR* szText, unsigned int buttons )
{
	#ifdef VDB_NO_DIALOGS
		return 1;
	#else
		return ::MessageBox( vdbApp::GetMessageParent(), szText, vdbApp::GetMessageTitle(), buttons | MB_SETFOREGROUND );
	#endif
}

