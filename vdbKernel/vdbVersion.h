//=============================================================================
//
// FILE NAME:       vdbVersion.h
//
// OVERVIEW:        Class declaration for vdbVersion
//
// INITIAL DATE:    01/14/96 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __vdbVersion_h
#define __vdbVersion_h

//{{IncludeFiles
#include <vdbObject.h>
#include "vdbVersionTable.h"
//}}

//=============================================================================
class vdbVersion : public vdbObject
{
public:
	// Essential class functions
	vdbVersion( OID oid = 0, HANDLE_DATABASE hDB = 0 );
	virtual ~vdbVersion();
	vdbVersion( const vdbVersion& rhs );
	vdbVersion& operator=( const vdbVersion& rhs );

public:
	//{{MemberAccessFunctions
	inline DBINT       GetTableID() const                                   { return tableID; }
	inline const char* GetTableName() const                                 { return tableName; }
	inline DBINT       GetVersion() const                                   { return version; }

	inline void SetTableID( const DBINT& i )                                { Modified(); tableID = i; }
	inline void SetTableName( const char* s )                               { Modified(); vdbUtility::SafeCopy(tableName, s, sizeof(tableName)); }
	inline void SetVersion( const DBINT& i )                                { Modified(); version = i; }
	//}}

public:
	// Required virtual functions
	virtual RETCODE Initialize( vdbTable* pTable );
	virtual void Bind( vdbStatement& stmt );
	virtual void Parameter( vdbStatement& stmt, bool bInsert );
	virtual const char* ClientIDColumn( vdbObject* pClient );
	vdbVersionTable* GetTable();

public:
	//{{VirtualFunctions
	virtual RETCODE PostRetrieveTrigger();
	//}}

private:
	//{{MemberDeclarations
private:
	DBINT tableID;
	char tableName[41];
	DBINT version;
	//}}
};


#endif  // vdbVersion_h
