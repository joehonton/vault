//=============================================================================
//
// CLASS:           ReportFontSpecs
//
// OVERVIEW:		The ReportFontSpecs class holds the font attributes
//                  for a report.
//
// FILE NAME:       ReportFontSpecs.h
//
// INITIAL DATE:    01/15/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __ReportFontSpecs_h
#define __ReportFontSpecs_h

#ifndef __vdbString_h
	#include <vdbString.h>
#endif

#include <ostream>
#include <istream>


//=============================================================================
class ReportFontSpecs
{
public:
	ReportFontSpecs();
	~ReportFontSpecs();
	ReportFontSpecs( const ReportFontSpecs& rhs );				// copy constructor
	ReportFontSpecs& operator=( const ReportFontSpecs& rhs );	// assignment operator

public:
	vdbString _sFontSpecDescription;
	vdbString _sFontFaceName;		// "Times New Roman", etc,
	vdbString _sFontStyle;			// bold, italic, regular
	int _pointSize;
	int _orientation;				// 0 (normal), 90 (bottom-to-top), 180 (upsidedown), 270 (top-to-bottom)
	COLORREF _textColor;
	COLORREF _backgroundColor;		// -1 means TRANSPARENT

	friend std::ostream& operator<< ( std::ostream& os, const ReportFontSpecs& obj );
	friend std::istream& operator>> ( std::istream& is, ReportFontSpecs& obj );
};

#endif  // ReportFontSpecs_h
