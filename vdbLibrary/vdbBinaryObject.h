//=============================================================================
//
// FILE NAME:       vdbBinaryObject.cpp
//
// ESSENCE:         A variably-sized binary object with no pre-set maximum size.
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    12/2/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __vdbBinaryObject_h
#define __vdbBinaryObject_h

#include <ostream>
#include <istream>

#define NO_MAX_LENGTH  -1		// may be used in constructors or MaxLength()


class vdbBinaryObject
{
public:
	// Essential class functions
	vdbBinaryObject();
	vdbBinaryObject( long maxLength );
	vdbBinaryObject( void* pData, long dataLength, long maxLength );
	virtual ~vdbBinaryObject();
	vdbBinaryObject( const vdbBinaryObject& rhs );
	const vdbBinaryObject& operator= ( const vdbBinaryObject& rhs );

	// Concatenation operators
	const vdbBinaryObject& operator+= ( const vdbBinaryObject& rhs );
	friend vdbBinaryObject operator+ ( const vdbBinaryObject& object1, const vdbBinaryObject& object2 );

	// Multiple byte access functions
	void* GetDataPointer();
	void GetData( void* pData, long& dataLength ) const;
	void SetData( void* pData, long& dataLength );

	// Single byte access functions
	char& operator[] ( long index );
	const char& operator[] ( long index ) const;
	char GetByte( long index ) const;
	void SetByte( long index, char ch );
	
	// Comparison operators
	bool operator== ( const vdbBinaryObject& rhs ) const;
	bool operator!= ( const vdbBinaryObject& rhs ) const;
	bool Compare( const vdbBinaryObject& rhs, long numBytes = 0 ) const;

	// Information functions
	long ActualLength() const;
	long* ActualLengthPointer();
	virtual long MaxLength() const;
	virtual void MaxLength( long maxLength );

	// Modification routines
	void Replace( char oldValue, char newValue );

	// Sub-string matching
	long Find( char ch ) const;
	long FindNext( char ch, long startingPosition ) const;

	// Stream operators
	void Put( std::ostream& os ) const;
	void Get( std::istream& is );

	friend std::ostream& operator<< ( std::ostream& os, const vdbBinaryObject& obj );
	friend std::istream& operator>> ( std::istream& is, vdbBinaryObject& obj );

protected:
	char* _pData;
	long _actualLength;
	long _maxLength;
};

#endif  // __vdbBinaryObject_h

