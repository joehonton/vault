//=============================================================================
//
// CLASS:           ReportBuilderPageHolder
//
// ESSENCE:         The property sheet for the Report Builder Wizard
//
// FILE NAME:       ReportBuilderPageHolder.h
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportBuilderPageHolder.h"
#include "..\\..\\Vault\\vdbResources\\resource.h"
#include "ReportBuilderPage1.h"
#include "ReportBuilderPage2.h"
#include "ReportBuilderPage3.h"
#include "ReportBuilderPage4.h"
#include "ReportBuilderPage5.h"
#include "ReportBuilderPage6.h"
#include "ReportBuilderPage7.h"
#include "ReportBuilderPage8.h"
#include "ReportBuilderPage9.h"
#include "ReportBuilderPage10.h"
#include <vdbServerInfo.h>
#include <vdbSetOfServers.h>


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
//^^ Constructor
ReportBuilderPageHolder::ReportBuilderPageHolder( CWnd* pParentWnd, ReportDefinition* pDef )
	: CPropertySheet( "Report builder wizard", pParentWnd, 0 ),
	  _pDef( pDef ),
	  _bReuseFile( false ),
	  _bValid( false ),
	  _bUseSelectionCriteria( false ),
	  _bDefineAnotherSelection( false ),
	  _bUseSortCriteria( false ),
	  _bDefineAnotherSort( false )
{
	// create the pages
	_pPage1  = new ReportBuilderPage1( this );
	_pPage2  = new ReportBuilderPage2( this );
	_pPage3  = new ReportBuilderPage3( this );
	_pPage4  = new ReportBuilderPage4( this );
	_pPage5  = new ReportBuilderPage5( this );
	_pPage6  = new ReportBuilderPage6( this );
	_pPage7  = new ReportBuilderPage7( this );
	_pPage8  = new ReportBuilderPage8( this );
	_pPage9  = new ReportBuilderPage9( this );
	_pPage10  = new ReportBuilderPage10( this );

	AddPage( _pPage1 );
	AddPage( _pPage2 );
	AddPage( _pPage3 );
	AddPage( _pPage4 );
	AddPage( _pPage5 );
	AddPage( _pPage6 );
	AddPage( _pPage7 );
	AddPage( _pPage8 );
	AddPage( _pPage9 );
	AddPage( _pPage10 );
}


//-------------------------------------
//^^ Destructor
ReportBuilderPageHolder::~ReportBuilderPageHolder()
{
	delete _pPage1; _pPage1 = 0;
	delete _pPage2; _pPage2 = 0;
	delete _pPage3; _pPage3 = 0;
	delete _pPage4; _pPage4 = 0;
	delete _pPage5; _pPage5 = 0;
	delete _pPage6; _pPage6 = 0;
	delete _pPage7; _pPage7 = 0;
	delete _pPage8; _pPage8 = 0;
	delete _pPage9; _pPage9 = 0;
	delete _pPage10; _pPage10 = 0;

	vdbSetOfServers::Destroy();
}


//=============================================================================
// Message map and data exchange
//=============================================================================

//-------------------------------------
//Message Map
BEGIN_MESSAGE_MAP(ReportBuilderPageHolder, CPropertySheet)
	//{{AFX_MSG_MAP(ReportBuilderPageHolder)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



//-------------------------------------
int ReportBuilderPageHolder::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// get the pointer to the server for the connected data source
	char szServerName[80];
	vdbPool::Instance()->GetDatabase()->ServerNameFromConnection( szServerName, sizeof(szServerName) );

	// read the set of Servers in order to get the Server-to-Vault datatype maps
	vdbSetOfServers* pAllServers = vdbSetOfServers::Instance();
	pAllServers->ReadFromArchive();
	_pServerInfo = pAllServers->Find( vdbString(szServerName) );

	return 0;
}


//-------------------------------------
//^ The DetermineNextPage function is called by each page during the OnWizardNext
//  function processing.
//> currentPage is the page calling this function
//< returns -1 to go nowhere, 0 to go to the next page, or a 1-based number to
//  go to a particular page
//
int ReportBuilderPageHolder::DetermineNextPage( int currentPage )
{
	switch ( currentPage )
	{
		case IDD_REPORT_BUILDER1:
			return IDD_REPORT_BUILDER2;

		case IDD_REPORT_BUILDER2:
				return IDD_REPORT_BUILDER3;

		case IDD_REPORT_BUILDER3:
			if ( _bUseSelectionCriteria == true )
			{
				if ( _pDef->_selectionCriteria.Count() == 0 )
					return IDD_REPORT_BUILDER4;
				else
					return IDD_REPORT_BUILDER5;
			}
			else
				return IDD_REPORT_BUILDER6;

		case IDD_REPORT_BUILDER4:
			return IDD_REPORT_BUILDER5;

		case IDD_REPORT_BUILDER5:
			if ( _bDefineAnotherSelection == true )
				return IDD_REPORT_BUILDER4;
			else
				return IDD_REPORT_BUILDER6;

		case IDD_REPORT_BUILDER6:
			if ( _bUseSortCriteria == true )
			{
				if ( _pDef->_sortCriteria.Count() == 0 )
					return IDD_REPORT_BUILDER7;
				else
					return IDD_REPORT_BUILDER8;
			}
			else
				return IDD_REPORT_BUILDER9;

		case IDD_REPORT_BUILDER7:
			return IDD_REPORT_BUILDER8;

		case IDD_REPORT_BUILDER8:
			if ( _bDefineAnotherSort == true )
				return IDD_REPORT_BUILDER7;
			else
				return IDD_REPORT_BUILDER9;

		case IDD_REPORT_BUILDER9:
			return IDD_REPORT_BUILDER10;

		case IDD_REPORT_BUILDER10:
			return -1;

		default:
			return 0;
	}
}


//-------------------------------------
//^ The DetermineBackPage function is called by each page during the OnWizardBack
//  function processing.
//> currentPage is the page calling this function
//< returns -1 to go nowhere, 0 to go to the previous page, or a 1-based number to
//  go to a particular page
//
int ReportBuilderPageHolder::DetermineBackPage( int currentPage )
{
	switch ( currentPage )
	{
		case IDD_REPORT_BUILDER10:
			return IDD_REPORT_BUILDER9;

		case IDD_REPORT_BUILDER9:
			if ( _bUseSortCriteria == true )
				return IDD_REPORT_BUILDER8;
			else
				return IDD_REPORT_BUILDER6;

		case IDD_REPORT_BUILDER8:
			return IDD_REPORT_BUILDER7;

		case IDD_REPORT_BUILDER7:
			return IDD_REPORT_BUILDER6;

		case IDD_REPORT_BUILDER6:
			if ( _bUseSelectionCriteria == true )
				return IDD_REPORT_BUILDER5;
			else
				return IDD_REPORT_BUILDER3;

		case IDD_REPORT_BUILDER5:
			return IDD_REPORT_BUILDER4;

		case IDD_REPORT_BUILDER4:
			return IDD_REPORT_BUILDER3;

		case IDD_REPORT_BUILDER3:
			return IDD_REPORT_BUILDER2;

		case IDD_REPORT_BUILDER2:
			return IDD_REPORT_BUILDER1;

		case IDD_REPORT_BUILDER1:
			return -1;

		default:
			return 0;
	}
}


