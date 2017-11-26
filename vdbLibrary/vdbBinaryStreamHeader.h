//=============================================================================
//
// CLASS:           vdbBinaryStreamHeader
//
// OVERVIEW:		The vdbBinaryStreamHeader class implements io stream header and
//                  version sanity checks on files opened in binary mode
//					First developed for keyWordMap
//
// FILE NAME:       vdbBinaryStreamHeader.h
//
// INITIAL DATE:    03/05/2006 
//
// COPYRIGHT:		Copyright © 2006 by Joe Honton
//
//=============================================================================

#ifndef __vdbBinaryStreamHeader_h
#define __vdbBinaryStreamHeader_h

#ifndef __vdbStreamHeader_h
	#include <vdbStreamHeader.h>
#endif

//=============================================================================
class vdbBinaryStreamHeader : public vdbStreamHeader
{
public:
	// essential class functions
	vdbBinaryStreamHeader( const TCHAR* szHeader, int version );		// constructor
	vdbBinaryStreamHeader( const TCHAR* szHeader, int lowestVersion, int highestVersion );	// constructor
	~vdbBinaryStreamHeader();											// destructor

public:	// Stream operators for getting and putting header
	friend std::ostream& operator<< ( std::ostream& os, const vdbBinaryStreamHeader& obj );
	friend std::istream& operator>> ( std::istream& is, vdbBinaryStreamHeader& obj );
};

#endif  // vdbBinaryStreamHeader_h

