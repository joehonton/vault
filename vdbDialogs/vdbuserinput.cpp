//=============================================================================
//
// CLASS:           vdbUserInput
//
// FILE NAME:       vdbUserInput.h
//
// ESSENCE:         Dialog box for generic user input
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    1/02/97
//
// COPYRIGHT:       Copyright © 1997 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbAssert.h"
#include "vdbUserInput.h"
#include "vdbMessageBox.h"
#include "..\\vdbResources\\resource.h"
#include "VaultResourceInstance.h"
#include <vdbException.h>
#include "vdbWindowSupport.h"


//=============================================================================
//  Public function for showing the dialog
//=============================================================================

//-------------------------------------
//^ The vdbUserInputDialog function shows the dialog box
//> data is the structure for transferring data into and out of the dialog
//< returns IDOK, IDCANCEL, or IDSKIP
//
int vdbUserInputDialog( vdbUserInputDlgData& data )
{
	assert ( &data != 0 );
	int rc = DialogBoxParam( VaultResourceInstance::Handle(), MAKEINTRESOURCE(IDD_USER_INPUT), NULL, (DLGPROC) UserInputDlgProc, (LPARAM) &data );
	VaultResourceInstance::UnloadDLL();
	return rc;
}


//=============================================================================
// Dialog box procedure
//=============================================================================

//--------------------------------
// Procedure for UserInput Dialog Dialog box
BOOL CALLBACK UserInputDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static UserInputDlgClass* pDlgClass = 0;

	switch ( message )
	{
		case WM_INITDIALOG:
			if ( !(pDlgClass = new UserInputDlgClass) )
				return EndDialog( hwnd, IDABORT );
			else
				return pDlgClass->OnInitDialog( hwnd, (vdbUserInputDlgData*) lParam );
					
		
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

				case IDC_EXTRA:
					pDlgClass->OnSkip( hwnd );
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
BOOL UserInputDlgClass::OnInitDialog( HWND hwnd, vdbUserInputDlgData* pData )
{
	assert( pData != 0 );
	_pData = pData;

	vdbCenterWindow( hwnd );

	SendDlgItemMessage( hwnd, IDC_MESSAGE, WM_SETTEXT, 0, (long)(const char*) _pData->_szMessage );
	SendDlgItemMessage( hwnd, IDC_USER_INPUT, WM_SETTEXT, 0, (long)(const char*) _pData->_szUserInput );
	SetWindowText( hwnd, (const char*) _pData->_szTitle );
	SetFocus( GetDlgItem( hwnd, IDC_USER_INPUT ) );
	
	// NOTE: only MB_OK MB_OKCANCEL and MB_OKCANCELSIP are enabled
	unsigned int buttons = ( _pData->_buttons & 0x0000000F );
	if ( buttons == MB_OK || buttons == MB_OKCANCEL || buttons == MB_OKCANCELSKIP )
		ShowWindow( GetDlgItem( hwnd, IDOK ), SW_SHOW );
	else
		ShowWindow( GetDlgItem( hwnd, IDOK ), SW_HIDE );
		 

	if ( buttons == MB_OKCANCEL || buttons == MB_OKCANCELSKIP )
		ShowWindow( GetDlgItem( hwnd, IDCANCEL ), SW_SHOW );
	else
		ShowWindow( GetDlgItem( hwnd, IDCANCEL ), SW_HIDE );


	if ( buttons == MB_OKCANCELSKIP )
	{
		ShowWindow( GetDlgItem( hwnd, IDC_EXTRA ), SW_SHOW );
		SetWindowText( GetDlgItem( hwnd, IDC_EXTRA ), "Skip" );
	}
	else
		ShowWindow( GetDlgItem( hwnd, IDC_EXTRA ), SW_HIDE );

	// center the window
	HWND hwndParent = GetParent( hwnd );
	if ( hwndParent == 0 )
		hwndParent = GetDesktopWindow();
	RECT parentRect;
	GetWindowRect ( hwndParent, &parentRect );
	RECT thisRect;
	GetWindowRect ( hwnd, &thisRect );

	int x = parentRect.left + ( ( parentRect.right - thisRect.right) / 2);
	int y = parentRect.top + ( ( parentRect.bottom - thisRect.bottom) / 2);
	x = max( x, 0 );
	y = max( y, 0 );
	SetWindowPos( hwnd, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE );

	return TRUE;
}


//---------------------------------------
void UserInputDlgClass::OnOK( HWND hwnd )
{
	// get edit control value
	int len = SendDlgItemMessage( hwnd, IDC_USER_INPUT, WM_GETTEXTLENGTH, 0, 0 );
	char* psz = new char[len + 1];
	if ( psz == 0 ) throw vdbMemoryException();
	SendDlgItemMessage( hwnd, IDC_USER_INPUT, WM_GETTEXT, len + 1, (long)(const char*) psz );
	_pData->_szUserInput = psz;
	delete[] psz; psz = 0;

	EndDialog( hwnd, IDOK );
}


//---------------------------------------
void UserInputDlgClass::OnCancel( HWND hwnd )
{
	EndDialog( hwnd, IDCANCEL );
}


//---------------------------------------
void UserInputDlgClass::OnSkip( HWND hwnd )
{
	EndDialog( hwnd, IDSKIP );
}

