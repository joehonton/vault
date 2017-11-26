//=============================================================================
//
// CLASS:           vdbTimeStampDB
//
// ESSENCE:         vdbTimeStamp-derived class for use with vdbObject
//                  and vdbStatement
//
// FILE NAME:       vdbTimeStampDB.h
//
// INITIAL DATE:    12/5/2000
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================
 
#ifndef __vdbTimeStampDB_h
#define __vdbTimeStampDB_h
 
#ifndef __vdbTimeStamp_h
	#include "vdbTimeStamp.h"
#endif

class vdbTimeStampDB : public vdbTimeStamp
{
public:
	// constructors
	vdbTimeStampDB();
	vdbTimeStampDB( int totalLength, int fractionalDigits );
	vdbTimeStampDB( const vdbTimeStamp& rhs );
	vdbTimeStampDB( const vdbTimeStampDB& rhs );

	inline operator vdbTimeStamp&()				{ return (vdbTimeStamp&)( *this ); }
	inline DBTIMESTAMP* GetDataPointer()		{ return &_ts; }
	inline int FractionalDigits()				{ return _fractionalDigits; }
	inline int TotalLength()					{ return _totalLength; }

private:
	int _fractionalDigits;		// number of digits in fractional seconds part (from 0 to 3)
	int _totalLength;			// use 23 for "yyyy-mm-dd hh:mm:ss.fff"
								// use 16 for "yyyy-mm-dd hh:mm"

	friend class vdbStatement;
};


#endif  // __vdbTimeStampDB_h
