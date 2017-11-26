//=============================================================================
//
// FILE NAME:       vdbAssert.cpp
//
// ESSENCE:         assertion macro
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    7/17/97
//
// COPYRIGHT:       Copyright © 1997 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"

#ifndef __vdbAssert_h
	#include "vdbAssert.h"
#endif

#ifndef __vdbException_h
	#include "vdbException.h"
#endif

#ifndef __vdbString_h
	#include "vdbString.h"
#endif

#ifndef _INC_STDLIB
	#include <stdlib.h>
#endif

#ifndef __vdbMessageBox_h
	#include "vdbMessageBox.h"
#endif

#ifndef _WIN32_WCE
	#ifndef _INC_TYPEINFO
		#include <typeinfo.h>
	#endif
#endif

//warning C4996: 'strcpy': This function or variable may be unsafe. Consider using strcpy_s instead.
#pragma warning( disable : 4996 )



//-------------------------------------
#if defined (_M_IX86)
	#define _DbgBreak() __asm { int 3 }

#elif defined (_M_ALPHA)
	void _BPT();
	#ifdef USE_AFX
		#pragma intrinsic(_BPT)
	#endif // USE_AFX
	#define _DbgBreak() _BPT()

#else
	#define _DbgBreak() DebugBreak()

#endif


//-------------------------------------
//^ The DisplayAssertion function is called by the macro assert
//> szAssertion is the expression that failed.
//> szFile is the name of the file that the assertion occured in.
//> iLine is the line number that the assertion occured on.
//
void DisplayAssertion( const TCHAR* szAssertion, const TCHAR* szFile, const int iLine )
{
	vdbString s;
	s.Format( _T("An unexpected combination of data has been encountered for which no\nprogramming logic has been written.  Because this situation is not\nsupposed to occur, continuing will produce undefined results.\n\nProgrammer's clues:\n\n   Statement: %s\n\n   File:  %s\n\n   Line:  %d\n\nDo you want to try to continue anyway?"), szAssertion, szFile, iLine );
	#ifdef _DEBUG
		s += _T("\n\nPress No to break into debug mode.");
	#endif
	int rc =  vdbMessageBox( NULL, s, _T("Assertion failed"), MB_YESNO );
	if ( rc == IDYES )
		vdbThrowAssertException( szAssertion, szFile, iLine );
	else
	{
        _DbgBreak();
		exit(3);
	}
}


#ifndef _WIN32_WCE
//--------------------------------------------
//^ The DisplayRangeCheckCPP function is called when a subscript is out of
//  range for a set.  When the user presses YES an exception is thrown.
//  When the user presses NO the debugger is shown
//^ Use this function with C++ modules.
//> index is the index to validate
//> numItems is a count of the number valid of items in the set.  The largest
//  subscript is thus numItems-1
//> classType is the class type of the object that failed
//> szFile is the name of the file that the range check occured in.
//> iLine is the line number that the range check occured on.
//
void DisplayRangeCheckCPP( int index, int numItems, const type_info& classType, const TCHAR* szFile, const int iLine )
{
	int len = strlen( classType.name() ) + 1;
	TCHAR* pszClassType = new TCHAR[len];
	#ifdef UNICODE
		MultiByteToWideChar( CP_ACP, 0, classType.name(), -1, pszClassType, len );
	#else
		strcpy( pszClassType, classType.name() );
	#endif

	vdbString s;
	s.Format( _T("The subscript is out of range in %s.\n\nSubscript value is %d.\n\nValid range of subscripts is 0 to %d.\n\nFile:  %s\n\nLine:  %d\n\nDo you want to try to continue?"), pszClassType, index, (numItems - 1), szFile, iLine );
	#ifdef _DEBUG
		s += _T("\n\nPress No to break into debug mode.");
	#endif
	int rc =  vdbMessageBox( NULL, s, _T("Out of range"), MB_YESNO );

	if ( rc == IDYES )
	{
		vdbRangeException e( index, (numItems-1), pszClassType );
		delete[] pszClassType, pszClassType = 0;
		throw e;
	}
	else
	{
		delete[] pszClassType, pszClassType = 0;
        _DbgBreak();
		exit(3);
	}
}
#endif // _WIN32_WCE


//--------------------------------------------
//^ The DisplayRangeCheckC function is called when a subscript is out of
//  range for a set.  When the user presses YES an exception is thrown.
//  When the user presses NO the debugger is shown
//^ Use this function with C modules.
//> index is the index to validate
//> numItems is a count of the number valid of items in the set.  The largest
//  subscript is thus numItems-1
//> szFile is the name of the file that the range check occured in.
//> iLine is the line number that the range check occured on.
//
void DisplayRangeCheckC( int index, int numItems, const TCHAR* szFile, const int iLine )
{
	vdbString s;
	s.Format( _T("The subscript is out of range.\n\nSubscript value is %d.\n\nValid range of subscripts is 0 to %d.\n\nFile:  %s\n\nLine:  %d\n\nDo you want to try to continue?"), index, (numItems - 1), szFile, iLine );
	#ifdef _DEBUG
		s += _T("\n\nPress No to break into debug mode.");
	#endif
	int rc =  vdbMessageBox( NULL, s, _T("Out of range"), MB_YESNO );
	if ( rc == IDYES )
		vdbThrowRangeException( index, (numItems-1), _T("C") );
	else
	{
		_DbgBreak();
		exit(3);
	}
}

