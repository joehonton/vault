//=============================================================================
//
// CLASS:           vdbSetOfVersions
//
// ESSENCE:         vdbSetOfVersions class
//
// FILE NAME:       vdbSetOfVersions.cpp
//
// INITIAL DATE:    1/26/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifdef USE_PRE_HDR
	#include "vdbHeader.h"
#else
	#include "boolean.h"
#endif

#include "vdbSetOfVersions.h"
#include <string.h>

//--------------------------------------------
//^ The FindTable function searches the version cache to find the vdbVersion
//  object which has given name.
//> tableName is the unqualified name (no owner, upper and lower case) to
//  search for.
//< Returns a pointer to the object if found.
//< Returns NULL if not found.
//
vdbVersion* vdbSetOfVersions::FindTable( const char* tableName )
{
	vdbVersion* p;
	for ( int index = 0; index < Count(); index++ )
	{
		p = &Get( index );
		if ( !strcmp( p->GetTableName(), tableName ) )
			return p;
	}

	return NULL;
}


//--------------------------------------------
//  Remove the entry in the set which corresponds to the tableName
//  of the passed argument.  Return DBS_NOT_FOUND if no match.  Note that
//  pointers to vdbVersion objects may now point to the wrong object.  Be sure
//  to use FindTable or the operator[] again to refetch these pointers.
//
RETCODE vdbSetOfVersions::Remove( const char* tableName )
{
	vdbVersion* p;
	for ( int index = 0; index < Count(); index++ )
	{
		p = &Get( index );
		if ( !strcmp( p->GetTableName(), tableName ) )
		{
			p->Delete();	// delete from the DBMS
			vdbSet<vdbVersion>::Remove( index );
			return DBS_OK;
		}
	}
	return DBS_NOT_FOUND;
}

