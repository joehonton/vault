//=============================================================================
//
// FILE NAME:       vdbURL.cpp
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

#include "vdbWindows.h"
#include "vdbURL.h"
#include <stdio.h>
#include "vdbUtility.h"
#include <stdlib.h>
#include <wininet.h>
#include <vdbFilename.h>

// warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
#pragma warning( disable : 4800 )

//warning C4996: 'fopen': This function or variable may be unsafe. Consider using fopen_s instead.
#pragma warning( disable : 4996 )

//=============================================================================
// Constructors
//=============================================================================

//-------------------------------------
//^^ default constructor
vdbURL::vdbURL()
	: vdbString()
{
}


//-------------------------------------
//^^ constructor taking character string
vdbURL::vdbURL( const TCHAR* sz )
	: vdbString( sz )
{
	StripLeadingWhitespace();
	StripTrailingWhitespace();
}
	

//-------------------------------------
//^^ constructor taking vdbString
vdbURL::vdbURL( const vdbString& s )
	: vdbString( s )
{
	StripLeadingWhitespace();
	StripTrailingWhitespace();
}


//-------------------------------------
//^^ copy constructor
vdbURL::vdbURL( const vdbURL& rhs )
	: vdbString( (const vdbString&) rhs )
{
}


//=============================================================================
// Assignment
//=============================================================================

//-------------------------------------
//^^ assignment from vdbURL
const vdbURL& vdbURL::operator= ( const vdbURL& rhs )
{
	vdbString::operator=( (const vdbString&) rhs );
	return *this;
}


//-------------------------------------
//^^ assignment from vdbString
const vdbURL& vdbURL::operator= ( const vdbString& rhs )
{
	vdbString::operator=( rhs );
	return *this;
}


//-------------------------------------
//^^ assignment from const TCHAR*
const vdbURL& vdbURL::operator= ( const TCHAR* sz )
{
	vdbString::operator=( sz );
	return *this;
}


//-------------------------------------
//^^ const TCHAR* cast operator
vdbURL::operator const TCHAR*() const
{
	return vdbString::operator const TCHAR*();
}


//=============================================================================
// Modification	functions
//=============================================================================

//-------------------------------------
//^ The ReplaceMissingProtocol function ensures that the current URL has a protocol
//  specifier at the beginning.  If it does not, the passed value is added to
//  the beginning of the string.
//> szProtocol is something like "http"
//
void vdbURL::ReplaceMissingProtocol( const TCHAR* szProtocol )
{
	if ( HasProtocol() )
		return;

	vdbString sTemp = *this;
	this->Format( "%s://%s", szProtocol, (const TCHAR*) sTemp );
}


