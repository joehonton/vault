//=============================================================================
//
// CLASS:           ReportOIDColumn
//
// OVERVIEW:		The ReportOIDColumn class is a ReportColumn-derived class
//                  with specializations for OID columns.
//
// FILE NAME:       ReportOIDColumn.h
//
// INITIAL DATE:    11/13/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#ifndef __ReportOIDColumn_h
#define __ReportOIDColumn_h

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
class ReportOIDColumn : public ReportColumn
{
public:
	// essential class functions
	ReportOIDColumn();
	ReportOIDColumn( ReportDefinition* pParent, const char* szColumnName, const char* szDisplayName, int displaySize, const char* szFormatString );
	virtual ~ReportOIDColumn();
	ReportOIDColumn( const ReportOIDColumn& rhs );				// copy constructor 
	ReportOIDColumn& operator=( const ReportOIDColumn& rhs );	// assignment operator 

public:
	static void BestGuess( vdbString sColumnName, vdbString& sDisplayName, int& displaySize, vdbString& sFormatString );
	virtual void Prepare();
	virtual bool OutputValue( const char* szValue, int styleIndex );

public:
	vdbString _sForeignTable;
	vdbString _sForeignColumn;

	friend ostream& operator<< ( ostream& os, ReportOIDColumn& obj );
	friend istream& operator>> ( istream& is, ReportOIDColumn& obj );
};

#endif  // ReportOIDColumn_h
