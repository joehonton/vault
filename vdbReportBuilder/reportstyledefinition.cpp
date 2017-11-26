//=============================================================================
//
// CLASS:           ReportStyleDefinition
//
// OVERVIEW:		The ReportStyleDefinition class holds the styles and 
//                  column assignments for a report.
//
// FILE NAME:       ReportStyleDefinition.cpp
//
// INITIAL DATE:    01/15/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportStyleDefinition.h"
#include "ReportFontSpecs.h"
#include <fstream>
#include <vdbStreamHeader.h>


//=============================================================================
// ReportStyleDefinition essential class functions
//=============================================================================

//-------------------------------------
ReportStyleDefinition::ReportStyleDefinition()
{
}


//-------------------------------------
ReportStyleDefinition::~ReportStyleDefinition()
{
}


//------------------------------------------
//^^ ReportStyleDefinition copy constructor
//
ReportStyleDefinition::ReportStyleDefinition( const ReportStyleDefinition& rhs )
	: _sStyleDescription( rhs._sStyleDescription ),
	  _fontSpecs( rhs._fontSpecs )
{
}


//------------------------------------------
//^^ ReportStyleDefinition assignment operator
//
ReportStyleDefinition& ReportStyleDefinition::operator=( const ReportStyleDefinition& rhs )
{
	if ( this == &rhs )
		return *this;

	_sStyleDescription = rhs._sStyleDescription;
	_fontSpecs = rhs._fontSpecs;

	return *this;
}


//=============================================================================
// I/O stream functions
//=============================================================================

//-------------------------------------
std::ostream& operator<< ( std::ostream& os, const ReportStyleDefinition& obj )
{
	vdbStreamHeader h( "ReportStyleDefinition", 1 );
	os << h;

	os << obj._sStyleDescription;
	os << obj._fontSpecs;

	return os;
}


//-------------------------------------
std::istream& operator>> ( std::istream& is, ReportStyleDefinition& obj )
{
	vdbStreamHeader h( "ReportStyleDefinition", 1 );
	h.QuenchMessages();
	is >> h;
	if ( strcmp( "ReportStyleDefinition", h.GetActualHeader() ) ||
	     h.GetActualVersion() != 1 )
	{
		is.clear( std::ios_base::failbit );	// set the failbit
		return is;
	}

	is >> obj._sStyleDescription;
	is >> obj._fontSpecs;

	return is;
}

