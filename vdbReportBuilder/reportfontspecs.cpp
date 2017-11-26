//=============================================================================
//
// CLASS:           ReportFontSpecs
//
// OVERVIEW:		The ReportFontSpecs class holds the font attributes
//                  for a report.
//
// FILE NAME:       ReportFontSpecs.cpp
//
// INITIAL DATE:    01/15/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportFontSpecs.h"
#include <fstream>
#include <vdbStreamHeader.h>


//=============================================================================
// ReportFontSpecs essential class functions
//=============================================================================

//-------------------------------------
ReportFontSpecs::ReportFontSpecs()
	: _pointSize( 0 ),
	  _orientation( 0 ),
	  _textColor( 0 ),
	  _backgroundColor( -1 )
{
}


//-------------------------------------
ReportFontSpecs::~ReportFontSpecs()
{
}


//-------------------------------------
// ReportFontSpecs copy constructor
ReportFontSpecs::ReportFontSpecs( const ReportFontSpecs& rhs )
	: _sFontSpecDescription( rhs._sFontSpecDescription ),
	  _sFontFaceName( rhs._sFontFaceName ),
	  _sFontStyle( rhs._sFontStyle ),
	  _pointSize( rhs._pointSize ),
	  _orientation( rhs._orientation ),
	  _textColor( rhs._textColor ),
	  _backgroundColor( rhs._backgroundColor )
{
}


//-------------------------------------
// ReportFontSpecs assignment operator
ReportFontSpecs& ReportFontSpecs::operator=( const ReportFontSpecs& rhs )
{
	if ( this == &rhs )
		return *this;

	_sFontSpecDescription = rhs._sFontSpecDescription;
	_sFontFaceName = rhs._sFontFaceName;
	_sFontStyle = rhs._sFontStyle;
	_pointSize = rhs._pointSize;
	_orientation = rhs._orientation;
	_textColor = rhs._textColor;
	_backgroundColor = rhs._backgroundColor;

	return *this;
}


//=============================================================================
// I/O stream functions
//=============================================================================

//-------------------------------------
std::ostream& operator<< ( std::ostream& os, const ReportFontSpecs& obj )
{
	const char sep = ',';
	vdbStreamHeader h( "RC", 1 );
	os << h;

	os << obj._sFontSpecDescription;
	os << obj._sFontFaceName;
	os << obj._sFontStyle;
	os << obj._pointSize << sep;
	os << obj._orientation << sep;
	os << obj._textColor << sep;
	os << obj._backgroundColor << std::endl;

	return os;
}


//-------------------------------------
std::istream& operator>> ( std::istream& is, ReportFontSpecs& obj )
{
	char sep;

	vdbStreamHeader h( "RC", 1 );
	is >> h;
	if ( is.fail() )
		return is;

	is >> obj._sFontSpecDescription;
	is >> obj._sFontFaceName;
	is >> obj._sFontStyle;
	is >> obj._pointSize >> sep;
	is >> obj._orientation >> sep;
	is >> obj._textColor >> sep;
	is >> obj._backgroundColor >> std::ws;

	return is;
}

