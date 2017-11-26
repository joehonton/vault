//=============================================================================
//
// CLASS:           vdbDatabaseOpen
//
// ESSENCE:         Select a database to open
//
// FILE NAME:       vdbDatabaseOpen.h
//
// INITIAL DATE:    6/14/00
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#include <vdbWindows.h>
#include "vdbDatabaseOpen.h"
#include <vdbDatabaseNew.h>
#include <VaultResourceInstance.h>
#include <vdbDatabase.h>
#include <vdbRegistrationKey.h>
#include <vdbSetSort.h>


//=============================================================================
// Global function
//=============================================================================

//------------------------------------------
//^ The ListDataSources function is a global function.
//  Call ListDataSources with a doubly subscripted character array
//  whose first dimension is "numSources" in size and whose second
//  dimension is SQL_MAX_DSN_LENGTH+1.
//  ListDataSources will fill the character array with all data sources
//  available and return the number found in "numSources".
//^ Beginning with Build61, only "SQL Server" datasources are returned
//
//> szSources is the character array to fill.
//> szDescriptions is the user description of each data source
//> numSources, on input, is the maximum size of the character array.
//> numSources, on output, is the number of valid entries in the array.
//  Note that if numSources is the same on output as on input, you may not
//  have gotten all of the possible sources.
//< returns DBS_OK on success or an ODBC return code on failure.
//  
RETCODE ListDataSources( char szSources[][SQL_MAX_DSN_LENGTH+1], char szDescriptions[][255], int& numSources )
{
	//  Allocate the environment
	RETCODE rc;
	HENV henv = SQL_NULL_HENV;
	if ( rc = SQLAllocEnv( &henv ) != SQL_SUCCESS )
		return rc;

	//  Allocate the connection
	HDBC hdbc = SQL_NULL_HDBC;
	if ( rc = SQLAllocConnect( henv, &hdbc ) != SQL_SUCCESS )
		return rc;
	
	vdbSet<vdbString> dataSources;
	vdbSet<vdbString> descriptions;

	// get all data sources
	int validCount = 0;
	while ( validCount < numSources )
	{
		char szSourceName[SQL_MAX_DSN_LENGTH+1];
		char szDriverDescription[255];
		
		rc = SQLDataSources( henv, SQL_FETCH_NEXT, (UCHAR*) szSourceName, SQL_MAX_DSN_LENGTH-1, NULL, (UCHAR*) szDriverDescription, sizeof(szDriverDescription)-1, NULL );
		if ( rc != SQL_SUCCESS )
			break;
		
		// mask out MQIS
		if ( _stricmp( szSourceName, "MQIS" ) )
		{
			// mask out LocalServer
			if ( _stricmp( szSourceName, "LocalServer" ) )
			{
				// Show only SQL Server data sources
				if ( !_stricmp( szDriverDescription, "SQL Server" ) )
				{
					dataSources.Add( new vdbString( szSourceName ) );

					// get the datasource description
					vdbString sSubkey;
					sSubkey.Format( "SOFTWARE\\ODBC\\ODBC.INI\\%s", szSourceName );
					vdbString sUserDescription;
					vdbRegistrationKey key1( HKEY_CURRENT_USER, (const char*) sSubkey, "Description" );
					bool rc2 = key1.GetValue( sUserDescription );
					if ( rc2 == false )
					{
						vdbRegistrationKey key2( HKEY_LOCAL_MACHINE, (const char*) sSubkey, "Description" );
						rc2 = key2.GetValue( sUserDescription );
					}
					if ( rc2 == true )
						descriptions.Add( new vdbString( sUserDescription ) );
					else
						descriptions.Add( new vdbString( "" ) );

					validCount++;
				}
			}
		}
	}

	// sort by data source name
	vdbSetSort<vdbString> s( dataSources );
	s.SelectionSort();

	// copy data sources and descriptions to the caller's variables
	numSources = min( dataSources.Count(), numSources );
	for ( int i = 0; i < numSources; i++ )
	{
		vdbUtility::SafeCopy( szSources[i], dataSources[s[i]], SQL_MAX_DSN_LENGTH+1 );
		vdbUtility::SafeCopy( szDescriptions[i], descriptions[s[i]], 255 );
	}

	// Free the connection
	if (hdbc != SQL_NULL_HDBC)
		if ( rc = SQLFreeConnect(hdbc) != SQL_SUCCESS )
			return rc;
	
	// Free the environment
	if (henv != SQL_NULL_HENV)
		if ( rc = SQLFreeEnv(henv) != SQL_SUCCESS )
			return rc;

	return rc;
}


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
vdbDatabaseOpen::vdbDatabaseOpen( const char* szDatasource, vdbDatabase* pDB, CWnd* pParent )
	: vdbDLLDialog(vdbDatabaseOpen::IDD, pParent),
	  _sDataSource( szDatasource ),
	  _pDB( pDB )
{
	//{{AFX_DATA_INIT(vdbDatabaseOpen)
	//}}AFX_DATA_INIT
}


//-------------------------------------
vdbDatabaseOpen::~vdbDatabaseOpen()
{
}


