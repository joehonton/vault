//=============================================================================
//
// CLASS:           vdbPrinterDialog
//
// ESSENCE:         Printer Dialog procedure and class
//
// FILE NAME:       vdbPrinterDialog.cpp
//
// INITIAL DATE:    6/7/99
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1999 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbAssert.h"
#include "vdbPrinterDialog.h"
#include "..\\vdbResources\\resource.h"
#include "VaultResourceInstance.h"
#include "vdbWindowSupport.h"
#include <pphPrinterSupport.h>
#include <cursors.h>
#include "winspool.h"
#include <vdbUtility.h>


//=============================================================================
//  Dialog Data functions
//=============================================================================

//------------------------------------
vdbPrinterDialogData::vdbPrinterDialogData()
{
	_enablePrintRange = false;
	_printRange = ALL;
	_fromPage = 1;
	_toPage = 1;
	_printerDC = NULL;
	_dmOrientation = 0;
	_szPrinterName[0] = 0x0;
}


//------------------------------------
vdbPrinterDialogData::vdbPrinterDialogData( bool enablePrintRange, int fromPage, int toPage )
{
	_enablePrintRange = enablePrintRange;
	_printRange = _enablePrintRange == true ? PAGES : ALL;
	_fromPage = fromPage;
	_toPage = toPage;
	_printerDC = NULL;
	_dmOrientation = 0;
	_szPrinterName[0] = 0x0;
}


//------------------------------------
vdbPrinterDialogData::~vdbPrinterDialogData()
{
	if ( _printerDC != NULL )
		DeleteDC( _printerDC );
}


//=============================================================================
//  Public function for showing the dialog
//=============================================================================

