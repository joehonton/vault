//=============================================================================
//
// CLASS:           vdbLoginDlg
//
// ESSENCE:         Login Dialog procedure and class
//
// FILE NAME:       vdbLoginDlg.cpp
//
// INITIAL DATE:    1/20/96
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbAssert.h"
#include "vdbLoginDialog.h"
#include "..\\vdbResources\\resource.h"
#include "VaultResourceInstance.h"
#include "vdbWindowSupport.h"


//=============================================================================
//  Public function for showing the dialog
//=============================================================================

//-------------------------------------
//^ The vdbLoginDialog function shows the dialog box
//> data is the structure for transferring data into and out of the dialog
//< returns true if the user presses OK, false if the user presses Cancel
//
bool vdbLoginDialog( vdbLoginDlgData& data )
{
	assert ( &data != 0 );
	int rc = DialogBoxParam( VaultResourceInstance::Handle(), MAKEINTRESOURCE(IDD_LOGIN), NULL, (DLGPROC) LoginDlgProc, (LPARAM) &data );
	VaultResourceInstance::UnloadDLL();
	if ( rc == IDOK )
		return true;
	else
		return false;
}


//=============================================================================
// Dialog box procedure
//=============================================================================

//--------------------------------
// Procedure for Login Dialog Dialog box
BOOL CALLBACK LoginDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static LoginDlgClass* pDlgClass = 0;

	switch ( message )
	{
		case WM_INITDIALOG:
			if ( !(pDlgClass = new LoginDlgClass) )
				return EndDialog( hwnd, IDABORT );
			else
				return pDlgClass->OnInitDialog( hwnd, (vdbLoginDlgData*) lParam );
					
		
		case WM_COMMAND:
		{	
			switch ( LOWORD( wParam ) )
			{
				case IDOK:
					pDlgClass->OnOK( hwnd );
					break;

				case IDCANCEL:
					pDlgClass->OnCancel( hwnd );
					break;

				default:
					break;
			}
	
			return TRUE;
		}
		
		case WM_DESTROY:
			delete pDlgClass;
			pDlgClass = 0;
			return TRUE;

		
		default:
    		return FALSE;
	}
}


//=============================================================================
// Class members
//=============================================================================

//---------------------------------------
BOOL LoginDlgClass::OnInitDialog( HWND hwnd, vdbLoginDlgData* pData )
{
	assert( pData != 0 );
	_pData = pData;

	vdbCenterWindow( hwnd );

	// fill two edit controls with default values passed in
	SendDlgItemMessage( hwnd, IDC_VDB_DATASOURCE, WM_SETTEXT, 0, (long) _pData->szDataSource );
	SendDlgItemMessage( hwnd, IDC_VDB_IDENTIFICATION, WM_SETTEXT, 0, (long) _pData->szAuthenticationID );
	SendDlgItemMessage( hwnd, IDC_VDB_PASSWORD, WM_SETTEXT, 0, (long) _pData->szAuthenticationPassword );
	return TRUE;
}


//---------------------------------------
void LoginDlgClass::OnOK( HWND hwnd )
{
	// get two edit control values
	SendDlgItemMessage( hwnd, IDC_VDB_IDENTIFICATION, WM_GETTEXT, sizeof(_pData->szAuthenticationID), (long) _pData->szAuthenticationID );
	SendDlgItemMessage( hwnd, IDC_VDB_PASSWORD, WM_GETTEXT, sizeof(_pData->szAuthenticationPassword), (long) _pData->szAuthenticationPassword );
	EndDialog( hwnd, IDOK );
}


//---------------------------------------
void LoginDlgClass::OnCancel( HWND hwnd )
{
	EndDialog( hwnd, IDCANCEL );
}

