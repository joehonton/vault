//=============================================================================
//
// CLASS:           ReportNormalColumn
//
// OVERVIEW:		The ReportNormalColumn class is a ReportNormalColumn-derived class
//                  with no specializations.
//
// FILE NAME:       ReportNormalColumn.cpp
//
// INITIAL DATE:    11/13/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportNormalColumn.h"
#include "ReportDefinition.h"
#include <fstream>
#include <strstream>


//=============================================================================
// essential class functions
//=============================================================================

//-------------------------------------
ReportNormalColumn::ReportNormalColumn()
	: ReportColumn()
{
	// Be sure to call Prepare()
}


//-------------------------------------
ReportNormalColumn::ReportNormalColumn( ReportDefinition* pParent, const char* szColumnName, const char* szColumnType, int dataSize, const char* szDisplayName, int displaySize, const char* szFormatString )
	: ReportColumn( pParent, szColumnName, szColumnType, dataSize, szDisplayName, displaySize, szFormatString )
{
	Prepare();
}


//-------------------------------------
//^^ Destructor
ReportNormalColumn::~ReportNormalColumn()
{
}


//-------------------------------------
//^^ Copy constructor
ReportNormalColumn::ReportNormalColumn( const ReportNormalColumn& rhs )
	: ReportColumn( (ReportColumn&) rhs )
{
}


//-------------------------------------
//^^ Assignment operator
ReportNormalColumn& ReportNormalColumn::operator=( const ReportNormalColumn& rhs )
{
	if ( this == &rhs )
		return *this;
	
	ReportColumn::operator=( rhs );

	return *this;
}


//-------------------------------------
//^ The BestGuess function is called to guess the best display name and size
//
void ReportNormalColumn::BestGuess( vdbString sColumnName, vdbString sVaultType, int dataSize, vdbString& sDisplayName, int& displaySize, vdbString& sFormatString )
{
	sDisplayName = InterpretCapitalization( sColumnName );

	if ( sVaultType == "DBINT" || sVaultType == "OID" )  // remember that the actual "oid" is considered a "normal" column
	{
		displaySize = max( 6, sDisplayName.Length() );
		sFormatString = "%d";
	}
	else if ( sVaultType == "DBFLOAT" || sVaultType == "DBDOUBLE" )
	{
		displaySize = max( 6, sDisplayName.Length() );
		sFormatString = "%6.2f";
	}
	else if ( sVaultType == "DBTIMESTAMP" )
	{
		displaySize = max( 17, sDisplayName.Length() );
		sFormatString = "%m/%d/%y  %I:%M %p";
	}
	else // ( sVaultType == "char[]" )
	{
		displaySize = max( dataSize, sDisplayName.Length() );
		sFormatString.Format( "%%%ds", displaySize );
	}
}


//-------------------------------------
//^ The Prepare function
void ReportNormalColumn::Prepare()
{
}


//-------------------------------------
//^ The OutputValue function is called by ReportDefinition::Execute() to print
//  the given value.
//  
bool ReportNormalColumn::OutputValue( const char* szValue, int styleIndex )
{
	if ( _sColumnType == "OID" || _sColumnType == "DBINT" )
	{
		int iValue = atoi( szValue );
		vdbString sFormatted;
		sFormatted.Format( _sFormatString, iValue );

		PadWithSpaces( sFormatted );
		_pParent->Output( sFormatted, styleIndex, _widthInPixels );
	}
	
	else if ( _sColumnType == "DBDOUBLE" || _sColumnType == "DBFORMAT" )
	{
		double dValue = atof( szValue );
		vdbString sFormatted;
		sFormatted.Format( _sFormatString, dValue );

		PadWithSpaces( sFormatted );
		_pParent->Output( sFormatted, styleIndex, _widthInPixels );
	}
	
	else if ( _sColumnType == "DBTIMESTAMP" )
	{
		vdbTimeStamp t;
		t.SetFromDatabaseString( szValue ); 
		
		char szFormatted[80];
		t.Format( szFormatted, 80, _sFormatString );

		vdbString s( szFormatted );
		PadWithSpaces( s );
		_pParent->Output( s, styleIndex, _widthInPixels );
	}

	else // ( _sColumnType == "char[]" )
	{
		vdbString sFormatted;
		sFormatted.Format( _sFormatString, szValue );

		PadWithSpaces( sFormatted );
		_pParent->Output( sFormatted, styleIndex, _widthInPixels );
	}

	return true;
}


//=============================================================================
// I/O stream functions
//=============================================================================

//-------------------------------------
std::ostream& operator<< ( std::ostream& os, ReportNormalColumn& obj )
{
	os << dynamic_cast<ReportColumn&>( obj );

	return os;
}


//-------------------------------------
std::istream& operator>> ( std::istream& is, ReportNormalColumn& obj )
{
	if ( is.fail() )
		return is;

	is >> dynamic_cast<ReportColumn&>( obj );

	return is;
}
