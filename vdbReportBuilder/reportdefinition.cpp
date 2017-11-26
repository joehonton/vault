//=============================================================================
//
// CLASS:           ReportDefinition
//
// OVERVIEW:		The ReportDefinition class is the top-level class which
//                  holds attributes associated with one report
//
// FILE NAME:       ReportDefinition.cpp
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportDefinition.h"
#include <fstream>
#include <vdbStreamHeader.h>
#include "ReportOIDColumn.h"
#include "ReportLookupColumn.h"
#include "ReportNormalColumn.h"
#include "ReportStyleDefinition.h"
#include "ReportFontSpecs.h"

//warning C4996: 'strcpy': This function or variable may be unsafe. Consider using strcpy_s instead.
#pragma warning( disable : 4996 )


//=============================================================================
// essential class functions
//=============================================================================

//-------------------------------------
ReportDefinition::ReportDefinition()
	: _pOutputEdit( 0 ),
	  _pOutputStream( 0 ),
	  _hdc( 0 ),
	  _countFontSpecs( 0 ),
	  _pFonts( 0 ),
	  _pColors( 0 ),
	  _pBackgroundColor( 0 ),
	  _pPens( 0 ),
	  _cx( 0 ),
	  _cy( 0 ),
	  _averageCharWidth( 0 ),
	  _deltaY( 0 ),
	  _pageNumber( 0 ),
	  _lineNumber( 0 ),
	  _pageHeaderStyle( 0 ),
	  _pageFooterStyle( 0 ),
	  _pageLeftMarginStyle( 0 ),
	  _pageRightMarginStyle( 0 ),
	  _columnLabelsStyle( 0 ),
	  _reportStyle1( 0 ),
	  _reportStyle2( 0 )
{
	memset( &_printableArea, 0x0, sizeof(_printableArea) );
	memset( &_dataArea, 0x0, sizeof(_dataArea) );
}


//-------------------------------------
ReportDefinition::~ReportDefinition()
{
	_columns.ClearAll();
	_selectionCriteria.ClearAll();
	_sortCriteria.ClearAll();
}


//------------------------------------------
//^^ ReportDefinition copy constructor copies only the persistant items.
//   The Execute(HDC) related items are not copied because these are recalculated
//   by Execute
//
ReportDefinition::ReportDefinition( const ReportDefinition& rhs )
	: _pOutputEdit( rhs._pOutputEdit ),
	  _pOutputStream( rhs._pOutputStream ),
	  _hdc( rhs._hdc ),
	  _sOwner( rhs._sOwner ),
	  _sDefinitionFile( rhs._sDefinitionFile ),
	  _sTitle( rhs._sTitle ),
	  _sHeader( rhs._sHeader ),
	  _sFooter( rhs._sFooter ),
	  _sLeftMargin( rhs._sLeftMargin ),
	  _sRightMargin( rhs._sRightMargin ),
	  _sTableName( rhs._sTableName ),
	  _selectionCriteria( rhs._selectionCriteria ),
	  _sortCriteria( rhs._sortCriteria )
{
	// because vdbSet<T> cannot make polymorphic calls, we must copy ourselves
	int countColumns = rhs._columns.Count();
	for ( int i = 0; i < countColumns; i++ )
	{
		ReportColumn* pColumn = &rhs._columns[i];

		const type_info& t = typeid(*pColumn);

		if ( t == typeid( ReportOIDColumn ) )
		{
			ReportOIDColumn* p = dynamic_cast<ReportOIDColumn*>( pColumn );
			ReportOIDColumn* pNew = new ReportOIDColumn( *p );
			pNew->_pParent = this;
			_columns.Add( pNew );
		}
		else if ( t == typeid( ReportLookupColumn ) )
		{
			ReportLookupColumn* p = dynamic_cast<ReportLookupColumn*>( pColumn );
			ReportLookupColumn* pNew = new ReportLookupColumn( *p );
			pNew->_pParent = this;
			_columns.Add( pNew );
		}
		else if ( t == typeid( ReportNormalColumn ) )
		{
			ReportNormalColumn* p = dynamic_cast<ReportNormalColumn*>( pColumn );
			ReportNormalColumn* pNew = new ReportNormalColumn( *p );
			pNew->_pParent = this;
			_columns.Add( pNew );
		}
		else
			assert( false );
	}
}


