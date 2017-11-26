//=============================================================================
//
// CLASS:           vdbTypedef
//
// OVERVIEW:		iostreams for database types
//
// FILE NAME:       vdbTypedef.cpp
//
// INITIAL DATE:    11/27/96
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#include "vdbTypedef.h"
#include <ostream>
#include <istream>


//=========================================================================
// Database types IO stream
//=========================================================================

//------------------------------------------
//^^ ostream << OID
std::ostream& operator<< ( std::ostream& os, const OID& item )
{
	os.write( (const char*) &item, sizeof(OID) );
	return os;
}


//------------------------------------------
//^^ istream >> OID
std::istream& operator>> ( std::istream& is, OID& item )
{
	if ( is.fail() )
		return is;

	is.read( (char *) &item, sizeof(OID) );

	is.isfx();
	return is;
}


//------------------------------------------
//^^ ostream << DBSMALL
std::ostream& operator<< ( std::ostream& os, const DBSMALL& item )
{
	os.write( (const char*) &item, sizeof(DBSMALL) );
	return os;
}


//------------------------------------------
//^^ istream >> DBSMALL
std::istream& operator>> ( std::istream& is, DBSMALL& item )
{
	if ( is.fail() )
		return is;

	is.read( (char *) &item, sizeof(DBSMALL) );

	is.isfx();
	return is;
}


//------------------------------------------
//^^ ostream << DBINT
std::ostream& operator<< ( std::ostream& os, const DBINT& item )
{
	os.write( (const char*) &item, sizeof(DBINT) );
	return os;
}


//------------------------------------------
//^^ istream >> DBINT
std::istream& operator>> ( std::istream& is, DBINT& item )
{
	if ( is.fail() )
		return is;

	is.read( (char *) &item, sizeof(DBINT) );

	is.isfx();
	return is;
}


//------------------------------------------
//^^ ostream << DBFLOAT
std::ostream& operator<< ( std::ostream& os, const DBFLOAT& item )
{
	os.write( (const char*) &item, sizeof(DBFLOAT) );
	return os;
}


//------------------------------------------
//^^ istream >> DBFLOAT
std::istream& operator>> ( std::istream& is, DBFLOAT& item )
{
	if ( is.fail() )
		return is;

	is.read( (char *) &item, sizeof(DBFLOAT) );

	is.isfx();
	return is;
}


//------------------------------------------
//^^ ostream << DBDOUBLE
std::ostream& operator<< ( std::ostream& os, const DBDOUBLE& item )
{
	os.write( (const char*) &item, sizeof(DBDOUBLE) );
	return os;
}


//------------------------------------------
//^^ istream >> DBDOUBLE
std::istream& operator>> ( std::istream& is, DBDOUBLE& item )
{
	if ( is.fail() )
		return is;

	is.read( (char *) &item, sizeof(DBDOUBLE) );

	is.isfx();
	return is;
}


//------------------------------------------
//^^ ostream << DBDATE
std::ostream& operator<< ( std::ostream& os, const DBDATE& item )
{
	const char sep = ',';

	os.write( (const char*) &item.year, sizeof(SQLSMALLINT) );
	os << sep;
	os.write( (const char*) &item.month, sizeof(SQLUSMALLINT) );
	os << sep;
	os.write( (const char*) &item.day, sizeof(SQLUSMALLINT) );
	return os;
}


//------------------------------------------
//^^ istream >> DBDATE
std::istream& operator>> ( std::istream& is, DBDATE& item )
{
	char sep;

	if ( is.fail() )
		return is;

	is.read( (char*) &item.year, sizeof(SQLSMALLINT) );
	is >> sep;
	is.read( (char*) &item.month, sizeof(SQLUSMALLINT) );
	is >> sep;
	is.read( (char*) &item.day, sizeof(SQLUSMALLINT) );

	is.isfx();
	return is;
}


//------------------------------------------
//^^ ostream << DBTIME
std::ostream& operator<< ( std::ostream& os, const DBTIME& item )
{
	const char sep = ',';

	os.write( (const char*) &item.hour, sizeof(SQLUSMALLINT) );
	os << sep;
	os.write( (const char*) &item.minute, sizeof(SQLUSMALLINT) );
	os << sep;
	os.write( (const char*) &item.second, sizeof(SQLUSMALLINT) );
	return os;
}


//------------------------------------------
//^^ istream >> DBTIME
std::istream& operator>> ( std::istream& is, DBTIME& item )
{
	char sep;

	if ( is.fail() )
		return is;

	is.read( (char*) &item.hour, sizeof(SQLUSMALLINT) );
	is >> sep;
	is.read( (char*) &item.minute, sizeof(SQLUSMALLINT) );
	is >> sep;
	is.read( (char*) &item.second, sizeof(SQLUSMALLINT) );

	is.isfx();
	return is;
}


//------------------------------------------
//^^ ostream << DBTIMESTAMP
std::ostream& operator<< ( std::ostream& os, const DBTIMESTAMP& item )
{
	const char sep = ',';

	os.write( (const char*) &item.year, sizeof(SQLSMALLINT) );
	os << sep;
	os.write( (const char*) &item.month, sizeof(SQLUSMALLINT) );
	os << sep;
	os.write( (const char*) &item.day, sizeof(SQLUSMALLINT) );
	os << sep;
	os.write( (const char*) &item.hour, sizeof(SQLUSMALLINT) );
	os << sep;
	os.write( (const char*) &item.minute, sizeof(SQLUSMALLINT) );
	os << sep;
	os.write( (const char*) &item.second, sizeof(SQLUSMALLINT) );
	os << sep;
	os.write( (const char*) &item.fraction, sizeof(SQLUINTEGER) );
	return os;
}


//------------------------------------------
//^^ istream >> DBTIMESTAMP
std::istream& operator>> ( std::istream& is, DBTIMESTAMP& item )
{
	char sep;

	if ( is.fail() )
		return is;

	is.read( (char*) &item.year, sizeof(SQLSMALLINT) );
	is >> sep;
	is.read( (char*) &item.month, sizeof(SQLUSMALLINT) );
	is >> sep;
	is.read( (char*) &item.day, sizeof(SQLUSMALLINT) );
	is >> sep;
	is.read( (char*) &item.hour, sizeof(SQLUSMALLINT) );
	is >> sep;
	is.read( (char*) &item.minute, sizeof(SQLUSMALLINT) );
	is >> sep;
	is.read( (char*) &item.second, sizeof(SQLUSMALLINT) );
	is >> sep;
	is.read( (char*) &item.fraction, sizeof(SQLUINTEGER) );

	is.isfx();
	return is;
}
