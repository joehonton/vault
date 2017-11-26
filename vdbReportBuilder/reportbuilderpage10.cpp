//=============================================================================
//
// CLASS:           ReportBuilderPage10
//
// ESSENCE:         Property page for specifying the title and filename
//
// FILE NAME:       ReportBuilderPage10.cpp
//
// INITIAL DATE:    10/26/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportBuilderPage10.h"
#include "ReportBuilderPageHolder.h"
#include "ReportPreview.h"
#include <vdbFilename.h>
#include <fstream>


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
//^^ Constructor
ReportBuilderPage10::ReportBuilderPage10( ReportBuilderPageHolder* pParent )
	: CPropertyPage( ReportBuilderPage10::IDD ),
	  _pParent( pParent )
{
	m_psp.hInstance = VaultResourceInstance::Handle();

	//{{AFX_DATA_INIT(ReportBuilderPage10)
	//}}AFX_DATA_INIT
}


//-------------------------------------
//^^ Destructor
ReportBuilderPage10::~ReportBuilderPage10()
{
}


//=============================================================================
// Data exchange and message map
//=============================================================================

//-------------------------------------
//^^ DoDataExchange
void ReportBuilderPage10::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ReportBuilderPage10)
	DDX_Control(pDX, IDC_VDB_TITLE, m_ctlTitle);
	DDX_Control(pDX, IDC_VDB_PREVIEW, m_ctlPreview);
	DDX_Control(pDX, IDC_VDB_FILENAME, m_ctlFilename);
	//}}AFX_DATA_MAP
}


//-------------------------------------
// MessageMap
BEGIN_MESSAGE_MAP(ReportBuilderPage10, CPropertyPage)
	//{{AFX_MSG_MAP(ReportBuilderPage10)
	ON_BN_CLICKED(IDC_VDB_PREVIEW, OnPreview)
	ON_EN_CHANGE(IDC_VDB_TITLE, OnChangeTitle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
// Message response functions
//=============================================================================

//-------------------------------------
// The OnInitDialog function is called the first time this page is shown
//
BOOL ReportBuilderPage10::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// get the directory location of the report definition files
	char szModuleFilename[_MAX_PATH];
	GetModuleFileName( NULL, szModuleFilename, sizeof(szModuleFilename) );
	vdbFilename sModule( szModuleFilename );
	_sReportDefinitionDirectory = sModule.Directory();

	m_ctlPreview.EnableWindow( FALSE );

	if ( _pParent->_bReuseFile == true )
		m_ctlFilename.EnableWindow( FALSE );
	else
		m_ctlFilename.EnableWindow( TRUE );

	return FALSE;
}


//-------------------------------------
// The OnSetActive function is called when this page becomes the active page.
//
BOOL ReportBuilderPage10::OnSetActive() 
{
	BOOL rc = CPropertyPage::OnSetActive();

	m_ctlTitle.SetWindowText( _pParent->_pDef->_sTitle );
	m_ctlFilename.SetWindowText( _pParent->_pDef->_sDefinitionFile );

	_pParent->SetWizardButtons( PSWIZB_BACK | PSWIZB_FINISH );
	
	return rc;
}


//-------------------------------------
//^ The OnWizardFinish function is called when the user presses the Next button.
//
BOOL ReportBuilderPage10::OnWizardFinish() 
{
	if ( MoveDataToHolder( true ) == true )
	{
		WriteDefinitionFile();
		_pParent->_bValid = true;
		return TRUE;
	}
	else
		return FALSE;
}


//-------------------------------------
//^ The OnWizardBack function is called when the user presses the Back button.
//
LRESULT ReportBuilderPage10::OnWizardBack() 
{
	MoveDataToHolder( false );
	return _pParent->DetermineBackPage( IDD_REPORT_BUILDER10 );
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
bool ReportBuilderPage10::MoveDataToHolder( bool bValidate )
{
	if ( bValidate == true )
	{
		// title
		char sz[255];
		m_ctlTitle.GetWindowText( sz, sizeof(sz) );
		if ( sz[0] == 0x0 )
		{
			vdbMessageBox( (int) m_hWnd, "Please specify a title for this report", "Report Builder", MB_OK );
			return false;
		}
		else
			_pParent->_pDef->_sTitle = sz;
	
		// filename
		vdbFilename sFullname;
		m_ctlFilename.GetWindowText( sz, sizeof(sz) );
		if ( sz[0] == 0x0 )
		{
			vdbMessageBox( (int) m_hWnd, "Please specify a filename for this report definition", "Report Builder", MB_OK );
			return false;
		}
		else
			sFullname = sz;
		
		if ( _pParent->_bReuseFile == false && sFullname.Exists() )
		{
			vdbMessageBox( (int) m_hWnd, "Please specify a different filename, this name is already used for another report definition", "Report Builder", MB_OK );
			return false;
		}
		_pParent->_pDef->_sDefinitionFile = sFullname;

	}

	return true;
}


//=============================================================================
// Utility functions
//=============================================================================

//-------------------------------------
void ReportBuilderPage10::WriteDefinitionFile()
{
	ReportDefinition* pDef = _pParent->_pDef;

	std::ofstream os( pDef->_sDefinitionFile, std::ios_base::out );
	if ( !os.is_open() )
		return;

	os << *pDef;
	os.close();
}


//=============================================================================
// User interface callback functions
//=============================================================================

//-------------------------------------
void ReportBuilderPage10::OnPreview() 
{
	char sz[255];
	m_ctlTitle.GetWindowText( sz, sizeof(sz) );
	_pParent->_pDef->_sTitle = sz;

	ReportPreview dlg( _pParent->_pDef );
	dlg.DoModal();	
}


//-------------------------------------
void ReportBuilderPage10::OnChangeTitle() 
{
	// purposely restrict to 20 characters for the filename
	char sz[20];
	m_ctlTitle.GetWindowText( sz, sizeof(sz) );

	// if we're not modifying an existing report, automatically supply a reasonable filename
	if ( _pParent->_bReuseFile == false )
	{
		vdbFilename sFullname;
		sFullname = _sReportDefinitionDirectory;
		sFullname.Append( sz );
		sFullname.Extension( "def" );
		sFullname.Replace( ' ', '_' );
		sFullname.ReplaceInvalidCharacters();

		m_ctlFilename.SetWindowText( sFullname );
	}

	if ( sz[0] == 0x0 )
		m_ctlPreview.EnableWindow( FALSE );
	else
		m_ctlPreview.EnableWindow( TRUE );
}
