//=============================================================================
//
// CLASS:           vdbStreamHeader
//
// OVERVIEW:		The vdbStreamHeader class implements io stream header and
//                  version sanity checks on files opened in text mode
//
// FILE NAME:       vdbStreamHeader.cpp
//
// INITIAL DATE:    11/17/96 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbStreamHeader.h"
#include "vdbAssert.h"
#include <vdbMessageBox.h>
#include <string.h>				 
#include "vdbUtility.h"
#include "vdbException.h"

#ifndef UNICODE
	#include <istream>
	#include <ostream>
	#include <strstream>
#endif


//-------------------------------------
bool vdbStreamHeader::_bQuenchMessages = false;


//=============================================================================
// Essential class functions
//=============================================================================

//------------------------------------------
//^^ vdbStreamHeader constructor
//> szHeader is a character string which is used to identify this file type.
//> version is a number which is used to identify the version of this file type.
//
vdbStreamHeader::vdbStreamHeader( const TCHAR* szHeader, int version )
	: _pszHeader( 0 ),
	  _lowestVersion( version ),
	  _highestVersion( version ),
	  _actualVersion( 0 )
{
	assert( szHeader != 0 );
	int len = _tcslen(szHeader) + 1;
	_pszHeader = new TCHAR[len];
	if ( _pszHeader == 0 ) vdbThrowMemoryException();
	vdbUtility::SafeCopy( _pszHeader, szHeader, len );
	_szActualHeader[0] = 0;
}


//------------------------------------------
//^^ vdbStreamHeader constructor
//> szHeader is a character string which is used to identify this file type.
//> lowestVersion is the lowest version of this file that is acceptable
//> highestVersion is the highest version of this file that is acceptable
//
vdbStreamHeader::vdbStreamHeader( const TCHAR* szHeader, int lowestVersion, int highestVersion )
	: _pszHeader( 0 ),
	  _lowestVersion( lowestVersion ),
	  _highestVersion( highestVersion ),
	  _actualVersion( 0 )
{
	assert( szHeader != 0 );
	int len = _tcslen(szHeader) + 1;
	_pszHeader = new TCHAR[len];
	if ( _pszHeader == 0 ) vdbThrowMemoryException();
	vdbUtility::SafeCopy( _pszHeader, szHeader, len );
	_szActualHeader[0] = 0;
}


//------------------------------------------
//^^ vdbStreamHeader destructor
//
vdbStreamHeader::~vdbStreamHeader()
{
	delete[] _pszHeader; _pszHeader = 0;
}


//=============================================================================
// non-member stream functions
//=============================================================================

//------------------------------------------
//^^ ostream << operator
//^  This function writes the header and version to the output stream
//
#ifndef UNICODE
	std::ostream& operator<< ( std::ostream& os, const vdbStreamHeader& obj )
	{
		os << obj._pszHeader << ',' << obj._highestVersion << std::endl;
		return os;
	}
#endif


//------------------------------------------
//^^ istream >> operator
//^ This function reads and verifies the header and version from the input stream.
//  If they do not match the user-supplied values, the user is alerted with a
//  message box.  If the user chooses not to continue, this function will flag
//  the input stream as bad.
//^ The message can be suppressed by calling QuenchMessages()
//
#ifndef UNICODE
	std::istream& operator>> ( std::istream& is, vdbStreamHeader& obj )
	{
		is.ipfx();
		if ( is.fail() )
			return is;

		is.getline( obj._szActualHeader, sizeof(obj._szActualHeader) + 1, ',' );
		if ( strcmp( obj._szActualHeader, obj._pszHeader ) != 0 )
			if ( obj._bQuenchMessages == false )
			{
				std::ostrstream os;
				os << "This file should begin with the keyword '" << obj._pszHeader;
				os << "' but instead begins with '" << obj._szActualHeader << "'." << std::endl << std::endl;
				os << "(Note that this message may also result from an invalid section keyword.)" << std::endl << std::endl;
				os << "Continuing is probably not a good idea." << std::endl << std::endl;
				os << "Continue anyway?" << std::ends;
				TCHAR* s = os.str();
				if ( vdbMessageBox( NULL, s, "", MB_YESNO ) == IDNO )
				{
					delete[] s; s = 0;
					is.setf( std::ios_base::badbit );
					return is;
				}
				else
				{
					delete[] s; s = 0;
				}
			}

		is >> obj._actualVersion >> std::ws;
		if ( obj._actualVersion < obj._lowestVersion || obj._actualVersion > obj._highestVersion )
			if ( obj._bQuenchMessages == false )
			{
				std::ostrstream os;
				os << "This file has version number " << obj._actualVersion;
				os << " but the program was expecting a version between " << obj._lowestVersion;
				os << " and " << obj._highestVersion << "." << std::endl << std::endl;
				os << "Continuing is probably not a good idea." << std::endl << std::endl;
				os << "Continue anyway?" << std::ends;
				TCHAR* s = os.str();
				if ( vdbMessageBox( NULL, s, "", MB_YESNO ) == IDNO )
				{
					delete[] s; s = 0;
					is.setf( std::ios_base::badbit );
					return is;
				}
				else
				{
					delete[] s; s = 0;
				}
			}

		return is;
	}
#endif


