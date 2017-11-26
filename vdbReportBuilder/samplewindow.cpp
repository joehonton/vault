//=============================================================================
//
// CLASS:           SampleWindow
//
// OVERVIEW:        CStatic-derived class to show a sample in the FormatStyles
//                  dialog
//
// FILE NAME:       SampleWindow.cpp
//
// INITIAL DATE:    01/16/00
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#include <vdbWindows.h>
#include "SampleWindow.h"
#include "ReportLayout.h"


//-------------------------------------
BEGIN_MESSAGE_MAP(SampleWindow, CStatic)
	//{{AFX_MSG_MAP(SampleWindow)
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
SampleWindow::SampleWindow()
	: _pDef( 0 ),
	  _entryID( 0 )
{
}


//-------------------------------------
SampleWindow::~SampleWindow()
{
}


//-------------------------------------
void SampleWindow::Setup( ReportStyleDefinition* pDef, OID entryID )
{
	_pDef = pDef;
	_entryID = entryID;
}


//-------------------------------------
void SampleWindow::OnSize(UINT nType, int cx, int cy)
{
	Refresh();
}


//=============================================================================
// Drawing
//=============================================================================

//-------------------------------------
void SampleWindow::Refresh()
{
	if ( _pDef == 0 )
		return;

	RECT rect;
	GetClientRect( &rect );
	InvalidateRect( &rect );
	UpdateWindow();
}


//-------------------------------------
void SampleWindow::OnPaint() 
{
	CPaintDC dc( this );

	RECT clipRect;
	GetClientRect( &clipRect );

	ReportLayout sll( dc.m_ps.hdc, _pDef );
	sll.Paint( _entryID, clipRect );

	return;
}


