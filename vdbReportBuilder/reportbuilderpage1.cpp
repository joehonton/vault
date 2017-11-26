//=============================================================================
//
// CLASS:           ReportBuilderPage1
//
// ESSENCE:         Property page for choosing which table to use
//
// FILE NAME:       ReportBuilderPage1.cpp
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportBuilderPage1.h"
#include "ReportBuilderPageHolder.h"


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
//^^ Constructor
ReportBuilderPage1::ReportBuilderPage1( ReportBuilderPageHolder* pParent )
	: CPropertyPage( ReportBuilderPage1::IDD ),
	  _pParent( pParent )
{
	m_psp.hInstance = VaultResourceInstance::Handle();

	//{{AFX_DATA_INIT(ReportBuilderPage1)
	//}}AFX_DATA_INIT
}


//-------------------------------------
//^^ Destructor
ReportBuilderPage1::~ReportBuilderPage1()
{
}


//=============================================================================
// Data exchange and message map
//=============================================================================

//-------------------------------------
//^^ DoDataExchange
void ReportBuilderPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ReportBuilderPage1)
	DDX_Control(pDX, IDC_VDB_TABLES, m_ctlTables);
	//}}AFX_DATA_MAP
}


//-------------------------------------
// MessageMap
BEGIN_MESSAGE_MAP(ReportBuilderPage1, CPropertyPage)
	//{{AFX_MSG_MAP(ReportBuilderPage1)
	ON_LBN_DBLCLK(IDC_VDB_TABLES, OnDblclkTables)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
// Message response functions
//=============================================================================

//-------------------------------------
// The OnInitDialog function is called the first time this page is shown
//
BOOL ReportBuilderPage1::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	GetAllTables();

	return FALSE;
}


//-------------------------------------
// The OnSetActive function is called when this page becomes the active page.
//
BOOL ReportBuilderPage1::OnSetActive() 
{
	BOOL rc = CPropertyPage::OnSetActive();

	int index = m_ctlTables.FindString( -1, _pParent->_pDef->_sTableName );
	if ( index != -1 )
		m_ctlTables.SetCurSel( index );

	_pParent->SetWizardButtons( PSWIZB_NEXT );
	
	return rc;
}


//-------------------------------------
//^ The OnWizardNext function is called when the user presses the Next button.
//
LRESULT ReportBuilderPage1::OnWizardNext() 
{
	if ( MoveDataToHolder( true ) == true )
		return _pParent->DetermineNextPage( IDD_REPORT_BUILDER1 );
	else
		return -1;
}


//-------------------------------------
//^ The OnWizardBack function is called when the user presses the Back button.
//
LRESULT ReportBuilderPage1::OnWizardBack() 
{
	MoveDataToHolder( false );
	return _pParent->DetermineBackPage( IDD_REPORT_BUILDER1 );
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
bool ReportBuilderPage1::MoveDataToHolder( bool bValidate )
{
	if ( bValidate == true )
	{
		int index = m_ctlTables.GetCurSel();
		if ( index == -1 )
		{
			vdbMessageBox( (int) m_hWnd, "Please select which type of record to use", "Report Builder", MB_OK );
			return false;
		}
		
		char sz[80];
		m_ctlTables.GetText( index, sz );
		_pParent->_pDef->_sTableName = sz;
	}

	return true;
}


//=============================================================================
// Utility functions
//=============================================================================

//-----------------------------------------
void ReportBuilderPage1::GetAllTables()
{
	char szTableName[80];

	vdbStatement stmt( vdbPool::Instance()->GetDatabase() );
	stmt.Bind( 3, szTableName, sizeof(szTableName) );
	
	// get all tables and add to listbox
	m_ctlTables.ResetContent();
	if ( stmt.Tables( NULL, _pParent->_pDef->_sOwner, NULL, "TABLE" ) == SQL_SUCCESS )
	{
		while ( stmt.Fetch() == SQL_SUCCESS )
			m_ctlTables.AddString( szTableName ); 
	}
}


//=============================================================================
// User interface callback functions
//=============================================================================

#define IDC_WIZARD_NEXT		0x3024

//-------------------------------------
void ReportBuilderPage1::OnDblclkTables() 
{
	HWND hNext = ::GetDlgItem( _pParent->m_hWnd, IDC_WIZARD_NEXT );

	WPARAM wParam = MAKEWPARAM( IDC_WIZARD_NEXT, BN_CLICKED );

	::SendMessage( _pParent->m_hWnd, WM_COMMAND, wParam, (LPARAM) hNext );
}


