//=============================================================================
//
// CLASS:           vdbToken
//
// OVERVIEW:		The vdbToken class has static member functions for separating
//                  strings into substrings.
//
// FILE NAME:       vdbToken.h
//
// INITIAL DATE:    8/10/02
//
// COPYRIGHT:		Copyright © 2002 by Joe Honton
//
//=============================================================================

#ifndef __vdbToken_h
#define __vdbToken_h

#ifndef __vdbSet_h
	#include <vdbSet.h>
#endif

#ifndef __vdbString_h
	#include <vdbString.h>
#endif


class vdbToken
{
public:
	static void Separate( const TCHAR* szSource, vdbSet<vdbString>& sSubstrings, const TCHAR* seps );
	static void Assemble( vdbSet<vdbString>& sSubstrings, vdbString& sDest, const TCHAR* sep );
	static void Merge( vdbString& sInput1, vdbString& sInput2, vdbString& sOutput, const TCHAR* sep );
};

#endif // __vdbToken_h