//-------------------------------------
//^ The AppendReference function appends the hyperlink reference to this URL.
//  NOTE: Any trailing filename, extension, CGI script, or Bookmark of this
//        URL is lost.
//> szHyperlinkReference can be something like
//   "NewFile.ext"
//   "../../NewFile.ext"
//   "newDir"
//   "/newDir"
//   "newDir/"
//   "filename.ext#Bookmark"
//   "filename.ext?CGI"
//   "filename.ext/javascript:"
//   "./"
//
void vdbURL::AppendReference( const TCHAR* szHyperlinkReference )
{
	// first remove any leading/trailing spaces from the hyperlink
	vdbURL sHyper( szHyperlinkReference );
	sHyper.StripLeadingSpaces();
	sHyper.StripTrailingSpaces();

	if ( sHyper.Length() > 0 )
	{
		// replace any leading slashes in the hyperlink
		if ( sHyper[0] == '/' || sHyper[0] == '\\' )
			sHyper = sHyper.Middle(1);

		int lastChar = sHyper.Length() - 1;
		if ( lastChar >= 0 )
		{
			// next replace any trailing slashes in the hyperlink
			if ( sHyper[lastChar] == '/' || sHyper[lastChar] == '\\' )
				sHyper = sHyper.First(lastChar);

			// special check for "this directory"
			if ( sHyper == "." )
				sHyper = "";
		}
	}

	// remove Bookmarks, CGI Scripts, and Javascripts from the hyperlink
	int startPos;
	int endPos;
	if ( sHyper.Bookmark( startPos, endPos ) == true )
		sHyper = sHyper.First( startPos - 1 );

	if ( sHyper.CGI( startPos, endPos ) == true )
		sHyper = sHyper.First( startPos - 1 );

	if ( sHyper.Javascript( startPos, endPos ) == true )
		sHyper = sHyper.First( startPos );


	// now move upward for each ..\ or ../ found at the beginning of the string
	vdbString sThisURLDirectory = Directory();
	int dotdot = sHyper.Find( ".." );

	while ( dotdot == 0 )
	{
		// remove the last subdirectory of the current URL
		int lastSlash = sThisURLDirectory.ReverseFind( '\\' );
		if ( lastSlash == -1 )
			lastSlash = sThisURLDirectory.ReverseFind( '/' );

		if ( lastSlash == -1 )
			sThisURLDirectory = "";
		else
			sThisURLDirectory = sThisURLDirectory.First( lastSlash );

		// remove the ..\ or ../ from the hyperlink reference
		sHyper = sHyper.Middle( 3 );

		// look for next one
		dotdot = sHyper.Find( ".." );
	}

	// Append to this URL.
	// Any Port, Filename, Extension, CGI Script, Bookmark, or Javascript
	// part of "this" is intentionally lost here
	vdbURL sTempURL;
	
	if ( sHyper.IsNull() )
	{
		if ( sThisURLDirectory.IsNull() )
			sTempURL.Format( "%s://%s", Protocol(), Domain() );
		else
			sTempURL.Format( "%s://%s/%s", Protocol(), Domain(), (const TCHAR*) sThisURLDirectory );
	}
	else
	{
		if ( sThisURLDirectory.IsNull() )
			sTempURL.Format( "%s://%s/%s", Protocol(), Domain(), (const TCHAR*) sHyper );
		else
			sTempURL.Format( "%s://%s/%s/%s", Protocol(), Domain(), (const TCHAR*) sThisURLDirectory, (const TCHAR*) sHyper );
	}

	// replace
	*this = sTempURL;
}


//-------------------------------------
//^ The Canonicalize function
void vdbURL::Canonicalize()
{
}


//=============================================================================
// Index position functions
//=============================================================================

//-------------------------------------
// These index position functions search from both ends of the string like this:
//   Protocol searches from the beginning
//   Domain searches to the right of Protocol
//   Port searches to the right of Domain
//   Directory searches to the right of Domain & Port and to the left of Filename
//   Filename searches to the left of Extension
//   Extension searches to the left of Bookmark or CGI or the end of string
//   Bookmark searches to the left of the end of string
//   CGI searches to the left of the end of string
//   Javascript searches to the left of the end of string
//
//   "?" and "#" cannot be embedded the protocol, domain, directory, filename, or extension
//   "." cannot be embedded in the directory
//   if the filename does not have a "." it is included as part of the final portion
//      the directory; it is not recognized as a separate entity.

//-------------------------------------
//^ The Protocol function finds the starting and ending position of the protocol
//  Something like "http" without the colon.
//< returns true if the starting and ending values are both valid
//< returns false if one of the values could not be determined.  The undetermined
//  value is set to -1.
//
bool vdbURL::Protocol( int& startPos, int& endPos ) const
{
	startPos = 0;
	endPos = Find( ':' );

	if ( endPos == NULL )
		return false;
	
	// do not include the colon	
	endPos--;
		
	if ( startPos >= endPos )
		return false;
	else
		return true;
}


//-------------------------------------
//^ The Domain function finds the starting and ending position of the domain
//  Requires that the current URL contain a protocol at the beginning.
//  Something like "www.mycompany.com" with no trailing backslash.
//< returns true if the starting and ending values are both valid
//< returns false if one of the values could not be determined.  The undetermined
//  value is set to -1.
//
bool vdbURL::Domain( int& startPos, int& endPos ) const
{
	startPos = -1;
	endPos = -1;

	int startProtocol;
	int endProtocol;
	if ( Protocol( startProtocol, endProtocol ) == false )
		return false;

	// find beginning of domain by skipping over the colon and slashes between protocol and domain
	startPos = endProtocol + 2;
	if ( startPos >= Length() - 1 )
		return false;

	char c = this->Get(startPos);
	while ( c == '/' || c == '\\' )
	{
		startPos++;
		if ( startPos >= Length() )
			return false;
		c = this->Get(startPos);
	}

	// find the end of domain as the first slash or a colon or the end of the string
	int slash = FindOneOfNext( "/\\:", startPos );
	if ( slash == -1 )
		endPos = Length() - 1;
	else
		endPos = slash - 1;

	if ( startPos >= endPos )
		return false;
	else
		return true;
}