//-------------------------------------
void vdbDatabaseOpen::DoDataExchange(CDataExchange* pDX)
{
	vdbDLLDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(vdbDatabaseOpen)
	DDX_Control(pDX, IDC_VDB_LIST_SOURCES, m_ctlDataSources);
	//}}AFX_DATA_MAP

	if ( pDX->m_bSaveAndValidate == 0 )
	{
		m_ctlOK.Attach( ::GetDlgItem( m_hWnd, IDOK ) );
		m_ctlNew.Attach( ::GetDlgItem( m_hWnd, IDC_VDB_NEW_DATABASE ) );
		m_ctlCancel.Attach( ::GetDlgItem( m_hWnd, IDCANCEL ) );
	}
}


//-------------------------------------
BEGIN_MESSAGE_MAP(vdbDatabaseOpen, vdbDLLDialog)
	//{{AFX_MSG_MAP(vdbDatabaseOpen)
	ON_NOTIFY(LVN_SELCHANGED, IDC_VDB_LIST_SOURCES, OnSelchangedDataSource)
	ON_NOTIFY(NM_DBLCLK, IDC_VDB_LIST_SOURCES, OnDblClkDataSource)
	ON_BN_CLICKED(IDC_VDB_NEW_DATABASE, OnNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
// Window creation and intialization
//=============================================================================

//-------------------------------------
BOOL vdbDatabaseOpen::OnInitDialog() 
{
	vdbDLLDialog::OnInitDialog();
	
	m_ctlOK.EnableWindow( FALSE );

	HINSTANCE hInstance = VaultResourceInstance::Handle();
	m_ctlOK.SetResource( hInstance, IDB_VDB_OK );
	m_ctlOK.SetText( "Open" );
	m_ctlOK.SetToolTip( "Open the selected database" );

	m_ctlNew.SetResource( hInstance, IDB_VDB_NEW );
	m_ctlNew.SetText( "Create new..." );
	m_ctlNew.SetToolTip( "Create a new database" );

	m_ctlCancel.SetResource( hInstance, IDB_VDB_CLOSE );
	m_ctlCancel.SetText( "Cancel" );

	if ( _pDB == 0 )
		m_ctlNew.ShowWindow( FALSE );
	else
		m_ctlNew.ShowWindow( TRUE );

	// set up the list control
	HBITMAP hBitmap = ::LoadBitmap( hInstance, MAKEINTRESOURCE(IDB_VDB_DATABASE) );
	_listBitmap.Attach( hBitmap );
	_listImage.Create( 16, 16, FALSE, 3, 3 );
	_listImage.Add( &_listBitmap, (CBitmap*) NULL );   
	m_ctlDataSources.SetImageList( &_listImage, LVSIL_SMALL );

	RECT rect;
	m_ctlDataSources.GetClientRect( &rect );
	int columnWidth0 = 20;
	int columnWidth1 = ( rect.right - columnWidth0 ) / 3;
	int columnWidth2 = columnWidth1 * 2;

	m_ctlDataSources.PostCreate();
	m_ctlDataSources.InsertColumn( 0, "", LVCFMT_LEFT, columnWidth0, 0 );
	m_ctlDataSources.InsertColumn( 1, "Data source", LVCFMT_LEFT, columnWidth1, 1 );
	m_ctlDataSources.InsertColumn( 2, "Description", LVCFMT_LEFT, columnWidth2, 2 );

	// add all SQL Server Desktop Engine datasources to the list
	char szSources[50][SQL_MAX_DSN_LENGTH+1];
	char szDescriptions[50][255];
	int numSources = 50;
	ListDataSources( szSources, szDescriptions, numSources );
	for ( int i = 0; i < numSources; i++ )
	{
		int index = m_ctlDataSources.InsertItem( m_ctlDataSources.GetItemCount(), "", 0 );
		m_ctlDataSources.SetItemText( index, 1, szSources[i] );
		m_ctlDataSources.SetItemText( index, 2, szDescriptions[i] );
		if ( _sDataSource == szSources[i] )
		{
			m_ctlDataSources.SetItemState( index, 0xFFFF, LVIS_SELECTED );
			m_ctlDataSources.EnsureVisible( index, FALSE );
			m_ctlOK.EnableWindow( TRUE );
		}
	}

	return TRUE;
}


//=============================================================================
// User interface callbacks
//=============================================================================

//-------------------------------------
//^ The OnSelchangedDataSource file is a callback triggered by selecting an
//  item in the recent backup list
void vdbDatabaseOpen::OnSelchangedDataSource(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_ctlOK.EnableWindow( TRUE );
}


//-------------------------------------
//^ The OnDblClkDataSource file is a callback triggered by selecting an
//  item in the recent backup list
void vdbDatabaseOpen::OnDblClkDataSource(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_ctlOK.EnableWindow( TRUE );
	OnOK();
}


//=============================================================================
// Functions to create and initialize the database
//=============================================================================

//-------------------------------------
void vdbDatabaseOpen::OnOK() 
{
	int which = m_ctlDataSources.GetNextItem( -1 , LVNI_SELECTED );
	if ( which == -1 )
		return;
	else
	{
		char sz[33];
		m_ctlDataSources.GetItemText( which, 1, sz, sizeof(sz) );
		_sDataSource = sz;
	}

	vdbDLLDialog::OnOK();
}


//-------------------------------------
void vdbDatabaseOpen::OnNew() 
{
	vdbDatabaseNew dlg( _pDB );
	if ( dlg.DoModal() == IDOK )
		_sDataSource = dlg._sDataSource;
	else
		return;

	vdbDLLDialog::OnOK();
}

