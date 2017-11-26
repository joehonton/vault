//=============================================================================
//
// CLASS:           vdbStreamHeader
//
// OVERVIEW:		The vdbStreamHeader class implements io stream header and
//                  version sanity checks on files opened in text mode
//
// FILE NAME:       vdbStreamHeader.h
//
// INITIAL DATE:    11/17/96 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __vdbStreamHeader_h
#define __vdbStreamHeader_h

#ifndef _INC_TCHAR
	#include <tchar.h>
#endif

#include <istream>
#include <ostream>

//=============================================================================
class vdbStreamHeader
{
public:
	// essential class functions
	vdbStreamHeader( const TCHAR* szHeader, int version );		// constructor
	vdbStreamHeader( const TCHAR* szHeader, int lowestVersion, int highestVersion );	// constructor
	~vdbStreamHeader();											// destructor

public:
	static inline void QuenchMessages();
	static inline void DisplayMessages();
	inline int GetActualVersion();
	inline const TCHAR* GetActualHeader();

private:
	vdbStreamHeader( const vdbStreamHeader& rhs );				// copy constructor disabled
	vdbStreamHeader& operator=( const vdbStreamHeader& rhs );	// assignment operator disabled

protected:
	TCHAR* _pszHeader;
	TCHAR _szActualHeader[80];
	int _lowestVersion;
	int _highestVersion;
	int _actualVersion;
	static bool _bQuenchMessages;

	// Stream operators for getting and putting header
	friend std::ostream& operator<< ( std::ostream& os, const vdbStreamHeader& obj );
	friend std::istream& operator>> ( std::istream& is, vdbStreamHeader& obj );
};

//=============================================================================
// inlines
//=============================================================================

//-------------------------------------
inline void vdbStreamHeader::QuenchMessages()
{
	_bQuenchMessages = true;
}


//-------------------------------------
inline void vdbStreamHeader::DisplayMessages()
{
	_bQuenchMessages = false;
}


//-------------------------------------
inline int vdbStreamHeader::GetActualVersion()
{
	return _actualVersion;
}


//-------------------------------------
inline const TCHAR* vdbStreamHeader::GetActualHeader()
{
	return _szActualHeader;
}


#endif  // vdbStreamHeader_h

