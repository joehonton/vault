//=============================================================================
//
// CLASS:           ReportLookupColumn
//
// OVERVIEW:		The ReportLookupColumn class is a ReportLookupColumn-derived class
//                  with specializations for Lookup columns.
//
// FILE NAME:       ReportLookupColumn.cpp
//
// INITIAL DATE:    11/13/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportLookupColumn.h"
#include "ReportDefinition.h"
#include <fstream>
#include <strstream>
#include "SimpleDatabase.h"


//=============================================================================
// essential class functions
//=============================================================================

//-------------------------------------
ReportLookupColumn::ReportLookupColumn()
	: ReportColumn()
{
	// Be sure to call Prepare()
}


//-------------------------------------
ReportLookupColumn::ReportLookupColumn( ReportDefinition* pParent, const char* szColumnName, const char* szDisplayName, int displaySize, const char* szFormatString )
	: ReportColumn( pParent, szColumnName, "char[]", 7, szDisplayName, displaySize, szFormatString )
{
	Prepare();
}


//-------------------------------------
//^^ Destructor
ReportLookupColumn::~ReportLookupColumn()
{
}


//-------------------------------------
//^^ Copy constructor
ReportLookupColumn::ReportLookupColumn( const ReportLookupColumn& rhs )
	: ReportColumn( (ReportColumn&) rhs )
{
	_sLookupType = rhs._sLookupType;
}


//-------------------------------------
//^^ Assignment operator
ReportLookupColumn& ReportLookupColumn::operator=( const ReportLookupColumn& rhs )
{
	if ( this == &rhs )
		return *this;
	
	ReportColumn::operator=( rhs );

	_sLookupType = rhs._sLookupType;

	return *this;
}


//-------------------------------------
//^ The BestGuess function is called to guess the best display name and size
//
void ReportLookupColumn::BestGuess( vdbString sColumnName, vdbString& sDisplayName, int& displaySize, vdbString& sFormatString )
{
	sDisplayName = InterpretCapitalization( sColumnName );
	displaySize = max( 40, sDisplayName.Length() );
	sFormatString.Format( "%%%is", displaySize );
}


//-------------------------------------
//^ The Prepare function is called to perform special processing for lookups
void ReportLookupColumn::Prepare()
{
	if ( _sColumnName == "BatchSizeUnits" )
		_sLookupType = "BSU";
	else if ( _sColumnName == "Conditions" )
		_sLookupType = "C";
	else if ( _sColumnName == "Diluent" )
		_sLookupType = "D";
	else if ( _sColumnName == "Classification" )
		_sLookupType = "E";
	else if ( _sColumnName == "KnownPolymorphs" )
		_sLookupType = "KP";
	else if ( _sColumnName == "LightSensitivity" )
		_sLookupType = "LS";
	else if ( _sColumnName == "MeasurementDiluent" )
		_sLookupType = "MD";
	else if ( _sColumnName == "Media" )
		_sLookupType = "M";
	else if ( _sColumnName == "MediaSizeUnits" )
		_sLookupType = "MSU";
	else if ( _sColumnName == "ParticleSizer" )
		_sLookupType = "PS";
	else if ( _sColumnName == "RouteOfAdministration" )
		_sLookupType = "R";
	else if ( _sColumnName == "SaltOrFreeBased" )
		_sLookupType = "S";
	else if ( _sColumnName == "SlurryLoadUnits" )
		_sLookupType = "SLU";
	else if ( _sColumnName == "TemperatureSensitivity" )
		_sLookupType = "TS";
	else if ( _sColumnName == "TherapeuticArea" )
		_sLookupType = "TA";
	else if ( _sColumnName == "TypeOfMill" )
		_sLookupType = "TM";
	else
		_sLookupType = "?";
}


//-------------------------------------
//^ The OutputValue function is called by ReportDefinition::Execute() to print
//  the given value.
//  
bool ReportLookupColumn::OutputValue( const char* szValue, int styleIndex )
{
	std::ostrstream os;
	os << "SELECT Description";
	os << " FROM " << (const char*) _pParent->_sOwner << "." << "cdbLookupCode";
	os << " WHERE type = '" << (const char*) _sLookupType << "' AND ";
	os << " code = '" << (const char*) szValue << "'";
	os << std::ends;
	char* sql = os.str();

	SimpleDatabase* pDB = 0;
	if ( _hDB == -1 )
	{
		// establish a second connection to the database
		// if it hasn't already been established.
		pDB = new SimpleDatabase;
		if ( pDB == 0 )
			throw vdbMemoryException();
		
		// place the database instance under Pool management
		_hDB = vdbPool::Instance()->AdoptDatabase( pDB );

		// obtain the datasource name, ID and passowrd of the default connection
		// and use to establish this connection
		SimpleDatabase* pDefaultDB = (SimpleDatabase*) vdbPool::Instance()->GetDatabase();
		if ( pDB->Connect( pDefaultDB->DataSource(), pDefaultDB->AuthenticationID(), pDefaultDB->AuthenticationPassword() ) != SQL_SUCCESS )
		{
			vdbMessageBox( NULL, "Unable to establish auxillary connection to the database.  Shut down any other applications using the database, or log-off any inactive users.", "Report Execution", MB_OK );
			return false;
		}
	}
	else
		pDB = (SimpleDatabase*) vdbPool::Instance()->GetDatabase( _hDB );


	vdbStatement stmt( pDB );																
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;

	vdbString s;
	if ( rc == SQL_SUCCESS )
	{
		char sz[MAX_COLUMN_SIZE];
		stmt.Bind( 1, sz, sizeof(sz) );

		rc = stmt.Fetch();																
		if ( rc == SQL_SUCCESS )													
			s = sz;
		else
			s = szValue;
	}

	vdbString sFormatted;
	sFormatted.Format( _sFormatString, (const char*) s );

	PadWithSpaces( sFormatted );
	_pParent->Output( sFormatted, styleIndex, _widthInPixels );
	return true;
}


//=============================================================================
// I/O stream functions
//=============================================================================

//-------------------------------------
std::ostream& operator<< ( std::ostream& os, ReportLookupColumn& obj )
{
	os << dynamic_cast<ReportColumn&>( obj );

	os << obj._sLookupType;

	return os;
}


//-------------------------------------
std::istream& operator>> ( std::istream& is, ReportLookupColumn& obj )
{
	if ( is.fail() )
		return is;

	is >> dynamic_cast<ReportColumn&>( obj );

	is >> obj._sLookupType;

	return is;
}
