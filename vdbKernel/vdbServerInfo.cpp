//=============================================================================
//
// FILE NAME:       vdbServerInfo.cpp
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

#include "vdbServerInfo.h"
#include "vdbDataTypeMap.h"


//----------------------------------------
//^ The GetVaultType function finds the Vault datatype mapped
//  to the specified ODBC datatype.
//> ServerType is the server type to find.  For example: int, smallint, float.
//< The return is the Vault type found.  For example:  DBINT, DBSMALL, DBFLOAT.
//< Returns "<unsupported>" if not found.
//
vdbString& vdbServerInfo::GetVaultType( vdbString& ServerType )
{
	static vdbString unsupportedString( "<unsupported>" );

	for ( int i = 0; i < _datatypes.Count(); i++ )
	{
		if ( _datatypes[i]._serverDataType == ServerType )
			return _datatypes[i]._vaultDataType; 
	}
	return unsupportedString;
}


//----------------------------------------
//^ The GetServerType function finds the server datatype mapped
//  to the specified Vault datatype.
//> VaultType is the Vault type to find.  For example:  DBINT, DBSMALL, DBFLOAT.
//< The return is the server type found.  For example: int, smallint, float.
//< Returns "<unsupported>" if not found.
//
vdbString& vdbServerInfo::GetServerType( vdbString& VaultType )
{
	static vdbString unsupportedString( "<unsupported>" );

	for ( int i = 0; i < _datatypes.Count(); i++ )
	{
		if ( _datatypes[i]._vaultDataType == VaultType )
			return _datatypes[i]._serverDataType; 
	}
	return unsupportedString;
}
