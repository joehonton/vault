//=============================================================================
//
// CLASS:           vdbColorConversions
//
// ESSENCE:         Hue, saturation, brightness conversions to and from COLORREF values
//                  From Rich Franzen's Wheel of Saturation, Intensity, and Hue
//                  http://home.att.net/~rocq/SIHwheel.html
//
// FILE NAME:       ColorConversions.cpp
//
// INITIAL DATE:    6/15/01
//
// COPYRIGHT:		Copyright © 2001 Crescent Bloom
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbColorConversions.h"
#include "vdbAssert.h"


//-------------------------------------
//^ The HueSaturationAndBrightness function
//> color is the input value
//< hue 0.0 to 1.0
//< saturation 0.0 to 1.0
//< brightness 0.0 to 1.0
//
void vdbColorConversions::HueSaturationBrightness( COLORREF color, float& hue, float& saturation, float& brightness )
{
	int red = GetRValue(color);
	int green = GetGValue(color);
	int blue = GetBValue(color);

	HueSaturationBrightness( red, green, blue, hue, saturation, brightness );
}


//-------------------------------------
//^ The HueSaturationAndBrightness function
//> red 0 - 255
//> green 0 - 255
//> blue 0 - 255
//< hue 0.0 to 1.0
//< saturation 0.0 to 1.0
//< brightness 0.0 to 1.0
//
void vdbColorConversions::HueSaturationBrightness( int red, int green, int blue, float& hue, float& saturation, float& brightness )
{
    int bri = max(red, max(green, blue) );
    if (bri == 0)
    {   // short-circuit now and avoid division by zero problems later
        brightness = 0.0;
        saturation = 0.0;
        hue        = 0.0;
        return;
    }

    brightness = bri / 255.0f;
    int desaturator = min(red, min(green, blue) );
    if (bri == desaturator)
    {   // we're grey (and still have division by zero issues to bypass)
        saturation = 0.0;
        hue        = 0.0;
        return;
    }

    saturation = (brightness - (desaturator / 255.0f)) / brightness;
    int midigator;
	if ( red <= green && green <= blue )
		midigator = green;
	else if ( red >= green && green >= blue )
		midigator = green;
	else if ( green <= blue && blue <= red )
		midigator = blue;
	else if ( green >= blue && blue >= red )
		midigator = blue;
	else if ( blue <= red && red <= green )
		midigator = red;
	else if ( blue >= red && red >= green )
		midigator = red;
	else
		assert( false );

	
	//int midigator = mid(red, green, blue);

    // "domains" are 60 degrees of red, yellow, green, cyan, blue, or magenta
    // compute how far we are from a domain base
    float domainBase;
    float oneSixth = 1.0f / 6.0f;
    float domainOffset = (midigator - desaturator) /
                         (float)(bri - desaturator) / 6.0f;
	if (red == bri)
	{
		if (midigator == green)
		{
			// green is ascending
			domainBase = 0 / 6.0f;       // red domain
		}
		else
		{
			// blue is descending
			domainBase = 5 / 6.0f;       // magenta domain
			domainOffset = oneSixth - domainOffset;
		}
	}
	else if (green == bri)
	{
		if (midigator == blue)
		{
			// blue is ascending
			domainBase = 2 / 6.0f;       // green domain
		}
		else
		{
			// red is descending
			domainBase = 1 / 6.0f;       // yellow domain
			domainOffset = oneSixth - domainOffset;
		}
	}
	else // ( blue == bri )
	{
		if (midigator == red)
		{
			// red is ascending
			domainBase = 4 / 6.0f;       // blue domain
		}
		else
		{
			// green is descending
			domainBase = 3 / 6.0f;       // cyan domain
			domainOffset = oneSixth - domainOffset;
		}
	}

    hue = domainBase + domainOffset;
}


//-------------------------------------
//^ The Colorref function returns a COLORREF value
//> hue        0.0 to 1.0
//> saturation 0.0 to 1.0
//> brightness 0.0 to 1.0
//< color is the return value
//
void vdbColorConversions::Colorref( float hue, float saturation, float brightness, COLORREF& color )
{
	int red;
	int green;
	int blue;

	RedGreenBlue( hue, saturation, brightness, red, green, blue );

	color = RGB( red, green, blue );
}


//-------------------------------------
//^ The RGB function returns RGB in 0-255 integer format
//> hue        0.0 to 1.0
//> saturation 0.0 to 1.0
//> brightness 0.0 to 1.0
//< red   0 to 255
//< green 0 to 255
//< blue  0 to 255
//
void vdbColorConversions::RedGreenBlue( float hue, float saturation, float brightness, int& red, int& green, int& blue )
{
	float fRed;
	float fGreen;
	float fBlue;

	RedGreenBlue( hue, saturation, brightness, fRed, fGreen, fBlue );

	red = static_cast<int>(fRed * 255.0);
	green = static_cast<int>(fGreen * 255.0);
	blue = static_cast<int>(fBlue * 255.0);
}


