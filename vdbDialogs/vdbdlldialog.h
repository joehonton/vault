//=============================================================================
//
// CLASS:           vdbDLLDialog
//
// ESSENCE:         CDialog-derived class for dialogs in a DLL when used with
//                  the static library MFC 
//
// FILE NAME:       vdbDLLDialog.h
//
// INITIAL DATE:    12/28/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#ifndef __vdbDLLDialog_h
#define __vdbDLLDialog_h


//-------------------------------------
class vdbDLLDialog : public CDialog
{
public:
	vdbDLLDialog( UINT resourceID, CWnd* pParentWnd );

	virtual int DoModal();
	virtual BOOL Create( UINT resourceID, CWnd* pParentWnd );
	virtual BOOL CheckAutoCenter();

	//{{AFX_DATA(vdbDLLDialog)
	//}}AFX_DATA

	//{{AFX_VIRTUAL(vdbDLLDialog)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(vdbDLLDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__vdbDLLDialog_h
