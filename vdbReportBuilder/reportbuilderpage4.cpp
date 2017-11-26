//=============================================================================
//
// CLASS:           ReportBuilderPage4
//
// ESSENCE:         Property page for choosing which column to use as a filter
//
// FILE NAME:       ReportBuilderPage4.cpp
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportBuilderPage4.h"
#include "ReportBuilderPageHolder.h"
#include <vdbServerInfo.h>
#include <vdbException.h>
#include "ReportOIDColumn.h"


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
//^^ Constructor
ReportBuilderPage4::ReportBuilderPage4( ReportBuilderPageHolder* pParent )
	: CPropertyPage( ReportBuilderPage4::IDD ),
	  _pParent( pParent )
{
	m_psp.hInstance = VaultResourceInstance::Handle();

	//{{AFX_DATA_INIT(ReportBuilderPage4)
	//}}AFX_DATA_INIT
}


//-------------------------------------
//^^ Destructor
ReportBuilderPage4::~ReportBuilderPage4()
{
}


//=============================================================================
// Data exchange and message map
//=============================================================================

//-------------------------------------
//^^ DoDataExchange
void ReportBuilderPage4::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ReportBuilderPage4)
	DDX_Control(pDX, IDC_VDB_OPERATOR, m_ctlOperator);
	DDX_Control(pDX, IDC_VDB_FOREIGN_LIST, m_ctlForeignList);
	DDX_Control(pDX, IDC_VDB_COMPARISON_VALUE, m_ctlComparisonValue);
	DDX_Control(pDX, IDC_VDB_COLUMNS, m_ctlColumns);
	//}}AFX_DATA_MAP
}


//-------------------------------------
// MessageMap
BEGIN_MESSAGE_MAP(ReportBuilderPage4, CPropertyPage)
	//{{AFX_MSG_MAP(ReportBuilderPage4)
	ON_LBN_SELCHANGE(IDC_VDB_COLUMNS, OnSelchangeColumns)
	ON_LBN_SELCHANGE(IDC_VDB_OPERATOR, OnSelchangeOperator)
	ON_EN_CHANGE(IDC_VDB_COMPARISON_VALUE, OnChangeComparisonValue)
	ON_LBN_SELCHANGE(IDC_VDB_FOREIGN_LIST, OnSelchangeForeignList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
// Message response functions
//=============================================================================

//-------------------------------------
// The OnInitDialog function is called the first time this page is shown
//
BOOL ReportBuilderPage4::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	return FALSE;
}


//-------------------------------------
// The OnSetActive function is called when this page becomes the active page.
//
BOOL ReportBuilderPage4::OnSetActive() 
{
	BOOL rc = CPropertyPage::OnSetActive();

	_sColumnName = "";
	_sColumnType = "";
	_sOperator = "";
	_sValue = "";
	
	GetAllColumns();
	FillOperatorList();

	m_ctlComparisonValue.SetWindowText( "" );	
	m_ctlForeignList.ShowWindow( SW_HIDE );
	m_ctlComparisonValue.ShowWindow( SW_SHOW );
	
	_pParent->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	
	return rc;
}


//-------------------------------------
//^ The OnWizardNext function is called when the user presses the Next button.
//
LRESULT ReportBuilderPage4::OnWizardNext() 
{
	if ( MoveDataToHolder( true ) == true )
		return _pParent->DetermineNextPage( IDD_REPORT_BUILDER4 );
	else
		return -1;
}


