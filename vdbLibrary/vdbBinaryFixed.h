//=============================================================================
//
// FILE NAME:       vdbBinaryFixed.cpp
//
// ESSENCE:         A fixed size binary object.
//                  _actualLength always equals _maxLength
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    12/2/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __vdbBinaryFixed_h
#define __vdbBinaryFixed_h

#ifndef __vdbBinaryObject_h
	#include "vdbBinaryObject.h"
#endif

#include <istream>
#include <ostream>


class vdbBinaryFixed : public vdbBinaryObject
{
public:
	// Essential class functions
	vdbBinaryFixed( long maxLength );
	vdbBinaryFixed( void* pData, long dataLength );
	virtual ~vdbBinaryFixed();
	vdbBinaryFixed( const vdbBinaryObject& rhs );
	vdbBinaryFixed( const vdbBinaryFixed& rhs );
	const vdbBinaryFixed& operator= ( const vdbBinaryFixed& rhs );

	// Multiple byte access functions
	virtual void SetData( void* pData, long& dataLength );

	// Reset length function
	virtual long MaxLength() const;
	virtual void MaxLength( long maxLength );

	// I/O stream functions
	friend std::ostream& operator<< ( std::ostream& os, const vdbBinaryFixed& obj );
	friend std::istream& operator>> ( std::istream& is, vdbBinaryFixed& obj );
};

#endif  // __vdbBinaryFixed_h

