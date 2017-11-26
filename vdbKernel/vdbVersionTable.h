//=============================================================================
//
// CLASS:           vdbVersionTable
//
// OVERVIEW:        vdbTable-derived class for vdbVersion
//
// FILE NAME:       vdbVersionTable.h
//
// INITIAL DATE:    01/14/96 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __vdbVersionTable_h
#define __vdbVersionTable_h

#include <boolean.h>
#include <vdbTable.h>

class vdbDatabase;


class vdbVersionTable : public vdbTable
{
public:
	vdbVersionTable( vdbDatabase* pDatabase ) : vdbTable( pDatabase )
	{
		AddColumn( new vdbColumn( pDatabase, "oid", "OID" ) );
		//{{AddColumn
		AddColumn( new vdbColumn( pDatabase, "tableID", "DBINT" ) );
		AddColumn( new vdbColumn( pDatabase, "tableName", "char(40)" ) );
		AddColumn( new vdbColumn( pDatabase, "version", "DBINT" ) );
		//}}
		AddIndex( new vdbIndex( "vdbVersionKey", "oid", true ) );
	}

public:
	// required virtual functions
	virtual inline const char* TableNameCS()	{ return "vdbVersion"; }
	virtual inline int GetCodeVersion()			{ return 1; }
};


#endif  // vdbVersionTable_h
