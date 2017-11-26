//=============================================================================
//
// CLASS:           ReportSelectionCriteria
//
// OVERVIEW:		The ReportSelectionCriteria class holds the attributes of one
//                  selection criteria.  This class is used in conjunction with
//                  the ReportDefinition class
//
// FILE NAME:       ReportSelectionCriteria.cpp
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportSelectionCriteria.h"
#include <fstream>


//=============================================================================
// essential class functions
//=============================================================================

//-------------------------------------
ReportSelectionCriteria::ReportSelectionCriteria()
	: _sOperator( "=" ),
	  _sConnector( "AND" )
{
}


//-------------------------------------
ReportSelectionCriteria::ReportSelectionCriteria( const char* szColumnName, const char* szValue, const char* szOperator, const char* szConnector )
	: _sColumnName( szColumnName ),
	  _sValue( szValue ),
	  _sOperator( szOperator ),
	  _sConnector( szConnector )
{
}


//=============================================================================
// I/O stream functions
//=============================================================================

//-------------------------------------
std::ostream& operator<< ( std::ostream& os, const ReportSelectionCriteria& obj )
{
	os << obj._sColumnName;
	os << obj._sValue;
	os << obj._sOperator;
	os << obj._sConnector;

	return os;
}


//-------------------------------------
std::istream& operator>> ( std::istream& is, ReportSelectionCriteria& obj )
{
	if ( is.fail() )
		return is;

	is >> obj._sColumnName;
	is >> obj._sValue;
	is >> obj._sOperator;
	is >> obj._sConnector;

	return is;
}
