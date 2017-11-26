//=============================================================================
//
// CLASS:           ReportPreview
//
// OVERVIEW:        Window to display a preview of a report
//
// FILE NAME:       ReportPreview.cpp
//
// INITIAL DATE:    10/26/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportPreview.h"
#include <cursors.h>


//=============================================================================
// Essential class functions
//=============================================================================

ReportPreview::ReportPreview( ReportDefinition* pDef )
	: vdbDLLDialog( ReportPreview::IDD, NULL ),
	  _pDef( pDef )
{
	//{{AFX_DATA_INIT(ReportPreview)
	//}}AFX_DATA_INIT
}


//-------------------------------------
ReportPreview::~ReportPreview()
{
}


//-------------------------------------
BEGIN_MESSAGE_MAP(ReportPreview, vdbDLLDialog)
	//{{AFX_MSG_MAP(ReportPreview)
	ON_WM_SIZE()
	ON_EN_ERRSPACE(IDC_VDB_EDIT, OnErrspaceEdit)
	ON_EN_MAXTEXT(IDC_VDB_EDIT, OnMaxtextEdit)
	ON_EN_SETFOCUS(IDC_VDB_EDIT, OnSetfocusEdit)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
// Creation and initialization
//=============================================================================

//-------------------------------------------
int ReportPreview::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (vdbDLLDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}


//----------------------------------------
void ReportPreview::DoDataExchange(CDataExchange* pDX)
{
	vdbDLLDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ReportPreview)
	DDX_Control(pDX, IDC_VDB_EDIT, m_ctlEdit);
	//}}AFX_DATA_MAP
}


//----------------------------------------
//^ The OnInitDialog function
//
BOOL ReportPreview::OnInitDialog() 
{
	vdbDLLDialog::OnInitDialog();
	
	courierFont.CreatePointFont( 100, "Courier New" );
	m_ctlEdit.SetFont( &courierFont, TRUE );
	m_ctlEdit.SetTabStops( 50 );
	m_ctlEdit.SetMargins( 20, 20 );

	RECT rectDesktop;
	SystemParametersInfo( SPI_GETWORKAREA, 0, &rectDesktop, 0 );

	RECT newRect;
	newRect.left = rectDesktop.left + 20;
	newRect.top = rectDesktop.top + 20;
	newRect.right = rectDesktop.right - rectDesktop.left - 40;
	newRect.bottom = rectDesktop.bottom - rectDesktop.top - 40;
	MoveWindow( &newRect, TRUE );

	SetWindowText( _pDef->_sTitle );

	// assemble the query, execute, and add to window
	ShowHourglass();
	_pDef->Execute( m_ctlEdit );
	ShowPointer();
		
	return TRUE;
}


//-------------------------------------
void ReportPreview::OnSize(UINT nType, int cx, int cy) 
{
	vdbDLLDialog::OnSize(nType, cx, cy);
	
	if ( IsWindow( m_ctlEdit.m_hWnd ) )
	{
		int spacer = 7;
		m_ctlEdit.MoveWindow( spacer, spacer, cx - (2*spacer), cy - (2*spacer) );
	}
}


//=============================================================================
// Helper functions
//=============================================================================


//=============================================================================
// User interface callbacks
//=============================================================================


//-------------------------------------------
//^ The OnErrspaceEdit function is triggered when the trace window has run out
//  of space.
//
void ReportPreview::OnErrspaceEdit() 
{
}


//-------------------------------------------
//^ The OnMaxtextEdit function is triggered when the trace window has exceeded the
//  maximum length allowed.
//
void ReportPreview::OnMaxtextEdit() 
{
	OnErrspaceEdit();
}


//-------------------------------------------
void ReportPreview::OnSetfocusEdit() 
{
	m_ctlEdit.SetSel( -1, -1 );
}




