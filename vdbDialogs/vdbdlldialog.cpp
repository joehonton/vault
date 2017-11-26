//=============================================================================
//
// CLASS:           vdbDLLDialog
//
// ESSENCE:         CDialog-derived class for dialogs in a DLL when used with
//                  the static library MFC 
//
// FILE NAME:       vdbDLLDialog.cpp
//
// INITIAL DATE:    12/28/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#include <vdbWindows.h>
#include "vdbDLLDialog.h"
#include <VaultResourceInstance.h>
#include <AFXPRIV.H>


//=============================================================================
//  Essential class functions
//=============================================================================

//-------------------------------------
vdbDLLDialog::vdbDLLDialog( UINT resourceID, CWnd* pParentWnd )
	: CDialog( resourceID, pParentWnd )
{
	//{{AFX_DATA_INIT(vdbDLLDialog)
	//}}AFX_DATA_INIT
}


//-------------------------------------
BEGIN_MESSAGE_MAP(vdbDLLDialog, CDialog)
	//{{AFX_MSG_MAP(vdbDLLDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
//  Initialization function
//=============================================================================

int vdbDLLDialog::DoModal()
{
	// load resource as necessary
	LPCDLGTEMPLATE lpDialogTemplate = m_lpDialogTemplate;
	HGLOBAL hDialogTemplate = m_hDialogTemplate;
	HINSTANCE hInst = VaultResourceInstance::Handle();
	if (m_lpszTemplateName != NULL)
	{
		HRSRC hResource = ::FindResource(hInst, m_lpszTemplateName, RT_DIALOG);
		hDialogTemplate = LoadResource(hInst, hResource);
	}
	if (hDialogTemplate != NULL)
		lpDialogTemplate = (LPCDLGTEMPLATE)LockResource(hDialogTemplate);

	// return -1 in case of failure to load the dialog template resource
	if (lpDialogTemplate == NULL)
		return -1;

	// disable parent (before creating dialog)
	HWND hWndParent = PreModal();
	AfxUnhookWindowCreate();
	BOOL bEnableParent = FALSE;
	if (hWndParent != NULL && ::IsWindowEnabled(hWndParent))
	{
		::EnableWindow(hWndParent, FALSE);
		bEnableParent = TRUE;
	}

	TRY
	{
		// create modeless dialog
		AfxHookWindowCreate(this);
		if (CreateDlgIndirect(lpDialogTemplate,	CWnd::FromHandle(hWndParent), hInst))
		{
			if (m_nFlags & WF_CONTINUEMODAL)
			{
				// enter modal loop
				DWORD dwFlags = MLF_SHOWONIDLE;
				if (GetStyle() & DS_NOIDLEMSG)
					dwFlags |= MLF_NOIDLEMSG;
				VERIFY(RunModalLoop(dwFlags) == m_nModalResult);
			}

			// hide the window before enabling the parent, etc.
			if (m_hWnd != NULL)
				SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
		}
	}
	CATCH_ALL(e)
	{
		m_nModalResult = -1;
	}
	END_CATCH_ALL

	if (bEnableParent)
		::EnableWindow(hWndParent, TRUE);
	if (hWndParent != NULL && ::GetActiveWindow() == m_hWnd)
		::SetActiveWindow(hWndParent);

	// destroy modal window
	DestroyWindow();
	PostModal();

	// unlock/free resources as necessary
	if (m_lpszTemplateName != NULL || m_hDialogTemplate != NULL)
		UnlockResource(hDialogTemplate);
	if (m_lpszTemplateName != NULL)
		FreeResource(hDialogTemplate);

	return m_nModalResult;
}


//-------------------------------------
BOOL vdbDLLDialog::Create( UINT resourceID, CWnd* pParentWnd )
{
	ASSERT(HIWORD(MAKEINTRESOURCE(resourceID)) == 0 ||
		AfxIsValidString(MAKEINTRESOURCE(resourceID)));

	m_lpszTemplateName = MAKEINTRESOURCE(resourceID);
	if (HIWORD(m_lpszTemplateName) == 0 && m_nIDHelp == 0)
		m_nIDHelp = LOWORD((DWORD)m_lpszTemplateName);

	HINSTANCE hInst = VaultResourceInstance::Handle();
	HRSRC hResource = ::FindResource(hInst, MAKEINTRESOURCE(resourceID), RT_DIALOG);
	HGLOBAL hTemplate = LoadResource(hInst, hResource);
	BOOL bResult = CreateIndirect(hTemplate, pParentWnd, hInst);
	FreeResource(hTemplate);

	return bResult;
}


//-------------------------------------
BOOL vdbDLLDialog::CheckAutoCenter()
{
	// load resource as necessary
	LPCDLGTEMPLATE lpDialogTemplate = m_lpDialogTemplate;
	HGLOBAL hDialogTemplate = m_hDialogTemplate;
	if (m_lpszTemplateName != NULL)
	{
		HINSTANCE hInst = VaultResourceInstance::Handle();
		HRSRC hResource = ::FindResource(hInst, m_lpszTemplateName, RT_DIALOG);
		hDialogTemplate = LoadResource(hInst, hResource);
	}
	if (hDialogTemplate != NULL)
		lpDialogTemplate = (LPCDLGTEMPLATE)LockResource(hDialogTemplate);

	// determine if dialog should be centered
	BOOL bResult = TRUE;

	if (lpDialogTemplate != NULL)
	{
		DWORD dwStyle = lpDialogTemplate->style;
		short x = lpDialogTemplate->x;
		short y = lpDialogTemplate->y;

		bResult = !(dwStyle & (DS_CENTER|DS_CENTERMOUSE|DS_ABSALIGN)) &&
			x == 0 && y == 0;
	}

	// unlock/free resources as necessary
	if (m_lpszTemplateName != NULL || m_hDialogTemplate != NULL)
		UnlockResource(hDialogTemplate);
	if (m_lpszTemplateName != NULL)
		FreeResource(hDialogTemplate);

	return bResult; // TRUE if auto-center is ok
}
