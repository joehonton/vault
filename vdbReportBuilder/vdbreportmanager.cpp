//=============================================================================
//
// CLASS:           vdbReportManager
//
// ESSENCE:         Add/duplicate/modify/delete reports
//
// FILE NAME:       vdbReportManager.cpp
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "vdbReportManager.h"
#include <Cursors.h>
#include <fstream>
#include <vdbFilename.h>
#include <vdbException.h>
#include <vdbStreamHeader.h>
#include "ReportBuilderPageHolder.h"
#include "ReportPreview.h"


//=============================================================================
//  Essential class functions
//=============================================================================

//-------------------------------------
vdbReportManager::vdbReportManager()
	: vdbDLLDialog(vdbReportManager::IDD, NULL)
{
	//{{AFX_DATA_INIT(vdbReportManager)
	//}}AFX_DATA_INIT
}


//-------------------------------------
void vdbReportManager::DoDataExchange(CDataExchange* pDX)
{
	vdbDLLDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(vdbReportManager)
	DDX_Control(pDX, IDC_VDB_NEW, m_ctlNew);
	DDX_Control(pDX, IDC_VDB_LIST, m_ctlList);
	DDX_Control(pDX, IDC_VDB_DUPLICATE, m_ctlDuplicate);
	DDX_Control(pDX, IDC_VDB_MODIFY, m_ctlModify);
	DDX_Control(pDX, IDC_VDB_DELETE, m_ctlDelete);
	//}}AFX_DATA_MAP
}


//-------------------------------------
BEGIN_MESSAGE_MAP(vdbReportManager, vdbDLLDialog)
	//{{AFX_MSG_MAP(vdbReportManager)
	ON_BN_CLICKED(IDC_VDB_NEW, OnNew)
	ON_BN_CLICKED(IDC_VDB_DUPLICATE, OnDuplicate)
	ON_BN_CLICKED(IDC_VDB_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_VDB_MODIFY, OnModify)
	ON_LBN_SELCHANGE(IDC_VDB_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_VDB_PREVIEW, OnPreview)
	ON_LBN_DBLCLK(IDC_VDB_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
//  Initialization function
//=============================================================================

//-------------------------------------
//^ The OnInitDialog function is called just before the window is being shown.
//
BOOL vdbReportManager::OnInitDialog() 
{
	vdbDLLDialog::OnInitDialog();
	ShowHourglass();

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

	RefreshButtonStates();
	ShowPointer();
	return TRUE;
}


//-------------------------------------
//^ The AddOneFile function
//> szFilename is the fully qualified name of the file containing the definition
//
void vdbReportManager::AddOneFile( const char* szFilename )
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
//^ The OnNew function is called to create a new report
//
void vdbReportManager::OnNew() 
{
	ReportDefinition* pDef = new ReportDefinition;
	if ( pDef == 0 )
		throw vdbMemoryException();
	pDef->_sOwner = vdbPool::Instance()->GetDatabase()->Owner();

	ReportBuilderPageHolder sheet( this, pDef );
	sheet.SetWizardMode();
	sheet.DoModal(); 

	if ( sheet._bValid == true )
	{
		ShowHourglass();

		// add to the master list and the listbox
		_reportDefinitions.Add( pDef, pDef->_sTitle );
		int index = m_ctlList.AddString( pDef->_sTitle );
		m_ctlList.SetCurSel( index );
		RefreshButtonStates();

		ShowPointer();
	}	
	else
	{
		delete pDef; pDef = 0;
	}
}


//-------------------------------------
//^ The OnDuplicate function is called to duplicate an existing report
//
void vdbReportManager::OnDuplicate() 
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

	ReportDefinition* pNew = new ReportDefinition( *pDef );
	if ( pNew == 0 )
		throw vdbMemoryException();
	pNew->_sTitle += " (Duplicate)";
	vdbFilename s( pNew->_sDefinitionFile.Basename() );
	s += "2";
	pNew->_sDefinitionFile.ReplaceFilename( s );
	pNew->_sDefinitionFile.Extension( "def" );
	
	ReportBuilderPageHolder sheet( this, pNew );
	sheet.SetWizardMode();
	sheet.DoModal(); 

	if ( sheet._bValid == true )
	{
		ShowHourglass();

		// add to the master list and the listbox
		_reportDefinitions.Add( pNew, pNew->_sTitle );
		index = m_ctlList.AddString( pNew->_sTitle );
		m_ctlList.SetCurSel( index );
		RefreshButtonStates();

		ShowPointer();
	}	
	else
	{
		delete pNew; pNew = 0;
	}
}


//-------------------------------------
void vdbReportManager::OnDblclkList() 
{
	OnModify();
}


//-------------------------------------
//^ The OnModify function is called to modify a Property
//
void vdbReportManager::OnModify() 
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

	ReportBuilderPageHolder sheet( this, pDef );
	sheet.ReuseFile();
	sheet.SetWizardMode();
	sheet.DoModal(); 

	if ( sheet._bValid == true )
	{
		ShowHourglass();

		// copy the definition, because the name may have changed and we need to remove it from the map
		ReportDefinition* pNew = new ReportDefinition( *pDef );
		if ( pNew == 0 )
			throw vdbMemoryException();

		// remove the old one from the master list and the listbox
		_reportDefinitions.Remove( sz );
		m_ctlList.DeleteString( index );

		// add the new one to the master list and the listbox
		_reportDefinitions.Add( pNew, pNew->_sTitle );
		index = m_ctlList.AddString( pNew->_sTitle );
		m_ctlList.SetCurSel( index );
		RefreshButtonStates();

		ShowPointer();
	}	
}


//-------------------------------------
//^ The OnDelete function is called to delete a Property
//
void vdbReportManager::OnDelete() 
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

	// delete the definition file
	if ( pDef->_sDefinitionFile.Delete() == false )
	{
		vdbMessageBox( (int) m_hWnd, "Unable to delete the report definition file", "Formulation Database", MB_OK );
		return;
	}

	// remove from the map
	_reportDefinitions.Remove( sz );

	// remove from user interface
	m_ctlList.DeleteString( index );

	// select the one at the same point in the list
	int count = m_ctlList.GetCount();
	if ( index >= count )
		index--;
	m_ctlList.SetCurSel( index );

	RefreshButtonStates();
}


//-------------------------------------
void vdbReportManager::OnPreview() 
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

	ReportPreview dlg( pDef );
	dlg.DoModal();	
}


//=============================================================================
//  User interface message response functions
//=============================================================================

//-------------------------------------
//^ The RefreshButtonStates function is called to enable/disable the
//  add/modify/delete buttons.
//
void vdbReportManager::RefreshButtonStates() 
{
	int index = m_ctlList.GetCurSel();
	if ( index == -1 )
	{
		m_ctlDuplicate.EnableWindow( FALSE );
		m_ctlModify.EnableWindow( FALSE );
		m_ctlDelete.EnableWindow( FALSE );
	}
	else
	{
		m_ctlDuplicate.EnableWindow( TRUE );
		m_ctlModify.EnableWindow( TRUE );
		m_ctlDelete.EnableWindow( TRUE );
	}
}


//-------------------------------------
void vdbReportManager::OnSelchangeList() 
{
	RefreshButtonStates();
}


