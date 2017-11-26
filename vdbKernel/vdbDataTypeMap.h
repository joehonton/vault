//=============================================================================
//
// FILE NAME:       vdbDataTypeMap.h
//
// ESSENCE:         Server Information to be kept on disk
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    1/19/96
//
// COPYRIGHT:       Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __vdbDataTypeMap_h
#define __vdbDataTypeMap_h

#include "vdbString.h"


class vdbDataTypeMap
{
public:
	vdbDataTypeMap( vdbString& s, vdbString& v ) : _serverDataType( s ), _vaultDataType( v ) {}
	vdbDataTypeMap() {}

public:
	vdbString _serverDataType;
	vdbString _vaultDataType;
};

#endif  // __vdbDataTypeMap_h
