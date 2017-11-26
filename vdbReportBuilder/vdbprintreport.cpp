//=============================================================================
//
// CLASS:           vdbPrintReport
//
// ESSENCE:         Select a printer and send a report to it
//
// FILE NAME:       vdbPrintReport.cpp
//
// INITIAL DATE:    11/12/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "vdbPrintReport.h"
#include <Cursors.h>
#include <fstream>
#include <vdbFilename.h>
#include <vdbException.h>
#include <vdbStreamHeader.h>
#include "vdbFormatStyles.h"


//=============================================================================
//  Essential class functions
//=============================================================================

//-------------------------------------
vdbPrintReport::vdbPrintReport()
	: vdbDLLDialog(vdbPrintReport::IDD, NULL),
	  vdbPrinterDialogClass()
{
	//{{AFX_DATA_INIT(vdbPrintReport)
	//}}AFX_DATA_INIT
}


//-------------------------------------
vdbPrintReport::~vdbPrintReport()
{
}


//-------------------------------------
void vdbPrintReport::DoDataExchange(CDataExchange* pDX)
{
	vdbDLLDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(vdbPrintReport)
	DDX_Control(pDX, IDC_VDB_PAGES, m_ctlPages);
	DDX_Control(pDX, IDC_VDB_ALL, m_ctlAll);
	DDX_Control(pDX, IDC_VDB_FROM_PAGE, m_ctlFromPage);
	DDX_Control(pDX, IDC_VDB_TO_PAGE, m_ctlToPage);
	DDX_Control(pDX, IDC_VDB_PRINT, m_ctlPrint);
	DDX_Control(pDX, IDC_VDB_LIST_DEFS, m_ctlListDefs);
	DDX_Control(pDX, IDC_VDB_FORMAT_STYLE, m_ctlFormatStyle);
	DDX_Control(pDX, IDC_VDB_LIST_STYLES, m_ctlListStyles);
	//}}AFX_DATA_MAP
}


