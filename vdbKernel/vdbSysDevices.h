//=============================================================================
//
// FILE NAME:       vdbSysDevices.h
//
// ESSENCE:         A small class for holding Microsoft SQL Server devices name
//                  and numbers.
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    8/15/95
//
// COPYRIGHT:       Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef __vdbSysDevices_h
#define __vdbSysDevices_h

#ifndef __vdbTypedef_h
	#include "vdbTypedef.h"
#endif

#ifndef __vdbUtility_h
	#include "vdbUtility.h"
#endif


class vdbSysDevices
{
public:
	vdbSysDevices() {}
	vdbSysDevices( const char* name, int number )
	{
		vdbUtility::SafeCopy( deviceName, name, sizeof(deviceName) );
		deviceNumber = number;
	}
public:
	char deviceName[31];
	DBINT deviceNumber;
};


#endif // __vdbSysDevices_h