//-------------------------------------
//^ The Port function finds the starting and ending position of the port number
//  Requires that the current URL contain a protocol and domain at the beginning.
//  Something like "80" or "8080" with no preceding colon.
//< returns true if the starting and ending values are both valid
//< returns false if one of the values could not be determined.  The undetermined
//  value is set to -1.
//
bool vdbURL::Port( int& startPos, int& endPos ) const
{
	startPos = -1;
	endPos = -1;

	int startDomain;
	int endDomain;
	if ( Domain( startDomain, endDomain ) == false )
		return false;

	// Find the beginning of the port by looking for a colon immediately after the Domain
	// If not found, then there is no port number specified.
	startPos = endDomain + 1;
	if ( startPos >= Length() - 1 )
		return false;

	char c = this->Get(startPos);
	if ( c != ':' )
		return false;

	startPos++;
	if ( startPos >= Length() - 1 )
		return false;

	c = this->Get(startPos);
	if ( c == 0x0 )
		return false;

	// find the end of the port as the first slash or the end of the string
	int slash = FindOneOfNext( "/\\", startPos );
	if ( slash == -1 )
		endPos = Length() - 1;
	else
		endPos = slash - 1;

	if ( startPos >= endPos )
		return false;
	else
		return true;
}


//-------------------------------------
//^ The DomainAndPort function finds the starting and ending position of the
//  domain and port together
//  Requires that the current URL contain a protocol at the beginning.
//  Something like "www.mycompany.com:80" with no trailing backslash.
//< returns true if the starting and ending values are both valid
//< returns false if one of the values could not be determined.  The undetermined
//  value is set to -1.
//
bool vdbURL::DomainAndPort( int& startPos, int& endPos ) const
{
	startPos = -1;
	endPos = -1;

	int startProtocol;
	int endProtocol;
	if ( Protocol( startProtocol, endProtocol ) == false )
		return false;

	// find beginning of domain by skipping over the colon and slashes between protocol and domain
	startPos = endProtocol + 2;
	if ( startPos >= Length() - 1 )
		return false;

	char c = this->Get(startPos);
	while ( c == '/' || c == '\\' )
	{
		startPos++;
		if ( startPos >= Length() )
			return false;
		c = this->Get(startPos);
	}

	// find the end of domain as the first slash or the end of the string
	int slash = FindOneOfNext( "/\\", startPos );
	if ( slash == -1 )
		endPos = Length() - 1;
	else
		endPos = slash - 1;

	if ( startPos >= endPos )
		return false;
	else
		return true;
}


//-------------------------------------
//^ The Directory function finds the starting and ending position of the directory
//  The domain and the filename are not part of the directory portion.
//  Requires that the current URL contain a protocol and domain at the beginning
//< Returns something like "mydir/mysubdir"
//< Does not include a preceding or trailing slash.
//< returns true if the starting and ending values are both valid
//< returns false if one of the values could not be determined.  The undetermined
//  value is set to -1.
//
bool vdbURL::Directory( int& startPos, int& endPos ) const
{
	startPos = -1;
	endPos = -1;

	int startDomain;
	int endDomain;
	if ( DomainAndPort( startDomain, endDomain ) == false )
	{
		startPos = 0;
	}
	else
	{
		if ( endDomain >= Length() - 1 )
			return false;

		// find beginning of directory by skipping over slashes between domain and directory
		startPos = endDomain + 1;
		char c = this->Get(startPos);
		while ( c == '/' || c == '\\' )
		{
			startPos++;
			if ( startPos >= Length() )
				return false;
			c = this->Get(startPos);
		}
	}

	// find end of directory by looking backwards from the filename
	int startFilename;
	int endFilename;
	if ( Filename( startFilename, endFilename ) == true )
		endPos = startFilename - 2;
	else // if there is no filename, stop at the CGI Script, Bookmark, Javascript or end of string
	{
		int startExtra;
		int endExtra;
		if ( Bookmark( startExtra, endExtra ) == true )
			endPos = startExtra - 2;
		else if ( CGI( startExtra, endExtra ) == true )
			endPos = startExtra - 2;
		else if ( Javascript( startExtra, endExtra ) == true )
			endPos = startExtra - 2;
		else
			endPos = Length() - 1;

		char c = this->Get(endPos);
		if ( c == '\\' || c == '/' )
			endPos--;
	}

	if ( startPos >= endPos )
		return false;
	else
		return true;
}