//-------------------------------------
//^ The RedGreenBlue function returns Red, Green, Blue in floating point from 0.0 - 1.0
//> hue        0.0 to 1.0
//> saturation 0.0 to 1.0
//> brightness 0.0 to 1.0
//< red   0.0 - 1.0
//< green 0.0 - 1.0
//< blue  0.0 - 1.0
//
void vdbColorConversions::RedGreenBlue( float hue, float saturation, float brightness, float& red, float& green, float& blue )
{
    if (brightness == 0.0)
    {   // safety short circuit again
        red   = 0.0;
        green = 0.0;
        blue  = 0.0;
        return;
    }

    if (saturation == 0.0)
    {   // grey
        red   = brightness;
        green = brightness;
        blue  = brightness;
        return;
    }

    float domainOffset;         // hue mod 1/6
    if (hue < 1.0/6)
    {   // red domain; green ascends
        domainOffset = hue;
        red   = brightness;
        blue  = static_cast<float>( (brightness * (1.0 - saturation) ) );
        green = blue + (brightness - blue) * domainOffset * 6;
    }
      else if (hue < 2.0/6)
      { // yellow domain; red descends
        domainOffset = static_cast<float>( hue - 1.0/6.0 );
        green = brightness;
        blue  = static_cast<float>( brightness * (1.0 - saturation) );
        red   = green - (brightness - blue) * domainOffset * 6;
      }
      else if (hue < 3.0/6)
      { // green domain; blue ascends
        domainOffset = static_cast<float>( hue - 2.0/6.0 );
        green = brightness;
        red   = static_cast<float>( brightness * (1.0 - saturation) );
        blue  = red + (brightness - red) * domainOffset * 6;
      }
      else if (hue < 4.0/6)
      { // cyan domain; green descends
        domainOffset = static_cast<float>( hue - 3.0/6.0 );
        blue  = brightness;
        red   = static_cast<float>( brightness * (1.0 - saturation) );
        green = blue - (brightness - red) * domainOffset * 6;
      }
      else if (hue < 5.0/6)
      { // blue domain; red ascends
        domainOffset = static_cast<float>( hue - 4.0/6.0 );
        blue  = brightness;
        green = static_cast<float>( brightness * (1.0 - saturation) );
        red   = green + (brightness - green) * domainOffset * 6;
      }
      else
      { // magenta domain; blue descends
        domainOffset = static_cast<float>( hue - 5.0/6.0 );
        red   = brightness;
        green = static_cast<float>( brightness * (1.0 - saturation) );
        blue  = red - (brightness - green) * domainOffset * 6;
      }
}


//-------------------------------------
//^ The SortOrder function returns a number suitable for use in sorting a list of colors
//> color is an RGB colorref
//
DWORD vdbColorConversions::SortOrder( COLORREF color )
{
	float hue;
	float saturation;
	float brightness;
	
	HueSaturationBrightness( color, hue, saturation, brightness );

	return SortOrder( hue, saturation, brightness );
}


//-------------------------------------
//^ The SortOrder function returns a number suitable for use in sorting a list of colors
//> red 0 - 255
//> green 0 - 255
//> blue 0 - 255
//
DWORD vdbColorConversions::SortOrder( int red, int green, int blue )
{
	float hue;
	float saturation;
	float brightness;
	
	HueSaturationBrightness( red, green, blue, hue, saturation, brightness );

	return SortOrder( hue, saturation, brightness );
}


//-------------------------------------
//^ The SortOrder function returns a number suitable for use in sorting a list of colors
//> hue        0.0 to 1.0
//> saturation 0.0 to 1.0
//> brightness 0.0 to 1.0
//
// hue needs to be banded into red, yellow, green, cyan, blue, or magenta
// saturation  0.0 --> 1.0  from white to full color
// brightness  1.0 --> 0.0  from full color to black
//
// if saturation < 0.2 then white
// if brightness < 0.7 then black
//
DWORD vdbColorConversions::SortOrder( float hue, float saturation, float brightness )
{
	DWORD dwHue;
	float oneSixth = 1.0f / 6.0f;

	if ( hue < (17.0 / 360.0) )
		dwHue = 10000;					// red
	else if ( hue < (46.0 / 360.0) )
		dwHue = 20000;					// orange
	else if ( hue < (63.0 / 360.0) )
		dwHue = 30000;					// yellow
	else if ( hue < (160.0 / 360) )	
		dwHue = 40000;					// green
	else if ( hue < (274.0 / 360.0) )
		dwHue = 50000;					// blue
	else if ( hue < (341.0 / 360.0) )
		dwHue = 60000;					// violet
	else
		dwHue = 10000;					// red, again

	DWORD dwSaturation = static_cast<DWORD>( saturation * 100.0 );
	DWORD dwBrightness = static_cast<DWORD>(( (1.0 - brightness) * 100.0 ) + 100.0);

	return ( dwHue + dwSaturation + dwBrightness );
}

