//=============================================================================
//
// CLASS:           vdbDLLBitmapButton
//
// ESSENCE:         CBitmapButton-derived class for buttons in a DLL when used 
//                  with the static library MFC 
//
// FILE NAME:       vdbDLLBitmapButton.h
//
// INITIAL DATE:    12/28/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#ifndef __vdbDLLBitmapButton_h
#define __vdbDLLBitmapButton_h


//-------------------------------------
class vdbDLLBitmapButton : public CBitmapButton
{
public:
	vdbDLLBitmapButton();

	void SetBitmaps( UINT upID, UINT downID, UINT focusID, UINT disabledID );

};

#endif //__vdbDLLBitmapButton_h