//------------------------------------------
//^^ ReportDefinition assignment operator copies only the persistant items.
//   The Execute(HDC) related items are not copied because these are recalculated
//   by Execute
//
ReportDefinition& ReportDefinition::operator=( const ReportDefinition& rhs )
{
	if ( this == &rhs )
		return *this;

	_pOutputEdit = rhs._pOutputEdit,
	_pOutputStream = rhs._pOutputStream;
	_hdc = rhs._hdc;
	_sOwner = rhs._sOwner;
	_sDefinitionFile = rhs._sDefinitionFile;
	_sTitle = rhs._sTitle;
	_sHeader = rhs._sHeader;
	_sFooter = rhs._sFooter;
	_sLeftMargin = rhs._sLeftMargin;
	_sRightMargin = rhs._sRightMargin;
	_sTableName = rhs._sTableName;
	_columns = rhs._columns;
	_selectionCriteria = rhs._selectionCriteria;
	_sortCriteria = rhs._sortCriteria;

	// because vdbSet<T> cannot make polymorphic calls, we must copy ourselves
	int countColumns = rhs._columns.Count();
	for ( int i = 0; i < countColumns; i++ )
	{
		ReportColumn* pColumn = &rhs._columns[i];

		const type_info& t = typeid(*pColumn);

		if ( t == typeid( ReportOIDColumn ) )
		{
			ReportOIDColumn* p = dynamic_cast<ReportOIDColumn*>( pColumn );
			ReportOIDColumn* pNew = new ReportOIDColumn( *p );
			pNew->_pParent = this;
			_columns.Add( pNew );
		}
		else if ( t == typeid( ReportLookupColumn ) )
		{
			ReportLookupColumn* p = dynamic_cast<ReportLookupColumn*>( pColumn );
			ReportLookupColumn* pNew = new ReportLookupColumn( *p );
			pNew->_pParent = this;
			_columns.Add( pNew );
		}
		else if ( t == typeid( ReportNormalColumn ) )
		{
			ReportNormalColumn* p = dynamic_cast<ReportNormalColumn*>( pColumn );
			ReportNormalColumn* pNew = new ReportNormalColumn( *p );
			pNew->_pParent = this;
			_columns.Add( pNew );
		}
		else
			assert( false );
	}

	return *this;
}


//=============================================================================
// Execute functions
//=============================================================================

//------------------------------------------
//^ This Execute is called by ExportData to send output to a stream control
//> os is an open output stream
//> bExport is true to make the output with tab-separators and no header underscores
//
bool ReportDefinition::Execute( std::ofstream& os, bool bExport )
{
	_pOutputEdit = 0;
	_pOutputStream = &os;
	_hdc = 0;
	_eol[0] = 0x0A;
	_eol[1] = 0x00;
	_bExport = bExport;

	if ( _bExport == true )
	{
		_columnSeparator[0] = '\t';
		_columnSeparator[1] = 0x00;
	}
	else
	{
		_columnSeparator[0] = ' ';
		_columnSeparator[1] = ' ';
		_columnSeparator[2] = 0x00;
	}

	return ExecuteGeneric();
}


//------------------------------------------
//^ This Execute is called by ReportPreview to display the report in an edit control
bool ReportDefinition::Execute( CEdit& editControl )
{
	_pOutputEdit = &editControl;
	_pOutputStream = 0;
	_hdc = 0;
	_eol[0] = 0x0D;
	_eol[1] = 0x0A;
	_eol[2] = 0x00;
	_bExport = false;

	_columnSeparator[0] = ' ';
	_columnSeparator[1] = ' ';
	_columnSeparator[2] = 0x00;

	return ExecuteGeneric();
}


