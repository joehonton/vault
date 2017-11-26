//=============================================================================
//
// CLASS:           ReportNormalColumn
//
// OVERVIEW:		The ReportNormalColumn class is a ReportColumn-derived class
//                  with no specializations
//
// FILE NAME:       ReportNormalColumn.h
//
// INITIAL DATE:    11/13/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#ifndef __ReportNormalColumn_h
#define __ReportNormalColumn_h

#ifndef __ReportColumn_h
	#include "ReportColumn.h"
#endif

class ostream;
class istream;
class ReportDefinition;


//=============================================================================
class ReportNormalColumn : public ReportColumn
{
public:
	// essential class functions
	ReportNormalColumn();
	ReportNormalColumn( ReportDefinition* pParent, const char* szColumnName, const char* szColumnType, int dataSize, const char* szDisplayName, int displaySize, const char* szFormatString );
	virtual ~ReportNormalColumn();
	ReportNormalColumn( const ReportNormalColumn& rhs );				// copy constructor 
	ReportNormalColumn& operator=( const ReportNormalColumn& rhs );		// assignment operator 

public:
	static void BestGuess( vdbString sColumnName, vdbString sVaultType, int dataSize, vdbString& sDisplayName, int& displaySize, vdbString& sFormatString );
	virtual void Prepare();
	virtual bool OutputValue( const char* szValue, int styleIndex );

public:
	friend ostream& operator<< ( ostream& os, ReportNormalColumn& obj );
	friend istream& operator>> ( istream& is, ReportNormalColumn& obj );
};

#endif  // ReportNormalColumn_h
