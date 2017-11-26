//=============================================================================
//
// CLASS:           vdbUnique
//
// OVERVIEW:		The vdbUnique class is for managing vdbUnique objects stored
//                  in the database.
//
// FILE NAME:       vdbUnique.h
//
// INITIAL DATE:    01/14/96 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __vdbUnique_h
#define __vdbUnique_h

//{{IncludeFiles
#include <vdbObject.h>
#include "vdbUniqueTable.h"
//}}


//=============================================================================
class vdbUnique : public vdbObject
{
public:
	// Essential class functions
	vdbUnique( OID oid = 0, HANDLE_DATABASE hDB = 0 );
	vdbUnique( OID oid, vdbDatabase* pDatabase );
	virtual ~vdbUnique();
	vdbUnique( const vdbUnique& rhs );
	vdbUnique& operator=( const vdbUnique& rhs );

public:
	//{{MemberAccessFunctions
	inline OID         GetNextOID() const                       { return nextOID; }
	inline OID         GetLastOID() const                       { return lastOID; }
	inline DBSMALL     GetCacheIndicator() const                { return cacheIndicator; }
	inline void SetNextOID( const OID& i )                      { Modified(); nextOID = i; }
	inline void SetLastOID( const OID& i )                      { Modified(); lastOID = i; }
	inline void SetCacheIndicator( const DBSMALL& i )           { Modified(); cacheIndicator = i; }
	//}}
	inline bool IsCacheActive() const							{ return (cacheIndicator == 1 ? true : false); }
	inline bool IsCacheAvailable() const						{ return (cacheIndicator == 0 ? true : false); }
	inline void SetCacheIndicatorActive()						{ Modified(); cacheIndicator = 1; }
	inline void SetCacheIndicatorAvailable()					{ Modified(); cacheIndicator = 0; }

public:
	// Required virtual functions
	virtual RETCODE Initialize( vdbTable* pTable );
	virtual void Bind( vdbStatement& stmt );
	virtual void Parameter( vdbStatement& stmt, bool bInsert );
	virtual const char* ClientIDColumn( vdbObject* pClient );
	vdbUniqueTable* GetTable();

public:
	//{{VirtualFunctions
	virtual RETCODE PreDeleteTrigger();
	virtual void GetNextObjectID();
	//}}

public:
	inline void SetMasterOID()									{ Modified(); SetOID(2); }

private:
	//{{MemberDeclarations
	OID nextOID;
	OID lastOID;
	DBSMALL cacheIndicator;
	//}}

	HANDLE_DATABASE _hDB;
};


#endif  // vdbUnique_h