//------------------------------------------
//^ This Execute is called by PrintReport to output the report to a device context
//> hdc is the device context;
//> pStyle is the Report Style Definition
//
bool ReportDefinition::Execute( HDC hdc, ReportStyleDefinition* pStyle )
{
	_pOutputEdit = 0;
	_pOutputStream = 0;
	_hdc = hdc;
	
	_eol[0] = 0x0B;					// flag to be interpreted by Output()
	_eol[1] = 0x00;
	_bExport = false;

	_columnSeparator[0] = ' ';
	_columnSeparator[1] = ' ';
	_columnSeparator[2] = 0x00;

	_pageNumber = 1;
	_currentStyleIndex = -1;

	// create the fonts, colors, and pens of the report style
	if ( CreateFonts( pStyle ) == false )
		return false;

	// determine the printable area in pixels
	_printableArea.left = 0;
	_printableArea.top = 0;
	_printableArea.right = GetDeviceCaps( _hdc, HORZRES );
	_printableArea.bottom = GetDeviceCaps( _hdc, VERTRES );

/* Use this when implementing true margins
	// determine the location of the printable area in relation to the physical page
	_physicalArea.left = GetDeviceCaps( _hdc, PHYSICALOFFSETX );
	_physicalArea.top = GetDeviceCaps( _hdc, PHYSICALOFFSETY );
	_physicalArea.right = _physicalArea.left + _printableArea.right;
	_physicalArea.bottom = _physicalArea.top + _printableArea.bottom;
*/
	// Determine the data area to be used for printing the columns of data
	// by subtracting any header, footer or margins.
	_dataArea.left = _printableArea.left;
	_dataArea.top = _printableArea.top;
	_dataArea.right = _printableArea.right;
	_dataArea.bottom = _printableArea.bottom;

	if ( _sHeader.IsNotNull() )
	{
		SIZE size;
		SelectObject( _hdc, _pFonts[_pageHeaderStyle] );
		GetTextExtentPoint32( _hdc, (const char*) _sHeader, _sHeader.Length(), &size );
		_dataArea.top += (size.cy + 1);
	}
	if ( _sFooter.IsNotNull() )
	{
		SIZE size;
		SelectObject( _hdc, _pFonts[_pageFooterStyle] );
		GetTextExtentPoint32( _hdc, (const char*) _sFooter, _sFooter.Length(), &size );
		_dataArea.bottom -= (size.cy + 1);
	}
	if ( _sLeftMargin.IsNotNull() )
	{
		SIZE size;
		SelectObject( _hdc, _pFonts[_pageLeftMarginStyle] );
		GetTextExtentPoint32( _hdc, (const char*) _sLeftMargin, _sLeftMargin.Length(), &size );
		_dataArea.left += (size.cy + 1);
	}
	if ( _sRightMargin.IsNotNull() )
	{
		SIZE size;
		SelectObject( _hdc, _pFonts[_pageRightMarginStyle] );
		GetTextExtentPoint32( _hdc, (const char*) _sRightMargin, _sRightMargin.Length(), &size );
		_dataArea.right -= (size.cy + 1);
	}

	// determine the column widths based on the HDC and ReportFontSpecs
	assert( _countFontSpecs > 0 );
	assert( _pFonts != 0x0 );
	for ( int i = 0; i < _columns.Count(); i++ )
		_columns[i].DetermineWidthInPixels( _hdc, _pFonts[_columnLabelsStyle], _pFonts[_reportStyle1], _pFonts[_reportStyle2] );
	
	// execute the report
	bool rc = ExecuteGeneric();

	// cleanup
	DeleteFonts();

	return rc;
}


//=============================================================================
// The output function
//=============================================================================

