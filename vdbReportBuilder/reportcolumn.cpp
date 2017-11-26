//=============================================================================
//
// CLASS:           ReportColumn
//
// OVERVIEW:		The ReportColumn class holds the attributes of one report
//                  column.  This class is used in conjunction with
//                  the ReportDefinition class
//
// FILE NAME:       ReportColumn.cpp
//
// INITIAL DATE:    10/26/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportColumn.h"
#include "ReportDefinition.h"
#include <fstream>
#include <strstream>


// database connection used for column lookups
HANDLE_DATABASE ReportColumn::_hDB = -1;


//=============================================================================
// essential class functions
//=============================================================================

//-------------------------------------
ReportColumn::ReportColumn()
	: _pParent( 0 ),
	  _sColumnName( "" ),
	  _sColumnType( "" ),
	  _sDisplayName( "" ),
	  _sFormatString( "" ),
	  _dataSize( 0 ),
	  _displaySize( 0 ),
	  _widthInPixels( 0 ),
	  _IOStreamVersion( 0 )
{
}


//-------------------------------------
ReportColumn::ReportColumn( ReportDefinition* pParent, const char* szColumnName, const char* szColumnType, int dataSize, const char* szDisplayName, int displaySize, const char* szFormatString )
	: _pParent( pParent ),
	  _sColumnName( szColumnName ),
	  _sColumnType( szColumnType ),
	  _sDisplayName( szDisplayName ),
	  _sFormatString( szFormatString ),
	  _dataSize( dataSize ),
	  _displaySize( displaySize ),
	  _widthInPixels( 0 ),
	  _IOStreamVersion( 0 )
{
}


//-------------------------------------
//^^ Destructor
ReportColumn::~ReportColumn()
{
}


//-------------------------------------
//^^ Copy constructor
ReportColumn::ReportColumn( const ReportColumn& rhs )
{
	_pParent = rhs._pParent;
	_sColumnName = rhs._sColumnName;
	_sColumnType = rhs._sColumnType;
	_sDisplayName = rhs._sDisplayName;
	_sFormatString = rhs._sFormatString;
	_dataSize = rhs._dataSize;
	_displaySize = rhs._displaySize;
	_widthInPixels = rhs._widthInPixels;
	_IOStreamVersion = rhs._IOStreamVersion;
}


//-------------------------------------
//^^ Assignment operator
ReportColumn& ReportColumn::operator=( const ReportColumn& rhs )
{
	if ( this == &rhs )
		return *this;
	
	_pParent = rhs._pParent;
	_sColumnName = rhs._sColumnName;
	_sColumnType = rhs._sColumnType;
	_sDisplayName = rhs._sDisplayName;
	_sFormatString = rhs._sFormatString;
	_dataSize = rhs._dataSize;
	_displaySize = rhs._displaySize;
	_widthInPixels = rhs._widthInPixels;
	_IOStreamVersion = rhs._IOStreamVersion;

	return *this;
}


//-------------------------------------
//^ The InterpretCapitalization function takes a DBMS column name and adds spaces
//  before each capitalized letter.
//^ If the column name is all capitals, no interpretation is done
//< Returns a reference to a static string containing the new value
//
vdbString& ReportColumn::InterpretCapitalization( vdbString& sColumnName )
{
	static vdbString sInterpreted;

	// if all capitals just return
	bool bAllCapitals = true;
	int len = sColumnName.Length();
	if ( len == 0 )
	{
		sInterpreted = "";
		return sInterpreted;
	}

	for ( int i = 0; i < len; i++ )
	{
		if ( isalpha( sColumnName[i] ) )
		{
			if ( islower( sColumnName[i] ) )
			{
				bAllCapitals = false;
				break;
			}
		}
	}

	if ( bAllCapitals == true )
	{
		sInterpreted = sColumnName;
		return sInterpreted;
	}

	// allocate a character array twice the size of the column name
	// because the number of spaces added cant't be more than that
	char* sz = new char[len*2];
	if ( sz == 0 )
		throw vdbMemoryException();

	// always copy the first letter and capitalize it
	sz[0] = toupper( sColumnName[0] );
	int mostRecentCapital = 0;

	// copy letter by letter to the interpreted string
	int j = 1;
	for ( int i = 1; i < len; i++ )
	{
		char c = sColumnName[i];
		if ( isalpha( c ) )
		{
			if ( isupper( c ) )
			{
				// Add a space before each capital unless the previous letter was also
				// capital.  Consecutive runs of capitals are not interpreted.
				if ( mostRecentCapital != i - 1 )
				{
					sz[j] = ' ';
					j++;
				}
				mostRecentCapital = i;
			}
		}
		
		// copy the letter itself
		sz[j] = c;
		j++;
	}

	sz[j] = 0x0;
	sInterpreted = sz;
	delete[] sz; sz = 0;

	return sInterpreted;
}