//-------------------------------------
bool vdbURL::Filename( int& startPos, int& endPos ) const
{
	startPos = -1;
	endPos = -1;

	// Find the end of the base filename as just to the left of the extension.
	// Filenames without a "." are by definition hereby considered to be part of the directory.
	int startExtra;
	int endExtra;
	if ( Extension( startExtra, endExtra ) == true )
		endPos = startExtra - 2;
	else
		return false;

	// find the beginning of the base filename by searching for the slash separating
	// it from the directory or domain.
	startPos = ReverseFindNext( '/', endPos );
	if ( startPos == -1 )
		startPos = ReverseFindNext( '\\', endPos );
	if ( startPos == -1 )
	{
		// no domain, just a filename
		startPos = 0;
		return true;
	}

	startPos++;
	if ( this->Get(startPos) == 0x0 )
		return false;

	// verify that we've really found the filename and not part of the domain name
	int startDomain;
	int endDomain;
	if ( DomainAndPort( startDomain, endDomain ) == true )
	{
		if ( startPos <= endDomain )
			return false;
	}

	if ( startPos >= endPos )
		return false;
	else
		return true;
}


//-------------------------------------
bool vdbURL::Extension( int& startPos, int& endPos ) const
{
	startPos = -1;
	endPos = -1;

	// find the end of the extension as just to the left of any optional CGI, Bookmark, or Javascript
	int startExtra;
	int endExtra;
	if ( Bookmark( startExtra, endExtra ) == true )
		endPos = startExtra - 2;
	else if ( CGI( startExtra, endExtra ) == true )
		endPos = startExtra - 2;
	else if ( Javascript( startExtra, endExtra ) == true )
		endPos = startExtra - 2;
	else
		endPos = Length() - 1;

	if ( endPos <= 0 )
		return false;

	int dot = ReverseFindNext( '.', endPos );
	if ( dot == -1 )
		return false;
	startPos = dot + 1;

	int slash = ReverseFindNext( '/', endPos );
	if ( slash > dot )
		return false;

	int backSlash = ReverseFindNext( '\\', endPos );
	if ( backSlash > dot )
		return false;

	// verify that we've really found the file extension and not part of the domain name
	int startDomain;
	int endDomain;
	if ( DomainAndPort( startDomain, endDomain ) == true )
	{
		if ( startPos <= endDomain )
			return false;
	}

	if ( startPos >= endPos )
		return false;
	else
		return true;
}


//-------------------------------------
bool vdbURL::DirectoryAndFilename( int& startPos, int& endPos ) const
{
	startPos = -1;
	endPos = -1;

	// find the end of the extension as just to the left of any optional CGI, Bookmark, or Javascript
	int startExtra;
	int endExtra;
	if ( Bookmark( startExtra, endExtra ) == true )
		endPos = startExtra - 2;
	else if ( CGI( startExtra, endExtra ) == true )
		endPos = startExtra - 2;
	else
		endPos = Length() - 1;

	// remove any trailing slash
	char c = this->Get(endPos);
	if ( c == '\\' || c == '/' )
		endPos--;

	if ( endPos <= 0 )
		return false;

	int startDomain;
	int endDomain;
	if ( DomainAndPort( startDomain, endDomain ) == true )
	{
		// find the beginning of the directory by skipping over slashes between domain and directory
		if ( endDomain >= Length() - 1 )
			return false;

		startPos = endDomain + 1;
		c = this->Get(startPos);
		while ( c == '/' || c == '\\' )
		{
			startPos++;
			if ( startPos >= Length() )
				return false;
			c = this->Get(startPos);
		}
	}
	else
		startPos = 0;

	// be sure to exclude any Javascript
	int startJavascript;
	int endJavascript;
	if ( Javascript( startJavascript, endJavascript ) == true )
		endPos = startJavascript - 2;

	if ( startPos >= endPos )
		return false;
	else
		return true;
}