//------------------------------------------
//^ The Output function is called by ReportColumn to send output to either an
//  edit control or a file stream or an HDC.  
//^ Call Execute(CEdit&) or Execute(ostream&) or Execute(HDC). They will call
//  ReportColumn which will callback to this function
//
//> szText is the data to print.  If szText begins with "--" it is reinterpreted 
//  in the case of HDC to be an underscore the width of the column.  If szText is
//  a 0x0B it is interpreted as an end of line in the case of HDC and may trigger
//  an EndPage/StartPage function call.
//> style is the index into the set of fonts/colors to use with this text.  This
//  value is used only with output to the _hdc.  It is ignored for ofstream and 
//  CEdit output.
//> widthInPixels is supplied only when outputing a column header or a data field.
//  If widthInPixels is greater than 0, then the output text is clipped to fit the
//  width.  If widthInPixels is 0, then the selected font, the average width of
//  a character, and the length of the text is used to is used to determine clipping
//  area. 
//
void ReportDefinition::Output( const char* szText, int styleIndex, int widthInPixels )
{
	if ( _pOutputEdit != 0 )
		_pOutputEdit->ReplaceSel( szText );
	
	else if ( _pOutputStream != 0 )
		*_pOutputStream << szText;
	
	else if ( _hdc != 0 )
	{
		assert( styleIndex >= 0 );
		assert( styleIndex < _countFontSpecs );

		SelectFont( styleIndex );

		if ( szText[0] == 0x0B )		// eol
		{
			_cx = _dataArea.left;
			_cy = _cy + _deltaY;
			_lineNumber++;
			if ( _cy > (_dataArea.bottom - _deltaY) )
			{
				EndPage( _hdc );
				OutputPageHeaderFooter();
			}
		}
		else if ( szText[0] == '-' && szText[1] == '-' ) 
		{
			int width = (widthInPixels > 0) ? widthInPixels : ( strlen(szText) * _averageCharWidth );
			POINT pt;
			MoveToEx( _hdc, _cx, _cy, &pt );
			LineTo( _hdc, min( _cx + width, _dataArea.right ), _cy );
			_cx = _cx + width;
		}
		else
		{
			int width = (widthInPixels > 0) ? widthInPixels : ( strlen(szText) * _averageCharWidth );
			RECT rect;
			rect.left = _cx;
			rect.top = _cy;
			rect.right = min( _cx + width, _dataArea.right );
			rect.bottom = _cy + _deltaY;
			ExtTextOut( _hdc, _cx, _cy, ETO_CLIPPED, &rect, szText, strlen(szText), NULL );
			_cx = _cx + width;
		}
	}

	else
		assert( false );
}


//-------------------------------------
//^ The OutputPageHeaderFooter function is called once for CEdit and ofstream executions
//  and once per page for printer CDC output
//
void ReportDefinition::OutputPageHeaderFooter()
{
	// output the report header, footer, and left and right margins
	if ( _hdc == 0 )
	{
		if ( _bExport == false )
		{
			// output header
			Output( _sTitle, _pageHeaderStyle ); 
			Output( _eol, _pageHeaderStyle );
			Output( _eol, _pageHeaderStyle );
		}
	}

/*
	//  To check actual printable area
	POINT pt;
	MoveToEx( _hdc, _printableArea.left, _printableArea.top, &pt );
	LineTo( _hdc, _printableArea.right-1, _printableArea.top );
	LineTo( _hdc, _printableArea.right-1, _printableArea.bottom-1 );
	LineTo( _hdc, _printableArea.left, _printableArea.bottom-1 );
	LineTo( _hdc, _printableArea.left, _printableArea.top );

	MoveToEx( _hdc, _dataArea.left, _dataArea.top, &pt );
	LineTo( _hdc, _dataArea.right-1, _dataArea.top );
	LineTo( _hdc, _dataArea.right-1, _dataArea.bottom-1 );
	LineTo( _hdc, _dataArea.left, _dataArea.bottom-1 );
	LineTo( _hdc, _dataArea.left, _dataArea.top );
*/

	if ( _hdc != 0 )
	{
		StartPage( _hdc );

		// output left margin
		if ( _sLeftMargin.IsNotNull() )
		{
			RECT rect;
			rect.left = _printableArea.left;
			rect.right = _dataArea.left - 1;
			rect.top = _printableArea.top;
			rect.bottom = _printableArea.bottom;
			if ( _pBackgroundColor[_pageLeftMarginStyle] != -1 )
				PaintArea( rect, _pBackgroundColor[_pageLeftMarginStyle] );

			int x = rect.left;
			int y = (rect.bottom - rect.top) / 2;
			OutputAt( (const char*) _sLeftMargin, _pageLeftMarginStyle, x, y, rect, TA_CENTER | TA_TOP ); 
		}

		// output right margin
		if ( _sRightMargin.IsNotNull() )
		{
			RECT rect;
			rect.left = _dataArea.right + 1;
			rect.right = _printableArea.right;
			rect.top = _printableArea.top;
			rect.bottom = _printableArea.bottom;
			if ( _pBackgroundColor[_pageRightMarginStyle] != -1 )
				PaintArea( rect, _pBackgroundColor[_pageRightMarginStyle] );

			int x = rect.left;
			int y = (rect.bottom - rect.top) / 2;
			OutputAt( (const char*) _sRightMargin, _pageRightMarginStyle, x, y, rect, TA_CENTER | TA_TOP ); 
		}

		// output header
		if ( _sHeader.IsNotNull() )
		{
			RECT rect;
			rect.left = _printableArea.left;
			rect.right = _printableArea.right;
			rect.top = _printableArea.top;
			rect.bottom = _dataArea.top - 1;
			if ( _pBackgroundColor[_pageHeaderStyle] != -1 )
				PaintArea( rect, _pBackgroundColor[_pageHeaderStyle] );

			int x = (rect.right - rect.left) / 2;
			int y = rect.top;
			OutputAt( (const char*) _sHeader, _pageHeaderStyle, x, y, rect, TA_CENTER | TA_TOP ); 
		}

		// output footer
		if ( _sFooter.IsNotNull() )
		{
			vdbString s;
			s.Format( _sFooter, _pageNumber );
			RECT rect;
			rect.left = _printableArea.left;
			rect.right = _printableArea.right;
			rect.top = _dataArea.bottom + 1;
			rect.bottom = _printableArea.bottom;
			if ( _pBackgroundColor[_pageFooterStyle] != -1 )
				PaintArea( rect, _pBackgroundColor[_pageFooterStyle] );

			int x = (rect.right - rect.left) / 2;
			int y = rect.top;
			OutputAt( (const char*) s, _pageFooterStyle, x, y, rect, TA_CENTER | TA_TOP ); 
		}
	}

	_cx = _dataArea.left;
	_cy = _dataArea.top;
	_lineNumber = 1;
	_pageNumber++;
	_currentReportStyle = _reportStyle2;

	if ( _hdc != 0 )
	{
		// paint background for column headers
		if ( _pBackgroundColor[_columnLabelsStyle] != -1 )
		{
			SelectFont( _columnLabelsStyle );
			RECT rect;
			rect.left = _dataArea.left;
			rect.right = _dataArea.right;
			rect.top = _cy;
			rect.bottom = _cy + _deltaY;
			PaintArea( rect, _pBackgroundColor[_columnLabelsStyle] );
		}
	}

	// output the column headers
	int countColumns = _columns.Count();
	assert( countColumns > 0 );
	for ( int i = 0; i < countColumns; i++ )
	{
		_columns[i].OutputHeader( _columnLabelsStyle );
		Output( _columnSeparator, _columnLabelsStyle );
	}
	Output( _eol, _columnLabelsStyle );

	// if this is not an "export" type of output place underscores below each column
	if ( _bExport == false )
	{
		// but only if the column background is not shaded
		if ( _hdc == 0 || _pBackgroundColor[_columnLabelsStyle] == -1 )
		{
			for ( int i = 0; i < countColumns; i++ )
			{
				_columns[i].OutputLineBreak( _columnLabelsStyle );
				Output( _columnSeparator, _columnLabelsStyle );
			}
		}
		Output( _eol, _columnLabelsStyle );
	}

}


