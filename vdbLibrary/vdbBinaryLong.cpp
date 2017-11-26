//=============================================================================
//
// FILE NAME:       vdbBinaryLong.cpp
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

#include <vdbWindows.h>
#include "vdbBinaryLong.h"
#include "vdbException.h"
#include <vdbAssert.h>


//=============================================================================
//  Essential class functions
//=============================================================================

//------------------------------------------
//^^ default constructor
vdbBinaryLong::vdbBinaryLong()
	: vdbBinaryObject(),
	  _bModified ( false ),
	  _bInMemory( false )
{
}


//------------------------------------------
//^^ simple constructor
vdbBinaryLong::vdbBinaryLong( long maxLength )
	: vdbBinaryObject( maxLength ),
	  _bModified ( false ),
	  _bInMemory( false )
{
}


//------------------------------------------
//^^ constructor with initialization
vdbBinaryLong::vdbBinaryLong( void* pData, long dataLength, long maxLength )
	: vdbBinaryObject( pData, dataLength, maxLength ),
	  _bModified ( false ),
	  _bInMemory( false )
{
}


//------------------------------------------
//^^ destructor
vdbBinaryLong::~vdbBinaryLong()
{
}


//------------------------------------------
//^^ copy constructor
vdbBinaryLong::vdbBinaryLong( const vdbBinaryLong& rhs )
	: vdbBinaryObject( (vdbBinaryObject&) rhs ),
	  _bModified ( rhs._bModified ),
	  _bInMemory( rhs._bInMemory )
{
}


//------------------------------------------
//^^ const vdbBinaryLong& assignment operator
const vdbBinaryLong& vdbBinaryLong::operator= ( const vdbBinaryLong& rhs )
{
	// nothing to do?
	if ( this == &rhs )
		return *this;

	vdbBinaryObject::operator=( rhs );
	_bModified  = rhs._bModified;
	_bInMemory = rhs._bInMemory;

	return *this;
}


//------------------------------------------
//^ The Initialize function sets the maximum length, allocates storage
//  and initializes memory to 0
//> maxLength is the new length.  It may be 0;
//
void vdbBinaryLong::Initialize( long maxLength )
{
	_maxLength = maxLength;
	_actualLength = maxLength;

	delete[] _pData; _pData = 0;
	if ( maxLength > 0 )
	{
		_pData = new char[_actualLength];
		if ( _pData == 0 ) throw vdbMemoryException();
		memset( _pData, 0x0, _actualLength );
	}
}


//------------------------------------------
//^ The GetDataPointer function returns a pointer to the actual storage location
//  of the binary data.
//^ Caution this will return NULL if the actual length is 0
//
void* vdbBinaryLong::GetDataPointer()
{
	return _pData;
}


//------------------------------------------
//^ The GetData function copies bytes to the caller's buffer from this object
//> pData is the destination
//> dataLength on input is the maximum number of bytes to transfer
//> dataLength on output is the actual number of bytes transfered
//
void vdbBinaryLong::GetData( void* pData, long& dataLength ) const
{
	vdbBinaryObject::GetData( pData, dataLength );
}


//------------------------------------------
void vdbBinaryLong::SetData( void* pData, long& dataLength )
{
	vdbBinaryObject::SetData( pData, dataLength );
	_bModified = true;
	_bInMemory = true;
}


//------------------------------------------
//^ The SetDataAt function replaces the data at the given offset with the supplied data
//> pData is the pointer to the binary data.
//> len is the actual length of the binary data.
//> index is the offset into the existing buffer of where to begin the replacement
//
void vdbBinaryLong::SetDataAt( void* pData, long dataLength, long index )
{
	// nothing to do?
	if ( pData == NULL || dataLength == 0 )
		return;

	if ( index < 0 || index >= _actualLength )
		return;

	long numBytes = min( dataLength, _actualLength - index );
	memcpy( &_pData[index], pData, numBytes );

	_bModified = true;
	_bInMemory = true;
}


//------------------------------------------
//^ The ActualLength function returns the actual length of the object
//
long vdbBinaryLong::ActualLength() const
{
	return _actualLength;
}


//=============================================================================
// Single byte access functions
//=============================================================================

//------------------------------------------
//^^ character access operator[]
//^ This overloaded operator[] is suitable for lefthand-side use.
//> index is the zero-based index into the string of the character to access.
//< returns the reference to the character at the zero-based index location
//  specified.
//
char& vdbBinaryLong::operator[] ( long index )
{
	assert( index < _actualLength );
	assert( index >= 0 );

	return _pData[index];
}


//------------------------------------------
//^^ character access operator[]
//> index is the zero-based index into the string of the character to return.
//< returns the character at the zero-based index location specified.
//
const char& vdbBinaryLong::operator[] ( long index ) const
{
	assert( index < _actualLength );
	assert( index >= 0 );

	return _pData[index];
}


//------------------------------------------
//^ The GetByte function returns the character at the specified index position.
//> index is the zero-based index into the string of the character to return.
//< returns the character at the zero-based index location specified.
//
char vdbBinaryLong::GetByte( long index ) const
{
	assert( index < _actualLength );
	assert( index >= 0 );

	return _pData[index];
}


//------------------------------------------
//^ The SetByte function replaces the character at the specified index position.
//> index is the zero-based index into the string of the character to replace.
//< returns the character at the zero-based index location specified.
//
void vdbBinaryLong::SetByte( long index, char ch )
{
	assert( index < _actualLength );
	assert( index >= 0 );

	_pData[index] = ch;
}



