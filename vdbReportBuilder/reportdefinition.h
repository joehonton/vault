//=============================================================================
//
// CLASS:           ReportDefinition
//
// OVERVIEW:		The ReportDefinition class is the top-level class which
//                  holds attributes associated with one report
//
// FILE NAME:       ReportDefinition.h
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#ifndef __ReportDefinition_h
#define __ReportDefinition_h

#ifndef __vdbSet_h
	#include <vdbSet.h>
#endif

#ifndef __vdbString_h
	#include <vdbString.h>
#endif

#ifndef __vdbFilename_h
	#include <vdbFilename.h>
#endif

#ifndef __ReportColumn_h
	#include "ReportColumn.h"
#endif

#ifndef __ReportSelectionCriteria_h
	#include "ReportSelectionCriteria.h"
#endif

#ifndef __ReportSortCriteria_h
	#include "ReportSortCriteria.h"
#endif

#include <ostream>
#include <istream>
#include <fstream>

class CEdit;
class ReportStyleDefinition;


//=============================================================================
class ReportDefinition
{
public:
	// essential class functions
	ReportDefinition();
	~ReportDefinition();
	ReportDefinition( const ReportDefinition& rhs );			// copy constructor
	ReportDefinition& operator=( const ReportDefinition& rhs );	// assignment operator

public:
	bool Execute( std::ofstream& os, bool bExport );
	bool Execute( CEdit& editControl );
	bool Execute( HDC hdc, ReportStyleDefinition* pStyle );

public:
	void Output( const char* szText, int styleIndex, int widthInPixels = 0 );
	void OutputPageHeaderFooter();
	void OutputAt( const char* szText, int styleIndex, int x, int y, RECT& clipRect, UINT textAlignFlags );
	void PaintArea( RECT& rect, COLORREF color );
	inline bool IsExport() const			{ return _bExport; }

private:
	bool CreateFonts( ReportStyleDefinition* pStyle );
	void DeleteFonts();
	void SelectFont( int styleIndex );

private:
	bool ExecuteGeneric();
	bool ExecuteHelper();
	void AssembleQuery( vdbString& sSQL );

private:
	// used by the Execute functions
	CEdit* _pOutputEdit;
	std::ofstream* _pOutputStream;
	HDC _hdc;
	char _eol[3];					// end of line marker
	char _columnSeparator[3];		// tab or spaces
	bool _bExport;					// true if this is an export request
	
	// used by Execute(HDC,...)
	int _cx;						// current position of hdc
	int _cy;						// current position of hdc
	int _averageCharWidth;			// the width of one average character
	int _deltaY;					// height of one line
	int _pageNumber;				// the current page number being printed
	int _lineNumber;				// the current line number being printed (used for alternating report styles)
	int _countFontSpecs;			// the number of valid fonts and colors in the three variables below
	HFONT* _pFonts;
	COLORREF* _pColors;
	COLORREF* _pBackgroundColor;	// -1 if transparent
	HPEN* _pPens;
	int _currentStyleIndex;			// the style selected into the DC
	int _currentReportStyle;		// which of the two alternating _reportStyle1 and _reportStyle2 is currently selected
	int _pageHeaderStyle;			// index into the fonts/colors/pens for the page header
	int _pageFooterStyle;			// index into the fonts/colors/pens for the page footer
	int _pageLeftMarginStyle;		// index into the fonts/colors/pens for the left margin
	int _pageRightMarginStyle;		// index into the fonts/colors/pens for the right margin
	int _columnLabelsStyle;			// index into the fonts/colors/pens for the column labels
	int _reportStyle1;				// index into the fonts/colors/pens for the report area (style 1)
	int _reportStyle2;				// index into the fonts/colors/pens for the report area (style 2)
	RECT _printableArea;			// the printable area of the page (in pixels)
	RECT _dataArea;					// the printable area less the header, footer, and margins

public:
	vdbString _sOwner;				// database table "owner" qualifier
	vdbFilename _sDefinitionFile;	// fully qualified name of the report definition file
	vdbString _sTitle;				// The descriptive name to display in dialog boxes
	vdbString _sHeader;				// text to place at the top of each page
	vdbString _sFooter;				// text to place at the bottom of each page
	vdbString _sLeftMargin;			// text to place along the left side of each page
	vdbString _sRightMargin;		// text to place along the right side of each page
	vdbString _sTableName;			// SQL data table name
	vdbSet<ReportColumn> _columns;
	vdbSet<ReportSelectionCriteria> _selectionCriteria;
	vdbSet<ReportSortCriteria> _sortCriteria;

	friend std::ostream& operator<< ( std::ostream& os, const ReportDefinition& obj );
	friend std::istream& operator>> ( std::istream& is, ReportDefinition& obj );
};

//-------------------------------------
// global functions


#endif  // ReportDefinition_h