//-------------------------------------
//^ The OutputAt function is called to place output to a particular area of
//  the page.  Clipping is to the rect.  The member variables _cx and _cy are not
//  used or updated.
//^ This function only works with HDC type execution
//> szText is the text to output
//> styleIndex is the index into the font/color/pen lists to use
//> x is the starting point for the drawing
//> y is the starting point for the drawing
//> clipRect is the location and clipping rect
//> textAlignFlags are TA_LEFT, TA_CENTER, TA_RIGHT and TA_TOP, TA_BOTTOM
//
void ReportDefinition::OutputAt( const char* szText, int styleIndex, int x, int y, RECT& clipRect, UINT textAlignFlags )
{
	if ( _pOutputEdit != 0 )
		return;
	if ( _pOutputStream != 0 )
		return;
	if ( _hdc == 0 )
		assert( false );

	UINT saveTA = GetTextAlign( _hdc );
	SelectFont( styleIndex );
	SetTextAlign( _hdc, textAlignFlags );
	ExtTextOut( _hdc, x, y, ETO_CLIPPED, &clipRect, szText, strlen(szText), NULL );
	SetTextAlign( _hdc, saveTA );
}


//-------------------------------------
//^ The PaintArea function fill the rectangle with the given color
void ReportDefinition::PaintArea( RECT& rect, COLORREF color )
{
	assert( _hdc != 0 );

	HBRUSH hBrush = CreateSolidBrush( color );

	// The Windows FillRect function does not fill the right and bottom pixels
	RECT fullSize;
	fullSize.left = rect.left;
	fullSize.top = rect.top;
	fullSize.right = rect.right + 1;
	fullSize.bottom = rect.bottom + 1;

	FillRect( _hdc, &fullSize, hBrush );

	DeleteObject( hBrush );
}


