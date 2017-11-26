//=============================================================================
//
// CLASS:           ReportBuilderPage7
//
// ESSENCE:         Property page for choosing which column to use as a sort
//
// FILE NAME:       ReportBuilderPage7.cpp
//
// INITIAL DATE:    10/26/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportBuilderPage7.h"
#include "ReportBuilderPageHolder.h"
#include <vdbServerInfo.h>


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
//^^ Constructor
ReportBuilderPage7::ReportBuilderPage7( ReportBuilderPageHolder* pParent )
	: CPropertyPage( ReportBuilderPage7::IDD ),
	  _pParent( pParent )
{
	m_psp.hInstance = VaultResourceInstance::Handle();

	//{{AFX_DATA_INIT(ReportBuilderPage7)
	//}}AFX_DATA_INIT
}


//-------------------------------------
//^^ Destructor
ReportBuilderPage7::~ReportBuilderPage7()
{
}


//=============================================================================
// Data exchange and message map
//=============================================================================

//-------------------------------------
//^^ DoDataExchange
void ReportBuilderPage7::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ReportBuilderPage7)
	DDX_Control(pDX, IDC_VDB_ASCENDING, m_ctlAscending);
	DDX_Control(pDX, IDC_VDB_DESCENDING, m_ctlDescending);
	DDX_Control(pDX, IDC_VDB_COLUMNS, m_ctlColumns);
	//}}AFX_DATA_MAP
}


//-------------------------------------
// MessageMap
BEGIN_MESSAGE_MAP(ReportBuilderPage7, CPropertyPage)
	//{{AFX_MSG_MAP(ReportBuilderPage7)
	ON_LBN_SELCHANGE(IDC_VDB_COLUMNS, OnSelchangeColumns)
	ON_BN_CLICKED(IDC_VDB_ASCENDING, OnAscending)
	ON_BN_CLICKED(IDC_VDB_DESCENDING, OnDescending)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
// Message response functions
//=============================================================================

//-------------------------------------
// The OnInitDialog function is called the first time this page is shown
//
BOOL ReportBuilderPage7::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	return FALSE;
}


//-------------------------------------
// The OnSetActive function is called when this page becomes the active page.
//
BOOL ReportBuilderPage7::OnSetActive() 
{
	BOOL rc = CPropertyPage::OnSetActive();

	_sColumnName = "";
	_sOrder = "";

	GetAllColumns();
	
	m_ctlAscending.SetCheck( 1 );
	m_ctlDescending.SetCheck( 0 );
	OnAscending();

	_pParent->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	
	return rc;
}


//-------------------------------------
//^ The OnWizardNext function is called when the user presses the Next button.
//
LRESULT ReportBuilderPage7::OnWizardNext() 
{
	if ( MoveDataToHolder( true ) == true )
		return _pParent->DetermineNextPage( IDD_REPORT_BUILDER7 );
	else
		return -1;
}


//-------------------------------------
//^ The OnWizardBack function is called when the user presses the Back button.
//
LRESULT ReportBuilderPage7::OnWizardBack() 
{
	MoveDataToHolder( false );
	return _pParent->DetermineBackPage( IDD_REPORT_BUILDER7 );
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
bool ReportBuilderPage7::MoveDataToHolder( bool bValidate )
{
	if ( bValidate == true )
	{
		if ( _sColumnName.IsNull() )
		{
			vdbMessageBox( (int) m_hWnd, "Please select which attribute to use for the sort", "Report Builder", MB_OK );
			return false;
		}
		if ( _sOrder.IsNull() )
		{
			vdbMessageBox( (int) m_hWnd, "Please select how the attribute should be sorted", "Report Builder", MB_OK );
			return false;
		}

		ReportSortCriteria* pNew = new ReportSortCriteria( _sColumnName, _sOrder );
		if ( pNew == 0 )
			throw vdbMemoryException();

		_pParent->_pDef->_sortCriteria.Add( pNew );
	}

	return true;
}


//=============================================================================
// Utility functions
//=============================================================================

//-----------------------------------------
void ReportBuilderPage7::GetAllColumns()
{
	m_ctlColumns.ResetContent();
	int count = _pParent->_pDef->_columns.Count();
	for ( int i = 0; i < count; i++ )
	{
		ReportColumn& rColumn = _pParent->_pDef->_columns[i];
		int index = m_ctlColumns.AddString( (const char*) rColumn._sColumnName );
		m_ctlColumns.SetItemData( index, i );
	}
}


//=============================================================================
// User interface callback functions
//=============================================================================

//-------------------------------------
void ReportBuilderPage7::OnSelchangeColumns() 
{
	int index = m_ctlColumns.GetCurSel();
	if ( index == -1 )
		return;

	int columnIndex = m_ctlColumns.GetItemData( index );
	ReportColumn& rColumn = _pParent->_pDef->_columns[columnIndex];

	_sColumnName = rColumn._sColumnName;
}


//-------------------------------------
void ReportBuilderPage7::OnAscending() 
{
	if ( m_ctlAscending.GetCheck() == 1 )
		_sOrder = "ASC";
	else
		_sOrder = "DESC";
}


//-------------------------------------
void ReportBuilderPage7::OnDescending() 
{
	if ( m_ctlDescending.GetCheck() == 1 )
		_sOrder = "DESC";
	else
		_sOrder = "ASC";
}
