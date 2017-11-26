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

#ifndef __vdbColorConversions_h
#define __vdbColorConversions_h


class vdbColorConversions
{
public:
	static void HueSaturationBrightness( COLORREF color, float& hue, float& saturation, float& brightness );
	static void HueSaturationBrightness( int red, int green, int blue, float& hue, float& saturation, float& brightness );
	static void Colorref( float hue, float saturation, float brightness, COLORREF& color );
	static void RedGreenBlue( float hue, float saturation, float brightness, int& red, int& green, int& blue );
	static void RedGreenBlue( float hue, float saturation, float brightness, float& red, float& green, float& blue );
	static DWORD SortOrder( COLORREF color );
	static DWORD SortOrder( int red, int green, int blue );
	static DWORD SortOrder( float hue, float saturation, float brightness );
};

#endif // __vdbColorConversions_h
