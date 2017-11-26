//=============================================================================
//
// FILE NAME:       vdbBinaryVar.cpp
//
// ESSENCE:         A variably-sized binary object with a maximum allowable size.
//                  The bytes between _actualLength and _maxLength are always
//                  allocated and available for use and are set to 0x0.
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    12/2/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __vdbBinaryVar_h
#define __vdbBinaryVar_h

#ifndef __vdbBinaryObject_h
	#include "vdbBinaryObject.h"
#endif


class vdbBinaryVar : public vdbBinaryObject
{
public:
	// Essential class functions
	vdbBinaryVar( long maxLength );
	vdbBinaryVar( void* pData, long dataLength, long maxLength );
	virtual ~vdbBinaryVar();
	vdbBinaryVar( vdbBinaryObject& rhs );
	vdbBinaryVar( const vdbBinaryVar& rhs );
	const vdbBinaryVar& operator= ( const vdbBinaryVar& rhs );

	// Multiple byte access functions
	virtual void SetData( void* pData, long& dataLength );

	// Reset length function
	virtual long MaxLength() const;
	virtual void MaxLength( long maxLength );

	// I/O stream functions
	friend std::ostream& operator<< ( std::ostream& os, const vdbBinaryVar& obj );
	friend std::istream& operator>> ( std::istream& is, vdbBinaryVar& obj );
};

#endif  // __vdbBinaryVar_h

