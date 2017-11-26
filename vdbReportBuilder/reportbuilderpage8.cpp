//=============================================================================
//
// CLASS:           ReportBuilderPage8
//
// ESSENCE:         Property page for showing all defined sorts 
//
// FILE NAME:       ReportBuilderPage8.cpp
//
// INITIAL DATE:    10/26/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportBuilderPage8.h"
#include "ReportBuilderPageHolder.h"


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
//^^ Constructor
ReportBuilderPage8::ReportBuilderPage8( ReportBuilderPageHolder* pParent )
	: CPropertyPage( ReportBuilderPage8::IDD ),
	  _pParent( pParent )
{
	m_psp.hInstance = VaultResourceInstance::Handle();

	//{{AFX_DATA_INIT(ReportBuilderPage8)
	//}}AFX_DATA_INIT
}


//-------------------------------------
//^^ Destructor
ReportBuilderPage8::~ReportBuilderPage8()
{
}


//=============================================================================
// Data exchange and message map
//=============================================================================

//-------------------------------------
//^^ DoDataExchange
void ReportBuilderPage8::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ReportBuilderPage8)
	DDX_Control(pDX, IDC_VDB_SORTS, m_ctlSorts);
	DDX_Control(pDX, IDC_VDB_REMOVE, m_ctlRemove);
	DDX_Control(pDX, IDC_VDB_YES_PLEASE, m_ctlYes);
	DDX_Control(pDX, IDC_VDB_NO_THANK_YOU, m_ctlNo);
	//}}AFX_DATA_MAP
}


//-------------------------------------
// MessageMap
BEGIN_MESSAGE_MAP(ReportBuilderPage8, CPropertyPage)
	//{{AFX_MSG_MAP(ReportBuilderPage8)
	ON_BN_CLICKED(IDC_VDB_REMOVE, OnRemove)
	ON_LBN_SELCHANGE(IDC_VDB_SORTS, OnSelchangeSorts)
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
BOOL ReportBuilderPage8::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	return FALSE;
}


//-------------------------------------
// The OnSetActive function is called when this page becomes the active page.
//
BOOL ReportBuilderPage8::OnSetActive() 
{
	BOOL rc = CPropertyPage::OnSetActive();

	_pParent->_bDefineAnotherSort = false;
	m_ctlNo.SetCheck( 1 );
	m_ctlYes.SetCheck( 0 );

	BuildSortList();
	OnSelchangeSorts();

	_pParent->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	
	return rc;
}


//-------------------------------------
//^ The OnWizardNext function is called when the user presses the Next button.
//
LRESULT ReportBuilderPage8::OnWizardNext() 
{
	if ( MoveDataToHolder( true ) == true )
		return _pParent->DetermineNextPage( IDD_REPORT_BUILDER8 );
	else
		return -1;
}


//-------------------------------------
//^ The OnWizardBack function is called when the user presses the Back button.
//
LRESULT ReportBuilderPage8::OnWizardBack() 
{
	MoveDataToHolder( false );

	// delete the most recently defined sort
	int count = _pParent->_pDef->_sortCriteria.Count();
	if ( count > 0 )
		_pParent->_pDef->_sortCriteria.Remove( count - 1 );


	return _pParent->DetermineBackPage( IDD_REPORT_BUILDER8 );
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
bool ReportBuilderPage8::MoveDataToHolder( bool bValidate )
{
	if ( bValidate == true )
	{
		if ( m_ctlYes.GetCheck() == 1 )
			_pParent->_bDefineAnotherSort = true;
		else if ( m_ctlNo.GetCheck() == 1 )
			_pParent->_bDefineAnotherSort = false;
		else
		{
			vdbMessageBox( (int) m_hWnd, "Please select whether or not to add another sort", "Report Builder", MB_OK );
			return false;
		}
	}

	return true;
}


//=============================================================================
// Utility functions
//=============================================================================

//-------------------------------------
void ReportBuilderPage8::BuildSortList()
{
	m_ctlSorts.ResetContent();

	vdbSet<ReportSortCriteria>& sorts = _pParent->_pDef->_sortCriteria;

	int count = sorts.Count();
	for ( int i = 0; i < count; i++ )
	{
		ReportSortCriteria& criteria = sorts[i];

		vdbString s;
		s.Format( "%s  %s", (const char*) criteria._sColumnName, (const char*) criteria._sOrder );
		m_ctlSorts.AddString( s );
	}
}


//=============================================================================
// User interface callback functions
//=============================================================================

//-------------------------------------
void ReportBuilderPage8::OnSelchangeSorts() 
{
	int index = m_ctlSorts.GetCurSel();
	if ( index == -1 )
		m_ctlRemove.EnableWindow( FALSE );
	else
		m_ctlRemove.EnableWindow( TRUE );
}


//-------------------------------------
void ReportBuilderPage8::OnRemove() 
{
	int index = m_ctlSorts.GetCurSel();
	if ( index == -1 )
		return;

	// the listbox control and the vdbSet are ordered identically, so just delete using the index
	vdbSet<ReportSortCriteria>& sorts = _pParent->_pDef->_sortCriteria;
	sorts.Remove( index );

	// rebuild the list because the vdbSet is reordered by the Remove function		
	BuildSortList();
	OnSelchangeSorts();
}


#define IDC_WIZARD_NEXT		0x3024

//-------------------------------------
void ReportBuilderPage8::OnNoThankYou() 
{
}


//-------------------------------------
void ReportBuilderPage8::OnYesPlease() 
{
	HWND hNext = ::GetDlgItem( _pParent->m_hWnd, IDC_WIZARD_NEXT );

	WPARAM wParam = MAKEWPARAM( IDC_WIZARD_NEXT, BN_CLICKED );

	::SendMessage( _pParent->m_hWnd, WM_COMMAND, wParam, (LPARAM) hNext );
}
