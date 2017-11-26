//=============================================================================
//
// CLASS:           ReportBuilderPage2
//
// ESSENCE:         Property page for choosing which columns to include in the report
//
// FILE NAME:       ReportBuilderPage2.cpp
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportBuilderPage2.h"
#include "ReportBuilderPageHolder.h"
#include <vdbServerInfo.h>
#include <vdbException.h>
#include "ReportOIDColumn.h"
#include "ReportLookupColumn.h"
#include "ReportNormalColumn.h"
#include "ReportChangeColumn.h"


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
//^^ Constructor
ReportBuilderPage2::ReportBuilderPage2( ReportBuilderPageHolder* pParent )
	: CPropertyPage( ReportBuilderPage2::IDD ),
	  _pParent( pParent )
{
	m_psp.hInstance = VaultResourceInstance::Handle();

	//{{AFX_DATA_INIT(ReportBuilderPage2)
	//}}AFX_DATA_INIT
}


//-------------------------------------
//^^ Destructor
ReportBuilderPage2::~ReportBuilderPage2()
{
}


//=============================================================================
// Data exchange and message map
//=============================================================================

//-------------------------------------
//^^ DoDataExchange
void ReportBuilderPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ReportBuilderPage2)
	DDX_Control(pDX, IDC_VDB_CHANGE, m_ctlChange);
	DDX_Control(pDX, IDC_VDB_INCLUDE, m_ctlInclude);
	DDX_Control(pDX, IDC_VDB_EXCLUDE, m_ctlExclude);
	DDX_Control(pDX, IDC_VDB_COLUMNS, m_ctlColumns);
	//}}AFX_DATA_MAP
}


//-------------------------------------
// MessageMap
BEGIN_MESSAGE_MAP(ReportBuilderPage2, CPropertyPage)
	//{{AFX_MSG_MAP(ReportBuilderPage2)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_VDB_EXCLUDE, OnExclude)
	ON_BN_CLICKED(IDC_VDB_INCLUDE, OnInclude)
	ON_NOTIFY(NM_DBLCLK, IDC_VDB_COLUMNS, OnDblclkColumns)
	ON_NOTIFY(LVN_SELCHANGED, IDC_VDB_COLUMNS, OnSelchangedColumns)
	ON_BN_CLICKED(IDC_VDB_CHANGE, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
// Message response functions
//=============================================================================

//-------------------------------------
int ReportBuilderPage2::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_ctlColumns.Subclass( IDC_VDB_COLUMNS, this );

	return 0;
}


//-------------------------------------
// The OnInitDialog function is called the first time this page is shown
//
BOOL ReportBuilderPage2::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_ctlColumns.PostCreate();
	m_ctlColumns.ChangeColors( "SOFTWARE\\ProgrammersHeaven\\Vault\\1.0\\Colors" );

	m_ctlColumns.InsertColumn( 0, "", LVCFMT_LEFT, 20, 0 );
	m_ctlColumns.InsertColumn( 1, "Attribute", LVCFMT_LEFT, 110, 1 );
	m_ctlColumns.InsertColumn( 2, "Column name", LVCFMT_LEFT, 110, 2 );
	m_ctlColumns.InsertColumn( 3, "Width", LVCFMT_LEFT, 45, 3 );
	m_ctlColumns.InsertColumn( 4, "Format", LVCFMT_LEFT, 100, 4 );
	m_ctlColumns.InsertColumn( 5, "Type", LVCFMT_LEFT, 65, 5 );
	m_ctlColumns.InsertColumn( 6, "Storage size", LVCFMT_LEFT, 65, 6 );

	// set up the list control bitmaps
	_listBitmap.Attach( ::LoadBitmap( VaultResourceInstance::Handle(), MAKEINTRESOURCE(IDB_VDB_CHECKMARK_BITMAP) ) );
	_listImage.Create( 16, 16, FALSE, 2, 2 );
	_listImage.Add( &_listBitmap, (CBitmap*) NULL );   
	m_ctlColumns.SetImageList( &_listImage, LVSIL_SMALL );

	return FALSE;
}


