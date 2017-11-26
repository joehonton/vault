//=============================================================================
//
// CLASS:           vdbBitmask32
//
// OVERVIEW:        Public static functions for converting an integer to
//                  a 32-bit bitmask
//
// FILE NAME:       vdbBitmask32.cpp
//
// INITIAL DATE:    8/28/02
//
// COPYRIGHT:		Copyright © 2002 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbBitmask32.h"


//=============================================================================
// Public static functions
//=============================================================================

//---------------------------------------
//^ The OneBitOn function returns a mask with the given bit on
//  The resultant bitmask is in reverse order, requesting whichBit of 1
//  returns 0x00000001 not 0x80000000
BITMASK32 vdbBitmask32::OneBitOn( int whichBit )
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

	else if ( whichBit == 17 )
		return 0x00010000;
	else if ( whichBit == 18 )
		return 0x00020000;
	else if ( whichBit == 19 )
		return 0x00040000;
	else if ( whichBit == 20 )
		return 0x00080000;

	else if ( whichBit == 21 )
		return 0x00100000;
	else if ( whichBit == 22 )
		return 0x00200000;
	else if ( whichBit == 23 )
		return 0x00400000;
	else if ( whichBit == 24 )
		return 0x00800000;

	else if ( whichBit == 25 )
		return 0x01000000;
	else if ( whichBit == 26 )
		return 0x02000000;
	else if ( whichBit == 27 )
		return 0x04000000;
	else if ( whichBit == 28 )
		return 0x08000000;

	else if ( whichBit == 29 )
		return 0x10000000;
	else if ( whichBit == 30 )
		return 0x20000000;
	else if ( whichBit == 31 )
		return 0x40000000;
	else if ( whichBit == 32 )
		return 0x80000000;

	else
		return 0x0000;
}


//---------------------------------------
//^ The OneBitOff function returns a mask with the given bit off
//  The resultant bitmask is in reverse order, requesting whichBit of 1
//  returns 0xFFFFFFFE not 0x8FFFFFFF
BITMASK32 vdbBitmask32::OneBitOff( int whichBit )
{
	return 0xFFFFFFFF ^ OneBitOn( whichBit );
}


//---------------------------------------
//^ The CountBitsOn function returns a count of the number of bits
//  which are 1's
//
int vdbBitmask32::CountBitsOn( BITMASK32 mask )
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

	if ( mask & 0x00010000 )	count++;
	if ( mask & 0x00020000 )	count++;
	if ( mask & 0x00040000 )	count++;
	if ( mask & 0x00080000 )	count++;
	if ( mask & 0x00100000 )	count++;
	if ( mask & 0x00200000 )	count++;
	if ( mask & 0x00400000 )	count++;
	if ( mask & 0x00800000 )	count++;
	if ( mask & 0x01000000 )	count++;
	if ( mask & 0x02000000 )	count++;
	if ( mask & 0x04000000 )	count++;
	if ( mask & 0x08000000 )	count++;
	if ( mask & 0x10000000 )	count++;
	if ( mask & 0x20000000 )	count++;
	if ( mask & 0x40000000 )	count++;
	if ( mask & 0x80000000 )	count++;

	return count;
}


//-------------------------------------
void vdbBitmask32::BitsToString( BITMASK32 mask, char* sz, int bitmaskSize )
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

