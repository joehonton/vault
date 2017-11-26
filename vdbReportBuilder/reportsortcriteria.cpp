//=============================================================================
//
// CLASS:           ReportSortCriteria
//
// OVERVIEW:		The ReportSortCriteria class holds the attributes of one
//                  sorting criteria.  This class is used in conjunction with
//                  the ReportDefinition class
//
// FILE NAME:       ReportSortCriteria.cpp
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportSortCriteria.h"
#include <fstream>


//=============================================================================
// essential class functions
//=============================================================================

//-------------------------------------
ReportSortCriteria::ReportSortCriteria()
	: _sOrder( "ASCENDING" )
{
}


//-------------------------------------
ReportSortCriteria::ReportSortCriteria( const char* szColumnName, const char* szOrder )
	: _sColumnName( szColumnName ),
	  _sOrder( szOrder )
{
}


//=============================================================================
// I/O stream functions
//=============================================================================

//-------------------------------------
std::ostream& operator<< ( std::ostream& os, const ReportSortCriteria& obj )
{
	os << obj._sColumnName;
	os << obj._sOrder;

	return os;
}


//-------------------------------------
std::istream& operator>> ( std::istream& is, ReportSortCriteria& obj )
{
	if ( is.fail() )
		return is;

	is >> obj._sColumnName;
	is >> obj._sOrder;

	return is;
}
