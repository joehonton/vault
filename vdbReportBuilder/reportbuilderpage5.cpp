//=============================================================================
//
// CLASS:           ReportBuilderPage5
//
// ESSENCE:         Property page for showing all defined filters 
//
// FILE NAME:       ReportBuilderPage5.cpp
//
// INITIAL DATE:    10/26/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportBuilderPage5.h"
#include "ReportBuilderPageHolder.h"


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
//^^ Constructor
ReportBuilderPage5::ReportBuilderPage5( ReportBuilderPageHolder* pParent )
	: CPropertyPage( ReportBuilderPage5::IDD ),
	  _pParent( pParent )
{
	m_psp.hInstance = VaultResourceInstance::Handle();

	//{{AFX_DATA_INIT(ReportBuilderPage5)
	//}}AFX_DATA_INIT
}


//-------------------------------------
//^^ Destructor
ReportBuilderPage5::~ReportBuilderPage5()
{
}


//=============================================================================
// Data exchange and message map
//=============================================================================

//-------------------------------------
//^^ DoDataExchange
void ReportBuilderPage5::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ReportBuilderPage5)
	DDX_Control(pDX, IDC_VDB_FILTERS, m_ctlFilters);
	DDX_Control(pDX, IDC_VDB_REMOVE, m_ctlRemove);
	DDX_Control(pDX, IDC_VDB_AND_OR, m_ctlAndOr);
	DDX_Control(pDX, IDC_VDB_YES_PLEASE, m_ctlYes);
	DDX_Control(pDX, IDC_VDB_NO_THANK_YOU, m_ctlNo);
	//}}AFX_DATA_MAP
}


//-------------------------------------
// MessageMap
BEGIN_MESSAGE_MAP(ReportBuilderPage5, CPropertyPage)
	//{{AFX_MSG_MAP(ReportBuilderPage5)
	ON_BN_CLICKED(IDC_VDB_REMOVE, OnRemove)
	ON_LBN_SELCHANGE(IDC_VDB_FILTERS, OnSelchangeFilters)
	ON_BN_CLICKED(IDC_VDB_AND_OR, OnAndOr)
	ON_BN_CLICKED(IDC_VDB_NO_THANK_YOU, OnNoThankYou)
	ON_BN_CLICKED(IDC_VDB_YES_PLEASE, OnYesPlease)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
// Message response functions
//=============================================================================

//-------------------------------------
// The OnInitDialog function is called the first time this page is shown
//
BOOL ReportBuilderPage5::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	return FALSE;
}


//-------------------------------------
// The OnSetActive function is called when this page becomes the active page.
//
BOOL ReportBuilderPage5::OnSetActive() 
{
	BOOL rc = CPropertyPage::OnSetActive();

	_pParent->_bDefineAnotherSelection = false;
	m_ctlNo.SetCheck( 1 );
	m_ctlYes.SetCheck( 0 );

	BuildFilterList();
	OnSelchangeFilters();

	_pParent->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	
	return rc;
}


//-------------------------------------
//^ The OnWizardNext function is called when the user presses the Next button.
//
LRESULT ReportBuilderPage5::OnWizardNext() 
{
	if ( MoveDataToHolder( true ) == true )
		return _pParent->DetermineNextPage( IDD_REPORT_BUILDER5 );
	else
		return -1;
}