//-------------------------------------
// The OnSetActive function is called when this page becomes the active page.
//
BOOL ReportBuilderPage2::OnSetActive() 
{
	BOOL rc = CPropertyPage::OnSetActive();

	GetAllColumns();
	
	// place a checkmark next to all columns previously selected by the user
	int count = _pParent->_pDef->_columns.Count();
	for ( int i = 0; i < count; i++ )
	{
		vdbString sColumnName( _pParent->_pDef->_columns[i]._sColumnName );

		int index = m_ctlColumns.GetNextItem( -1, LVNI_ALL );
		while ( index != -1 )
		{
			char sz[80];
			m_ctlColumns.GetItemText( index, 1, sz, sizeof(sz) );
			if ( sColumnName == sz )
			{
				// checkmark
				LV_ITEM lvi;
				lvi.mask = LVIF_IMAGE;
				lvi.iItem = index;
				lvi.iSubItem = 0;
				lvi.iImage = ( 0 );
				m_ctlColumns.SetItem( &lvi );

				// also change the column name and length to the user's previously chosen values
				m_ctlColumns.SetItemText( index, 2, _pParent->_pDef->_columns[i]._sDisplayName );

				char szLength[10];
				vdbUtility::SafeFormat( szLength, sizeof(szLength), "%d", _pParent->_pDef->_columns[i]._displaySize );
				m_ctlColumns.SetItemText( index, 3, szLength );

				m_ctlColumns.SetItemText( index, 4, _pParent->_pDef->_columns[i]._sFormatString );

				break;
			}
			index = m_ctlColumns.GetNextItem( index, LVNI_ALL );
		}
	}

	m_ctlInclude.EnableWindow( FALSE );
	m_ctlExclude.EnableWindow( FALSE );
	m_ctlChange.EnableWindow( FALSE );

	_pParent->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	
	return rc;
}


//-------------------------------------
//^ The OnWizardNext function is called when the user presses the Next button.
//
LRESULT ReportBuilderPage2::OnWizardNext() 
{
	if ( MoveDataToHolder( true ) == true )
		return _pParent->DetermineNextPage( IDD_REPORT_BUILDER2 );
	else
		return -1;
}