//-------------------------------------
//^ The vdbPrinterDialog function shows the dialog box
//> data is the structure for transferring data into and out of the dialog
//< returns true if the user presses OK, false if the user presses Cancel
//
bool vdbPrinterDialog( vdbPrinterDialogData& data )
{
	assert ( &data != 0 );
	int rc = DialogBoxParam( VaultResourceInstance::Handle(), MAKEINTRESOURCE(IDD_PRINT_DIALOG), NULL, (DLGPROC) vdbPrinterDialogProc, (LPARAM) &data );
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
// Procedure for Printer Dialog Dialog box
BOOL CALLBACK vdbPrinterDialogProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static vdbPrinterDialogClass* pDlgClass = 0;

	switch ( message )
	{
		case WM_INITDIALOG:
			if ( !(pDlgClass = new vdbPrinterDialogClass) )
				return EndDialog( hwnd, IDABORT );
			else
				return pDlgClass->OnInitDialog( hwnd, (vdbPrinterDialogData*) lParam );
					
		
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

				case IDC_VDB_SETUP:
					pDlgClass->OnSetup( hwnd );
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

//-------------------------------------
BOOL vdbPrinterDialogClass::OnInitDialog( HWND hwnd, vdbPrinterDialogData* pData )
{
	assert( pData != 0 );
	_pData = pData;

	vdbCenterWindow( hwnd );

	// enable or disable the page range button and edit controls
	if ( _pData->_enablePrintRange == true )
	{
		EnableWindow( GetDlgItem( hwnd, IDC_VDB_PAGES ), true );
		EnableWindow( GetDlgItem( hwnd, IDC_VDB_FROM_PAGE ), true );
		EnableWindow( GetDlgItem( hwnd, IDC_VDB_TO_PAGE ), true );
	}
	else
	{
		EnableWindow( GetDlgItem( hwnd, IDC_VDB_PAGES ), false );
		EnableWindow( GetDlgItem( hwnd, IDC_VDB_FROM_PAGE ), false );
		EnableWindow( GetDlgItem( hwnd, IDC_VDB_TO_PAGE ), false );
	}

	// check one of the print range radio buttons
	if ( _pData->_printRange == vdbPrinterDialogData::ALL )
		SendDlgItemMessage( hwnd, IDC_VDB_ALL, BM_SETCHECK, BST_CHECKED, 0 );
	else
		SendDlgItemMessage( hwnd, IDC_VDB_PAGES, BM_SETCHECK, BST_CHECKED, 0 );

	char sz[10];
	vdbUtility::SafeFormat( sz, sizeof(sz), "%d", _pData->_fromPage );
	SendDlgItemMessage( hwnd, IDC_VDB_FROM_PAGE, WM_SETTEXT, 0, (long) sz );
	vdbUtility::SafeFormat( sz, sizeof(sz), "%d", _pData->_toPage );
	SendDlgItemMessage( hwnd, IDC_VDB_TO_PAGE, WM_SETTEXT, 0, (long) sz );

	// fill the printers combo
	vdbSet<vdbString> printerNames;
	int defaultPrinter = pphGetPrinters( printerNames );
	if ( defaultPrinter == -1 )
	{
		EnableWindow( GetDlgItem( hwnd, IDC_VDB_SETUP ), false );
		EnableWindow( GetDlgItem( hwnd, IDOK ), false );
	}
	else
	{
		for ( int i = 0; i < printerNames.Count(); i++ )
			SendDlgItemMessage( hwnd, IDC_VDB_PRINTERS, CB_ADDSTRING, 0, (long) (const char*) printerNames[i] );

		SendDlgItemMessage( hwnd, IDC_VDB_PRINTERS, CB_SETCURSEL, defaultPrinter, 0 );
	}

	// query the default printer to get the orientation
	if ( defaultPrinter != -1 && printerNames.Count() > 0 )
	{
		int dmOrientation = pphGetPrinterOrientation( printerNames[defaultPrinter] );
		if ( dmOrientation == 1 )
			SendDlgItemMessage( hwnd, IDC_VDB_PORTRAIT, BM_SETCHECK, BST_CHECKED, 0 );
		else if ( dmOrientation == 2 )
			SendDlgItemMessage( hwnd, IDC_VDB_LANDSCAPE, BM_SETCHECK, BST_CHECKED, 0 );
	}

	return TRUE;
}


//---------------------------------------
void vdbPrinterDialogClass::OnOK( HWND hwnd )
{
	ShowHourglass();

	DialogToData( hwnd );

	EndDialog( hwnd, IDOK );
	ShowPointer();
}


//-------------------------------------
void vdbPrinterDialogClass::DialogToData( HWND hwnd )
{
	// retrieve orientation
	if ( SendDlgItemMessage( hwnd, IDC_VDB_PORTRAIT, BM_GETCHECK, 0, 0 ) == BST_CHECKED )
		_pData->_dmOrientation = 1;
	if ( SendDlgItemMessage( hwnd, IDC_VDB_LANDSCAPE, BM_GETCHECK, 0, 0 ) == BST_CHECKED )
		_pData->_dmOrientation = 2;

	// get printer DC
	int which = SendDlgItemMessage( hwnd, IDC_VDB_PRINTERS, CB_GETCURSEL, 0, 0 );
	if ( which == -1 )
		return;
	int textLen = SendDlgItemMessage( hwnd, IDC_VDB_PRINTERS, CB_GETLBTEXTLEN, which, 0 );
	char* pszPrinter = new char[textLen+1];
	SendDlgItemMessage( hwnd, IDC_VDB_PRINTERS, CB_GETLBTEXT, which, (long) pszPrinter );
	vdbUtility::SafeCopy( _pData->_szPrinterName, pszPrinter, sizeof(_pData->_szPrinterName) );
	_pData->_printerDC = pphGetPrinterDC( pszPrinter, _pData->_dmOrientation );
	delete[] pszPrinter; pszPrinter = 0;
	if ( _pData->_printerDC == NULL )
		return;

	// retrieve print range
	if ( SendDlgItemMessage( hwnd, IDC_VDB_ALL, BM_GETCHECK, 0, 0 ) == BST_CHECKED )
		_pData->_printRange = vdbPrinterDialogData::ALL;
	if ( SendDlgItemMessage( hwnd, IDC_VDB_PAGES, BM_GETCHECK, 0, 0 ) == BST_CHECKED )
		_pData->_printRange = vdbPrinterDialogData::PAGES;

	// retrieve 'from' and 'to' pages
	char sz[10];
	SendDlgItemMessage( hwnd, IDC_VDB_FROM_PAGE, WM_GETTEXT, 9, (long) sz );
	_pData->_fromPage = atoi( sz );
	SendDlgItemMessage( hwnd, IDC_VDB_TO_PAGE, WM_GETTEXT, 9, (long) sz );
	_pData->_toPage = atoi( sz );
}


//---------------------------------------
void vdbPrinterDialogClass::OnCancel( HWND hwnd )
{
	EndDialog( hwnd, IDCANCEL );
}


//---------------------------------------
//^ The OnSetup function displays printer settings but does not use any changes made
//  by the user.  In order to use these changes, the DocumentProperties function
//  must receive a DEVMODE structure which must be passed back to the call to
//  CreateDC.  Leave this for the future (6/7/99).
//
void vdbPrinterDialogClass::OnSetup( HWND hwnd )
{
	// get selected printer
	int which = SendDlgItemMessage( hwnd, IDC_VDB_PRINTERS, CB_GETCURSEL, 0, 0 );
	if ( which == -1 )
		return;
	int textLen = SendDlgItemMessage( hwnd, IDC_VDB_PRINTERS, CB_GETLBTEXTLEN, which, 0 );
	char* pszPrinter = new char[textLen+1];
	SendDlgItemMessage( hwnd, IDC_VDB_PRINTERS, CB_GETLBTEXT, which, (long) pszPrinter );

	// get printer handle
	HANDLE hPrinter;
	if ( OpenPrinter( pszPrinter, &hPrinter, NULL ) == FALSE )
		return;
	if ( hPrinter == NULL )
		return;

	// show dialog box
	DocumentProperties( hwnd, hPrinter, pszPrinter, NULL, NULL, DM_IN_PROMPT );

	delete[] pszPrinter; pszPrinter = 0;
	ClosePrinter( hPrinter );
}
