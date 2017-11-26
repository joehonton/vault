//=============================================================================
//
// CLASS:           vdbMeasurement
//
// OVERVIEW:		The vdbMeasurement class has static member functions for
//                  formatting measurement values such as inches/feet/miles
//                  or um/mm/cm/m/km
//
// FILE NAME:       vdbMeasurement.h
//
// INITIAL DATE:    8/11/02
//
// COPYRIGHT:		Copyright © 2002 by Joe Honton
//
//=============================================================================

#ifndef __vdbMeasurement_h
#define __vdbMeasurement_h

#ifndef _INC_TCHAR
	#include <tchar.h>
#endif

// measurementSystem
enum { MEA_ENGLISH_UNITS = 1,
	   MEA_METRIC_UNITS
	 };

// measurementScale
enum { MEA_UM = 0,
	   MEA_MM,
	   MEA_CM,
	   MEA_M,
	   MEA_KM,
	   MEA_32,
	   MEA_IN,
	   MEA_FT,
	   MEA_MI
	 };

enum { CM_IN,
	   MM_32
	 };

class vdbMeasurement
{
public:
	static const TCHAR* Format( int measurementSystem, int measurementScale, float fValue, int precision = 0 );
	static float Parse( int measurementUnits, int measurementScale, const char* szTextValue );
};

#endif // __vdbMeasurement_h

