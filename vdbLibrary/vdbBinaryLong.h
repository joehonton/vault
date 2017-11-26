//=============================================================================
//
// FILE NAME:       vdbBinaryLong.h
//
// ESSENCE:         A variably-sized binary object with no pre-set maximum size
//                  for use by a vdbObject-derived class containing a column
//                  of type "long varbinary".
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    12/6/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __vdbBinaryLong_h
#define __vdbBinaryLong_h

#ifndef __vdbBinaryObject_h
	#include "vdbBinaryObject.h"
#endif


class vdbBinaryLong : private vdbBinaryObject
{
public:
	// Essential class functions
	vdbBinaryLong();
	vdbBinaryLong( long maxLength );
	vdbBinaryLong( void* pData, long dataLength, long maxLength );
	virtual ~vdbBinaryLong();
	vdbBinaryLong( const vdbBinaryLong& rhs );
	const vdbBinaryLong& operator= ( const vdbBinaryLong& rhs );

	void Initialize( long maxLength );

	// Access functions
	void* GetDataPointer();
	long ActualLength() const;
	void GetData( void* pData, long& dataLength ) const;
	void SetData( void* pData, long& dataLength );
	void SetDataAt( void* pData, long dataLength, long index );

	// Single byte access functions
	char& operator[] ( long index );
	const char& operator[] ( long index ) const;
	char GetByte( long index ) const;
	void SetByte( long index, char ch );

	// Information functions
	inline bool IsModified() const					{ return _bModified; }
	inline bool IsInMemory() const					{ return _bInMemory; }
	inline void Modified( bool b = true )			{ _bModified = b; }
	inline void InMemory( bool b )					{ _bInMemory = b; }

protected:
	bool _bModified;
	bool _bInMemory;
};

#endif  // __vdbBinaryLong_h

