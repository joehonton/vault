//=============================================================================
//
// CLASS:           vdbMeasurement
//
// OVERVIEW:		The vdbMeasurement class has static member functions for
//                  formatting measurement values such as inches/feet/miles
//                  or um/mm/cm/m/km
//
// FILE NAME:       vdbMeasurement.cpp
//
// INITIAL DATE:    8/11/02
//
// COPYRIGHT:		Copyright © 2002 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbMeasurement.h"
#include "vdbString.h"
#include "vdbAssert.h"
#include "vdbUtility.h"
#include <math.h>

//-------------------------------------
//> measurementSystem is 1 for English, 2 for metric
//> measurementScale is the units of the value
//   for English use MEA_32, MEA_IN, MEA_FT, MEA_MI
//   for metric use MEA_UM, MEA_MM, MEA_CM, MEA_M, MEA_KM
//> fValue should be an integer value for English
//     and should be a floating point value for Metric
//> precision is the number of decimal points to show, applies to metric only (0, 1, 2, 3)
//
const char* vdbMeasurement::Format( int measurementSystem, int measurementScale, float fValue, int precision )
{
	static vdbString s;
	s = "";

	bool bNegative = fValue < 0.0 ? true : false;
	if ( bNegative )
		fValue = fabs( fValue );

	int value = (int) fValue;
	assert( value >= 0 );

	if ( measurementSystem == MEA_ENGLISH_UNITS )
	{
		if ( measurementScale == MEA_32 )
		{
			if ( value == 0 )
				s = "0\"";
			else if ( value % 32 != 0 )
			{
				if ( value < 32 )
				{
					if ( value % 2 == 1 )
						s.Format( "%ld/32\"", value );
					else
					{
						value /= 2;
						if ( value % 2 == 1 )
							s.Format( "%ld/16\"", value );
						else
						{
							value /= 2;
							if ( value % 2 == 1 )
								s.Format( "%ld/8\"", value );
							else
							{
								value /= 2;
								if ( value % 2 == 1 )
									s.Format( "%ld/4\"", value );
								else
								{
									value /= 2;
									assert( value == 1 );
									s = "1/2\"";
								}
							}
						}
					}
				}

				else // ( value > 32 )
				{
					int units = value / 32;
					int modulo = value % 32;
					if ( modulo % 2 == 1 )
						s.Format( "%ld %ld/32\"", units, modulo );
					else
					{
						modulo /= 2;
						if ( modulo % 2 == 1 )
							s.Format( "%ld %ld/16\"", units, modulo );
						else
						{
							modulo /= 2;
							if ( modulo % 2 == 1 )
								s.Format( "%ld %ld/8\"", units, modulo );
							else
							{
								modulo /= 2;
								if ( modulo % 2 == 1 )
									s.Format( "%ld %ld/4\"", units, modulo );
								else
								{
									modulo /= 2;
									assert( modulo == 1 );
									s.Format( "%ld 1/2\"", units );
								}
							}
						}
					}
				}
			}
			else // ( value % 32 == 0 )
			{
				value /= 32;
				measurementScale = MEA_IN;
			}
		}

		if ( measurementScale == MEA_IN )
		{
			if ( value == 0 )
				s = "0\"";
			else if ( value < 12 )
				s.Format( "%ld\"", value );
			else
			{
				if ( value % 12 == 0 )
				{
					value /= 12;
					measurementScale = MEA_FT;
				}
				else
					s.Format( "%ld' %ld\"", value / 12, value % 12 );
			}
		}

		if ( measurementScale == MEA_FT )
		{
			if ( value == 0 )
				s = "0'";
			else if ( value % 1320 == 0 )
			{
				if ( value <= 5280 )
				{
					if ( value == 1320 )
						s = "1/4 mi";
					else if ( value == 2640 )
						s = "1/2 mi";
					else if ( value == 3960 )
						s = "3/4 mi";
					else if ( value == 5280 )
						s = "1 mi";
				}
				else // ( value > 5280 )
				{
					int units = value / 5280;
					int modulo = value % 5280;

					if ( modulo == 1320 )
						s.Format( "%ld 1/4 mi", units );
					else if ( modulo == 2640 )
						s.Format( "%ld 1/2 mi", units );
					else if ( modulo == 3960 )
						s.Format( "%ld 3/4 mi", units );
					else // ( modulo == 0 )
						s.Format( "%ld mi", units );
				}
			}
			else if ( value < 5280 )
				s.Format( "%ld'", value );
			else
			{
				if ( value % 5280 == 0 )
				{
					value /= 5280;
					measurementScale = MEA_MI;
				}
				else
					s.Format( "%ld mi %ld'", value / 5280, value % 5280 );
			}
		}

		if ( measurementScale == MEA_MI )
			s.Format( "%ld mi", value );
	}

	else // ( _measurementSystem == MEA_METRIC_UNITS )
	{
		if ( measurementScale == MEA_UM )
		{
			if ( fValue == 0.0 )
				s = "0 um";
			else if ( fValue < 1000.0 )
			{
				if ( precision == 3 )
					s.Format( "%.3f um", fValue );
				else if ( precision == 2 )
					s.Format( "%.2f um", fValue );
				else if ( precision == 1 )
					s.Format( "%.1f um", fValue );
				else
					s.Format( "%.0f um", fValue );
			}
			else
			{
				if ( fmod( fValue, (float) 1000.0) == 0.0 )
				{
					fValue /= 1000.0;
					measurementScale = MEA_MM;
				}
				else
				{
					if ( precision == 3 )
						s.Format( "%.3f mm", fValue / 1000.0F );
					else if ( precision == 2 )
						s.Format( "%.2f mm", fValue / 1000.0F );
					else if ( precision == 1 )
						s.Format( "%.1f mm", fValue / 1000.0F );
					else
						s.Format( "%.0f mm", fValue / 1000.0F );
				}
			}
		}

		if ( measurementScale == MEA_MM )
		{
			if ( fValue == 0.0 )
				s = "0 mm";
			else if ( fValue < 1000.0 )
			{
				if ( precision == 3 )
					s.Format( "%.3f mm", fValue );
				else if ( precision == 2 )
					s.Format( "%.2f mm", fValue );
				else if ( precision == 1 )
					s.Format( "%.1f mm", fValue );
				else
					s.Format( "%.0f mm", fValue );
			}
			else
			{
				if ( fmod( value, 10.0) == 0.0 )
				{
					fValue /= 10.0;
					measurementScale = MEA_CM;
				}
				else
				{
					if ( precision == 3 )
						s.Format( "%.3f cm", fValue / 10.0F );
					else if ( precision == 2 )
						s.Format( "%.2f cm", fValue / 10.0F );
					else if ( precision == 1 )
						s.Format( "%.1f cm", fValue / 10.0F );
					else
						s.Format( "%.0f cm", fValue / 10.0F );
				}
			}
		}

		if ( measurementScale == MEA_CM )
		{
			if ( fValue == 0.0 )
				s = "0 cm";
			else if ( fValue < 100.0 )
			{
				if ( precision == 3 )
					s.Format( "%.3f cm", fValue );
				else if ( precision == 2 )
					s.Format( "%.2f cm", fValue );
				else if ( precision == 3)
					s.Format( "%.1f cm", fValue );
				else
					s.Format( "%.0f cm", fValue );
			}
			else
			{
				if ( fmod( (float) fValue, (float) 100.0) == 0.0 )
				{
					fValue /= 100.0;
					measurementScale = MEA_M;
				}
				else
				{
					if ( precision == 3 )
						s.Format( "%.3f m", fValue / 100.0F );
					else if ( precision == 2 )
						s.Format( "%.2f m", fValue / 100.0F );
					else if ( precision == 1 )
						s.Format( "%.1f m", fValue / 100.0F );
					else
						s.Format( "%.0f m", fValue / 100.0F );
				}
			}
		}

		if ( measurementScale == MEA_M )
		{
			if ( fValue == 0.0 )
				s = "0 m";
			else if ( fValue < 1000.0 )
			{
				if ( precision == 3 )
					s.Format( "%.3f m", fValue );
				else if ( precision == 2 )
					s.Format( "%.2f m", fValue );
				else if ( precision == 3)
					s.Format( "%.1f m", fValue );
				else
					s.Format( "%.0f m", fValue );
			}
			else
			{
				if ( fmod( value, 1000.0) == 0.0 )
				{
					fValue /= 1000.0;
					measurementScale = MEA_KM;
				}
				else
				{
					if ( precision == 3 )
						s.Format( "%.3f km", fValue / 1000.0F );
					else if ( precision == 2 )
						s.Format( "%.2f km", fValue / 1000.0F );
					else if ( precision == 1 )
						s.Format( "%.1f km", fValue / 1000.0F );
					else
						s.Format( "%.0f km", fValue / 1000.0F );
				}
			}
		}

		if ( measurementScale == MEA_KM )
		{
			if ( fValue == 0.0 )
				s = "0 km";
			else
				s.Format( "%.0f km", fValue );
		}
	}

	if ( bNegative == true )
	{
		vdbString sNeg;
		sNeg = "-" + s;
		s = sNeg;
	}

	return s;
}


