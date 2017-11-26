//=============================================================================
//
// CLASS:           vdbToken
//
// OVERVIEW:		The vdbToken class has static member functions for separating
//                  strings into substrings.
//
// FILE NAME:       vdbToken.cpp
//
// INITIAL DATE:    8/10/02
//
// COPYRIGHT:		Copyright © 2002 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbToken.h"

//warning C4996: 'strcat': This function or variable may be unsafe. Consider using strcat_s instead.
#pragma warning( disable : 4996 )


//-------------------------------------
//^ The Separate function splits the text into separate substrings
void vdbToken::Separate( const TCHAR* szSource, vdbSet<vdbString>& sSubstrings, const TCHAR* seps )
{
	vdbString s = szSource;

	int start = 0;
	int length = s.Length();
		
	while ( start < length )
	{
		int end = s.FindOneOfNext( seps, start );

		if ( end == -1 )
		{
			vdbString* pString = new vdbString;
			*pString = s.Middle( start );
			pString->StripLeadingSpaces();
			pString->StripTrailingSpaces();

			sSubstrings.Add( pString );
			break;
		}
		else
		{
			vdbString* pString = new vdbString;
			*pString = s.Middle( start, end-start );
			pString->StripLeadingSpaces();
			pString->StripTrailingSpaces();

			sSubstrings.Add( pString );
		}

		start = end + 1;
	}
}


//-------------------------------------
//^ The Assemble function combines the substrings into a single string separated
//  by the supplied character
void vdbToken::Assemble( vdbSet<vdbString>& sSubstrings, vdbString& sDest, const TCHAR* sep )
{
	int len = 0;
	for ( int i = 0; i < sSubstrings.Count(); i++ )
	{
		len += sSubstrings[i].Length();
		len++;
	}
	len++;
	
	char* p = new char[len];
	p[0] = 0x0;

	for ( int i = 0; i < sSubstrings.Count(); i++ )
	{
		if ( i > 0 )
			_tcscat( p, sep );
		_tcscat( p, sSubstrings[i] );
	} 

	sDest = p;
	delete[] p; p = 0;
}


//-------------------------------------
//^ The Merge function combines the two strings containing substrings into a single
//  string with the combinintion of the two but without duplicates
//> sInput1 is the first input string, the output will always begin with this
//> sInput2 is the second input string, individual substrings from this will be appended
//  to the output if they don't already exist
//> sOutput is the result string
//
void vdbToken::Merge( vdbString& sInput1, vdbString& sInput2, vdbString& sOutput, const TCHAR* sep )
{
	if ( sInput1 == "" && sInput2 == "" )
	{
		sOutput = "";
		return;
	}	
	else if ( sInput1 == "" )
	{
		sOutput = sInput2;
		return;
	}	
	else if ( sInput2 == "" )
	{
		sOutput = sInput1;
		return;
	}	

	vdbSet<vdbString> subStrings1;
	vdbToken::Separate( sInput1, subStrings1, sep );

	vdbSet<vdbString> subStrings2;
	vdbToken::Separate( sInput2, subStrings2, sep );

	// loop through the new substrings adding them to the current
	// substrings if they don't yet exist there
	for ( int i = 0; i < subStrings2.Count(); i++ )
	{
		bool bFound = false;
		for ( int j = 0; j < subStrings1.Count(); j++ )
		{
			if ( subStrings2[i] == subStrings1[j] )
			{
				bFound = true;
				break;
			}
		}
		if ( bFound == false )
			subStrings1.Add( new vdbString( subStrings2[i] ) );
	}

	// now assemble the merged output
	vdbToken::Assemble( subStrings1, sOutput, sep );  
}


