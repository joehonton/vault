//=============================================================================
//
// OVERVIEW:		enumeration of boolean until this becomes a keyword.
//
// FILE NAME:       boolean.h
//
// INITIAL DATE:    11/27/94 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __boolean_h
#define __boolean_h


#if _MSC_VER < 1100
	#pragma warning( disable : 4237 )
	enum bool { false, true };
#else
	// warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
	#pragma warning( disable : 4800 )
#endif


#endif // __boolean_h