//-------------------------------------
//^ The Parse function converts the text value into units of the requested scale
//> measurementSystem is 1 for English, 2 for metric
//> measurementScale is the units of the value
//   for English use MEA_32, MEA_IN
//   for metric use MEA_MM, MEA_CM
//< if scale is MEA_32 returns the number of 1/32's in the string
//< if scale is MEA_IN returns the number of inches in the string
//< if scale is MEA_MM returns the number of millimeters in the string
//< if scale is MEA_CM returns the number of centimeters in the string
//
float vdbMeasurement::Parse( int measurementUnits, int measurementScale, const char* szTextValue )
{
	assert( szTextValue != NULL );
	int len = strlen( szTextValue );
	if ( len == 0 )
		return 0;

	char* pCopy = new char[len+1];
	vdbUtility::SafeCopy( pCopy, szTextValue, len+1 );

	// inches and 1/32's on input: like "1"" or "1 1/32" or "1/32"
	// 1/32's on output
	if ( measurementUnits == MEA_ENGLISH_UNITS && measurementScale == MEA_32 )
	{
		int thirtySeconds = 0;

		char* pDecimal = strchr( pCopy, '.' );
		char* pSpace = strchr( pCopy, ' ' );
		char* pSlash = strchr( pCopy, '/' );
		char* pDoubleQuote = strchr( pCopy, '"' );

		// if this is a decimal number, not a fractional number
		// e.g. "1.25"" or "1.25"
		if ( pDecimal != 0 )
		{
			// ignore anything after the doublequote
			if ( pDoubleQuote != 0 )
				*pDoubleQuote = 0x0;
				
			// convert from decimal inches to 32's
			thirtySeconds = static_cast<int>(atof( pCopy )) * 32;
		}
		
		// if this is a whole number
		// e.g. "1"" or "1"
		else if ( pSpace == 0 && pSlash == 0 )
		{
			// ignore anything after the doublequote
			if ( pDoubleQuote != 0 )
				*pDoubleQuote = 0x0;
				
			// treat it as if it were full inches
			thirtySeconds = atoi( pCopy ) * 32;
		}

		// if a slash was found, but there was no space prior to the slash
		// e.g. "1/32"" or "1/32"
		else if ( pSlash != 0 && ( pSpace == 0 || pSpace > pSlash ) )
		{
			// ignore anything after the doublequote
			if ( pDoubleQuote != 0 )
				*pDoubleQuote = 0x0;

			*pSlash = 0x0;
			char* pNumeratorStart = pCopy;
			int numerator = atoi( pNumeratorStart );

			char* pDenominatorStart = pSlash + 1;
			int denominator = atoi( pDenominatorStart ); 
			if ( denominator > 0 )
				thirtySeconds = (32 / denominator) * numerator;
		}

		// if a slash was found, and a space was found prior to the slash
		// e.g. " 1 1/32"" or "1 1/32"
		else if ( pSlash != 0 && pSpace != 0 && ( pSpace < pSlash ) )
		{
			// ignore anything after the doublequote
			if ( pDoubleQuote != 0 )
				*pDoubleQuote = 0x0;

			*pSpace = 0x0;
			thirtySeconds = atoi( pCopy ) * 32;
			
			*pSlash = 0x0;
			char* pNumeratorStart = pSpace + 1;
			int numerator = atoi( pNumeratorStart );

			char* pDenominatorStart = pSlash + 1;
			int denominator = atoi( pDenominatorStart ); 
			if ( denominator > 0 )
				thirtySeconds = thirtySeconds + ( (32 / denominator) * numerator );
		}

		delete[] pCopy; pCopy = 0;
		return static_cast<float>(thirtySeconds);
	}

	// feet and inches on input:  like "2'" or "1' 11" or "23""
	// inches on output
	else if ( measurementUnits == MEA_ENGLISH_UNITS && measurementScale == MEA_IN )
	{
		int inches = 0;

		char* pInchesStart = pCopy;
		char* pDecimal = strchr( pCopy, '.' );
		char* pFeetEnd = strchr( pCopy, '\'' );

		// if this is a decimal number, (not feet and inches), treat as decimal feet
		// e.g.  "1.5" means 18 inches
		if ( pDecimal != 0 )
		{
			// ignore anything after the foot symbol
			if ( pFeetEnd != 0 )
				*pFeetEnd = 0x0;
				
			// convert from decimal feet to inches
			inches = static_cast<int>( atof( pCopy ) ) * 12;
		}
		
		else
		{
			if ( pFeetEnd != 0 )
			{
				*pFeetEnd = 0x0;
				inches = atoi( pCopy ) * 12;
				pInchesStart = pFeetEnd + 1;
			}
			
			char* pInchesEnd = strchr( pCopy, '"' );
			if ( pInchesEnd != 0 )
				*pInchesEnd = 0x0;
		}

		inches = inches + atoi( pInchesStart );

		delete[] pCopy; pCopy = 0;
		return static_cast<float>(inches);
	}

	// centimeters or millimeters on input: like "125 mm" or "12.5 cm" or just "125"
	// millimeters on output
	else if ( measurementUnits == MEA_METRIC_UNITS && measurementScale == MEA_MM )
	{
		float millimeters = 0.0;

		char* pMillimeters = strstr( pCopy, "mm" );
		if ( pMillimeters != 0 )
		{
			*pMillimeters = 0x0;
			millimeters = static_cast<float>(atof( pCopy ));
		}
		else
		{
			char* pCentimeters = strstr( pCopy, "cm" );
			if ( pCentimeters != 0 )
			{
				*pCentimeters = 0x0;
				millimeters = static_cast<float>(atof( pCopy ) * 10.0);
			}

			// if we didn't find "mm" or "cm", just assume millimeters
			else
			{
				millimeters = static_cast<float>(atof( pCopy ));
			}
		}

		delete[] pCopy; pCopy = 0;
		return millimeters;
	}

	// meters and centimeters on input:  like "125 cm" or "1.25m" or just "125"
	// centimeters on output
	else if ( measurementUnits == MEA_METRIC_UNITS && measurementScale == MEA_CM )
	{
		float centimeters = 0.0;

		char* pCentimeters = strstr( pCopy, "cm" );
		if ( pCentimeters != 0 )
		{
			*pCentimeters = 0x0;
			centimeters = static_cast<float>(atof( pCopy ));
		}
		else
		{
			char* pMeters = strstr( pCopy, "m" );
			if ( pMeters != 0 )
			{
				*pMeters = 0x0;
				centimeters = static_cast<float>(atof( pCopy ) * 100.0);
			}

			// if we didn't find "cm" or "m", just assume centimeters
			else
			{
				centimeters = static_cast<float>(atof( pCopy ));
			}
		}

		delete[] pCopy; pCopy = 0;
		return centimeters;
	}

	else
		assert( false );

	return 0;
}