//-------------------------------------
//^ The OnWizardBack function is called when the user presses the Back button.
//
LRESULT ReportBuilderPage4::OnWizardBack() 
{
	MoveDataToHolder( false );
	return _pParent->DetermineBackPage( IDD_REPORT_BUILDER4 );
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
bool ReportBuilderPage4::MoveDataToHolder( bool bValidate )
{
	if ( bValidate == true )
	{
		if ( _sColumnName.IsNull() )
		{
			vdbMessageBox( (int) m_hWnd, "Please select which attribute to use for the filter", "Report Builder", MB_OK );
			return false;
		}
		if ( _sOperator.IsNull() )
		{
			vdbMessageBox( (int) m_hWnd, "Please select how the attribute should be compared", "Report Builder", MB_OK );
			return false;
		}
		if ( _sValue.IsNull() )
		{
			vdbString s;
			s.Format( "The filter you specified will compare %s %s <nothing>.  Are you sure that's what you want to do?", (const char*) _sColumnName, ( const char*) _sOperator );
			if ( vdbMessageBox( (int) m_hWnd, (const char*) s, "Report Builder", MB_YESNO ) == IDNO )
				return false;
		}

		// check that the comparison value is valid for the column type
		if ( _sColumnType == "DBINT" || _sColumnType == "OID" )
		{
			int iValue = atoi( _sValue );
			vdbString sNewValue;
			sNewValue.Format( "%d", iValue );
			if ( sNewValue != _sValue )
			{
				vdbString s;
				s.Format( "The comparison value was not an integer.\n\nIt will be changed from \"%s\" to \"%s\".\n\nDo you want to do this?", (const char*) _sValue, (const char*) sNewValue );
				if ( vdbMessageBox( NULL, s, "Report Builder", MB_YESNO ) == IDNO )
					return false;
				_sValue = sNewValue;
			}
		}
		else if ( _sColumnType == "DBFLOAT" || _sColumnType == "DBDOUBLE" )
		{
			double dValue = atof( _sValue );
			vdbString sNewValue;
			sNewValue.Format( "%#g", dValue );
			_sValue = sNewValue;
		}

		
		ReportSelectionCriteria* pNew = new ReportSelectionCriteria( _sColumnName, _sValue, _sOperator, "AND" );
		if ( pNew == 0 )
			throw vdbMemoryException();

		_pParent->_pDef->_selectionCriteria.Add( pNew );
	}

	return true;
}


//=============================================================================
// Utility functions
//=============================================================================

//-----------------------------------------
void ReportBuilderPage4::GetAllColumns()
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


//-----------------------------------------
//^ The FillForeignList function retrieves all records from a foreign table
//  and displays them in the listbox.  The column name is used to determine
//  which foreign table to lookup.
//
void ReportBuilderPage4::FillForeignList( ReportOIDColumn& rColumn )
{
	m_ctlForeignList.ResetContent();

	std::ostrstream os;
	os << "SELECT oid, " << rColumn._sForeignColumn;
	os << " FROM " << (const char*) rColumn._pParent->_sOwner << "." << rColumn._sForeignTable;
	os << " ORDER BY " << rColumn._sForeignColumn;
	os << std::ends;
	char* sql = os.str();

	vdbStatement stmt( vdbPool::Instance()->GetDatabase() );																
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;

	if ( rc == SQL_SUCCESS )
	{
		OID oid;
		char sz[MAX_COLUMN_SIZE];

		stmt.Bind( 1, oid );
		stmt.Bind( 2, sz, sizeof(sz) );

		rc = stmt.Fetch();
		while ( rc == SQL_SUCCESS )
		{
			int index = m_ctlForeignList.AddString( sz );
			m_ctlForeignList.SetItemData( index, oid );

			rc = stmt.Fetch();
		}
	}
}


//=============================================================================
// User interface callback functions
//=============================================================================

//-------------------------------------
void ReportBuilderPage4::OnSelchangeColumns() 
{
	int index = m_ctlColumns.GetCurSel();
	if ( index == -1 )
		return;

	int columnIndex = m_ctlColumns.GetItemData( index );
	ReportColumn& rColumn = _pParent->_pDef->_columns[columnIndex];

	_sColumnName = rColumn._sColumnName;
	_sColumnType = rColumn._sColumnType;

	if ( _sColumnType == "OID" && _sColumnName != "oid" )
	{
		m_ctlForeignList.ShowWindow( SW_SHOW );
		m_ctlComparisonValue.ShowWindow( SW_HIDE );

		FillForeignList( (ReportOIDColumn&) rColumn );
	}
	else
	{
		m_ctlForeignList.ShowWindow( SW_HIDE );
		m_ctlComparisonValue.ShowWindow( SW_SHOW );
	}

	FillOperatorList();
}


//-------------------------------------
void ReportBuilderPage4::FillOperatorList()
{
	m_ctlOperator.ResetContent();

	if ( _sColumnType == "" )
		return;
	else if ( _sColumnType == "char[]" )
	{
		m_ctlOperator.AddString( "=" );
		m_ctlOperator.AddString( "<>" );
		m_ctlOperator.AddString( "LIKE" );
	}
	else
	{
		m_ctlOperator.AddString( "=" );
		m_ctlOperator.AddString( "<" );
		m_ctlOperator.AddString( "<=" );
		m_ctlOperator.AddString( ">" );
		m_ctlOperator.AddString( ">=" );
		m_ctlOperator.AddString( "<>" );
	}

	// reselect the previously selected operator
	int index = m_ctlOperator.FindStringExact( -1, _sOperator );
	if ( index != -1 )
		m_ctlOperator.SetCurSel( index );
}


//-------------------------------------
void ReportBuilderPage4::OnSelchangeOperator() 
{
	int index = m_ctlOperator.GetCurSel();
	if ( index == -1 )
	{
		_sOperator = "";
		return;
	}

	char sz[10];
	m_ctlOperator.GetText( index, sz );
	_sOperator = sz;
}


//-------------------------------------
void ReportBuilderPage4::OnChangeComparisonValue() 
{
	char sz[255];

	m_ctlComparisonValue.GetWindowText( sz, sizeof(sz) );
	_sValue = sz;
}


//-------------------------------------
void ReportBuilderPage4::OnSelchangeForeignList() 
{
	int index = m_ctlForeignList.GetCurSel();
	if ( index == -1 )
		return;

	OID oid = m_ctlForeignList.GetItemData( index );
	
	char sz[12];
	vdbUtility::SafeFormat( sz, sizeof(sz), "%d", oid );
	_sValue = sz;
}
