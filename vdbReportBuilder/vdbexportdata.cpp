//=============================================================================
//
// CLASS:           vdbExportData
//
// ESSENCE:         Export data via a report definition
//
// FILE NAME:       vdbExportData.cpp
//
// INITIAL DATE:    11/12/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "vdbExportData.h"
#include <Cursors.h>
#include <fstream>
#include <vdbFilename.h>
#include <vdbException.h>
#include <vdbStreamHeader.h>
#include <vdbDirectoryDialog.h>
#include <vdbRegistrationKey.h>


//=============================================================================
//  Essential class functions
//=============================================================================

//-------------------------------------
vdbExportData::vdbExportData()
	: vdbDLLDialog(vdbExportData::IDD, NULL)
{
	m_ctlDirectory.SetBitmaps(IDB_VDB_DIRECTORY_UP, IDB_VDB_DIRECTORY_DOWN, IDB_VDB_DIRECTORY_FOCUS, IDB_VDB_DIRECTORY_DISABLED );

	//{{AFX_DATA_INIT(vdbExportData)
	//}}AFX_DATA_INIT
}


//-------------------------------------
void vdbExportData::DoDataExchange(CDataExchange* pDX)
{
	vdbDLLDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(vdbExportData)
	DDX_Control(pDX, IDC_VDB_EXPORT, m_ctlExport);
	DDX_Control(pDX, IDC_VDB_EXPORT_FILENAME, m_ctlExportFilename);
	DDX_Control(pDX, IDC_VDB_EXPORT_DIRECTORY, m_ctlExportDirectory);
	DDX_Control(pDX, IDC_VDB_LIST, m_ctlList);
	DDX_Control(pDX, IDC_VDB_DIRECTORY, m_ctlDirectory);
	//}}AFX_DATA_MAP
}


//-------------------------------------
BEGIN_MESSAGE_MAP(vdbExportData, vdbDLLDialog)
	//{{AFX_MSG_MAP(vdbExportData)
	ON_BN_CLICKED(IDC_VDB_EXPORT, OnExport)
	ON_LBN_SELCHANGE(IDC_VDB_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_VDB_DIRECTORY, OnDirectory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
//  Initialization function
//=============================================================================

//-------------------------------------
//^ The OnInitDialog function is called just before the window is being shown.
//
BOOL vdbExportData::OnInitDialog() 
{
	vdbDLLDialog::OnInitDialog();
	ShowHourglass();

	m_ctlDirectory.SizeToContent();

	// get the directory location of this executable
	char szModuleFilename[_MAX_PATH];
	GetModuleFileName( NULL, szModuleFilename, sizeof(szModuleFilename) );
	vdbFilename sModule( szModuleFilename );
	vdbFilename sDirectory( sModule.Directory() );
	sDirectory.Append( "Filters" );

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
				AddOneFile( sFullname );
			}
			bMore = FindNextFile( hFind, &fd );
		}
		FindClose( hFind );
	}

	// set the default export directory
	char szDirectory[256];
	vdbRegistrationKey key( HKEY_CURRENT_USER, "SOFTWARE\\ProgrammersHeaven\\Vault\\1.0\\Reports", "Export Import Directory" );
	if ( key.GetValue( szDirectory, sizeof(szDirectory) ) == false )
		vdbUtility::SafeCopy( szDirectory, "C:\\ExportImport", sizeof(szDirectory) );
	m_ctlExportDirectory.SetWindowText( szDirectory );

	int index = m_ctlList.GetCurSel();
	if ( index == -1 )
		m_ctlExport.EnableWindow( FALSE );
	else
		m_ctlExport.EnableWindow( TRUE );

	ShowPointer();
	return TRUE;
}


//-------------------------------------
//^ The AddOneFile function
//> szFilename is the fully qualified name of the file containing the definition
//
void vdbExportData::AddOneFile( const char* szFilename )
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
	m_ctlList.AddString( pDef->_sTitle ); 
}


//=============================================================================
//  User action response functions
//=============================================================================

//-------------------------------------
void vdbExportData::OnExport() 
{
	int index = m_ctlList.GetCurSel();
	if ( index == -1 )
		return;

	char sz[255];
	m_ctlList.GetText( index, sz );

	// use the report title to find this report in the map of all reports
	ReportDefinition* pDef = 0;
	pDef = _reportDefinitions[ sz ];
	if ( pDef == 0 )
		return;

	// get the directory and save it to the Registry
	vdbFilename sFullname;
	m_ctlExportDirectory.GetWindowText( sz, sizeof(sz) );
	if ( sz[0] == 0x0 )
	{
		vdbMessageBox( NULL, "Please supply the directory location to use", "Export Data", MB_OK );
		return;
	}
	vdbRegistrationKey key( HKEY_CURRENT_USER, "SOFTWARE\\ProgrammersHeaven\\Vault\\1.0\\Reports", "Export Import Directory" );
	key.SetValue( sz );
	sFullname = sz;

	// get the filename
	m_ctlExportFilename.GetWindowText( sz, sizeof(sz) );
	if ( sz[0] == 0x0 )
	{
		vdbMessageBox( NULL, "Please supply the filename to use", "Export Data", MB_OK );
		return;
	}
	sFullname.Append( sz );
	sFullname.ReplaceInvalidCharacters();

	if ( sFullname.Exists() )
	{
		vdbString s;
		s.Format( "The file %s already exists.  Do you want to overwrite it?", (const char*) sFullname );
		if ( vdbMessageBox( NULL, s, "Export Data", MB_YESNO ) == IDNO )
			return;

		sFullname.Delete();
	}
	sFullname.CreateDir();

	// send it to the file
	ShowHourglass();
	std::ofstream os( sFullname );
	if ( !os.is_open() )
	{
		vdbString s;
		s.Format( "Couldn't open the file %s for export.  Check to see that the disk is not write-protected and try again", (const char*) sFullname );
		vdbMessageBox( NULL, s, "Export Data", MB_OK );
		return;
	}

	pDef->Execute( os, true );
	os.close();
	ShowPointer();
}


//-------------------------------------
void vdbExportData::OnSelchangeList() 
{
	int index = m_ctlList.GetCurSel();
	if ( index == -1 )
		m_ctlExport.EnableWindow( FALSE );
	else
		m_ctlExport.EnableWindow( TRUE );

	if ( index == -1 )
		return;

	char sz[255];
	m_ctlList.GetText( index, sz );

	vdbFilename sFilename = sz;
	sFilename.ReplaceInvalidCharacters();
	sFilename.Extension( "txt" );
	m_ctlExportFilename.SetWindowText( sFilename );

}


//-------------------------------------
void vdbExportData::OnDirectory() 
{
	char sz[120];
	m_ctlExportDirectory.GetWindowText( sz, sizeof(sz) );

	// show directory dialog box
	vdbDirectoryDlgData data( m_hWnd, "Select the export directory", sz, true );
	if ( vdbDirectoryDialog( data ) == false )
		return;

	m_ctlExportDirectory.SetWindowText( data.GetDirectory() );	
}