//-------------------------------------
//^ The OnWizardBack function is called when the user presses the Back button.
//
LRESULT ReportBuilderPage2::OnWizardBack() 
{
	MoveDataToHolder( false );
	return _pParent->DetermineBackPage( IDD_REPORT_BUILDER2 );
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
bool ReportBuilderPage2::MoveDataToHolder( bool bValidate )
{
	if ( bValidate == true )
	{
		// clear everything first
		_pParent->_pDef->_columns.ClearAll();

		int index = m_ctlColumns.GetNextItem( -1, LVNI_ALL );
		while ( index != -1 )
		{
			// get the checkmark 
			LV_ITEM lvi;
			lvi.mask = LVIF_IMAGE;
			lvi.iItem = index;
			lvi.iSubItem = 0;
			m_ctlColumns.GetItem( &lvi );

			if ( lvi.iImage == 0 )		// checkmark
			{
				char szColumnName[80];
				m_ctlColumns.GetItemText( index, 1, szColumnName, sizeof(szColumnName) );

				char szDisplayName[80];
				if ( m_ctlColumns.GetItemText( index, 2, szDisplayName, sizeof(szDisplayName) ) == 0 )
					szDisplayName[0] = 0x0;

				char szLength[10];
				m_ctlColumns.GetItemText( index, 3, szLength, sizeof(szLength) );
				int displaySize = atoi( szLength );

				char szFormatString[20];
				m_ctlColumns.GetItemText( index, 4, szFormatString, sizeof(szFormatString) );

				char szColumnType[20];
				m_ctlColumns.GetItemText( index, 5, szColumnType, sizeof(szColumnType) );

				m_ctlColumns.GetItemText( index, 6, szLength, sizeof(szLength) );
				int dataSize = atoi( szLength );

				// Determine what type of column this is and create an appropriate type
				// The oid column of each table is treated as a "normal" column
				if ( !strcmp( szColumnType, "OID" ) && strcmp( szColumnName, "oid" ) )
				{
					ReportOIDColumn* p = new ReportOIDColumn( _pParent->_pDef, szColumnName, szDisplayName, displaySize, szFormatString );
					if ( p == 0 )
						throw vdbMemoryException();
					_pParent->_pDef->_columns.Add( p );
				}
				else if ( !strcmp( szColumnType, "char[]" ) && dataSize == 7 )
				{
					ReportLookupColumn* p = new ReportLookupColumn( _pParent->_pDef, szColumnName, szDisplayName, displaySize, szFormatString );
					if ( p == 0 )
						throw vdbMemoryException();
					_pParent->_pDef->_columns.Add( p );
				}
				else
				{
					ReportNormalColumn* p = new ReportNormalColumn( _pParent->_pDef, szColumnName, szColumnType, dataSize, szDisplayName, displaySize, szFormatString );
					if ( p == 0 )
						throw vdbMemoryException();
					_pParent->_pDef->_columns.Add( p );
				}
			}

			index = m_ctlColumns.GetNextItem( index, LVNI_ALL );
		}

		int count = _pParent->_pDef->_columns.Count();
		if ( count == 0 )
		{
			vdbMessageBox( (int) m_hWnd, "Please include one or more attributes to use in the report", "Report Builder", MB_OK );
			return false;
		}
	}

	return true;
}


//=============================================================================
// Utility functions
//=============================================================================

//-----------------------------------------
//^ The GetAllColumns functions retrieves all columns from the database and shows
//  their name, type, and size in the list control.
//
void ReportBuilderPage2::GetAllColumns()
{
	char szColumnName[60];
	char szODBCtype[40];
	DBINT dataSize;

	vdbStatement stmt( vdbPool::Instance()->GetDatabase() );
	stmt.Bind( 4, szColumnName, sizeof(szColumnName) );
	stmt.Bind( 6, szODBCtype, sizeof(szODBCtype) );
	stmt.Bind( 8, dataSize );
	
	// get all columns for selected table
	m_ctlColumns.DeleteAllItems();

	if ( stmt.Columns( NULL, _pParent->_pDef->_sOwner, _pParent->_pDef->_sTableName, NULL ) == SQL_SUCCESS )
	{
		while ( stmt.Fetch() == SQL_SUCCESS )
		{
			vdbString sColumnName( szColumnName );

			vdbString sVaultType;
			sVaultType = _pParent->_pServerInfo->GetVaultType( vdbString(szODBCtype) );
			if ( sVaultType == "DBINT" )
				ConvertToOID( szColumnName, sVaultType );

			char szDataSize[10];
			vdbUtility::SafeFormat( szDataSize, sizeof(szDataSize), "%d", dataSize );

			// make the best guess for the display name and width and format string
			vdbString sDisplayName;
			vdbString sFormatString;
			int displaySize;
			if ( sVaultType == "OID" && sColumnName != "oid" )
				ReportOIDColumn::BestGuess( sColumnName, sDisplayName, displaySize, sFormatString );

			else if ( sVaultType == "char[]" && dataSize == 7 )
				ReportLookupColumn::BestGuess( sColumnName, sDisplayName, displaySize, sFormatString );

			else
				ReportNormalColumn::BestGuess( sColumnName, sVaultType, dataSize, sDisplayName, displaySize, sFormatString );

			char szDisplaySize[10];
			vdbUtility::SafeFormat( szDisplaySize, sizeof(szDisplaySize), "%d", displaySize );


			int where = m_ctlColumns.InsertItem( m_ctlColumns.GetItemCount(), "" );
			m_ctlColumns.SetItemText( where, 1, (const char*) sColumnName );
			m_ctlColumns.SetItemText( where, 2, (const char*) sDisplayName );
			m_ctlColumns.SetItemText( where, 3, szDisplaySize );
			m_ctlColumns.SetItemText( where, 4, (const char*) sFormatString );
			m_ctlColumns.SetItemText( where, 5, (const char*) sVaultType );
			m_ctlColumns.SetItemText( where, 6, szDataSize );

			LV_ITEM lvi;
			lvi.mask = LVIF_IMAGE;
			lvi.iItem = where;
			lvi.iSubItem = 0;
			lvi.iImage = ( 1 );		// no checkmark
			m_ctlColumns.SetItem( &lvi );
		}
	}
}


//----------------------------------------
//^ The ConvertToOID makes a best guess at whether or not this column name is
//  an OID column.  If it is, the vault type is changed to "OID".
//> szColumnName is the column name to search.
//> szVaultType is the type DBINT on input and either DBINT or OID on output.
//
void ReportBuilderPage2::ConvertToOID( const char* szColumnName, vdbString& sVaultType )
{
	vdbString s( szColumnName );

	if ( s == "oid" )
		sVaultType = "OID";

	else if ( s.Find( "ID" ) != -1 )
		sVaultType = "OID";
}


//-------------------------------------
void ReportBuilderPage2::OnExclude() 
{
	int index = m_ctlColumns.GetNextItem( -1, LVNI_SELECTED );
	while ( index != -1 )
	{
		LV_ITEM lvi;
		lvi.mask = LVIF_IMAGE;
		lvi.iItem = index;
		lvi.iSubItem = 0;
		lvi.iImage = ( 1 );		// no checkmark
		m_ctlColumns.SetItem( &lvi );

		index = m_ctlColumns.GetNextItem( index, LVNI_SELECTED );
	}
}


//-------------------------------------
void ReportBuilderPage2::OnInclude() 
{
	int index = m_ctlColumns.GetNextItem( -1, LVNI_SELECTED );
	while ( index != -1 )
	{
		LV_ITEM lvi;
		lvi.mask = LVIF_IMAGE;
		lvi.iItem = index;
		lvi.iSubItem = 0;
		lvi.iImage = ( 0 );		// checkmark
		m_ctlColumns.SetItem( &lvi );

		index = m_ctlColumns.GetNextItem( index, LVNI_SELECTED );
	}
}


//-------------------------------------
void ReportBuilderPage2::OnSelchangedColumns(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
	int where = pNMListView->iItem;
	if ( where == -1 )
	{
		m_ctlInclude.EnableWindow( FALSE );
		m_ctlExclude.EnableWindow( FALSE );
		m_ctlChange.EnableWindow( FALSE );
	}
	else
	{
		m_ctlInclude.EnableWindow( TRUE );
		m_ctlExclude.EnableWindow( TRUE );
		m_ctlChange.EnableWindow( TRUE );
	}
}


//-------------------------------------
void ReportBuilderPage2::OnDblclkColumns(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	int index = m_ctlColumns.GetNextItem( -1, LVNI_SELECTED );
	if ( index == -1 )
		return;

	// get the current checkmark state
	LV_ITEM lvi;
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = index;
	lvi.iSubItem = 0;
	m_ctlColumns.GetItem( &lvi );

	// toggle the current checkmark state
	lvi.iImage = ( lvi.iImage == 0 ? 1 : 0 );
	m_ctlColumns.SetItem( &lvi );
}


//-------------------------------------
void ReportBuilderPage2::OnChange() 
{
	int index = m_ctlColumns.GetNextItem( -1, LVNI_SELECTED );
	if ( index == -1 )
		return;

	char szDisplayName[MAX_COLUMN_SIZE];
	m_ctlColumns.GetItemText( index, 2, szDisplayName, sizeof(szDisplayName) );

	char szWidth[10];
	m_ctlColumns.GetItemText( index, 3, szWidth, sizeof(szWidth) );

	char szFormatString[20];
	m_ctlColumns.GetItemText( index, 4, szFormatString, sizeof(szFormatString) );

	ReportChangeColumn dlg;
	dlg._sDisplayName = szDisplayName;
	dlg._displayWidth = atoi( szWidth );
	dlg._sFormatString = szFormatString;
	if ( dlg.DoModal() == IDOK )
	{
		m_ctlColumns.SetItemText( index, 2, dlg._sDisplayName );

		char szWidth[10];
		vdbUtility::SafeFormat( szWidth, sizeof(szWidth), "%d", dlg._displayWidth );
		m_ctlColumns.SetItemText( index, 3, szWidth );

		m_ctlColumns.SetItemText( index, 4, dlg._sFormatString );
	}
}