//-------------------------------------
//^ The DetermineWidthInPixels function is called by ReportDefinition::Execute()
//  to determine the width in pixels of this column by checking the width of the
//  column name using both supplied fonts.
//> hdc is the device context to be used during Output
//> hFont1 is the font for the the column header
//> hFont2 is the font for the report style 1
//> hFont3 is the font for the report style 2
//
void ReportColumn::DetermineWidthInPixels( HDC hdc, HFONT hFont1, HFONT hFont2, HFONT hFont3 )
{
	// determine the width of the column name using the first font
	SIZE size;
	SelectObject( hdc, hFont1 );
	GetTextExtentPoint32( hdc, (const char*) _sDisplayName, _sDisplayName.Length(), &size );
	int width1 = size.cx;

	// determine the maximum width of a typical data field using the second font
	SelectObject( hdc, hFont2 );
	GetTextExtentPoint32( hdc, (const char*) "mn", 2, &size );
	int width2 = (size.cx * _displaySize) / 2;

	// determine the maximum width of a typical data field using the third font
	SelectObject( hdc, hFont3 );
	GetTextExtentPoint32( hdc, (const char*) "mn", 2, &size );
	int width3 = (size.cx * _displaySize) / 2;

	_widthInPixels = max( width1, max(width2, width3) );
}


//-------------------------------------
//^ The OutputHeader function is called by ReportDefinition::Execute() to print
//  the column header
void ReportColumn::OutputHeader( int styleIndex )
{
	vdbString s = _sDisplayName;

	PadWithSpaces( s );
	_pParent->Output( s, styleIndex, _widthInPixels );
}


//-------------------------------------
//^ The OutputLineBreak function is called by ReportDefinition::Execute() to print
//  dashes under the column header.
void ReportColumn::OutputLineBreak( int styleIndex )
{
	int size = _displaySize;
	if ( size < 0 )
		size = 0;

	char *pszDashes = new char[size +1];
	for ( int i = 0; i < size; i++ )
		pszDashes[i] = '-';

	pszDashes[size] = 0x0;				

	_pParent->Output( pszDashes, styleIndex, _widthInPixels );
	delete[] pszDashes; pszDashes = 0;
}


//-------------------------------------
//^ The PadWithSpaces function fills the given string with trailing spaces
//  so that the final string is exactly the width of _displaySize
//> s is both the input and output
//
void ReportColumn::PadWithSpaces( vdbString& s )
{
	// do not pad if exporting
	if ( _pParent->IsExport() )
		return;

	int size = _displaySize;
	if ( size < 0 )
		size = 0;

	char* sz = new char[size + 1];
	
	int length = s.Length();
	if ( length > size )
		length = size;

	for ( int i = 0; i < length; i++ )
		sz[i] = s[i];

	for ( int i = length; i < size; i++ )
		sz[i] = ' ';

	sz[size] = 0x0;
	
	// replace the input string with the padded string
	s = sz;

	delete[] sz; sz = 0;
}


//-------------------------------------
void ReportColumn::Prepare()
{
}


//-------------------------------------
bool ReportColumn::OutputValue( const char* szValue, int styleIndex  )
{
	return true;
}


//=============================================================================
// I/O stream functions
//=============================================================================

//-------------------------------------
std::ostream& operator<< ( std::ostream& os, const ReportColumn& obj )
{
	char sep = ',';

	os << obj._sColumnName;
	os << obj._sColumnType;
	os << obj._sDisplayName;
	os << obj._sFormatString;
	os << obj._dataSize << sep;
	os << obj._displaySize << std::endl;

	return os;
}


//-------------------------------------
std::istream& operator>> ( std::istream& is, ReportColumn& obj )
{
	char sep;

	if ( is.fail() )
		return is;

	is >> obj._sColumnName;
	is >> obj._sColumnType;
	is >> obj._sDisplayName;
		
	if ( obj._IOStreamVersion > 1 )		// set by ReportDefinition
		is >> obj._sFormatString;

	is >> obj._dataSize >> sep;
	is >> obj._displaySize >> std::ws;


	return is;
}