//-------------------------------------
BEGIN_MESSAGE_MAP(vdbPrintReport, vdbDLLDialog)
	//{{AFX_MSG_MAP(vdbPrintReport)
	ON_BN_CLICKED(IDC_VDB_PRINT, OnPrint)
	ON_BN_CLICKED(IDC_VDB_SETUP, OnPrinterSetup)
	ON_LBN_SELCHANGE(IDC_VDB_LIST_DEFS, OnSelchangeListDefs)
	ON_BN_CLICKED(IDC_VDB_PAGES, OnVdbPages)
	ON_BN_CLICKED(IDC_VDB_ALL, OnVdbAll)
	ON_BN_CLICKED(IDC_VDB_FORMAT_STYLE, OnFormatStyle)
	ON_LBN_SELCHANGE(IDC_VDB_LIST_STYLES, OnSelchangeListStyles)
	ON_LBN_DBLCLK(IDC_VDB_LIST_STYLES, OnDblclkListStyles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
//  Initialization function
//=============================================================================

//-------------------------------------
//^ The OnInitDialog function is called just before the window is being shown.
//
BOOL vdbPrintReport::OnInitDialog() 
{
	vdbDLLDialog::OnInitDialog();
	
	_dlgData._enablePrintRange = true;
	vdbPrinterDialogClass::OnInitDialog( m_hWnd, &_dlgData );

	ShowHourglass();

	// get the directory location of this executable
	char szModuleFilename[_MAX_PATH];
	GetModuleFileName( NULL, szModuleFilename, sizeof(szModuleFilename) );
	vdbFilename sModule( szModuleFilename );
	vdbFilename sDirectory( sModule.Directory() );

	// find report definition files
	vdbFilename sAllFiles( sDirectory );
	sAllFiles.Append( "*.def" );

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile( sAllFiles, &fd );
	if ( hFind != INVALID_HANDLE_VALUE )
	{
		BOOL bMore = TRUE;
		while( bMore )
		{
			if ( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
			{
				vdbFilename sFullname( sDirectory );
				sFullname.Append( fd.cFileName );
				AddOneReportDefinition( sFullname );
			}
			bMore = FindNextFile( hFind, &fd );
		}
		FindClose( hFind );
	}

	// find report style files
	sAllFiles = sDirectory;
	sAllFiles.Append( "*.sty" );

	hFind = FindFirstFile( sAllFiles, &fd );
	if ( hFind != INVALID_HANDLE_VALUE )
	{
		BOOL bMore = TRUE;
		while( bMore )
		{
			if ( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
			{
				vdbFilename sFullname( sDirectory );
				sFullname.Append( fd.cFileName );
				AddOneReportStyle( sFullname );
			}
			bMore = FindNextFile( hFind, &fd );
		}
		FindClose( hFind );
	}

	m_ctlFormatStyle.EnableWindow( FALSE );
	m_ctlPrint.EnableWindow( FALSE );

	m_ctlAll.SetCheck( 1 );
	m_ctlFromPage.SetWindowText( "" );
	m_ctlToPage.SetWindowText( "" );
	m_ctlFromPage.EnableWindow( FALSE );
	m_ctlToPage.EnableWindow( FALSE );

	ShowPointer();
	return TRUE;
}


//-------------------------------------
//^ The AddOneReportDefinition function
//> szFilename is the fully qualified name of the file containing the report definition
//
void vdbPrintReport::AddOneReportDefinition( const char* szFilename )
{
	vdbFilename f( szFilename );
	if ( !f.Exists() )
		return;

	std::ifstream is( szFilename, std::ios_base::in );
	if ( !is.is_open() )
		return;

	ReportDefinition* pDef = new ReportDefinition;
	if ( pDef == 0 )
		throw vdbMemoryException();

	is >> *pDef;
	if ( is.fail() )
	{
		is.close();
		delete pDef; pDef = 0;
		return;
	}
	is.close();

	// override the definition filename because the local copy has more accurate directory information
	pDef->_sDefinitionFile = szFilename;

	// add to the master list
	_reportDefinitions.Add( pDef, pDef->_sTitle );

	// add to the user's listbox
	m_ctlListDefs.AddString( pDef->_sTitle ); 
}


//-------------------------------------
//^ The AddOneReportStyle function
//> szFilename is the fully qualified name of the file containing the report style
//
void vdbPrintReport::AddOneReportStyle( const char* szFilename )
{
	vdbFilename f( szFilename );
	if ( !f.Exists() )
		return;

	std::ifstream is( szFilename, std::ios_base::in );
	if ( !is.is_open() )
		return;

	ReportStyleDefinition* pDef = new ReportStyleDefinition;
	if ( pDef == 0 )
		throw vdbMemoryException();

	is >> *pDef;
	if ( is.fail() )
	{
		is.close();
		delete pDef; pDef = 0;
		return;
	}
	is.close();

	// save the filename because it is not stored inside the file
	// and we need it when saving changes
	pDef->_sFilename = szFilename;

	// add to the master list
	_reportStyleDefinitions.Add( pDef, pDef->_sStyleDescription );

	// add to the user's listbox
	m_ctlListStyles.AddString( pDef->_sStyleDescription ); 
}


//-------------------------------------
void vdbPrintReport::SaveOneReportStyle( ReportStyleDefinition* pDef )
{
	std::ofstream os( pDef->_sFilename, std::ios_base::out );
	if ( !os.is_open() )
		return;

	os << *pDef;
	os.close();
}


//=============================================================================
//  User action response functions
//=============================================================================

//-------------------------------------
void vdbPrintReport::OnPrint() 
{
	// get the selected report style
	int index = m_ctlListStyles.GetCurSel();
	if ( index == -1 )
	{
		vdbMessageBox( NULL, "Please select which report style to use.", "Print Report", MB_OK );
		return;
	}
	char sz[255];
	m_ctlListStyles.GetText( index, sz );
	ReportStyleDefinition* pStyle = _reportStyleDefinitions[ sz ];
	if ( pStyle == 0 )
		return;

	// get the selected report definition
	index = m_ctlListDefs.GetCurSel();
	if ( index == -1 )
	{
		vdbMessageBox( NULL, "Please select which report definition to use.", "Print Report", MB_OK );
		return;
	}
	m_ctlListDefs.GetText( index, sz );
	ReportDefinition* pDef = _reportDefinitions[ sz ];
	if ( pDef == 0 )
		return;


	ShowHourglass();
	DialogToData( m_hWnd );

	if ( m_ctlPages.GetCheck() == 0 )
		PrintHelper( _dlgData._printerDC, pDef, pStyle, 0, 0 );
	else
		PrintHelper( _dlgData._printerDC, pDef, pStyle, _dlgData._fromPage, _dlgData._toPage );
	ShowPointer();
}


//-------------------------------------
void vdbPrintReport::OnPrinterSetup() 
{
	vdbPrinterDialogClass::OnSetup( m_hWnd );
}


//-------------------------------------
void vdbPrintReport::OnSelchangeListDefs() 
{
	int index1 = m_ctlListDefs.GetCurSel();
	int index2 = m_ctlListStyles.GetCurSel();

	if ( index1 == -1 || index2 == -1 )
		m_ctlPrint.EnableWindow( FALSE );
	else
		m_ctlPrint.EnableWindow( TRUE );

	// if the "selected pages" is enabled, count the report pages
	if ( index1 != -1 )
		if ( m_ctlPages.GetCheck() == 1 )
			OnVdbPages();
}


//-------------------------------------
void vdbPrintReport::OnSelchangeListStyles() 
{
	int index1 = m_ctlListDefs.GetCurSel();
	int index2 = m_ctlListStyles.GetCurSel();

	if ( index1 == -1 || index2 == -1 )
		m_ctlPrint.EnableWindow( FALSE );
	else
		m_ctlPrint.EnableWindow( TRUE );

	if ( index2 == -1 )
		m_ctlFormatStyle.EnableWindow( FALSE );
	else
		m_ctlFormatStyle.EnableWindow( TRUE );
}


//-------------------------------------
void vdbPrintReport::OnVdbPages() 
{
	if ( m_ctlPages.GetCheck() == 0 )
		return;

	int index = m_ctlListDefs.GetCurSel();
	if ( index == -1 )
		return;

	char sz[255];
	m_ctlListDefs.GetText( index, sz );

	// use the report title to find this report in the map of all reports
	ReportDefinition* pDef = 0;
	pDef = _reportDefinitions[ sz ];
	if ( pDef == 0 )
		return;

	// write the report to a temporary file
	vdbFilename sFullname( "C:\\TEMP" );
	sFullname.CreateDir();
	sFullname.Append( pDef->_sDefinitionFile.Basename() );
	sFullname.Extension( "txt" );
	sFullname.Delete();

	// send it to the file
	ShowHourglass();
	std::ofstream os( sFullname );
	if ( !os.is_open() )
	{
		vdbString s;
		s.Format( "Couldn't open the file %s for printing.  Check to see that the disk is not write-protected and try again", (const char*) sFullname );
		vdbMessageBox( NULL, s, "Export Data", MB_OK );
		return;
	}

	ShowHourglass();
	pDef->Execute( os, false );
	os.close();

//TODO	int pageCount = hnCountPages( sFullname, 60 );
	int pageCount = 1;

	char szToPages[10];
	vdbUtility::SafeFormat( szToPages, sizeof(szToPages), "%d", pageCount );
	m_ctlFromPage.SetWindowText( "1" );
	m_ctlToPage.SetWindowText( szToPages );
	m_ctlFromPage.EnableWindow( TRUE );
	m_ctlToPage.EnableWindow( TRUE );
	ShowPointer();
}


//-------------------------------------
void vdbPrintReport::OnVdbAll() 
{
	m_ctlFromPage.SetWindowText( "" );
	m_ctlToPage.SetWindowText( "" );
	m_ctlFromPage.EnableWindow( FALSE );
	m_ctlToPage.EnableWindow( FALSE );
}


//=============================================================================
// Private helpers
//=============================================================================

//-------------------------------------
void vdbPrintReport::PrintHelper( HDC hdc, ReportDefinition* pDef, ReportStyleDefinition* pStyle, int fromPage, int toPage )
{
	DOCINFO di;
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = "Report output";
	di.lpszOutput = 0x0;

	if ( StartDoc( hdc, &di ) > 0 )
	{
		pDef->Execute( hdc, pStyle );
		EndDoc( hdc );
	}
}


//-------------------------------------
void vdbPrintReport::OnFormatStyle() 
{
	ShowHourglass();

	// get the selected style definition
	int index = m_ctlListStyles.GetCurSel();
	if ( index == -1 )
		return;
	char sz[81];
	m_ctlListStyles.GetText( index, sz );
	ReportStyleDefinition* pStyle = _reportStyleDefinitions[ sz ];
	if ( pStyle == 0 )
		return;
	
	// get the printer DC
	DialogToData( m_hWnd );
	ShowPointer();

	// show the dialog
	vdbFormatStyles dlg( _dlgData._printerDC, pStyle, (OID) 0 );
	dlg.DoModal();

	// save user changes
	SaveOneReportStyle( pStyle );
}


//-------------------------------------
void vdbPrintReport::OnDblclkListStyles() 
{
	OnFormatStyle();
}