//-------------------------------------
bool vdbURL::FilenameAndExtension( int& startPos, int& endPos ) const
{
	startPos = -1;
	endPos = -1;

	// find the end of the extension as just to the left of any optional CGI, Bookmark, Javascript
	int startExtra;
	int endExtra;
	if ( Bookmark( startExtra, endExtra ) == true )
		endPos = startExtra - 2;
	else if ( CGI( startExtra, endExtra ) == true )
		endPos = startExtra - 2;
	else if ( Javascript( startExtra, endExtra ) == true )
		endPos = startExtra - 2;
	else
		endPos = Length() - 1;

	if ( endPos <= 0 )
		return false;

	// Find the beginning of the base filename by searching for the slash separating
	// it from the directory or domain.
	startPos = ReverseFindNext( '/', endPos );
	if ( startPos == -1 )
		startPos = ReverseFindNext( '\\', endPos );
	if ( startPos == -1 )
	{
		// no domain, just a filename
		startPos = 0;
		return true;
	}

	startPos++;
	if ( startPos >= Length() - 1 )
		return false;
	if ( this->Get(startPos) == 0x0 )
		return false;

	// Ensure that this filename contains a "."
	// Filenames without a "." are by definition hereby considered to be part
	// of the directory.
	int dot = ReverseFindNext( '.', endPos );
	if ( dot == -1 )
		return false;
	if ( dot <= startPos )
		return false;

	// verify that we've really found the filename and not part of the domain name
	int startDomain;
	int endDomain;
	if ( DomainAndPort( startDomain, endDomain ) == true )
	{
		if ( startPos <= endDomain )
			return false;
	}

	if ( startPos >= endPos )
		return false;
	else
		return true;
}


//-------------------------------------
//^ The CGI function finds the starting and ending position of the optional CGI script
//  A URL can have either a Bookmark or a CGI string but not both
//< Returns something like "a=1&b=2&c=3"
//< Does not include the preceding "?"
//< returns true if the starting and ending values are both valid
//< returns false if one of the values could not be determined.  The undetermined
//  value is set to -1.
bool vdbURL::CGI( int& startPos, int& endPos ) const
{
	startPos = -1;
	endPos = -1;

	startPos = Find( '?' );
	if ( startPos == -1 )
		return false;

	startPos++;
	if ( startPos >= Length() )
		return false;

	endPos = Length() - 1;
	if ( startPos >= endPos )
		return false;
	else
		return true;
}


//-------------------------------------
//^ The Bookmark function finds the starting and ending position of the optional bookmark
//  A URL can have either a Bookmark or a CGI string but not both
//< Returns something like "mybookmark"
//< Does not include the preceding #
//< returns true if the starting and ending values are both valid
//< returns false if one of the values could not be determined.  The undetermined
//  value is set to -1.
bool vdbURL::Bookmark( int& startPos, int& endPos ) const
{
	endPos = -1;

	startPos = Find( '#' );
	if ( startPos == -1 )
		return false;

	startPos++;
	if ( startPos >= Length() )
		return false;

	endPos = Length() - 1;
	if ( startPos >= endPos )
		return false;
	else
		return true;
}


//-------------------------------------
//> Whereas CGI and Bookmark return the string without the "?" and "#"
//     Javascript returns the string with the preceding "javascript:"
//     Fixing this will require a minor effort elsewhere
bool vdbURL::Javascript( int& startPos, int& endPos ) const
{
	endPos = -1;

	startPos = Find( "javascript:" );
	if ( startPos == -1 )
		return false;

	endPos = Length() - 1;
	if ( startPos >= endPos )
		return false;
	else
		return true;
}



