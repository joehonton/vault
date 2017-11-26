//=============================================================================
//
// CLASS:           vdbHeader
//
// OVERVIEW:		Vault class library declarations
//
// FILE NAME:       vdbHeader.h
//
// INITIAL DATE:    2/27/95 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __vdbHeader_h
#define __vdbHeader_h

#include "vdbSQL.h"				// ODBC headers
#include "vdbMessageBox.h"		// MessageBox 
#include <vdbAssert.h>			// assert macro
#include "boolean.h"			// enum bool
#include "vdbTypedef.h"			// DB types
#include "vdbExport.h"			// DLL export macro

#include "vdbUtility.h"			// Utility functions
#include "vdbPool.h"			// pool of database instances
#include "vdbDatabase.h"		// ODBC environment and database wrapper
#include "vdbServer.h"			// Server base class
#include "vdbStatement.h"		// ODBC statement wrapper
#include "vdbQuery.h"			// Query template class
#include "vdbDynamicQuery.h"	// Dynamic Query template class
#include "vdbLinkSet.h"			// Link Set template class
#include "vdbSet.h"				// Set template class
#include "vdbMap.h"				// Map template class
#include "vdbPtr.h"				// Smart pointer template class
#include "vdbTable.h"			// Table base class
#include "vdbObject.h"			// Object base class
#include "vdbException.h"		// Exception class
#include "vdbTimeStampDB.h"		// Timestamp class
#include "vdbString.h"			// String class
#include "vdbIndex.h"			// Index class
#include "vdbBinaryVar.h"		// Variable length binary object
#include "vdbBinaryFixed.h"		// Fixed length binary object
#include "vdbBinaryLong.h"		// support for "long varbinary" type	SQL Server "image" type
#include "vdbVarCharLong.h"		// support for "long varchar" type		SQL Server "text" type
#include <xmlDocument.h>
#include <xmlItem.h>

#endif  // __vdbHeader_h
