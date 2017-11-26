//=============================================================================
//
// CLASS:           ReportLayout
//
// OVERVIEW:		The ReportLayout class is the base class for defining 
//                  rendering routines for various type of reports.  Each
//                  derived class defines two routines Display and Print for
//                  rendering a particular layout to the screen or the printer.
//
// FILE NAME:       ReportLayout.cpp
//
// INITIAL DATE:    01/17/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportLayout.h"
#include "ReportStyleDefinition.h"
#include "ReportFontSpecs.h"
#include <math.h>


//=============================================================================
// ReportLayout essential class functions
//=============================================================================

//-------------------------------------
ReportLayout::ReportLayout( HDC hdc, ReportStyleDefinition* pDef )
	: _hdc( hdc ),
	  _pDef( pDef ),
	  _pFonts( 0 ),
	  _pColors( 0 ),
	  _pBackgroundColors( 0 ),
	  _pPens( 0 ),
	  _lineHeight( 0 ),
	  _currentStyleIndex( -1 )
{
	assert( _hdc != 0 );
	assert( _pDef != 0 );

	_countFontSpecs = _pDef->_fontSpecs.Count();
	if ( _countFontSpecs < 1 )
		return;

	//? should we move these arrays into ReportFontSpecs as individual entries?
	
	// allocate the fonts and colors
	_pFonts = new CFont[_countFontSpecs];
	_pColors = new COLORREF[_countFontSpecs];
	_pBackgroundColors = new COLORREF[_countFontSpecs];
	_pPens = new HPEN[_countFontSpecs];

	int pixelsPerInch = GetDeviceCaps( _hdc, LOGPIXELSY );

	for ( int i = 0; i < _countFontSpecs; i++ )
	{
		ReportFontSpecs& fs = pDef->_fontSpecs[i];

		_pColors[i] = fs._textColor;
		_pBackgroundColors[i] = fs._backgroundColor;
		_pPens[i] = CreatePen( PS_SOLID, 2, fs._textColor );

		LOGFONT lf;
		lf.lfCharSet = ANSI_CHARSET;
		lf.lfWidth = 0;
		lf.lfEscapement = 0;
		lf.lfOrientation = 0;

		if ( fs._sFontStyle == "Bold" )
		{
			lf.lfWeight = FW_BOLD;	
			lf.lfItalic = FALSE;
		}
		else if ( fs._sFontStyle == "Italic" )
		{
			lf.lfWeight = FW_REGULAR;	
			lf.lfItalic = TRUE;
		}
		else if ( fs._sFontStyle == "Bold Italic" )
		{
			lf.lfWeight = FW_BOLD;	
			lf.lfItalic = TRUE;
		}
		else //if ( fs._sFontStyle == "Regular" )
		{
			lf.lfWeight = FW_REGULAR;	
			lf.lfItalic = FALSE;
		}

		lf.lfUnderline = FALSE;
		lf.lfStrikeOut = FALSE;
		vdbUtility::SafeCopy( lf.lfFaceName, fs._sFontFaceName, sizeof(lf.lfFaceName) );
		lf.lfHeight = -MulDiv( fs._pointSize, pixelsPerInch, 72 );
 
		_pFonts[i].CreateFontIndirect( &lf );

		// select this font so that we can obtain the actual height and internal leading
		//TODO NOTE that this obtains the height of the last font only
		TEXTMETRIC tm;
		SelectObject( _hdc, &_pFonts[i] );
		GetTextMetrics( _hdc, &tm );
		_lineHeight = max( _lineHeight, tm.tmHeight + tm.tmInternalLeading );
	}
}


//-------------------------------------
ReportLayout::~ReportLayout()
{
	delete[] _pFonts; _pFonts = 0;
	delete[] _pColors; _pColors = 0;
	delete[] _pBackgroundColors; _pBackgroundColors = 0;
	delete[] _pPens; _pPens = 0;
}


//-------------------------------------
// ReportLayout copy constructor
ReportLayout::ReportLayout( const ReportLayout& rhs )
{
	assert( false );
}


//-------------------------------------
// ReportLayout assignment operator
ReportLayout& ReportLayout::operator=( const ReportLayout& rhs )
{
	if ( this == &rhs )
		return *this;

	assert( false );

	return *this;
}


//=============================================================================
// ReportLayout rendering functions
//=============================================================================

