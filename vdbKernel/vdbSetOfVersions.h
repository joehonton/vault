//=============================================================================
//
// CLASS:           vdbSetOfVersions
//
// ESSENCE:         vdbSetOfVersions class
//
// FILE NAME:       vdbSetOfVersions.h
//
// INITIAL DATE:    1/26/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef __vdbSetOfVersions_h
#define __vdbSetOfVersions_h


#include "boolean.h"
#include "vdbSet.h"
#include "vdbVersion.h"


//-------------------------------------
class vdbSetOfVersions : public vdbSet<vdbVersion>
{
public:
	vdbVersion* FindTable( const char* tableName );
	RETCODE Remove( const char* tableName );
};


#endif  // __vdbSetOfVersions_h



