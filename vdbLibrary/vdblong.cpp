//=============================================================================
//
// FILE NAME:       vdbLong.cpp
//
// ESSENCE:         long for use in iostreams
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    2/4/96
//
// COPYRIGHT:       Copyright © 1996 by Joe Honton
//
//=============================================================================

#include <istream>
#include <ostream>
#include "vdbLong.h"

//=============================================================================
//  Constructor and destructor
//=============================================================================

//------------------------------------------
//^^ default constructor
vdbLong::vdbLong()
	: _l(0)
{
}


//------------------------------------------
//^^ long constructor
vdbLong::vdbLong( long l )
	: _l(l)
{
}


//------------------------------------------
//^^ Assignment operator 
const vdbLong& vdbLong::operator= ( const long l )
{
	_l = l;
	return *this;
}


//------------------------------------------
//^^ long cast operator
vdbLong::operator long() const
{
	return _l;
}


//------------------------------------------
//^^ ostream << operator
std::ostream& operator<< ( std::ostream& os, const vdbLong& l )
{
	os << '\0';
	os << l._l;
	os << '\0';
	return os;
}


//------------------------------------------
//^^ istream >> operator
//
std::istream& operator>> ( std::istream& is, vdbLong& l )
{
	char separator;

	is.ipfx();
	if ( is.fail() )
		return is;

	is >> separator;
	is >> l._l;
	is >> separator;

	is.isfx();
	return is;
}