//=============================================================================
// Access functions
//=============================================================================

//-------------------------------------
//^ The Protocol function returns a pointer to the protocol of this URL.
//  Something like "http:"
//
const TCHAR* vdbURL::Protocol() const
{
	static TCHAR szProtocol[9];

	int startPos;
	int endPos;
	if ( Protocol( startPos, endPos ) == false )
		szProtocol[0] = 0x0;
	else
		vdbUtility::SafeCopy( szProtocol, (const TCHAR*) Middle(startPos, (endPos-startPos)+ 1), TSIZEOF(szProtocol) );

	return szProtocol;
}


//-------------------------------------
//^ The Domain function returns a pointer to the domain of this URL.
//  Requires that the current URL contain a protocol at the beginning.
//< Something like "www.mycompany.com"
//
const TCHAR* vdbURL::Domain() const
{
	static TCHAR szDomain[67 + 1 + 6 + 1];	// 67 chars before the dot + ".museum" + 0x0

	int startPos;
	int endPos;
	if ( Domain( startPos, endPos ) == false )
		szDomain[0] = 0x0;
	else
		vdbUtility::SafeCopy( szDomain, (const TCHAR*) Middle(startPos, (endPos-startPos)+ 1), TSIZEOF(szDomain) );

	return szDomain;
}


//-------------------------------------
//^ The Port function returns a pointer to the optional port number of this URL.
//  Requires that the current URL contain a protocol at the beginning.
//< Something like "80" or "8080"
//
const TCHAR* vdbURL::Port() const
{
	static TCHAR szPort[5];

	int startPos;
	int endPos;
	if ( Port( startPos, endPos ) == false )
		szPort[0] = 0x0;
	else
		vdbUtility::SafeCopy( szPort, (const TCHAR*) Middle(startPos, (endPos-startPos)+ 1), TSIZEOF(szPort) );

	return szPort;
}


//-------------------------------------
//^ The DomainAndPort function returns a pointer to the domain and port number of this URL.
//  Requires that the current URL contain a protocol at the beginning.
//< Something like "www.mycompany.com:80"
//
const TCHAR* vdbURL::DomainAndPort() const
{
	static TCHAR szDomainAndPort[64];

	int startPos;
	int endPos;
	if ( DomainAndPort( startPos, endPos ) == false )
		szDomainAndPort[0] = 0x0;
	else
		vdbUtility::SafeCopy( szDomainAndPort, (const TCHAR*) Middle(startPos, (endPos-startPos)+ 1), TSIZEOF(szDomainAndPort) );

	return szDomainAndPort;
}


//-------------------------------------
//^ The Directory function returns a pointer to the directory portion of this URL.
//  The domain and the filename is not part of the directory portion.
//  Requires that the current URL contain a protocol and domain at the beginning
//< Returns something like "mydir/mysubdir"
//< The output does not include a preceding or trailing slash.
//
const TCHAR* vdbURL::Directory() const
{
	static TCHAR szDirectory[128];

	int startPos;
	int endPos;
	if ( Directory( startPos, endPos ) == false )
		szDirectory[0] = 0x0;
	else
		vdbUtility::SafeCopy( szDirectory, (const TCHAR*) Middle(startPos, (endPos-startPos)+ 1), TSIZEOF(szDirectory) );

	return szDirectory;
}


//-------------------------------------
const TCHAR* vdbURL::Filename() const
{
	static TCHAR szBasename[32];

	int startPos;
	int endPos;
	if ( Filename( startPos, endPos ) == false )
		szBasename[0] = 0x0;
	else
		vdbUtility::SafeCopy( szBasename, (const TCHAR*) Middle(startPos, (endPos-startPos)+ 1), TSIZEOF(szBasename) );

	return szBasename;
}


//-------------------------------------
const TCHAR* vdbURL::Extension() const
{
	static TCHAR szExtension[6];	// up to ".shmtl" in length

	int startPos;
	int endPos;
	if ( Extension( startPos, endPos ) == false )
		szExtension[0] = 0x0;
	else
		vdbUtility::SafeCopy( szExtension, (const TCHAR*) Middle(startPos, (endPos-startPos)+ 1), TSIZEOF(szExtension) );

	return szExtension;
}


