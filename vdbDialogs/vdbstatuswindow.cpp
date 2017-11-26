//=============================================================================
//
// CLASS:           vdbStatusWindow
//
// ESSENCE:         A modeless dialog for displaying status to the user during
//                  lengthy operations.
//
// FILE NAME:       vdbStatusWindow.h
//
// INITIAL DATE:    10/1/96 
//
// COPYRIGHT:       Copyright © 1996 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#include <vdbWindows.h>
#include "vdbStatusWindow.h"


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
//^^ Constructor
vdbStatusWindow::vdbStatusWindow( const char* szWindowTitle, CWnd* pParent )
	: vdbDLLDialog(vdbStatusWindow::IDD, pParent),
	  _sWindowTitle( szWindowTitle )
{
	vdbDLLDialog::Create( vdbStatusWindow::IDD, pParent );

	//{{AFX_DATA_INIT(vdbStatusWindow)
	//}}AFX_DATA_INIT
}


//-------------------------------------
//^^ DoDataExchange
void vdbStatusWindow::DoDataExchange(CDataExchange* pDX)
{
	vdbDLLDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(vdbStatusWindow)
	DDX_Control(pDX, IDC_VDB_CLOSE, m_ctlClose);
	DDX_Control(pDX, IDC_VDB_TEXT, m_ctlText);
	//}}AFX_DATA_MAP
}


//-------------------------------------
// MessageMap
BEGIN_MESSAGE_MAP(vdbStatusWindow, vdbDLLDialog)
	//{{AFX_MSG_MAP(vdbStatusWindow)
	ON_BN_CLICKED(IDC_VDB_CLOSE, OnClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
BOOL vdbStatusWindow::OnInitDialog() 
{
	vdbDLLDialog::OnInitDialog();
	
	CenterWindow();
	SetWindowText( _sWindowTitle );
	m_ctlClose.ShowWindow( SW_HIDE );

	return TRUE;
}


//-------------------------------------
void vdbStatusWindow::Message( const char* szText )
{
	m_ctlText.SetWindowText( szText );
}


//-------------------------------------
void vdbStatusWindow::ShowCloseButton( bool show )
{
	if ( show == true )
		m_ctlClose.ShowWindow( SW_SHOW );
	else
		m_ctlClose.ShowWindow( SW_HIDE );

}


//-------------------------------------
void vdbStatusWindow::OnClose() 
{
	DestroyWindow();
	delete this;	
}
