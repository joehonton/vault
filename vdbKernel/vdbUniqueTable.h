//=============================================================================
//
// CLASS:           vdbUniqueTable
//
// OVERVIEW:        vdbTable-derived class for vdbUnique
//
// FILE NAME:       vdbUniqueTable.h
//
// INITIAL DATE:    01/14/96 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __vdbUniqueTable_h
#define __vdbUniqueTable_h

#include <vdbTable.h>

class vdbDatabase;


class vdbUniqueTable : public vdbTable
{
public:
	vdbUniqueTable( vdbDatabase* pDatabase ) : vdbTable( pDatabase )
	{
		AddColumn( new vdbColumn( pDatabase, "oid", "OID" ) );
		//{{AddColumn
		AddColumn( new vdbColumn( pDatabase, "nextOID", "OID" ) );
		AddColumn( new vdbColumn( pDatabase, "lastOID", "OID" ) );
		AddColumn( new vdbColumn( pDatabase, "cacheIndicator", "DBSMALL" ) );
		//}}
		AddIndex( new vdbIndex( "vdbUniqueKey", "oid", true ) );
	}

public:
	// required virtual functions
	virtual inline const char* TableNameCS()	{ return "vdbUnique"; }
	virtual inline int GetCodeVersion()			{ return 2; }
};


#endif  // vdbUniqueTable_h