//-------------------------------------
const TCHAR* vdbURL::DirectoryAndFilename() const
{
	static TCHAR szDirAndFile[160];

	int startPos;
	int endPos;
	if ( DirectoryAndFilename( startPos, endPos ) == false )
		szDirAndFile[0] = 0x0;
	else
		vdbUtility::SafeCopy( szDirAndFile, (const TCHAR*) Middle(startPos, (endPos-startPos)+ 1), TSIZEOF(szDirAndFile) );

	return szDirAndFile;
}


//-------------------------------------
const TCHAR* vdbURL::FilenameAndExtension() const
{
	static TCHAR szWithExtension[38];

	int startPos;
	int endPos;
	if ( FilenameAndExtension( startPos, endPos ) == false )
		szWithExtension[0] = 0x0;
	else
		vdbUtility::SafeCopy( szWithExtension, (const TCHAR*) Middle(startPos, (endPos-startPos)+ 1), TSIZEOF(szWithExtension) );

	return szWithExtension;
}


//-------------------------------------
const TCHAR* vdbURL::CGI() const
{
	static TCHAR szCGI[256];

	int startPos;
	int endPos;
	if ( CGI( startPos, endPos ) == false )
		szCGI[0] = 0x0;
	else
		vdbUtility::SafeCopy( szCGI, (const TCHAR*) Middle(startPos, (endPos-startPos)+ 1), TSIZEOF(szCGI) );

	return szCGI;
}


//-------------------------------------
const TCHAR* vdbURL::Bookmark() const
{
	static TCHAR szBookmark[32];

	int startPos;
	int endPos;
	if ( Bookmark( startPos, endPos ) == false )
		szBookmark[0] = 0x0;
	else
		vdbUtility::SafeCopy( szBookmark, (const TCHAR*) Middle(startPos, (endPos-startPos)+ 1), TSIZEOF(szBookmark) );

	return szBookmark;
}


//-------------------------------------
const TCHAR* vdbURL::Javascript() const
{
	static TCHAR szJavascript[256];

	int startPos;
	int endPos;
	if ( Javascript( startPos, endPos ) == false )
		szJavascript[0] = 0x0;
	else
		vdbUtility::SafeCopy( szJavascript, (const TCHAR*) Middle(startPos, (endPos-startPos)+ 1), TSIZEOF(szJavascript) );

	return szJavascript;
}


//=============================================================================
// status functions
//=============================================================================

//-------------------------------------
//^ The HasProtocol function returns true if this URL has a valid protocol at the
//  beginning of the string, false if it does not.
bool vdbURL::HasProtocol() const
{
	int colon = Find( ':' );
	if ( colon == -1 )
		return false;

	if ( Compare( "http", colon ) )
		return true;
	else if ( Compare( "https", colon ) )
		return true;
	else if ( Compare( "mailto", colon ) )
		return true;
	else if ( Compare( "ftp", colon ) )
		return true;
	else if ( Compare( "file", colon ) )
		return true;
	else if ( Compare( "news", colon ) )
		return true;
	else if ( Compare( "telnet", colon ) )
		return true;
	else if ( Compare( "gopher", colon ) )
		return true;
	else if ( Compare( "wais", colon ) )
		return true;
	else if ( Compare( "prospero", colon ) )
		return true;
	else if ( Compare( "nntp", colon ) )
		return true;
	else
		return false;
}


//=============================================================================
// network functions
//=============================================================================

