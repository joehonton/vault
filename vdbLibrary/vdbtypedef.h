//=============================================================================
//
// CLASS:           vdbTypedef
//
// OVERVIEW:		Typedefs for database types
//
// FILE NAME:       vdbTypedef.h
//
// INITIAL DATE:    1/26/95
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef __vdbTypedef_h
#define __vdbTypedef_h

#ifndef __vdbSQL_h
	#include "vdbSQL.h"
#endif

#ifndef __vdbTimedef_h
	#include "vdbTimedef.h"
#endif


//=========================================================================
typedef int HANDLE_DATABASE;

typedef unsigned long OID;				// object identifier
typedef unsigned char DBBIT;			// one bit in the database
typedef char   DBBYTE;					// one byte in the database
typedef short  DBSMALL;					// two bytes in the database
typedef int    DBINT;					// four bytes in the database
typedef long   DBLONG;					// four bytes in the database
typedef float  DBFLOAT;					// four bytes in the database
typedef double DBDOUBLE;				// eight bytes in the database

typedef enum { DBS_OK = SQL_SUCCESS,
				DBS_ROW_SIZE_EXCEEDED,
				DBS_NOT_OPENED,
				DBS_NOT_ALLOWED,
				DBS_NOT_FOUND,
				DBS_FAILED,
				DBS_TROUBLE,
				DBS_INVALID_DATASOURCE,
				DBS_BAD_VERSION_TABLE,
				DBS_MULTIPLE_MATCHES,
				DBS_ERROR,
				DBS_COLUMN_NOT_FOUND,
				DBS_MORE,
			 } vdbDBSTATUS;		// Dec 1, 2009

#endif // __vdbTypedef_h
