//=============================================================================
//
// CLASS:           vdbTroubleshootLoginDlg
//
// ESSENCE:         Dialog for troubleshooting a failed connection attempt
//
// FILE NAME:       vdbTroubleshootLoginDlg.cpp
//
// INITIAL DATE:    3/8/00
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbAssert.h"
#include "vdbDataSource.h"
#include "vdbTroubleshootLoginDialog.h"
#include "..\\vdbResources\\resource.h"
#include "VaultResourceInstance.h"
#include "vdbWindowSupport.h"
#include <vdbFilename.h>
#include <vdbMessageBox.h>
#include <vdbRegistrationKey.h>
#include "vdbDatabaseOpen.h"


//=============================================================================
//  Public function for showing the dialog
//=============================================================================

//-------------------------------------
//^ The vdbTroubleshootLoginDialog function shows the dialog box
//> data is the structure for transferring data into and out of the dialog
//< returns true if the user presses OK, false if the user presses Cancel
//
bool vdbTroubleshootLoginDialog( vdbTroubleshootLoginDlgData& data )
{
	assert ( &data != 0 );
	int rc = DialogBoxParam( VaultResourceInstance::Handle(), MAKEINTRESOURCE(IDD_TROUBLESHOOT_LOGIN), NULL, (DLGPROC) TroubleshootLoginDlgProc, (LPARAM) &data );
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
BOOL CALLBACK TroubleshootLoginDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static TroubleshootLoginDlgClass* pDlgClass = 0;

	switch ( message )
	{
		case WM_INITDIALOG:
			if ( !(pDlgClass = new TroubleshootLoginDlgClass) )
				return EndDialog( hwnd, IDABORT );
			else
				return pDlgClass->OnInitDialog( hwnd, (vdbTroubleshootLoginDlgData*) lParam );
					
		
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

				case IDC_VDB_SQL_SERVER_MANAGER:
					pDlgClass->OnSQLServerManager( hwnd );
					break;

				case IDC_VDB_ODBC_MANAGER:
					pDlgClass->OnODBCManager( hwnd );
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
BOOL TroubleshootLoginDlgClass::OnInitDialog( HWND hwnd, vdbTroubleshootLoginDlgData* pData )
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
		SendDlgItemMessage( hwnd, IDC_VDB_DATASOURCE, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) szSources[i] );

	// find the user supplied default and highlight
	SendDlgItemMessage( hwnd, IDC_VDB_DATASOURCE, CB_SELECTSTRING, -1, (LPARAM) (LPCTSTR) _pData->szDataSource );

	// fill other two controls with default values passed in
	SendDlgItemMessage( hwnd, IDC_VDB_IDENTIFICATION, WM_SETTEXT, 0, (long) _pData->szAuthenticationID );
	SendDlgItemMessage( hwnd, IDC_VDB_PASSWORD, WM_SETTEXT, 0, (long) _pData->szAuthenticationPassword );
	return TRUE;
}


//---------------------------------------
void TroubleshootLoginDlgClass::OnOK( HWND hwnd )
{
	// get three control values
	SendDlgItemMessage( hwnd, IDC_VDB_DATASOURCE, WM_GETTEXT, sizeof(_pData->szDataSource), (long) _pData->szDataSource );
	SendDlgItemMessage( hwnd, IDC_VDB_IDENTIFICATION, WM_GETTEXT, sizeof(_pData->szAuthenticationID), (long) _pData->szAuthenticationID );
	SendDlgItemMessage( hwnd, IDC_VDB_PASSWORD, WM_GETTEXT, sizeof(_pData->szAuthenticationPassword), (long) _pData->szAuthenticationPassword );
	EndDialog( hwnd, IDOK );
}


//-------------------------------------
void TroubleshootLoginDlgClass::OnCancel( HWND hwnd )
{
	EndDialog( hwnd, IDCANCEL );
}


//-------------------------------------
void TroubleshootLoginDlgClass::OnSQLServerManager( HWND hwnd )
{
	// get the installation directory for SQL Server
	vdbString sSubkey( "SOFTWARE\\Microsoft\\Microsoft SQL Server\\80\\Tools\\ClientSetup" );
	vdbString sServerDir;
	vdbRegistrationKey key( HKEY_LOCAL_MACHINE, sSubkey, "SQLPath" );
	bool rc = key.GetValue( sServerDir );
	if ( rc == false )
	{
		vdbMessageBox( NULL, "The location of the SQL Server Desktop Engine Manager can't be determined.  Is it properly installed?\n\nTry accessing the SQL Server Desktop Engine Manager directly from the Start menu", "Troubleshoot Open Database", MB_OK );
		return;
	}

	vdbFilename f( sServerDir );
	f.Append( "Binn\\sqlmangr.exe" );
	if ( !f.Exists() )
	{
		vdbString s;
		s.Format( "SQL Server Desktop Engine Manager can't be found at %s as expected.  Is it properly installed?\n\nTry accessing the SQL Server Desktop Engine Manager directly from the Start menu", (const char*) f );
		vdbMessageBox( NULL, s, "Troubleshoot Open Database", MB_OK );
		return;
	}

	// Launch the SQL Manager
	STARTUPINFO startup;
	memset( &startup, 0x0, sizeof(STARTUPINFO) );
	startup.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;

	BOOL rc2 = CreateProcess( f, NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startup, &pi );

	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
}


//-------------------------------------
void TroubleshootLoginDlgClass::OnODBCManager( HWND hwnd )
{
	SendDlgItemMessage( hwnd, IDC_VDB_PASSWORD, WM_GETTEXT, sizeof(_pData->szAuthenticationPassword), (long) _pData->szAuthenticationPassword );
	vdbFilename f;

	OSVERSIONINFO ver;
	ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx( &ver );
	// Windows 95, 98, Me
	if ( ver.dwPlatformId  == VER_PLATFORM_WIN32_WINDOWS )
		f = "%windir%\\system\\odbcad32.exe";
	// Windows NT, 2000
	else if ( ver.dwPlatformId  == VER_PLATFORM_WIN32_NT )
		f = "%SystemRoot%\\system32\\odbcad32.exe";

	f.ReplaceEnvironmentVariables();
	if ( !f.Exists() )
	{
		vdbString s;
		s.Format( "The ODBC Manager is not at %s as expected.\n\nTry using the System Control Panel to access the ODBC Manager.", (const char*) f );
		vdbTextBox( s, MB_OK | MB_ICONEXCLAMATION );
		return;
	}

	// Launch ODBC
	STARTUPINFO startup;
	memset( &startup, 0x0, sizeof(STARTUPINFO) );
	startup.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;

	BOOL rc = CreateProcess( f, NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startup, &pi );

	if ( rc == TRUE )
		WaitForSingleObject( pi.hProcess, INFINITE );
	
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	// refresh data sources to combobox
	SendDlgItemMessage( hwnd, IDC_VDB_DATASOURCE, CB_RESETCONTENT, 0, 0 );
	char szSources[50][SQL_MAX_DSN_LENGTH+1];
	char szDescriptions[50][255];
	int numSources = 50;
	ListDataSources( szSources, szDescriptions, numSources );
	for ( int i = 0; i < numSources; i++ )
		SendDlgItemMessage( hwnd, IDC_VDB_DATASOURCE, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) szSources[i] );
	SendDlgItemMessage( hwnd, IDC_VDB_DATASOURCE, CB_SELECTSTRING, -1, (LPARAM) (LPCTSTR) _pData->szDataSource );
}