//-------------------------------------
//^ The AbleToReach function returns true if this URL can be reached
bool vdbURL::AbleToReach() const
{
	if ( InternetAttemptConnect(0) != ERROR_SUCCESS )
		return false;

	HINTERNET hInternet = InternetOpen( "Harvester", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	if ( hInternet == NULL )
		return false;

	HINTERNET hUrl = InternetOpenUrl( hInternet, (const TCHAR*) *this, NULL, 0, 0, 0 );
	if ( hUrl == NULL )
		return false;

	InternetCloseHandle( hUrl );
	InternetCloseHandle( hInternet );
	return true;
}


//-------------------------------------
//^ The ResourceSize function obtains the size, in bytes, of the resource pointed to
//  by this.
//< return -1 if unknown
//
DWORD vdbURL::ResourceSize() const
{
	if ( InternetAttemptConnect(0) != ERROR_SUCCESS )
		return -1;

	HINTERNET hInternet = InternetOpen( "Harvester", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	if ( hInternet == NULL )
		return -1;

	char szHeaders[] = {"Accept-Language: en\r\n"};
	DWORD lenHeaders = -1;
	HINTERNET hUrl = InternetOpenUrl( hInternet, (const TCHAR*) *this, szHeaders, lenHeaders, 0, 0 );
	if ( hUrl == NULL )
		return -1;

	DWORD dwContentLen;
    DWORD dwBufLen = sizeof(dwContentLen);
	BOOL rc = HttpQueryInfo( hUrl, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&dwContentLen, &dwBufLen, 0);

	InternetCloseHandle( hUrl );
	InternetCloseHandle( hInternet );

	if ( rc == TRUE )
		return dwContentLen;
	else
		return -1;
}


//-------------------------------------
//^ The Capture function returns gets the URL and places it's contents in
//  the given file.  Only English pages.
//> maxBytes is the maximum number of bytes to download before quiting.
//  Note that more bytes may be read due to the internal 1024 byte buffer size.
//  Use 0 to download the entire file.
//< maxBytes returns the number of bytes actually read
//
bool vdbURL::Capture( const char* szFilename, unsigned long& maxBytes ) const
{
	if ( InternetAttemptConnect(0) != ERROR_SUCCESS )
		return false;

	HINTERNET hInternet = InternetOpen( "Harvester", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	if ( hInternet == NULL )
		return false;

	char szHeaders[] = {"Accept-Language: en\r\n"};
	DWORD lenHeaders = -1;
	HINTERNET hUrl = InternetOpenUrl( hInternet, (const TCHAR*) *this, szHeaders, lenHeaders, INTERNET_FLAG_NO_CACHE_WRITE, 0 );
	if ( hUrl == NULL )
	{
		InternetCloseHandle( hInternet );
		return false;
	}

	// If we definately don't have an English page, return false
	char szLanguage[10];
	DWORD bufLength = 10;
	if ( HttpQueryInfo( hUrl, HTTP_QUERY_CONTENT_LANGUAGE, szLanguage, &bufLength, 0 ) == TRUE )
	{
		if ( _strnicmp( szLanguage, "en", 2 ) )
		{
			InternetCloseHandle( hUrl );
			InternetCloseHandle( hInternet );
			return false;
		}
	}

	// get the page
	unsigned long totalBytesReceived = 0;
	vdbFilename sFilename( szFilename );
	FILE* fTemp = fopen( sFilename, "wb" );
	if ( fTemp != NULL )
	{
		char buffer[1024];
		DWORD bytesReceived = 0;

		BOOL rc = InternetReadFile( hUrl, &buffer, 1024, &bytesReceived );

		while ( rc == TRUE && bytesReceived != 0 )
		{
			fwrite( buffer, 1, bytesReceived, fTemp );
			rc = InternetReadFile( hUrl, &buffer, 1024, &bytesReceived );
			
			totalBytesReceived += bytesReceived;
			if ( maxBytes != 0 && totalBytesReceived >= maxBytes )
				break;
		}

		fclose( fTemp );
	}

	maxBytes = totalBytesReceived;

	InternetCloseHandle( hUrl );
	InternetCloseHandle( hInternet );
	return true;
}


//=============================================================================
//  iostream functions
//=============================================================================

//------------------------------------------
//^^ ostream << operator
std::ostream& operator<< ( std::ostream& os, const vdbURL& s )
{
	const vdbString& string = (const vdbString&) s;
	return operator<< ( os, string );
}


//------------------------------------------
//^^ istream >> operator
std::istream& operator>> ( std::istream& is, vdbURL& s )
{
	vdbString& string = (vdbString&) s;
	return operator>> ( is, string );
}
