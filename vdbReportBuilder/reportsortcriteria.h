//=============================================================================
//
// CLASS:           ReportSortCriteria
//
// OVERVIEW:		The ReportSortCriteria class holds the attributes of one
//                  sorting criteria.  This class is used in conjunction with
//                  the ReportDefinition class
//
// FILE NAME:       ReportSortCriteria.h
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#ifndef __ReportSortCriteria_h
#define __ReportSortCriteria_h

#ifndef __vdbString_h
	#include <vdbString.h>
#endif

#include <ostream>
#include <istream>


//=============================================================================
class ReportSortCriteria
{
public:
	// essential class functions
	ReportSortCriteria();
	ReportSortCriteria( const char* szColumnName, const char* szOrder = "ASCENDING" );

public:
	vdbString _sColumnName;
	vdbString _sOrder;			// ASCENDING or DESCENDING

	friend std::ostream& operator<< ( std::ostream& os, const ReportSortCriteria& obj );
	friend std::istream& operator>> ( std::istream& is, ReportSortCriteria& obj );
};

#endif  // ReportSortCriteria_h