//-------------------------------------
//^ The OnWizardBack function is called when the user presses the Back button.
//
LRESULT ReportBuilderPage5::OnWizardBack() 
{
	MoveDataToHolder( false );

	// delete the most recently defined filter
	int count = _pParent->_pDef->_selectionCriteria.Count();
	if ( count > 0 )
		_pParent->_pDef->_selectionCriteria.Remove( count - 1 );

	return _pParent->DetermineBackPage( IDD_REPORT_BUILDER5 );
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
bool ReportBuilderPage5::MoveDataToHolder( bool bValidate )
{
	if ( bValidate == true )
	{
		if ( m_ctlYes.GetCheck() == 1 )
			_pParent->_bDefineAnotherSelection = true;
		else if ( m_ctlNo.GetCheck() == 1 )
			_pParent->_bDefineAnotherSelection = false;
		else
		{
			vdbMessageBox( (int) m_hWnd, "Please select whether or not to add another filters", "Report Builder", MB_OK );
			return false;
		}
	}

	return true;
}


//=============================================================================
// Utility functions
//=============================================================================

//-------------------------------------
void ReportBuilderPage5::BuildFilterList()
{
	m_ctlFilters.ResetContent();

	vdbSet<ReportSelectionCriteria>& filters = _pParent->_pDef->_selectionCriteria;

	int count = filters.Count();
	for ( int i = 0; i < count; i++ )
	{
		ReportSelectionCriteria& criteria = filters[i];

		vdbString s1;
		if ( i == 0 )
			s1 = "   ";
		else if ( criteria._sConnector == "AND" )
			s1 = "AND";
		else if ( criteria._sConnector == "OR" )
			s1 = "OR ";
		else
			s1 = "???";

		vdbString s;
		s.Format( "%s  %s  %s  \"%s\"", (const char*) s1, (const char*) criteria._sColumnName, (const char*) criteria._sOperator, (const char*) criteria._sValue );
		m_ctlFilters.AddString( s );
	}
}


//=============================================================================
// User interface callback functions
//=============================================================================

//-------------------------------------
void ReportBuilderPage5::OnSelchangeFilters() 
{
	int index = m_ctlFilters.GetCurSel();
	if ( index == -1 )
	{
		m_ctlAndOr.EnableWindow( FALSE );
		m_ctlRemove.EnableWindow( FALSE );
	}
	else
	{
		m_ctlAndOr.EnableWindow( TRUE );
		m_ctlRemove.EnableWindow( TRUE );
	}
}


//-------------------------------------
void ReportBuilderPage5::OnRemove() 
{
	int index = m_ctlFilters.GetCurSel();
	if ( index == -1 )
		return;

	// the listbox control and the vdbSet are ordered identically, so just delete using the index
	vdbSet<ReportSelectionCriteria>& filters = _pParent->_pDef->_selectionCriteria;
	filters.Remove( index );

	// rebuild the list because the vdbSet is reordered by the Remove function		
	BuildFilterList();
	OnSelchangeFilters();
}


//-------------------------------------
void ReportBuilderPage5::OnAndOr() 
{
	int index = m_ctlFilters.GetCurSel();
	if ( index == -1 )
		return;

	vdbSet<ReportSelectionCriteria>& filters = _pParent->_pDef->_selectionCriteria;
	ReportSelectionCriteria& criteria = filters[index];

	// toggle
	if ( criteria._sConnector == "AND" )
		criteria._sConnector = "OR";
	else
		criteria._sConnector = "AND";

	// remove from the listbox
	m_ctlFilters.DeleteString( index );

	// add back to the listbox
	vdbString s1;
	if ( index == 0 )
		s1 = "   ";
	else if ( criteria._sConnector == "AND" )
		s1 = "AND";
	else if ( criteria._sConnector == "OR" )
		s1 = "OR ";
	else
		s1 = "???";

	vdbString s;
	s.Format( "%s  %s  %s  \"%s\"", (const char*) s1, (const char*) criteria._sColumnName, (const char*) criteria._sOperator, (const char*) criteria._sValue );
	m_ctlFilters.InsertString( index, s );
}


#define IDC_WIZARD_NEXT		0x3024

//-------------------------------------
void ReportBuilderPage5::OnNoThankYou() 
{
}


//-------------------------------------
void ReportBuilderPage5::OnYesPlease() 
{
	HWND hNext = ::GetDlgItem( _pParent->m_hWnd, IDC_WIZARD_NEXT );

	WPARAM wParam = MAKEWPARAM( IDC_WIZARD_NEXT, BN_CLICKED );

	::SendMessage( _pParent->m_hWnd, WM_COMMAND, wParam, (LPARAM) hNext );
}



