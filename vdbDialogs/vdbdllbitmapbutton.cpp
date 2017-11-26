//=============================================================================
//
// CLASS:           vdbDLLBitmapButton
//
// ESSENCE:         CBitmapButton-derived class for buttons in a DLL when used
//                  with the static library MFC 
//
// FILE NAME:       vdbDLLBitmapButton.cpp
//
// INITIAL DATE:    12/28/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#include <vdbWindows.h>
#include "vdbDLLBitmapButton.h"
#include <VaultResourceInstance.h>


//=============================================================================
//  Essential class functions
//=============================================================================

//-------------------------------------
vdbDLLBitmapButton::vdbDLLBitmapButton()
	: CBitmapButton()
{
}


//-------------------------------------
void vdbDLLBitmapButton::SetBitmaps( UINT upID, UINT downID, UINT focusID, UINT disabledID )
{
	HINSTANCE hInst = VaultResourceInstance::Handle();

	m_bitmap.Attach( ::LoadBitmap( hInst, MAKEINTRESOURCE(upID) ) );
	m_bitmapSel.Attach( ::LoadBitmap( hInst, MAKEINTRESOURCE(downID) ) );
	m_bitmapFocus.Attach( ::LoadBitmap( hInst, MAKEINTRESOURCE(focusID) ) );
	m_bitmapDisabled.Attach( ::LoadBitmap( hInst, MAKEINTRESOURCE(disabledID) ) );
}

