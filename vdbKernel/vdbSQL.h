//=============================================================================
//
// CLASS:           vdbSQL
//
// OVERVIEW:		Include SQL header files for ODBC
//
// FILE NAME:       vdbSQL.h
//
// INITIAL DATE:    6/26/96
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __vdbSQL_h
#define __vdbSQL_h


#ifndef __vdbWindows_h
	#include "vdbWindows.h"		// windef
#endif

#ifndef __SQL
	#include <sql.h>			// ODBC
#endif

#ifndef __SQLEXT 
	#include <sqlext.h>			// ODBC
#endif


#endif  // __vdbSQL_h
