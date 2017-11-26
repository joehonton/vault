//=============================================================================
//
// CLASS:           ReportBuilderPage9
//
// ESSENCE:         Property page for specifying the header footer and margin texts
//
// FILE NAME:       ReportBuilderPage9.cpp
//
// INITIAL DATE:    4/28/00
//
// COPYRIGHT:       Copyright © 2000 Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportBuilderPage9.h"
#include "ReportBuilderPageHolder.h"


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
//^^ Constructor
ReportBuilderPage9::ReportBuilderPage9( ReportBuilderPageHolder* pParent )
	: CPropertyPage( ReportBuilderPage9::IDD ),
	  _pParent( pParent )
{
	m_psp.hInstance = VaultResourceInstance::Handle();

	//{{AFX_DATA_INIT(ReportBuilderPage9)
	//}}AFX_DATA_INIT
}


//-------------------------------------
//^^ Destructor
ReportBuilderPage9::~ReportBuilderPage9()
{
}


//=============================================================================
// Data exchange and message map
//=============================================================================

//-------------------------------------
//^^ DoDataExchange
void ReportBuilderPage9::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ReportBuilderPage9)
	DDX_Control(pDX, IDC_VDB_HEADER, m_ctlHeader);
	DDX_Control(pDX, IDC_VDB_HEADER_COMMANDS, m_ctlHeaderCommands);
	DDX_Control(pDX, IDC_VDB_FOOTER, m_ctlFooter);
	DDX_Control(pDX, IDC_VDB_FOOTER_COMMANDS, m_ctlFooterCommands);
	DDX_Control(pDX, IDC_VDB_LEFT_MARGIN, m_ctlLeftMargin);
	DDX_Control(pDX, IDC_VDB_LEFT_MARGIN_COMMANDS, m_ctlLeftMarginCommands);
	DDX_Control(pDX, IDC_VDB_RIGHT_MARGIN, m_ctlRightMargin);
	DDX_Control(pDX, IDC_VDB_RIGHT_MARGIN_COMMANDS, m_ctlRightMarginCommands);
	//}}AFX_DATA_MAP
}


//-------------------------------------
// MessageMap
BEGIN_MESSAGE_MAP(ReportBuilderPage9, CPropertyPage)
	//{{AFX_MSG_MAP(ReportBuilderPage9)
	ON_BN_CLICKED(IDC_VDB_HEADER_COMMANDS, OnHeaderCommands)
	ON_BN_CLICKED(IDC_VDB_FOOTER_COMMANDS, OnFooterCommands)
	ON_BN_CLICKED(IDC_VDB_LEFT_MARGIN_COMMANDS, OnLeftMarginCommands)
	ON_BN_CLICKED(IDC_VDB_RIGHT_MARGIN_COMMANDS, OnRightMarginCommands)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
// Message response functions
//=============================================================================

//-------------------------------------
// The OnInitDialog function is called the first time this page is shown
//
BOOL ReportBuilderPage9::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	return FALSE;
}


//-------------------------------------
// The OnSetActive function is called when this page becomes the active page.
//
BOOL ReportBuilderPage9::OnSetActive() 
{
	BOOL rc = CPropertyPage::OnSetActive();

	m_ctlHeader.SetWindowText( _pParent->_pDef->_sHeader );
	m_ctlFooter.SetWindowText( _pParent->_pDef->_sFooter );
	m_ctlLeftMargin.SetWindowText( _pParent->_pDef->_sLeftMargin );
	m_ctlRightMargin.SetWindowText( _pParent->_pDef->_sRightMargin );

	_pParent->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	
	return rc;
}


//-------------------------------------
//^ The OnWizardFinish function is called when the user presses the Next button.
//
LRESULT ReportBuilderPage9::OnWizardNext() 
{
	if ( MoveDataToHolder( true ) == true )
		return _pParent->DetermineNextPage( IDD_REPORT_BUILDER9 );
	else
		return -1;
}


//-------------------------------------
//^ The OnWizardBack function is called when the user presses the Back button.
//
LRESULT ReportBuilderPage9::OnWizardBack() 
{
	MoveDataToHolder( false );
	return _pParent->DetermineBackPage( IDD_REPORT_BUILDER9 );
}


//=============================================================================
// Virtual function overrides
//=============================================================================

//-------------------------------------
//^ The MoveDataToHolder function is called whenever the user leaves this page.
//  Move the selected user options to the ReportBuilderPageHolder.
//> bValidate is true to validate user entries and false to skip validation.
//< return true to advance to next page, return false to stay on current page.
//
bool ReportBuilderPage9::MoveDataToHolder( bool bValidate )
{
	if ( bValidate == true )
	{
		char sz[100];
		m_ctlHeader.GetWindowText( sz, sizeof(sz) );
		_pParent->_pDef->_sHeader = sz;

		// set the report "title" to the same as the header
		if ( _pParent->_pDef->_sTitle.IsNull() )
			_pParent->_pDef->_sTitle = sz;
		
		m_ctlFooter.GetWindowText( sz, sizeof(sz) );
		_pParent->_pDef->_sFooter = sz;

		m_ctlLeftMargin.GetWindowText( sz, sizeof(sz) );
		_pParent->_pDef->_sLeftMargin = sz;

		m_ctlRightMargin.GetWindowText( sz, sizeof(sz) );
		_pParent->_pDef->_sRightMargin = sz;
	}

	return true;
}



//=============================================================================
// User interface callback functions
//=============================================================================

//-------------------------------------
void ReportBuilderPage9::OnHeaderCommands() 
{
}



//-------------------------------------
void ReportBuilderPage9::OnFooterCommands() 
{
}


//-------------------------------------
void ReportBuilderPage9::OnLeftMarginCommands() 
{
}



//-------------------------------------
void ReportBuilderPage9::OnRightMarginCommands() 
{
}

