//=============================================================================
//
// CLASS:           ReportOIDColumn
//
// OVERVIEW:		The ReportOIDColumn class is a ReportOIDColumn-derived class
//                  with specializations for OID columns.
//
// FILE NAME:       ReportOIDColumn.cpp
//
// INITIAL DATE:    11/13/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "ReportOIDColumn.h"
#include "ReportDefinition.h"
#include <fstream>
#include <strstream>
#include "SimpleDatabase.h"


//=============================================================================
// essential class functions
//=============================================================================

//-------------------------------------
ReportOIDColumn::ReportOIDColumn()
	: ReportColumn()
{
	// Be sure to call Prepare()
}


//-------------------------------------
ReportOIDColumn::ReportOIDColumn( ReportDefinition* pParent, const char* szColumnName, const char* szDisplayName, int displaySize, const char* szFormatString )
	: ReportColumn( pParent, szColumnName, "OID", 4, szDisplayName, displaySize, szFormatString )
{
	Prepare();
}


//-------------------------------------
//^^ Destructor
ReportOIDColumn::~ReportOIDColumn()
{
}


//-------------------------------------
//^^ Copy constructor
ReportOIDColumn::ReportOIDColumn( const ReportOIDColumn& rhs )
	: ReportColumn( (ReportColumn&) rhs )
{
	_sForeignTable = rhs._sForeignTable;
	_sForeignColumn = rhs._sForeignColumn;
}


//-------------------------------------
//^^ Assignment operator
ReportOIDColumn& ReportOIDColumn::operator=( const ReportOIDColumn& rhs )
{
	if ( this == &rhs )
		return *this;
	
	ReportColumn::operator=( rhs );

	_sForeignTable = rhs._sForeignTable;
	_sForeignColumn = rhs._sForeignColumn;

	return *this;
}


//-------------------------------------
//^ The BestGuess function is called to guess the best display name and size
//
void ReportOIDColumn::BestGuess( vdbString sColumnName, vdbString& sDisplayName, int& displaySize, vdbString& sFormatString )
{
	if ( sColumnName == "LookupCodeID" )
	{
		sDisplayName = "Acceptable Routes";
		displaySize = 40;
	} 
	else if ( sColumnName == "FeasibilityStudyID" )
	{
		sDisplayName = "Feasibility Study";
		displaySize = 17;
	} 
	else if ( sColumnName == "FormulationID" )
	{
		sDisplayName = "Formulation";
		displaySize = 20;
	} 
	else if ( sColumnName == "DrugID" )
	{
		sDisplayName = "Drug";
		displaySize = 80;
	} 
	else if ( sColumnName == "StabilizerID" || sColumnName == "StabilizerID1" || sColumnName == "StabilizerID2" || sColumnName == "StabilizerID3" )
	{
		sDisplayName = "Stabilizer";
		displaySize = 80;
	} 
	else if ( sColumnName == "ExcipientID1" || sColumnName == "ExcipientID2" )
	{
		sDisplayName = "Excipient";
		displaySize = 80;
	} 
	else if ( sColumnName == "ExperimentID" )
	{
		sDisplayName = "Notebook Number";
		displaySize = 15;
	} 
	// if this is an unknown type of OID, just treat it as a normal column
	else
	{
		sDisplayName = InterpretCapitalization( sColumnName );
		displaySize = max( 8, sDisplayName.Length() );
	}

	sFormatString.Format( "%%%ds", displaySize );
}


//-------------------------------------
//^ The Prepare function is called to perform special processing with OID's
void ReportOIDColumn::Prepare()
{
	if ( _sColumnName == "LookupCodeID" )
	{
		_sForeignTable = "cdbLookupCode";
		_sForeignColumn = "Description";
	} 
	else if ( _sColumnName == "FeasibilityStudyID" )
	{
		_sForeignTable = "cdbFeasibilityStudy";
		_sForeignColumn = "ProjectNumber";
	} 
	else if ( _sColumnName == "FormulationID" )
	{
		_sForeignTable = "cdbFormulation";
		_sForeignColumn = "Description";
	} 
	else if ( _sColumnName == "DrugID" )
	{
		_sForeignTable = "cdbDrug";
		_sForeignColumn = "Name";
	} 
	else if ( _sColumnName == "StabilizerID" || _sColumnName == "StabilizerID1" || _sColumnName == "StabilizerID2" || _sColumnName == "StabilizerID3" )
	{
		_sForeignTable = "cdbStabilizer";
		_sForeignColumn = "Description";
	} 
	else if ( _sColumnName == "ExcipientID1" || _sColumnName == "ExcipientID2" )
	{
		_sForeignTable = "cdbExcipient";
		_sForeignColumn = "Description";
	} 
	else if ( _sColumnName == "ExperimentID" )
	{
		_sForeignTable = "cdbExperiment";
		_sForeignColumn = "NotebookNumber";
	} 
	// if this is an unknown type of OID, just treat it as a normal column
	else
	{
		_sForeignTable = "unknown";
		_sForeignColumn = "unknown";
	}
}


//-------------------------------------
//^ The OutputValue function is called by ReportDefinition::Execute() to print
//  the given value.  This function is overridden by ReportOIDColumn and
//  ReportLookupColumn to perform custom output.
//  
bool ReportOIDColumn::OutputValue( const char* szValue, int styleIndex )
{
	// if this OID is unknown, just print out its value
	if ( _sForeignColumn == "unknown" || _sForeignTable == "unknown" )
	{
		vdbString s( szValue );
		PadWithSpaces( s );
		_pParent->Output( s, styleIndex );
		return true;
	}

	std::ostrstream os;
	os << "SELECT " << _sForeignColumn;
	os << " FROM " << (const char*) _pParent->_sOwner << "." << _sForeignTable;
	os << " WHERE oid = " << szValue;
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
			s = "<unspecified>";
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
std::ostream& operator<< ( std::ostream& os, ReportOIDColumn& obj )
{
	os << dynamic_cast<ReportColumn&>( obj );

	os << obj._sForeignTable;
	os << obj._sForeignColumn;

	return os;
}


//-------------------------------------
std::istream& operator>> ( std::istream& is, ReportOIDColumn& obj )
{
	if ( is.fail() )
		return is;

	is >> dynamic_cast<ReportColumn&>( obj );

	is >> obj._sForeignTable;
	is >> obj._sForeignColumn;

	return is;
}
