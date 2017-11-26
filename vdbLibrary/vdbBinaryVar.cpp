//=============================================================================
//
// FILE NAME:       vdbBinaryVar.cpp
//
// ESSENCE:         A variably-sized binary object with a maximum allowable size.
//                  The bytes between _actualLength and _maxLength are always
//                  allocated and available for use and are set to 0x0.  Thus the
//                  maxLength must be greater than 0 and must not be NO_MAX_LENGTH
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    12/2/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#include <vdbWindows.h>
#include "vdbBinaryVar.h"
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
vdbBinaryVar::vdbBinaryVar( long maxLength )
	: vdbBinaryObject()
{
	assert( maxLength > 0 );
	_maxLength = maxLength;
	_actualLength = 0;

	_pData = new char[_maxLength];
	if ( _pData == 0 ) throw vdbMemoryException();
	memset( _pData, 0x0, _maxLength );
}


//------------------------------------------
//^^ constructor with initialization
vdbBinaryVar::vdbBinaryVar( void* pData, long dataLength, long maxLength )
	: vdbBinaryObject()
{
	assert( pData != 0 );
	assert( dataLength >= 0 );
	assert( dataLength <= maxLength );
	assert( maxLength > 0 );

	_maxLength = maxLength;
	_actualLength = dataLength;

	_pData = new char[_maxLength];
	if ( _pData == 0 ) throw vdbMemoryException();
	
	memcpy( _pData, pData, _actualLength );

	if ( _actualLength < _maxLength )
		memset( &_pData[_actualLength], 0x0, _maxLength - _actualLength );
}


//------------------------------------------
//^^ destructor
vdbBinaryVar::~vdbBinaryVar()
{
}


//------------------------------------------
//^^ copy constructor
vdbBinaryVar::vdbBinaryVar( vdbBinaryObject& rhs )
	: vdbBinaryObject()
{
	assert( rhs.MaxLength() > 0 );
	_maxLength = rhs.MaxLength();
	_actualLength = rhs.ActualLength();

	_pData = new char[_maxLength];
	if ( _pData == 0 ) throw vdbMemoryException();
	memcpy( _pData, (char*) rhs.GetDataPointer(), _maxLength );
}


//------------------------------------------
//^^ copy constructor
vdbBinaryVar::vdbBinaryVar( const vdbBinaryVar& rhs )
	: vdbBinaryObject()
{
	assert( rhs.MaxLength() > 0 );
	_maxLength = rhs.MaxLength();
	_actualLength = rhs.ActualLength();

	_pData = new char[_maxLength];
	if ( _pData == 0 ) throw vdbMemoryException();
	memcpy( _pData, rhs._pData, _maxLength );
}


//------------------------------------------
//^^ const vdbBinaryVar& assignment operator
//^  The maximum length of this object is respected
const vdbBinaryVar& vdbBinaryVar::operator= ( const vdbBinaryVar& rhs )
{
	// nothing to do?
	if ( this == &rhs )
		return *this;

	_actualLength = min( _maxLength, rhs.ActualLength() );
	memcpy( _pData, rhs._pData, _actualLength );

	return *this;
}


//=============================================================================
// Multiple byte access functions
//=============================================================================

//------------------------------------------
//^ The SetData function copies bytes from the caller's buffer to this object
//> pData is the source
//> dataLength on input is the maximum number of bytes to transfer
//> dataLength on output is the actual number of bytes transfered
//
void vdbBinaryVar::SetData( void* pData, long& dataLength )
{
	assert( pData != 0 );
	
	if ( dataLength < 0 )
		dataLength = 0;

	if ( dataLength > _maxLength )
		dataLength = _maxLength;

	_actualLength = dataLength;

	if ( _actualLength > 0 )
		memcpy( _pData, pData, _actualLength );

	if ( _actualLength < _maxLength )
		memset( &_pData[_actualLength], 0x0, _maxLength - _actualLength );
}


//=============================================================================
// Reset length function
//=============================================================================

//------------------------------------------
//^ The MaxLength function returns the maximum allowable length of the object
//
long vdbBinaryVar::MaxLength() const
{
	return _maxLength;
}


//------------------------------------------
//^ The MaxLength function changes the maximum allowable length of the object.
//  Note that this function deletes and reallocates the storage buffer and
//  sets everything to zeroes.
//  This behavior is different from vdbBinaryObject
//
void vdbBinaryVar::MaxLength( long maxLength )
{
	if ( maxLength < 0 )
		maxLength = 0;
	_maxLength = maxLength;
	_actualLength = maxLength;

	delete[] _pData; _pData = 0;

	if ( _actualLength > 0 )
	{
		_pData = new char[_actualLength];
		if ( _pData == 0 ) throw vdbMemoryException();
		memset( _pData, 0x0, _actualLength );
	}
}


//=============================================================================
// I/O stream functions
//=============================================================================

//------------------------------------------
//^^ ostream << operator writes the object to the output stream
//
std::ostream& operator<< ( std::ostream& os, const vdbBinaryVar& obj )
{
	obj.Put( os );
	return os;
}


//------------------------------------------
//^^ istream >> operator reads the object from the input stream.
//
std::istream& operator>> ( std::istream& is, vdbBinaryVar& obj )
{
	obj.Get( is );
	return is;
}


