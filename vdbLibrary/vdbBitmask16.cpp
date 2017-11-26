//=============================================================================
//
// CLASS:           vdbBitmask16
//
// OVERVIEW:        Public static functions for converting an integer to
//                  a 16-bit bitmask
//
// FILE NAME:       vdbBitmask16.cpp
//
// INITIAL DATE:    9/14/2003
//
// COPYRIGHT:		Copyright © 2003 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbBitmask16.h"


//=============================================================================
// Public static functions
//=============================================================================

//---------------------------------------
//^ The OneBitOn function returns a mask with the given bit on
//  The resultant bitmask is in reverse order, requesting whichBit of 1
//  returns 0x00000001 not 0x80000000
BITMASK16 vdbBitmask16::OneBitOn( int whichBit )
{
	if ( whichBit == 0 )
		return 0x0000;

	else if ( whichBit == 1 )
		return 0x0001;
	else if ( whichBit == 2 )
		return 0x0002;
	else if ( whichBit == 3 )
		return 0x0004;
	else if ( whichBit == 4 )
		return 0x0008;

	else if ( whichBit == 5 )
		return 0x0010;
	else if ( whichBit == 6 )
		return 0x0020;
	else if ( whichBit == 7 )
		return 0x0040;
	else if ( whichBit == 8 )
		return 0x0080;

	else if ( whichBit == 9 )
		return 0x0100;
	else if ( whichBit == 10 )
		return 0x0200;
	else if ( whichBit == 11 )
		return 0x0400;
	else if ( whichBit == 12 )
		return 0x0800;

	else if ( whichBit == 13 )
		return 0x1000;
	else if ( whichBit == 14 )
		return 0x2000;
	else if ( whichBit == 15 )
		return 0x4000;
	else if ( whichBit == 16 )
		return 0x8000;

	else
		return 0x0000;
}


//---------------------------------------
//^ The OneBitOff function returns a mask with the given bit off
//  The resultant bitmask is in reverse order, requesting whichBit of 1
//  returns 0xFFFFFFFE not 0x8FFFFFFF
BITMASK16 vdbBitmask16::OneBitOff( int whichBit )
{
	return 0xFFFFFFFF ^ OneBitOn( whichBit );
}


//---------------------------------------
//^ The CountBitsOn function returns a count of the number of bits
//  which are 1's
//
int vdbBitmask16::CountBitsOn( BITMASK16 mask )
{
	int count = 0;

	if ( mask & 0x0001 )	count++;
	if ( mask & 0x0002 )	count++;
	if ( mask & 0x0004 )	count++;
	if ( mask & 0x0008 )	count++;
	if ( mask & 0x0010 )	count++;
	if ( mask & 0x0020 )	count++;
	if ( mask & 0x0040 )	count++;
	if ( mask & 0x0080 )	count++;
	if ( mask & 0x0100 )	count++;
	if ( mask & 0x0200 )	count++;
	if ( mask & 0x0400 )	count++;
	if ( mask & 0x0800 )	count++;
	if ( mask & 0x1000 )	count++;
	if ( mask & 0x2000 )	count++;
	if ( mask & 0x4000 )	count++;
	if ( mask & 0x8000 )	count++;

	return count;
}


//-------------------------------------
void vdbBitmask16::BitsToString( BITMASK16 mask, char* sz, int bitmaskSize )
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

