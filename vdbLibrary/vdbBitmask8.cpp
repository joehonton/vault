//=============================================================================
//
// CLASS:           vdbBitmask8
//
// OVERVIEW:        Public static functions for converting an integer to
//                  an 8-bit bitmask
//
// FILE NAME:       vdbBitmask8.cpp
//
// INITIAL DATE:    9/14/2003
//
// COPYRIGHT:		Copyright © 2003 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbBitmask8.h"


//=============================================================================
// Public static functions
//=============================================================================

//---------------------------------------
//^ The OneBitOn function returns a mask with the given bit on
//  The resultant bitmask is in reverse order, requesting whichBit of 1
//  returns 0x00000001 not 0x80000000
BITMASK8 vdbBitmask8::OneBitOn( int whichBit )
{
	if ( whichBit == 0 )
		return 0x00;

	else if ( whichBit == 1 )
		return 0x01;
	else if ( whichBit == 2 )
		return 0x02;
	else if ( whichBit == 3 )
		return 0x04;
	else if ( whichBit == 4 )
		return 0x08;

	else if ( whichBit == 5 )
		return 0x10;
	else if ( whichBit == 6 )
		return 0x20;
	else if ( whichBit == 7 )
		return 0x40;
	else if ( whichBit == 8 )
		return 0x80;

	else
		return 0x00;
}


//---------------------------------------
//^ The OneBitOff function returns a mask with the given bit off
//  The resultant bitmask is in reverse order, requesting whichBit of 1
//  returns 0xFFFFFFFE not 0x8FFFFFFF
BITMASK8 vdbBitmask8::OneBitOff( int whichBit )
{
	return 0xFF ^ OneBitOn( whichBit );
}


//---------------------------------------
//^ The CountBitsOn function returns a count of the number of bits
//  which are 1's
//
int vdbBitmask8::CountBitsOn( BITMASK8 mask )
{
	int count = 0;

	if ( mask & 0x01 )	count++;
	if ( mask & 0x02 )	count++;
	if ( mask & 0x04 )	count++;
	if ( mask & 0x08 )	count++;
	if ( mask & 0x10 )	count++;
	if ( mask & 0x20 )	count++;
	if ( mask & 0x40 )	count++;
	if ( mask & 0x80 )	count++;

	return count;
}


//-------------------------------------
void vdbBitmask8::BitsToString( BITMASK8 mask, char* sz, int bitmaskSize )
{
	for ( int i = 1; i <= bitmaskSize; i++ )
	{
		if ( (mask & OneBitOn(i)) != 0 )
			sz[bitmaskSize - i] = '1';
		else
			sz[bitmaskSize - i] = '0';
	}

	sz[bitmaskSize] = 0x0;
}

