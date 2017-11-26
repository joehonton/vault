//=============================================================================
//
// FILE NAME:       vdbServerInfo.h
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

#ifndef __vdbServerInfo_h
#define __vdbServerInfo_h

#include "vdbString.h"
#include "vdbSet.h"
#include "vdbDataTypeMap.h"


class vdbServerInfo
{
public:
	vdbString& GetVaultType( vdbString& ServerType );
	vdbString& GetServerType( vdbString& VaultType );

public:
	vdbString _serverDescription;		   	// to be displayed to the user
	vdbString _serverIdentificationString;	// text ID given by the ODBC driver ( vdbDatabase::ServerName )
	vdbString _administratorID;				// master administrator ID
	vdbString _administratorPassword;		// master administrator password
	vdbSet<vdbDataTypeMap>	_datatypes;		// set of data types for this server
};

#endif  // __vdbServerInfo_h
