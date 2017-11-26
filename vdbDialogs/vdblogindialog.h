//=============================================================================
//
// CLASS:           LoginDlg
//
// ESSENCE:         Login Dialog procedure and class
//
// FILE NAME:       LoginDlg.cpp
//
// INITIAL DATE:    1/20/96
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __LoginDlg_h
#define __LoginDlg_h

#include "vdbUtility.h"
#include "vdbExport.h"


//=============================================================================
//  Data transfer into and out of the dialog
//=============================================================================
class vdbLoginDlgData
{
public:
	vdbLoginDlgData() {}
	vdbLoginDlgData( const char* source, const char* id, const char* password )
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
extern "C" bool vdbLoginDialog( vdbLoginDlgData& data );


//=============================================================================
//  Class data and member functions for handling window messages
//=============================================================================
class LoginDlgClass
{
	friend BOOL CALLBACK LoginDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

private:	 
	// message processing functions
	BOOL OnInitDialog( HWND hwnd, vdbLoginDlgData* pData );
	void OnOK( HWND hwnd );
	void OnCancel( HWND hwnd );

private:
	vdbLoginDlgData* _pData;
};


#endif // __LoginDlg_h
