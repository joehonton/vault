//=============================================================================
//
// CLASS:           ReportColumn
//
// OVERVIEW:		The ReportColumn class holds the attributes of one report
//                  column.  This class is used in conjunction with
//                  the ReportDefinition class
//
// FILE NAME:       ReportColumn.h
//
// INITIAL DATE:    10/26/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#ifndef __ReportColumn_h
#define __ReportColumn_h

#ifndef __vdbTypedef_h
	#include <vdbTypedef.h>
#endif

#ifndef __vdbString_h
	#include <vdbString.h>
#endif

class ostream;
class istream;
class ReportDefinition;

const int MAX_COLUMN_SIZE = 256;

//=============================================================================
class ReportColumn
{
public:
	// essential class functions
	ReportColumn();
	ReportColumn( ReportDefinition* pParent, const char* szColumnName, const char* szColumnType, int dataSize, const char* szDisplayName, int displaySize, const char* szFormatString );
	virtual ~ReportColumn();
	ReportColumn( const ReportColumn& rhs );				// copy constructor 
	ReportColumn& operator=( const ReportColumn& rhs );		// assignment operator 

public:
	static vdbString& InterpretCapitalization( vdbString& sColumnName );
	virtual void Prepare();
	virtual void OutputHeader( int styleIndex );
	virtual void OutputLineBreak( int styleIndex );
	virtual bool OutputValue( const char* szValue, int styleIndex );
	void DetermineWidthInPixels( HDC hdc, HFONT hFont1, HFONT hFont2, HFONT hFont3 );

protected:
	void PadWithSpaces( vdbString& s );

public:
	ReportDefinition* _pParent;
	vdbString _sColumnName;
	vdbString _sColumnType;
	vdbString _sDisplayName;				// the name to display at the top of this column
	vdbString _sFormatString;
	int _dataSize;							// size of the SQL column (number of bytes)
	int _displaySize;						// number of characters to display
	int _widthInPixels;						// pixel width determined for an HDC and a ReportFontSpec (only used when sending output to an HDC)
	int _IOStreamVersion;					// set by ReportDefinition

public:
	static HANDLE_DATABASE _hDB;			// database connection used for column lookups

	friend std::ostream& operator<< ( std::ostream& os, const ReportColumn& obj );
	friend std::istream& operator>> ( std::istream& is, ReportColumn& obj );
};

#endif  // ReportColumn_h
