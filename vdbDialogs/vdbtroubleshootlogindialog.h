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

#ifndef __TroubleshootLoginDlg_h
#define __TroubleshootLoginDlg_h

#include "vdbUtility.h"
#include "vdbExport.h"


//=============================================================================
//  Data transfer into and out of the dialog
//=============================================================================
class vdbTroubleshootLoginDlgData
{
public:
	vdbTroubleshootLoginDlgData() {}
	vdbTroubleshootLoginDlgData( const char* source, const char* id, const char* password )
	{
		assert ( source != 0 );
		vdbUtility::SafeCopy( szDataSource, source, sizeof(szDataSource) );
		
		if ( id != 0 )
			vdbUtility::SafeCopy( szAuthenticationID, id, sizeof(szAuthenticationID) );
		else
			vdbUtility::SafeCopy( szAuthenticationID, "", sizeof(szAuthenticationID) );
	
		if ( password != 0 )
			vdbUtility::SafeCopy( szAuthenticationPassword, password, sizeof(szAuthenticationPassword) );
		else
			vdbUtility::SafeCopy( szAuthenticationPassword, "", sizeof(szAuthenticationPassword) );
}

public:
	char szDataSource[40];
	char szAuthenticationID[40];
	char szAuthenticationPassword[40];
};


//=============================================================================
//  Public function for showing the dialog
//=============================================================================
extern "C" bool vdbTroubleshootLoginDialog( vdbTroubleshootLoginDlgData& data );


//=============================================================================
//  Class data and member functions for handling window messages
//=============================================================================
class TroubleshootLoginDlgClass
{
	friend BOOL CALLBACK TroubleshootLoginDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

private:	 
	// message processing functions
	BOOL OnInitDialog( HWND hwnd, vdbTroubleshootLoginDlgData* pData );
	void OnOK( HWND hwnd );
	void OnCancel( HWND hwnd );
	void OnSQLServerManager( HWND hwnd );
	void OnODBCManager( HWND hwnd );

private:
	vdbTroubleshootLoginDlgData* _pData;
};


#endif // __TroubleshootLoginDlg_h
