//=============================================================================
//
// FILE NAME:       vdbLong.cpp
//
// ESSENCE:         long for use in iostreams
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    2/4/96
//
// COPYRIGHT:       Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __vdbLong_h
#define __vdbLong_h

class ostream;
class istream;


class vdbLong
{
public:
	//  Constructor
	vdbLong();
	vdbLong( long l );

	//  Assignment operators
	const vdbLong& operator= ( const long l );

	// Access
	operator long() const;

	// Stream operators
	friend std::ostream& operator<< ( std::ostream& os, const vdbLong& l );
	friend std::istream& operator>> ( std::istream& is, vdbLong& l );

public:
	long _l;
};


#endif  // __vdbLong_h