//=============================================================================
// The create and delete fonts functions
//=============================================================================

//-------------------------------------
bool ReportDefinition::CreateFonts( ReportStyleDefinition* pStyle )
{
	assert( pStyle != 0 );
	assert( _hdc != 0 );

	// set indexes to the first style in case a needed style is missing
	_pageHeaderStyle = 0;
	_pageFooterStyle = 0;
	_columnLabelsStyle = 0;
	_reportStyle1 = 0;
	_reportStyle2 = 0;
	_pageLeftMarginStyle = 0;
	_pageRightMarginStyle = 0;

	_countFontSpecs = pStyle->_fontSpecs.Count();
	if ( _countFontSpecs < 1 )
		return false;

	// allocate the fonts and colors
	_pFonts = new HFONT[_countFontSpecs];
	_pColors = new COLORREF[_countFontSpecs];
	_pBackgroundColor = new COLORREF[_countFontSpecs];
	_pPens = new HPEN[_countFontSpecs];

	int pixelsPerInch = GetDeviceCaps( _hdc, LOGPIXELSY );

	for ( int i = 0; i < _countFontSpecs; i++ )
	{
		ReportFontSpecs& fs = pStyle->_fontSpecs[i];

		_pColors[i] = fs._textColor;
		_pBackgroundColor[i] = fs._backgroundColor;
		_pPens[i] = CreatePen( PS_SOLID, 2, fs._textColor );

		LOGFONT lf;
		lf.lfCharSet = ANSI_CHARSET;
		lf.lfWidth = 0;
		lf.lfEscapement = fs._orientation * 10;
		lf.lfOrientation = fs._orientation * 10;

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
 
		_pFonts[i] = CreateFontIndirect( &lf );

		// save the indexes to the fonts for use by ExecuteHelper
		if ( fs._sFontSpecDescription == "Page header" )
			_pageHeaderStyle = i;
		else if ( fs._sFontSpecDescription == "Page footer" )
			_pageFooterStyle = i;
		else if ( fs._sFontSpecDescription == "Left margin" )
			_pageLeftMarginStyle = i;
		else if ( fs._sFontSpecDescription == "Right margin" )
			_pageRightMarginStyle = i;
		else if ( fs._sFontSpecDescription == "Column labels" )
			_columnLabelsStyle = i;
		else if ( fs._sFontSpecDescription == "Report area (style 1)" )
			_reportStyle1 = i;
		else if ( fs._sFontSpecDescription == "Report area (style 2)" )
			_reportStyle2 = i;
	}
	
	return true;
}


//-------------------------------------
void ReportDefinition::DeleteFonts()
{
	for ( int i = 0; i < _countFontSpecs; i++ )
		DeleteObject( _pFonts[i] );

	delete[] _pFonts; _pFonts = 0;
	delete[] _pColors; _pColors = 0;
	delete[] _pBackgroundColor; _pBackgroundColor = 0;
	delete[] _pPens; _pPens = 0;
}


//-------------------------------------
//^ The SelectFont function selects the font, color and pen of the specified
//  index
void ReportDefinition::SelectFont( int styleIndex )
{
	if ( styleIndex != _currentStyleIndex )
	{
		SetTextColor( _hdc, _pColors[styleIndex] );
		SelectObject( _hdc, _pPens[styleIndex] );
		SelectObject( _hdc, _pFonts[styleIndex] );
		if ( _pBackgroundColor[styleIndex] == -1 )
			SetBkMode( _hdc, TRANSPARENT );
		else
		{
			SetBkColor( _hdc, _pBackgroundColor[styleIndex] );
			SetBkMode( _hdc, OPAQUE );
		}

		TEXTMETRIC tm;
		SIZE size;
		GetTextMetrics( _hdc, &tm );
		_deltaY = tm.tmHeight;
		GetTextExtentPoint32( _hdc, "m", 1, &size );
		_averageCharWidth = size.cx;

		_currentStyleIndex = styleIndex;
	}
}


//=============================================================================
// Execute
//=============================================================================

//-------------------------------------
//^ This ExecuteGeneric is called by the other public Execute functions
bool ReportDefinition::ExecuteGeneric()
{
	bool rc = false;

	try
	{
		rc = ExecuteHelper();
	}
	catch(...)
	{
	}

	return rc;
}


