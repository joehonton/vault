//=============================================================================
//
// CLASS:           DataSourceDlg
//
// ESSENCE:         Data sources dialog procedure and class
//
// FILE NAME:       DataSourceDlg.cpp
//
// INITIAL DATE:    9/7/97
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1997 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbDataSource.h"
#include "vdbAssert.h"
#include "..\\vdbResources\\resource.h"
#include "VaultResourceInstance.h"
#include "vdbWindowSupport.h"
#include "vdbString.h"
#include <vdbDatabase.h>
#include <vdbDatabaseNew.h>
#include <vdbDatabaseOpen.h>


//=============================================================================
//  Public function for showing the dialog
//=============================================================================

//-------------------------------------
//^ The vdbDataSourceDialog function shows the dialog box
//> data is the structure for transferring data into and out of the dialog
//< returns true if the user presses OK, false if the user presses Cancel
//
bool vdbDataSourceDialog( vdbDataSourceDlgData& data )
{
	assert ( &data != 0 );
	int rc = DialogBoxParam( VaultResourceInstance::Handle(), MAKEINTRESOURCE(IDD_LIST_DATASOURCES), NULL, (DLGPROC) DataSourceDlgProc, (LPARAM) &data );
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
// Procedure for Data sources dialog box
BOOL CALLBACK DataSourceDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static DataSourceDlgClass* pDlgClass = 0;

	switch ( message )
	{
		case WM_INITDIALOG:
			if ( !(pDlgClass = new DataSourceDlgClass) )
				return EndDialog( hwnd, IDABORT );
			else
				return pDlgClass->OnInitDialog( hwnd, (vdbDataSourceDlgData*) lParam );
					
		
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

				case IDC_VDB_NEW_DATABASE:
					pDlgClass->OnNewDatabase( hwnd );
					break;

				case IDC_VDB_LIST_SOURCES:
					if ( HIWORD( wParam ) == LBN_SELCHANGE )
						pDlgClass->OnSelchangeSource( hwnd );
					else if ( HIWORD( wParam ) == LBN_DBLCLK )
						pDlgClass->OnDblClickSource( hwnd );
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
BOOL DataSourceDlgClass::OnInitDialog( HWND hwnd, vdbDataSourceDlgData* pData )
{
	assert( pData != 0 );
	_pData = pData;

	vdbCenterWindow( hwnd );

	// add all sources to listbox
	char szSources[50][SQL_MAX_DSN_LENGTH+1];
	char szDescriptions[50][255];
	int numSources = 50;
	ListDataSources( szSources, szDescriptions, numSources );
	for ( int i = 0; i < numSources; i++ )
		SendDlgItemMessage( hwnd, IDC_VDB_LIST_SOURCES, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) szSources[i] );

	// find the user supplied default and highlight
	SendDlgItemMessage( hwnd, IDC_VDB_LIST_SOURCES, LB_SELECTSTRING, -1, (LPARAM) (LPCTSTR) _pData->_sSelectedDataSource );
	OnSelchangeSource( hwnd );

	if ( pData->_pDB != 0 )
		EnableWindow( GetDlgItem( hwnd, IDC_VDB_NEW_DATABASE ), TRUE );
	else		
		EnableWindow( GetDlgItem( hwnd, IDC_VDB_NEW_DATABASE ), FALSE );

	return TRUE;
}


//---------------------------------------
void DataSourceDlgClass::OnOK( HWND hwnd )
{
	// get the selected item
	int which = SendDlgItemMessage( hwnd, IDC_VDB_LIST_SOURCES, LB_GETCURSEL, 0, 0 );

	if ( which == LB_ERR )
		_pData->_sSelectedDataSource = "";
	else
	{
		char sz[SQL_MAX_DSN_LENGTH+1];
		SendDlgItemMessage( hwnd, IDC_VDB_LIST_SOURCES, LB_GETTEXT, which, (LPARAM) (LPCTSTR) sz );
		_pData->_sSelectedDataSource = sz;
	}

	EndDialog( hwnd, IDOK );
}


//---------------------------------------
void DataSourceDlgClass::OnCancel( HWND hwnd )
{
	_pData->_sSelectedDataSource = "";
	EndDialog( hwnd, IDCANCEL );
}


//---------------------------------------
//^ The OnNewDatabase functions shows the dialog to create a new database.
//  If the creation was successful, pass the new data source name back to
//  the caller just like a normal open.
//
void DataSourceDlgClass::OnNewDatabase( HWND hwnd )
{
	assert( _pData->_pDB != 0 );

	vdbDatabaseNew dlg( _pData->_pDB );
	if ( dlg.DoModal() == IDOK )
	{
		_pData->_sSelectedDataSource = dlg._sDataSource;
		EndDialog( hwnd, IDOK );
	}
	else
	{
		_pData->_sSelectedDataSource = "";
		EndDialog( hwnd, IDCANCEL );
	}
}


//---------------------------------------
void DataSourceDlgClass::OnSelchangeSource( HWND hwnd )
{
	if ( SendDlgItemMessage( hwnd, IDC_VDB_LIST_SOURCES, LB_GETCURSEL, 0, 0 ) == -1 )
		EnableWindow( GetDlgItem( hwnd, IDOK ), FALSE );
	else		
		EnableWindow( GetDlgItem( hwnd, IDOK ), TRUE );
}


//---------------------------------------
void DataSourceDlgClass::OnDblClickSource( HWND hwnd )
{
	OnOK( hwnd );
}
