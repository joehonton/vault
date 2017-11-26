//=============================================================================
//
// CLASS:           vdbBitmask32
//
// OVERVIEW:        Public static functions for converting an integer to
//                  a 32-bit bitmask
//
// FILE NAME:       vdbBitmask32.h
//
// INITIAL DATE:    8/28/02
//
// COPYRIGHT:		Copyright © 2002 by Joe Honton
//
//=============================================================================

#ifndef __vdbBitmask32_h
#define __vdbBitmask32_h

typedef unsigned int BITMASK32;


class vdbBitmask32
{
public:
	static BITMASK32 OneBitOn( int whichBit );
	static BITMASK32 OneBitOff( int whichBit );
	static int CountBitsOn( BITMASK32 mask );
	static void BitsToString( BITMASK32 mask, char* sz, int bitmaskSize );
};

#endif // __vdbBitmask32_h

