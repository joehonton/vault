//=============================================================================
//
// CLASS:           ReportChangeColumn
//
// OVERVIEW:        Changes the display name and width of a column
//
// FILE NAME:       ReportChangeColumn.h
//
// INITIAL DATE:    11/22/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportChangeColumn.h"


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
ReportChangeColumn::ReportChangeColumn(CWnd* pParent /*=NULL*/)
	: vdbDLLDialog(ReportChangeColumn::IDD, pParent)
{
	//{{AFX_DATA_INIT(ReportChangeColumn)
	//}}AFX_DATA_INIT
}


//-------------------------------------
void ReportChangeColumn::DoDataExchange(CDataExchange* pDX)
{
	vdbDLLDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ReportChangeColumn)
	DDX_Control(pDX, IDC_VDB_DISPLAY_NAME, m_ctlDisplayName);
	DDX_Control(pDX, IDC_VDB_DISPLAY_WIDTH, m_ctlDisplayWidth);
	DDX_Control(pDX, IDC_VDB_FORMAT_STRING, m_ctlFormatString);
	//}}AFX_DATA_MAP
}


//-------------------------------------
BEGIN_MESSAGE_MAP(ReportChangeColumn, vdbDLLDialog)
	//{{AFX_MSG_MAP(ReportChangeColumn)
	ON_EN_CHANGE(IDC_VDB_DISPLAY_NAME, OnChangeDisplayName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
// Initialization functions
//=============================================================================

//-------------------------------------
BOOL ReportChangeColumn::OnInitDialog() 
{
	vdbDLLDialog::OnInitDialog();
	
	m_ctlDisplayName.SetWindowText( (const char*) _sDisplayName );

	char szValue[10];
	vdbUtility::SafeFormat( szValue, sizeof(szValue), "%d", _displayWidth );
	m_ctlDisplayWidth.SetWindowText( szValue );
	
	m_ctlFormatString.SetWindowText( (const char*) _sFormatString );

	return TRUE;
}


//=============================================================================
// Response functions
//=============================================================================

//-------------------------------------
void ReportChangeColumn::OnOK() 
{
	CString sValue;
	m_ctlDisplayWidth.GetWindowText( sValue );
	_displayWidth = atoi( sValue );

	if ( _displayWidth < 0 )
		_displayWidth = 0;

	m_ctlDisplayName.GetWindowText( _sDisplayName );
	int len = _sDisplayName.GetLength();
	if ( len > _displayWidth )
		_sDisplayName = _sDisplayName.Left( _displayWidth );
	m_ctlFormatString.GetWindowText( _sFormatString );

	vdbDLLDialog::OnOK();
}


//-------------------------------------
void ReportChangeColumn::OnChangeDisplayName() 
{
	CString sValue;
	m_ctlDisplayWidth.GetWindowText( sValue );
	int displayWidth = atoi( sValue );

	m_ctlDisplayName.GetWindowText( sValue );
	int len = sValue.GetLength();

	if ( len > displayWidth )
	{
		char szWidth[10];
		vdbUtility::SafeFormat( szWidth, sizeof(szWidth), "%d", len );
		m_ctlDisplayWidth.SetWindowText( szWidth );
	}
}
