//=============================================================================
//
// CLASS:           ReportColumnAssignment
//
// OVERVIEW:		The ReportColumnAssignment class connects a column to a
//                  report style definition.
//
// FILE NAME:       ReportColumnAssignment.cpp
//
// INITIAL DATE:    01/15/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportColumnAssignment.h"
#include <fstream>
#include <vdbStreamHeader.h>


//=============================================================================
// ReportColumnAssignment essential class functions
//=============================================================================

//-------------------------------------
ReportColumnAssignment::ReportColumnAssignment()
{
}


//-------------------------------------
ReportColumnAssignment::~ReportColumnAssignment()
{
}


//-------------------------------------
// ReportColumnAssignment copy constructor
ReportColumnAssignment::ReportColumnAssignment( const ReportColumnAssignment& rhs )
	: _sColumnName( rhs._sColumnName ),
	 _sFontSpecDescription( rhs._sFontSpecDescription )
{
}


//-------------------------------------
// ReportColumnAssignment assignment operator
ReportColumnAssignment& ReportColumnAssignment::operator=( const ReportColumnAssignment& rhs )
{
	if ( this == &rhs )
		return *this;

	_sColumnName = rhs._sColumnName;
	_sFontSpecDescription = rhs._sFontSpecDescription;

	return *this;
}



//=============================================================================
// I/O stream functions
//=============================================================================

//-------------------------------------
std::ostream& operator<< ( std::ostream& os, const ReportColumnAssignment& obj )
{
	vdbStreamHeader h( "RCA", 1 );
	os << h;

	os << obj._sColumnName;
	os << obj._sFontSpecDescription;

	return os;
}


//-------------------------------------
std::istream& operator>> ( std::istream& is, ReportColumnAssignment& obj )
{
	vdbStreamHeader h( "RCA", 1 );
	is >> h;
	if ( is.fail() )
		return is;

	is >> obj._sColumnName;
	is >> obj._sFontSpecDescription;

	return is;
}

