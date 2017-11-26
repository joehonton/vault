//=============================================================================
//
// CLASS:           vdbWindowSupport
//
// ESSENCE:         This module contains Window support routines
//
// FILE NAME:       vdbWindowSupport.cpp
//
// INITIAL DATE:    5/8/95
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbWindowSupport.h"


//=============================================================================
// Global functions
//=============================================================================

//-------------------------------------
//^^ The vdbCenterWindow function
void vdbCenterWindow( HWND hWindow )
{
	RECT rectWindow, rectDesktop;
	WORD wTop, wLeft;
	WORD wWidth, wHeight;

	GetWindowRect( hWindow, &rectWindow );
	wWidth = (WORD) (rectWindow.right - rectWindow.left);
	wHeight = (WORD) (rectWindow.bottom - rectWindow.top);

	GetWindowRect( ::GetDesktopWindow(), &rectDesktop );
	wLeft = (WORD) (rectDesktop.right - wWidth) / 2;
	wTop = (WORD) (rectDesktop.bottom - wHeight) / 2;

	MoveWindow( hWindow, wLeft, wTop, wWidth, wHeight, TRUE );
}

