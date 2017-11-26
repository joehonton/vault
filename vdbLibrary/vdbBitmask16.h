//=============================================================================
//
// CLASS:           vdbBitmask16
//
// OVERVIEW:        Public static functions for converting an integer to
//                  a 16-bit bitmask
//
// FILE NAME:       vdbBitmask16.h
//
// INITIAL DATE:    9/14/2003
//
// COPYRIGHT:		Copyright © 2003 by Joe Honton
//
//=============================================================================

#ifndef __vdbBitmask16_h
#define __vdbBitmask16_h

typedef unsigned short int BITMASK16;


class vdbBitmask16
{
public:
	static BITMASK16 OneBitOn( int whichBit );
	static BITMASK16 OneBitOff( int whichBit );
	static int CountBitsOn( BITMASK16 mask );
	static void BitsToString( BITMASK16 mask, char* sz, int bitmaskSize );
};

#endif // __vdbBitmask16_h

