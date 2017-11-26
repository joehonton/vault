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

#include <vdbWindows.h>
#include "vdbBinaryFixed.h"
#include "vdbException.h"
#include <memory.h>
#include <istream>
#include <ostream>
#include <vdbAssert.h>


//=============================================================================
//  Essential class functions
//=============================================================================

//------------------------------------------
//^^ default constructor
vdbBinaryFixed::vdbBinaryFixed( long maxLength )
	: vdbBinaryObject( maxLength )
{
	assert( maxLength > 0 );

	_actualLength = maxLength;
	_pData = new char[_actualLength];
	if ( _pData == 0 ) throw vdbMemoryException();

	memset( _pData, 0x0, _actualLength );
}


//------------------------------------------
//^^ constructor with initialization
vdbBinaryFixed::vdbBinaryFixed( void* pData, long dataLength )
	: vdbBinaryObject( pData, dataLength, dataLength )
{
}


//------------------------------------------
//^^ destructor
vdbBinaryFixed::~vdbBinaryFixed()
{
}


//------------------------------------------
//^^ copy constructor
vdbBinaryFixed::vdbBinaryFixed( const vdbBinaryObject& rhs )
	: vdbBinaryObject( rhs )
{
	_maxLength = _actualLength;
}


//------------------------------------------
//^^ copy constructor
vdbBinaryFixed::vdbBinaryFixed( const vdbBinaryFixed& rhs )
	: vdbBinaryObject( (vdbBinaryObject&) rhs )
{
}


//------------------------------------------
//^^ const vdbBinaryFixed& assignment operator
//^  The maximum length of this object is changed to be the same as the
//   right hand side object
const vdbBinaryFixed& vdbBinaryFixed::operator= ( const vdbBinaryFixed& rhs )
{
	// nothing to do?
	if ( this == &rhs )
		return *this;

	// delete existing string
	delete[] _pData; _pData = 0;

	_actualLength = rhs._actualLength;
	_maxLength = rhs._maxLength;

	// special case
	if ( _actualLength == 0 )
		return *this;

	// reallocate and copy		
	_pData = new char[_actualLength];
	if ( _pData == 0 ) throw vdbMemoryException();
	memcpy( _pData, rhs._pData, _actualLength );

	return *this;
}


//=============================================================================
// Multiple byte access functions
//=============================================================================

//------------------------------------------
//^ The SetData function copies bytes from the caller's buffer to this object.
//  If the source is larger than this object's fixed size the source is truncated.
//  If the source is smaller than this object's fixed size, the trailing bytes
//  are padded with zeroes.
//> pData is the source
//> dataLength on input is the maximum number of bytes to transfer
//> dataLength on output is the actual number of bytes transfered
//
void vdbBinaryFixed::SetData( void* pData, long& dataLength )
{
	assert( pData != 0 );
	
	if ( dataLength < 0 )
		dataLength = 0;

	if ( dataLength > _maxLength )
		dataLength = _maxLength;

	if ( dataLength > 0 )
		memcpy( _pData, pData, dataLength );

	if ( dataLength < _maxLength )
		memset( &_pData[dataLength], 0x0, _maxLength - dataLength );
}


//=============================================================================
// Reset length function
//=============================================================================

//------------------------------------------
//^ The MaxLength function returns the maximum allowable length of the object
//
long vdbBinaryFixed::MaxLength() const
{
	return _maxLength;
}


//------------------------------------------
//^ The MaxLength function changes the maximum allowable length of the object.
//  Note that this function deletes and reallocates the storage buffer and
//  sets everything to zeroes.
//  This behavior is different from vdbBinaryObject
//
void vdbBinaryFixed::MaxLength( long maxLength )
{
	_maxLength = maxLength;
	_actualLength = maxLength;

	delete[] _pData; _pData = 0;
	_pData = new char[_actualLength];
	if ( _pData == 0 ) throw vdbMemoryException();
	memset( _pData, 0x0, _actualLength );
}


//=============================================================================
// I/O stream functions
//=============================================================================

//------------------------------------------
//^^ ostream << operator writes the object to the output stream
//
std::ostream& operator<< ( std::ostream& os, const vdbBinaryFixed& obj )
{
	obj.Put( os );
	return os;
}


//------------------------------------------
//^^ istream >> operator reads the object from the input stream.
//
std::istream& operator>> ( std::istream& is, vdbBinaryFixed& obj )
{
	obj.Get( is );
	return is;
}


