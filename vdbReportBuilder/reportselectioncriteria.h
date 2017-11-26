//=============================================================================
//
// CLASS:           ReportSelectionCriteria
//
// OVERVIEW:		The ReportSelectionCriteria class holds the attributes of one
//                  selection criteria.  This class is used in conjunction with
//                  the ReportDefinition class
//
// FILE NAME:       ReportSelectionCriteria.h
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#ifndef __ReportSelectionCriteria_h
#define __ReportSelectionCriteria_h

#ifndef __vdbString_h
	#include <vdbString.h>
#endif

#include <ostream>
#include <istream>


//=============================================================================
class ReportSelectionCriteria
{
public:
	// essential class functions
	ReportSelectionCriteria();
	ReportSelectionCriteria( const char* szColumnName, const char* szValue, const char* szOperator = "=", const char* szConnector = "AND" );

public:
	vdbString _sColumnName;
	vdbString _sOperator;
	vdbString _sValue;
	vdbString _sConnector;		// AND or OR

	friend std::ostream& operator<< ( std::ostream& os, const ReportSelectionCriteria& obj );
	friend std::istream& operator>> ( std::istream& is, ReportSelectionCriteria& obj );
};

#endif  // ReportSelectionCriteria_h
