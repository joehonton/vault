//=============================================================================
//
// CLASS:           vdbBitmask8
//
// OVERVIEW:        Public static functions for converting an integer to
//                  an 8-bit bitmask
//
// FILE NAME:       vdbBitmask8.h
//
// INITIAL DATE:    9/14/2003
//
// COPYRIGHT:		Copyright © 2003 by Joe Honton
//
//=============================================================================

#ifndef __vdbBitmask8_h
#define __vdbBitmask8_h

typedef unsigned char BITMASK8;


class vdbBitmask8
{
public:
	static BITMASK8 OneBitOn( int whichBit );
	static BITMASK8 OneBitOff( int whichBit );
	static int CountBitsOn( BITMASK8 mask );
	static void BitsToString( BITMASK8 mask, char* sz, int bitmaskSize );
};

#endif // __vdbBitmask_h