//------------------------------------------
//^ The ExecuteHelper function
bool ReportDefinition::ExecuteHelper()
{
	int countColumns = _columns.Count();
	assert( countColumns > 0 );

	OutputPageHeaderFooter();

	// assemble and execute the query
	vdbString sql;
	AssembleQuery( sql );

	vdbStatement stmt( vdbPool::Instance()->GetDatabase() );																
	RETCODE rc = stmt.Execute( sql );														

	// bind the parameter values
	char** pColumns = new char*[countColumns];

	for ( int i = 0; i < countColumns; i++ )
	{
		pColumns[i] = new char[MAX_COLUMN_SIZE];
		stmt.Bind( i + 1, pColumns[i], MAX_COLUMN_SIZE );
	}

	// fetch until done
	while ( rc == SQL_SUCCESS )															
	{																					
		rc = stmt.Fetch();																
		if ( rc == SQL_NO_DATA_FOUND )													
			break;																		
		else if ( rc != SQL_SUCCESS )
		{
			for ( int i = 0; i < countColumns; i++ )
				{ delete[] pColumns[i]; pColumns[i] = 0; }
			delete[] pColumns; pColumns = 0;
			return false;
		}

		if ( _lineNumber % 2 == 1 )
			_currentReportStyle = (_currentReportStyle == _reportStyle1 ? _reportStyle2 : _reportStyle1 );

		if ( _hdc != 0 )
		{		
			// paint background for the entire row
			if ( _pBackgroundColor[_currentReportStyle] != -1 )
			{
				SelectFont( _currentReportStyle );
				RECT rect;
				rect.left = _dataArea.left;
				rect.right = _dataArea.right;
				rect.top = _cy;
				rect.bottom = _cy + _deltaY;
				PaintArea( rect, _pBackgroundColor[_currentReportStyle] );
			}
		}

		// output the data for this record
		for ( int i = 0; i < countColumns; i++ )
		{
			if ( _columns[i].OutputValue( pColumns[i], _currentReportStyle ) == false )
				return false;
			Output( _columnSeparator, _currentReportStyle );
		}
		Output( _eol, _currentReportStyle );
	}                                                                                   

	// delete column variables
	for ( int i = 0; i < countColumns; i++ )
		{ delete[] pColumns[i]; pColumns[i] = 0; }
	delete[] pColumns; pColumns = 0;

	return true;
}


//-------------------------------------
void ReportDefinition::AssembleQuery( vdbString& sSQL )
{
	std::ostrstream os;
	os << "SELECT ";

	int countColumns = _columns.Count();
	for ( int i = 0; i < countColumns; i++ )
	{
		if ( i > 0 )
			os << ", ";
		os << _columns[i]._sColumnName;
	}	

	os << " FROM " << (const char*) _sOwner << "." << _sTableName;

	int countSelections = _selectionCriteria.Count();
	if ( countSelections > 0 )
	{
		os << " WHERE ";
		for ( int j = 0; j < countSelections; j++ )
		{
			if ( j == 0 )
				os << "(";
			else
			{
				if ( _selectionCriteria[j]._sConnector == "AND" )
					os << ") AND (";
				else
					os << " OR ";
			}

			os << (const char*) _selectionCriteria[j]._sColumnName << " ";
			os << (const char*) _selectionCriteria[j]._sOperator << " '";
			os << (const char*) _selectionCriteria[j]._sValue << "'";

		}
		os << ") ";	
	}

	int countSorts = _sortCriteria.Count();
	if ( countSorts > 0 )
	{
		os << " ORDER BY ";
		for ( int k = 0; k < countSorts; k++ )
		{
			if ( k > 0 )
				os << ", ";
			os << (const char*) _sortCriteria[k]._sColumnName << " ";
			os << (const char*) _sortCriteria[k]._sOrder;

		}
	}
	// close stream and assign to caller's parameter
	os << std::ends;
	char* sql = os.str();
	sSQL = sql;
	delete[] sql; sql = 0;
}


//=============================================================================
// I/O stream functions
//=============================================================================

