//=============================================================================
//
// CLASS:           ReportStyleDefinition
//
// OVERVIEW:		The ReportStyleDefinition class holds the styles and 
//                  column assignments for a report.
//
// FILE NAME:       ReportStyleDefinition.h
//
// INITIAL DATE:    01/15/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __ReportStyleDefinition_h
#define __ReportStyleDefinition_h

#ifndef __vdbSet_h
	#include <vdbSet.h>
#endif

#ifndef __vdbString_h
	#include <vdbString.h>
#endif

#ifndef __vdbFilename_h
	#include <vdbFilename.h>
#endif

class ReportFontSpecs;

class ostream;
class istream;

//=============================================================================
class ReportStyleDefinition
{
public:
	// essential class functions
	ReportStyleDefinition();
	~ReportStyleDefinition();
	ReportStyleDefinition( const ReportStyleDefinition& rhs );				// copy constructor
	ReportStyleDefinition& operator=( const ReportStyleDefinition& rhs );	// assignment operator

public:
	vdbString _sStyleDescription;
	vdbFilename _sFilename;							// the name of the file where this definition is stored
	vdbSet<ReportFontSpecs> _fontSpecs;				// definitions for font, size, color

	friend std::ostream& operator<< ( std::ostream& os, const ReportStyleDefinition& obj );
	friend std::istream& operator>> ( std::istream& is, ReportStyleDefinition& obj );
};

#endif  // ReportStyleDefinition_h