//-------------------------------------
//^ The Print routine should be called for printer device contexts
//
void ReportLayout::Print( vdbSet<OID>& entryIDs )
{
	int labelsPerPage = LabelsPerPage();

	// Start the doc and print
	DOCINFO di;
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = "Catalog labels";
	di.lpszOutput = 0x0;

	if ( StartDoc( _hdc, &di ) == 0 )
	{
		AbortDoc( _hdc );
		return;
	}
	if ( StartPage( _hdc ) == 0 )
	{
		AbortDoc( _hdc );
		return;
	}

	// Get the page size for the printer
	int pageWidth  = GetDeviceCaps( _hdc, HORZRES );
	int pageHeight = GetDeviceCaps( _hdc, VERTRES );
	
	// determine print area for the page
	int x0 = int(floor(pageWidth  * 0.07));		// left margin and first column
	int y0 = int(floor(pageHeight * 0.07));
	
	int cx = x0;
	int cy = y0;

	int labelCount = 1;
	int entryCount = entryIDs.Count();
	for ( int i = 0; i < entryCount; i++ )
	{
		Render( entryIDs[i], cx, cy );
		
		labelCount++;
		if ( labelCount > labelsPerPage )
		{
			EndPage( _hdc );
			if ( entryCount > i + 1 )
				StartPage( _hdc );

			cx = x0;
			cy = y0;
			labelCount = 1;
		}
	}

	EndDoc( _hdc );
}


//-------------------------------------
//^ The Paint routine should be called for screen device contexts.  In this
//  case the _pDC should be a CPaintDC
//> clipRect is the clipping rectangle to render into
//
void ReportLayout::Paint( OID entryID, RECT& clipRect )
{
	// paint background
	HBRUSH hBrush = CreateSolidBrush( ::GetSysColor(COLOR_3DFACE) );
//?	SetBkColor( _hdc, ::GetSysColor(COLOR_3DFACE) );
	FillRect( _hdc, &clipRect, hBrush );
	
	// setup clipping region
	HRGN r = CreateRectRgn( clipRect.left, clipRect.top, clipRect.right, clipRect.bottom );
	SelectClipRgn( _hdc, r );

	int cx = 0;
	int cy = 0;
	Render( entryID, cx, cy );

	// cleanup
	SelectClipRgn( _hdc, NULL );
	DeleteObject( r );
	DeleteObject( hBrush );
}


//-------------------------------------
//^ The Render routine should be overridden by each derived class
//> entryID is the OID of the cdbEntry to render
//> cy is the distance from the top of the device context to begin at
//  This routine should update the value of cy so that a subsequent call to
//  Render will not cause overlapping results
//
void ReportLayout::Render( OID entryID, int& cx, int& cy )
{
	_currentStyleIndex = -1;
	// When overriding this function do not call this base class implementation

	// In case this class is used without derivation or in case no virtual
	// override is supplied use this:
	for ( int i = 0; i < _countFontSpecs; i++ )
	{
		SelectFont( i );

		vdbString sText( _pDef->_fontSpecs[i]._sFontSpecDescription );
		TextOut( _hdc, cx, cy, (const char*) sText, sText.Length() );

		SIZE size;
		GetTextExtentPoint32( _hdc, (const char*) sText, sText.Length(), &size );
		cy += size.cy;
	}
}


//-------------------------------------
//^ The LabelsPerPage function should be overridden to determine the number of
//  labels that will fit on a page
//
int ReportLayout::LabelsPerPage()
{
	return 1;
}


//-------------------------------------
//^ The SelectFont function selects the font, color and pen of the specified
//  index
void ReportLayout::SelectFont( int styleIndex )
{
	if ( styleIndex != _currentStyleIndex )
	{
		SetTextColor( _hdc, _pColors[styleIndex] );
		SelectObject( _hdc, _pPens[styleIndex] );
		SelectObject( _hdc, _pFonts[styleIndex] );
		if ( _pBackgroundColors[styleIndex] == -1 )
			SetBkMode( _hdc, TRANSPARENT );
		else
		{
			SetBkColor( _hdc, _pBackgroundColors[styleIndex] );
			SetBkMode( _hdc, OPAQUE );
		}
/*
// should we make an array of _deltaY and _averageCharWidth?
		TEXTMETRIC tm;
		SIZE size;
		GetTextMetrics( _hdc, &tm );
		_deltaY = tm.tmHeight;
		GetTextExtentPoint32( _hdc, "m", 1, &size );
		_averageCharWidth = size.cx;
*/
		_currentStyleIndex = styleIndex;
	}
}



