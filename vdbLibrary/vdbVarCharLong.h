//=============================================================================
//
// FILE NAME:       vdbVarCharLong.h
//
// ESSENCE:         A variably-sized string object with no pre-set maximum size
//                  for use by a vdbObject-derived class containing a column
//                  of type "long varchar".
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    12/6/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __vdbVarCharLong_h
#define __vdbVarCharLong_h

#ifndef __vdbString_h
	#include "vdbString.h"
#endif


class vdbVarCharLong : private vdbString
{
public:
	// Essential class functions
	vdbVarCharLong();
	vdbVarCharLong( long maxLength );
	vdbVarCharLong( void* pData, long dataLength, long maxLength );
	virtual ~vdbVarCharLong();
	vdbVarCharLong( const vdbVarCharLong& rhs );
	const vdbVarCharLong& operator= ( const vdbVarCharLong& rhs );

	// Access functions
	operator const char*() const;
	char* GetDataPointer();
	void SetData( const char* sz );
	void Append( const char* sz, int length );
	int Length() const;

	// Status functions
	inline bool IsModified() const					{ return _bModified; }
	inline bool IsInMemory() const					{ return _bInMemory; }
	inline void Modified( bool b = true )			{ _bModified = b; }
	inline void InMemory( bool b )					{ _bInMemory = b; }

protected:
	bool _bModified;
	bool _bInMemory;
};

#endif  // __vdbVarCharLong_h

