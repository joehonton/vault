//=============================================================================
//
// CLASS:           vdbTimeStampDB
//
// ESSENCE:         vdbTimeStampDB-derived class for use with vdbObject
//                  and vdbStatement
//
// FILE NAME:       vdbTimeStampDB.cpp
//
// INITIAL DATE:    12/5/2000
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================
 
#include <vdbTimeStampDB.h>


//=============================================================================
// Constructors
//=============================================================================

//--------------------------------
// Default constructor
vdbTimeStampDB::vdbTimeStampDB()
	: vdbTimeStamp(),
	  _totalLength( 23 ),
	  _fractionalDigits( 3 )
{
}


//-------------------------------------------
//^ Constructor for use by vdbObject-derived classes
vdbTimeStampDB::vdbTimeStampDB( int totalLength, int fractionalDigits )
	: vdbTimeStamp(),
	  _totalLength( totalLength ),
	  _fractionalDigits( fractionalDigits )
{
}


//-------------------------------------------
vdbTimeStampDB::vdbTimeStampDB( const vdbTimeStamp& rhs )
	: vdbTimeStamp( rhs ),
	  _totalLength( 23 ),
	  _fractionalDigits( 3 )
{
}


//-------------------------------------------
vdbTimeStampDB::vdbTimeStampDB( const vdbTimeStampDB& rhs )
	: vdbTimeStamp( (vdbTimeStamp&) rhs ),
	  _totalLength( rhs._totalLength ),
	  _fractionalDigits( rhs._fractionalDigits )
{
}

