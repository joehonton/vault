//=============================================================================
//
// CLASS:           vdbBinaryStreamHeader
//
// OVERVIEW:		The vdbBinaryStreamHeader class implements io stream header and
//                  version sanity checks on files opened in binary mode
//					First developed for keyWordMap
//
// FILE NAME:       vdbBinaryStreamHeader.cpp
//
// INITIAL DATE:    03/05/2006 
//
// COPYRIGHT:		Copyright © 2006 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbBinaryStreamHeader.h"
#include <vdbMessageBox.h>
#include <vdbString.h>				 
#include "vdbUtility.h"

#include <istream>
#include <ostream>
#include <strstream>


//=============================================================================
// Essential class functions
//=============================================================================

//------------------------------------------
//^^ vdbBinaryStreamHeader constructor
//> szHeader is a character string which is used to identify this file type.
//> version is a number which is used to identify the version of this file type.
//
vdbBinaryStreamHeader::vdbBinaryStreamHeader( const TCHAR* szHeader, int version )
	: vdbStreamHeader( szHeader, version )
{
}


//------------------------------------------
//^^ vdbBinaryStreamHeader constructor
//> szHeader is a character string which is used to identify this file type.
//> lowestVersion is the lowest version of this file that is acceptable
//> highestVersion is the highest version of this file that is acceptable
//
vdbBinaryStreamHeader::vdbBinaryStreamHeader( const TCHAR* szHeader, int lowestVersion, int highestVersion )
	: vdbStreamHeader( szHeader, lowestVersion, highestVersion )
{
}


//------------------------------------------
//^^ vdbBinaryStreamHeader destructor
//
vdbBinaryStreamHeader::~vdbBinaryStreamHeader()
{
}


//=============================================================================
// non-member stream functions
//=============================================================================

//------------------------------------------
//^^ ostream << operator
//^  This function writes the header and version to the output stream
//
std::ostream& operator<< ( std::ostream& os, const vdbBinaryStreamHeader& obj )
{
	vdbString s( obj._pszHeader );
	s.write( os, 1 );
	os.write( (const char*) &obj._highestVersion, sizeof(obj._highestVersion) );
	return os;
}


//------------------------------------------
//^^ istream >> operator
//^ This function reads and verifies the header and version from the input stream.
//  If they do not match the user-supplied values, the user is alerted with a
//  message box.  If the user chooses not to continue, this function will flag
//  the input stream as bad.
//^ The message can be suppressed by calling QuenchMessages()
//
#ifndef UNICODE
	std::istream& operator>> ( std::istream& is, vdbBinaryStreamHeader& obj )
	{
//		is.ipfx();
//		if ( is.fail() )
//			return is;

		vdbString s;
		s.read( is, 1 );
		vdbUtility::SafeCopy( obj._szActualHeader, s, sizeof(obj._szActualHeader) );
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

		is.read( (char*) &obj._actualVersion, sizeof(obj._actualVersion) );
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


