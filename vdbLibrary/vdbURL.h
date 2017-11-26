//=============================================================================
//
// FILE NAME:       vdbURL.h
//
// ESSENCE:         String class with semantic information about Uniform
//                  Resource Locators
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    10/15/01
//
// COPYRIGHT:       Copyright © 2001 by Joe Honton
//
//=============================================================================

#ifndef __vdbURL_h
#define __vdbURL_h

#ifndef __vdbString_h
	#include "vdbString.h"
#endif

#ifndef _UNICODE
	#ifndef _IOSTREAM_
		#include <iostream>
	#endif
#endif

class vdbTimeStamp;

class vdbURL : public vdbString
{
public:
	// constructors
	vdbURL();
	vdbURL( const TCHAR* sz );
	vdbURL( const vdbString& s );
	vdbURL( const vdbURL& rhs );

	// assignment
	const vdbURL& operator= ( const vdbURL& rhs );
	const vdbURL& operator= ( const vdbString& rhs );
	const vdbURL& operator= ( const TCHAR* sz );
	operator const TCHAR*() const;

	// modification	functions
	void ReplaceMissingProtocol( const TCHAR* szProtocol );
	void AppendReference( const TCHAR* szHyperlinkReference );
	void Canonicalize();

	// index position functions	
	bool Protocol( int& startPos, int& endPos ) const;
	bool Domain( int& startPos, int& endPos ) const;
	bool Port( int& startPos, int& endPos ) const;
	bool DomainAndPort( int& startPos, int& endPos ) const;
	bool Directory( int& startPos, int& endPos ) const;
	bool Filename( int& startPos, int& endPos ) const;
	bool Extension( int& startPos, int& endPos ) const;
	bool DirectoryAndFilename( int& startPos, int& endPos ) const;
	bool FilenameAndExtension( int& startPos, int& endPos ) const;
	bool CGI( int& startPos, int& endPos ) const;
	bool Bookmark( int& startPos, int& endPos ) const;
	bool Javascript( int& startPos, int& endPos ) const;

	// access functions
	const TCHAR* Protocol() const;
	const TCHAR* Domain() const;
	const TCHAR* Port() const;
	const TCHAR* DomainAndPort() const;
	const TCHAR* Directory() const;
	const TCHAR* Filename() const;
	const TCHAR* Extension() const;
	const TCHAR* DirectoryAndFilename() const;
	const TCHAR* FilenameAndExtension() const;
	const TCHAR* CGI() const;
	const TCHAR* Bookmark() const;
	const TCHAR* Javascript() const;

	// status functions
	bool HasProtocol() const;

	// network functions
	bool AbleToReach() const;
	DWORD ResourceSize() const;
	bool Capture( const char* szFilename, unsigned long& maxBytes ) const;

	// Stream operators
	#ifndef _UNICODE
		friend std::ostream& operator<< ( std::ostream& os, const vdbURL& s );
		friend std::istream& operator>> ( std::istream& is, vdbURL& s );
	#endif
};

#endif // __vdbURL_h

