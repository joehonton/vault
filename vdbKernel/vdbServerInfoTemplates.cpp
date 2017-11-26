//=============================================================================
//
// CLASS:           vdbServerInfoTemplates.cpp
//
// OVERVIEW:		Template instantiation for vdbServerInfo and vdbDataTypeMap
//
// FILE NAME:       vdbServerInfoTemplates.cpp
//
// INITIAL DATE:    01/19/96 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================


// pragma to generate the instances
#ifdef __BORLANDC__
	#pragma option -Jgd
#endif
// vdbSet<vdbDataTypeMap> instantiation
#include <vdbSet.cpp>
#include "vdbDataTypeMap.h"
#ifdef __BORLANDC__
	typedef vdbSet<vdbDataTypeMap> inst_vdbSetvdbDataTypeMap;
#else
	template class vdbSet<vdbDataTypeMap>;
#endif


// vdbSet<vdbServerInfo> instantiation
#include "vdbServerInfo.h"
#ifdef __BORLANDC__
	typedef vdbSet<vdbServerInfo> inst_vdbSetvdbServerInfo;
#else
	template class vdbSet<vdbServerInfo>;
#endif
