//=============================================================================
//
// FILE NAME:       vdbAssert.h
//
// ESSENCE:         assertion macro
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    1/22/96
//
// COPYRIGHT:       Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __vdbAssert_h
#define __vdbAssert_h

#ifndef _INC_TCHAR
	#include <tchar.h>
#endif

class vdbThrowAssertException;

//warning C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#pragma warning( disable : 4290 )


#ifdef __cplusplus
	#ifdef _WIN32_WCE
		extern "C" void DisplayAssertion( const TCHAR* szAssertion, const TCHAR* szFile, const int iLine );
		extern "C" void DisplayRangeCheckC( int index, int numItems, const TCHAR* szFile, const int iLine );
	#else
		#include <typeinfo.h>
		extern "C" void DisplayAssertion( const TCHAR* szAssertion, const TCHAR* szFile, const int iLine ) throw(vdbThrowAssertException);
		extern "C" void DisplayRangeCheckCPP( int index, int numItems, const type_info& classType, const TCHAR* szFile, const int iLine ) throw(vdbThrowAssertException);
	#endif // _WIN32_WCE
#else
	void DisplayAssertion( const TCHAR* szAssertion, const TCHAR* szFile, const int iLine );
	void DisplayRangeCheckC( int index, int numItems, const TCHAR* szFile, const int iLine );
#endif


#undef	assert
#undef	RangeCheck

//-------------------------------------
//^^ The assert macro
#ifdef UNICODE
	#define assert(exp)															\
	{																			\
		if ( !(exp) )															\
			DisplayAssertion( _T(#exp), _T(__FILE__), __LINE__ );				\
	}
#else
	#define assert(exp)															\
	{																			\
		if ( !(exp) )															\
			DisplayAssertion( #exp, __FILE__, __LINE__ );						\
	}
#endif // UNICODE


//-------------------------------------
//^^ The RangeCheck macro
#ifdef UNICODE
	#ifdef __cplusplus
		#ifdef _WIN32_WCE
			#define RangeCheck( index, numItems, object )							\
			{																		\
				if ( (index) < 0 || (index) >= (numItems) )							\
					DisplayRangeCheckC( index, numItems, _T(__FILE__), __LINE__ );	\
			}
		#else
			#define RangeCheck( index, numItems, object )							\
			{																		\
				if ( (index) < 0 || (index) >= (numItems) )							\
					DisplayRangeCheckCPP											\
						( index, numItems, typeid(object), _T(__FILE__), __LINE__ );\
			}
		#endif // _WIN32_CE
	#else	// __cplusplus
		#define RangeCheck( index, numItems, object )							\
		{																		\
			if ( (index) < 0 || (index) >= (numItems) )							\
				DisplayRangeCheckC( index, numItems, _T(__FILE__), __LINE__ );	\
		}
	#endif	// __cplusplus
#else
	#ifdef __cplusplus
		#define RangeCheck( index, numItems, object )							\
		{																		\
			if ( (index) < 0 || (index) >= (numItems) )							\
				DisplayRangeCheckCPP											\
					( index, numItems, typeid(object), __FILE__, __LINE__ );	\
		}
	#else
		#define RangeCheck( index, numItems, object )							\
		{																		\
			if ( (index) < 0 || (index) >= (numItems) )							\
				DisplayRangeCheckC( index, numItems, __FILE__, __LINE__ );		\
		}
	#endif	// __cplusplus
#endif  // UNICODE

#endif // __vdbAssert_h