//-------------------------------------
std::ostream& operator<< ( std::ostream& os, const ReportDefinition& obj )
{
	char sep = ',';
	int ReportColumnStreamVersion = 2;		// used by ReportColumn::operator>>()

	vdbStreamHeader h( "ReportDefinition", 2 );
	os << h;

	os << obj._sOwner;
	os << obj._sDefinitionFile;
	os << obj._sTitle;
	os << obj._sHeader;
	os << obj._sFooter;
	os << obj._sLeftMargin;
	os << obj._sRightMargin;
	os << obj._sTableName;

	int count = obj._columns.Count();
	os << count << std::endl;

	for ( int i = 0; i < count; i++ )
	{
		ReportColumn* pColumn = &obj._columns[i];

		const type_info& t = typeid(*pColumn);

		if ( t == typeid( ReportOIDColumn ) )
		{
			ReportOIDColumn* p = dynamic_cast<ReportOIDColumn*>( pColumn );
			os << "OIDColumn" << sep << ReportColumnStreamVersion << std::endl;
			os << *p;
		}
		else if ( t == typeid( ReportLookupColumn ) )
		{
			ReportLookupColumn* p = dynamic_cast<ReportLookupColumn*>( pColumn );
			os << "LookupColumn" << sep << ReportColumnStreamVersion << std::endl;
			os << *p;
		}
		else if ( t == typeid( ReportNormalColumn ) )
		{
			ReportNormalColumn* p = dynamic_cast<ReportNormalColumn*>( pColumn );
			os << "NormalColumn" << sep << ReportColumnStreamVersion << std::endl;
			os << *p;
		}
		else
			assert( false );
	}

	os << obj._selectionCriteria;
	os << obj._sortCriteria;

	return os;
}


//-------------------------------------
std::istream& operator>> ( std::istream& is, ReportDefinition& obj )
{
	vdbStreamHeader h( "ReportDefinition", 1, 2 );
	h.QuenchMessages();
	is >> h;
	if ( strcmp( "ReportDefinition", h.GetActualHeader() ) ||
	     h.GetActualVersion() > 2 )
	{
		is.clear( std::ios_base::failbit );	// set the failbit
		return is;
	}

	is >> obj._sOwner;
	is >> obj._sDefinitionFile;
	is >> obj._sTitle;
	if ( h.GetActualVersion() >= 2 ) 		
	{
		is >> obj._sHeader;
		is >> obj._sFooter;
		is >> obj._sLeftMargin;
		is >> obj._sRightMargin;
	}
	is >> obj._sTableName;
	
	int count;
	is >> count >> std::ws;
	for ( int i = 0; i < count; i++ )
	{
		char szTypeAndVersion[40];
		char szReportColumnType[20];
		int ReportColumnStreamVersion;
	
		is.getline( szTypeAndVersion, sizeof(szTypeAndVersion) );
		char* p = strchr( szTypeAndVersion, ',' );
		if ( p == 0 )
		{
			// version 1 does not have an actual version number
			ReportColumnStreamVersion = 1;
			strcpy( szReportColumnType, szTypeAndVersion );
		}
		else
		{
			ReportColumnStreamVersion = atoi( p+1 );
			*p = 0x0;
			strcpy( szReportColumnType, szTypeAndVersion );
		}

		
		if ( !strcmp( szReportColumnType, "OIDColumn" ) )
		{
			ReportOIDColumn* p = new ReportOIDColumn;
			if ( p == 0 )
				throw vdbMemoryException();
			p->_IOStreamVersion = ReportColumnStreamVersion;
			is >> *p;
			p->_pParent = &obj;
			p->Prepare();
			obj._columns.Add( p );
		}
		else if ( !strcmp( szReportColumnType, "LookupColumn" ) )
		{
			ReportLookupColumn* p = new ReportLookupColumn;
			if ( p == 0 )
				throw vdbMemoryException();
			p->_IOStreamVersion = ReportColumnStreamVersion;
			is >> *p;
			p->_pParent = &obj;
			p->Prepare();
			obj._columns.Add( p );
		}
		else // if ( !strcmp( szReportColumnType, "NormalColumn" ) )
		{
			ReportNormalColumn* p = new ReportNormalColumn;
			if ( p == 0 )
				throw vdbMemoryException();
			p->_IOStreamVersion = ReportColumnStreamVersion;
			is >> *p;
			p->_pParent = &obj;
			p->Prepare();
			obj._columns.Add( p );
		}
	}

	is >> obj._selectionCriteria;
	is >> obj._sortCriteria;

	return is;
}

