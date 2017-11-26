//=============================================================================
//
// CLASS:           ReportLayout
//
// OVERVIEW:		The ReportLayout class is the base class for defining 
//                  rendering routines for various type of reports.  Each
//                  derived class defines two routines Display and Print for
//                  rendering a particular layout to the screen or the printer.
//
// FILE NAME:       ReportLayout.h
//
// INITIAL DATE:    01/17/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __ReportLayout_h
#define __ReportLayout_h

#ifndef __vdbSet_h
	#include <vdbSet.h>
#endif

#ifndef __vdbTypedef_h
	#include <vdbTypedef.h>
#endif

class ReportStyleDefinition;


//=============================================================================
class ReportLayout
{
public:
	ReportLayout( HDC hdc, ReportStyleDefinition* pDef );
	~ReportLayout();
	ReportLayout( const ReportLayout& rhs );			// copy constructor
	ReportLayout& operator=( const ReportLayout& rhs );	// assignment operator

public:
	virtual void Print( vdbSet<OID>& entryIDs );
	virtual void Paint( OID entryID, RECT& clipRect );

protected:
	virtual int LabelsPerPage();
	virtual void Render( OID entryID, int& cx, int& cy );
	void SelectFont( int styleIndex );

protected:
	HDC _hdc;						// a printer or screen Device Context
	ReportStyleDefinition* _pDef;
	int _countFontSpecs;			// the number of valid fonts and colors in the two variables below
	CFont* _pFonts;
	COLORREF* _pColors;
	COLORREF* _pBackgroundColors;	// -1 if transparent
	HPEN* _pPens;
	int _lineHeight;				// the height of each line (in the device context units)
	int _currentStyleIndex;			// the index into the fonts/colors/pens of the style selected into the DC
};

#endif  // ReportLayout_h
