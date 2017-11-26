//=============================================================================
//
// CLASS:           ReportBuilderPage3
//
// ESSENCE:         Property page for choosing whether or not to filter records
//
// FILE NAME:       ReportBuilderPage3.cpp
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportBuilderPage3.h"
#include "ReportBuilderPageHolder.h"


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
//^^ Constructor
ReportBuilderPage3::ReportBuilderPage3( ReportBuilderPageHolder* pParent )
	: CPropertyPage( ReportBuilderPage3::IDD ),
	  _pParent( pParent )
{
	m_psp.hInstance = VaultResourceInstance::Handle();

	//{{AFX_DATA_INIT(ReportBuilderPage3)
	//}}AFX_DATA_INIT
}


//-------------------------------------
//^^ Destructor
ReportBuilderPage3::~ReportBuilderPage3()
{
}


//=============================================================================
// Data exchange and message map
//=============================================================================

//-------------------------------------
//^^ DoDataExchange
void ReportBuilderPage3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ReportBuilderPage3)
	DDX_Control(pDX, IDC_VDB_YES_PLEASE, m_ctlYes);
	DDX_Control(pDX, IDC_VDB_NO_THANK_YOU, m_ctlNo);
	//}}AFX_DATA_MAP
}


//-------------------------------------
// MessageMap
BEGIN_MESSAGE_MAP(ReportBuilderPage3, CPropertyPage)
	//{{AFX_MSG_MAP(ReportBuilderPage3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
// Message response functions
//=============================================================================

//-------------------------------------
// The OnInitDialog function is called the first time this page is shown
//
BOOL ReportBuilderPage3::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();


	return FALSE;
}


//-------------------------------------
// The OnSetActive function is called when this page becomes the active page.
//
BOOL ReportBuilderPage3::OnSetActive() 
{
	BOOL rc = CPropertyPage::OnSetActive();

	if( _pParent->_pDef->_selectionCriteria.Count() > 0 )
//	if ( _pParent->_bUseSelectionCriteria == true )
	{
		m_ctlYes.SetCheck( 1 );
		m_ctlNo.SetCheck( 0 );
	}
	else
	{
		m_ctlYes.SetCheck( 0 );
		m_ctlNo.SetCheck( 1 );
	}

	_pParent->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	
	return rc;
}


//-------------------------------------
//^ The OnWizardNext function is called when the user presses the Next button.
//
LRESULT ReportBuilderPage3::OnWizardNext() 
{
	if ( MoveDataToHolder( true ) == true )
		return _pParent->DetermineNextPage( IDD_REPORT_BUILDER3 );
	else
		return -1;
}


//-------------------------------------
//^ The OnWizardBack function is called when the user presses the Back button.
//
LRESULT ReportBuilderPage3::OnWizardBack() 
{
	MoveDataToHolder( false );
	return _pParent->DetermineBackPage( IDD_REPORT_BUILDER3 );
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
bool ReportBuilderPage3::MoveDataToHolder( bool bValidate )
{
	if ( bValidate == true )
	{
		if ( m_ctlYes.GetCheck() == 1 )
			_pParent->_bUseSelectionCriteria = true;
		else if ( m_ctlNo.GetCheck() == 1 )
		{
			_pParent->_bUseSelectionCriteria = false;
			_pParent->_pDef->_selectionCriteria.ClearAll();
		}
		else
		{
			vdbMessageBox( (int) m_hWnd, "Please select whether or not to use filters", "Report Builder", MB_OK );
			return false;
		}
	}

	return true;
}


