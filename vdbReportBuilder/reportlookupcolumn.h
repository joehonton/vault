//=============================================================================
//
// CLASS:           ReportLookupColumn
//
// OVERVIEW:		The ReportLookupColumn class is a ReportColumn-derived class
//                  with specializations for Lookup columns.
//
// FILE NAME:       ReportLookupColumn.h
//
// INITIAL DATE:    11/13/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#ifndef __ReportLookupColumn_h
#define __ReportLookupColumn_h

#ifndef __ReportColumn_h
	#include "ReportColumn.h"
#endif

#ifndef __vdbString_h
	#include <vdbString.h>
#endif

class ostream;
class istream;
class ReportDefinition;


//=============================================================================
class ReportLookupColumn : public ReportColumn
{
public:
	// essential class functions
	ReportLookupColumn();
	ReportLookupColumn( ReportDefinition* pParent, const char* szColumnName, const char* szDisplayName, int displaySize, const char* szFormatString );
	virtual ~ReportLookupColumn();
	ReportLookupColumn( const ReportLookupColumn& rhs );				// copy constructor 
	ReportLookupColumn& operator=( const ReportLookupColumn& rhs );		// assignment operator 

public:
	static void BestGuess( vdbString sColumnName, vdbString& sDisplayName, int& displaySize, vdbString& sFormatString );
	virtual void Prepare();
	virtual bool OutputValue( const char* szValue, int styleIndex );

public:
	vdbString _sLookupType;

	friend ostream& operator<< ( ostream& os, ReportLookupColumn& obj );
	friend istream& operator>> ( istream& is, ReportLookupColumn& obj );
};

#endif  // ReportLookupColumn_h
