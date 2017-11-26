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

#include <vdbWindows.h>
#include "vdbBinaryObject.h"
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
vdbBinaryObject::vdbBinaryObject()
	: _pData( 0 ),
	  _actualLength( 0 ),
	  _maxLength( NO_MAX_LENGTH )
{
}


//------------------------------------------
//^^ simple constructor
vdbBinaryObject::vdbBinaryObject( long maxLength )
	: _pData( 0 ),
	  _actualLength( 0 ),
	  _maxLength( maxLength )
{
}


//------------------------------------------
//^^ constructor with initialization
vdbBinaryObject::vdbBinaryObject( void* pData, long dataLength, long maxLength )
	: _pData( 0 ),
	  _actualLength( 0 ),
	  _maxLength( maxLength )
{
	assert( _maxLength >= -1 );

	if ( dataLength < 0 )
		dataLength = 0;

	if ( _maxLength == NO_MAX_LENGTH )
		_actualLength = dataLength;
	else
		_actualLength = min( dataLength, _maxLength );

	if ( pData != 0 && _actualLength > 0 )
	{
		_pData = new char[_actualLength];
		if ( _pData == 0 ) throw vdbMemoryException();
		memcpy( _pData, pData, _actualLength );
	}
}


//------------------------------------------
//^^ destructor
vdbBinaryObject::~vdbBinaryObject()
{
	delete[] _pData; _pData = 0;
}


//------------------------------------------
//^^ copy constructor
vdbBinaryObject::vdbBinaryObject( const vdbBinaryObject& rhs )
	: _pData( 0 ),
	  _actualLength( 0 ),
	  _maxLength( rhs._maxLength )
{
	// nothing to do?
	if ( rhs._actualLength <= 0 )
		return; 

	// allocate new string
	_pData = new char[rhs._actualLength];
	if ( _pData == 0 ) throw vdbMemoryException();
	
	// copy bytes
	memcpy( _pData, rhs._pData, rhs._actualLength );
	_actualLength = rhs._actualLength;
}


//------------------------------------------
//^^ const vdbBinaryObject& assignment operator
//^  The maximum length of this object is respected
const vdbBinaryObject& vdbBinaryObject::operator= ( const vdbBinaryObject& rhs )
{
	// nothing to do?
	if ( this == &rhs )
		return *this;

	// delete existing string
	delete[] _pData; _pData = 0;

	// special case
	if ( rhs._actualLength <= 0 )
	{
		_actualLength = 0;
		return *this;
	}

	if ( _maxLength == NO_MAX_LENGTH )
		_actualLength = rhs._actualLength;
	else
		_actualLength = min( rhs._actualLength, _maxLength );

	// reallocate and copy		
	if ( rhs._pData != 0 && _actualLength > 0 )
	{
		_pData = new char[_actualLength];
		if ( _pData == 0 ) throw vdbMemoryException();
		memcpy( _pData, rhs._pData, _actualLength );
	}

	return *this;
}


//=============================================================================
// Concatenation operators
//=============================================================================

//------------------------------------------
//^^ const vdbBinaryObject& concatenation operator
const vdbBinaryObject& vdbBinaryObject::operator+= ( const vdbBinaryObject& rhs )
{
	// nothing to do?
	if ( rhs._actualLength <= 0 )
		return *this;

	// no more room?
	if ( _actualLength == _maxLength )
		return *this;

	// very little to do?
	if ( _actualLength == 0 )
		return operator=( rhs );

	long newLength = 0;
	if ( _maxLength == NO_MAX_LENGTH )
		newLength = _actualLength + rhs._actualLength;
	else
		newLength = min( _actualLength + rhs._actualLength, _maxLength );

	// reallocate new string
	char* pNewData = new char[newLength];
	if ( pNewData == 0 ) throw vdbMemoryException();

	// copy bytes
	memcpy( &pNewData[0], _pData, _actualLength );
	memcpy( &pNewData[_actualLength], rhs._pData, newLength - _actualLength );

	// delete existing string
	delete[] _pData; _pData = 0;

	// repositition pointer
	_pData = pNewData;
	_actualLength = newLength;

	return *this;
}


//------------------------------------------
//^^ const vdbBinaryObject& + const vdbBinaryObject& operator
vdbBinaryObject operator+ ( const vdbBinaryObject& object1, const vdbBinaryObject& object2 )
{
	// very little to do?
	if ( object1.ActualLength() == 0 )
		return object2;
	if ( object2.ActualLength() == 0 )
		return object1;

	// copy first string
	vdbBinaryObject newObject( object1 );

	// concatenate second string
	newObject += object2;

	return newObject;
}


//=============================================================================
// Multiple byte access functions
//=============================================================================

//------------------------------------------
//^ The GetDataPointer function returns a pointer to the actual storage location
//  of the binary data.
//^ Caution this will return NULL if the actual length is 0
//
void* vdbBinaryObject::GetDataPointer()
{
	return _pData;
}


//------------------------------------------
//^ The GetData function copies bytes to the caller's buffer from this object
//> pData is the destination
//> dataLength on input is the maximum number of bytes to transfer
//> dataLength on output is the actual number of bytes transfered
//
void vdbBinaryObject::GetData( void* pData, long& dataLength ) const
{
	assert( pData != 0 );
	
	if ( dataLength < 0 )
		dataLength = 0;

	if ( dataLength > _actualLength )
		dataLength = _actualLength;

	memcpy( pData, _pData, dataLength );
}


//------------------------------------------
//^ The SetData function copies bytes from the caller's buffer to this object
//> pData is the source.  It may be NULL if dataLength is 0.
//> dataLength on input is the maximum number of bytes to transfer
//> dataLength on output is the actual number of bytes transfered
//
void vdbBinaryObject::SetData( void* pData, long& dataLength )
{
	if ( dataLength < 0 )
		dataLength = 0;

	// delete existing string
	delete[] _pData; _pData = 0;

	if ( _maxLength == NO_MAX_LENGTH )
		_actualLength = dataLength;
	else
		_actualLength = min( dataLength, _maxLength );

	// reallocate and copy		
	if ( _actualLength > 0 )
	{
		_pData = new char[_actualLength];
		if ( _pData == 0 ) throw vdbMemoryException();
		memcpy( _pData, pData, _actualLength );
	}
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
char& vdbBinaryObject::operator[] ( long index )
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
const char& vdbBinaryObject::operator[] ( long index ) const
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
char vdbBinaryObject::GetByte( long index ) const
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
void vdbBinaryObject::SetByte( long index, char ch )
{
	assert( index < _actualLength );
	assert( index >= 0 );

	_pData[index] = ch;
}



//=============================================================================
// Comparison operators
//=============================================================================

//------------------------------------------
//^^ const vdbBinaryObject operator==
bool vdbBinaryObject::operator== ( const vdbBinaryObject& rhs ) const
{
	// nothing to do?
	if ( this == &rhs )
		return true;

	if( _actualLength == 0 && rhs._actualLength == 0 )
		return true;

	if ( _actualLength != rhs._actualLength )
		return false;

	if ( _actualLength == 0  )
		return false;

	if ( rhs._actualLength == 0 )
		return false;

	for ( long i = 0; i < _actualLength; i++ )
		if ( _pData[i] != rhs._pData[i] )
			return false;

	return true;
}


//------------------------------------------
//^^ const vdbBinaryObject operator!=
bool vdbBinaryObject::operator!= ( const vdbBinaryObject& rhs ) const
{
	// nothing to do?
	if ( this == &rhs )
		return false;

	if ( _actualLength == 0 && rhs._actualLength == 0 )
		return false;

	if ( _actualLength != rhs._actualLength )
		return true;

	if ( _actualLength == 0  )
		return true;

	if ( rhs._actualLength == 0 )
		return true;

	for ( long i = 0; i < _actualLength; i++ )
		if ( _pData[i] != rhs._pData[i] )
			return true;

	return false;
}


//------------------------------------------
//^^ Compare const vdbBinaryObject
//> numBytess is the maximum number of bytes to compare.  If this value
//  is zero, the entire string is compared.
//< true if equal, false if not equal
bool vdbBinaryObject::Compare( const vdbBinaryObject& rhs, long numBytes ) const
{
	assert( numBytes >= 0 );

	// nothing to do?
	if ( this == &rhs )
		return true;

	if ( numBytes == 0 )
		numBytes = _actualLength;

	if ( numBytes > _actualLength )
		return false;

	if ( numBytes > rhs._actualLength )
		return false;

	for ( long i = 0; i < numBytes; i++ )
		if ( _pData[i] != rhs._pData[i] )
			return false;

	return true;
}


//=============================================================================
// Information functions
//=============================================================================

//------------------------------------------
//^ The ActualLength function returns the actual length of the object
//
long vdbBinaryObject::ActualLength() const
{
	return _actualLength;
}


//------------------------------------------
//^ The ActualLengthPointer function returns the address of the variable holding
//  the actual length of the object.
//^ This function was written for use when calling vdbStatement::BindVarBinary()
//
long* vdbBinaryObject::ActualLengthPointer()
{
	return &_actualLength;
}


//------------------------------------------
//^ The MaxLength function returns the maximum allowable length of the object
//  or NO_MAX_LENGTH if there is no maximum
//
long vdbBinaryObject::MaxLength() const
{
	return _maxLength;
}


//------------------------------------------
//^ The MaxLength function sets the maximum allowable length of the object.
//  Use NO_MAX_LENGTH if there is no maximum.
//
void vdbBinaryObject::MaxLength( long maxLength )
{
	_maxLength = maxLength;
}


//=============================================================================
// Modification routines
//=============================================================================

//------------------------------------------
//^ The Replace function replaces every occurance of the first character 
//  with the second character.
//^ Caution:  Visual C++ 5.0 debug builds cause an assertion upon deletion
//  of this string when the replacement value is '\0';  
//> oldValue is the character to replace.
//> newValue is the character to replace it with.
//
void vdbBinaryObject::Replace( char oldValue, char newValue )
{
	for ( long i = 0; i < _actualLength; i++ )
		if ( _pData[i] == oldValue )
			_pData[i] = newValue;
}


//=============================================================================
// Sub-string matching
//=============================================================================

//------------------------------------------
//^ The Find function finds the first byte in the object matching the
//  specified character.
//> ch is the character to search for.
//< The return value is the zero-based index of the first character found,
//  or -1 if no such character was found.
//
long vdbBinaryObject::Find( char ch ) const
{
    // nothing to do?
    if ( _actualLength == 0 )
		return -1;

	// find first single character
	for ( long i = 0; i < _actualLength; i++ )
		if ( _pData[i] == ch )
			return i;

	return -1;
}


//------------------------------------------
//^ The FindNext function finds the first matching character after the starting
//  position.
//> ch is the character to search for.
//> startingPosition is the zero-based index of the first character to begin
//  searching with.
//< The return value is the zero-based index of the first character found,
//  or -1 if no such character was found.
//
long vdbBinaryObject::FindNext( char ch, long startingPosition ) const
{
    // nothing to do?
    if ( _actualLength == 0 )
		return -1;

    if ( startingPosition >= _actualLength )
		return -1;

    if ( startingPosition < 0 )
		return -1;

	// find first single character
	for ( long i = startingPosition; i < _actualLength; i++ )
		if ( _pData[i] == ch )
			return i;

	return -1;
}


//=============================================================================
//  iostream functions
//=============================================================================

//------------------------------------------
//^ The Put function writes the object to the output stream with ESCAPE separators
//  between the actual length and the object itself
//^ This function is suitable for binary files
//
void vdbBinaryObject::Put( std::ostream& os ) const
{
	os.put( '\033' );
	os << _actualLength;
	os.put( '\033' );
	if ( _actualLength > 0 )
		os.write( _pData, _actualLength );
}


//------------------------------------------
//^ The Get function expects a ESCAPE separator character followed by the
//  length of the object followed by a second ESCAPE separator character
//  followed by the object itself.
//^ This function is suitable for binary files
//
void vdbBinaryObject::Get( std::istream& is )
{
	long dataLength;
	char separator;

	if ( is.fail() )
		return;

	// delete existing string
	delete[] _pData; _pData = 0;

	// read the length
	is.get( separator );
	is >> dataLength;
	is.get( separator );
	
	if ( _maxLength == NO_MAX_LENGTH )
		_actualLength = dataLength;
	else
		_actualLength = min( dataLength, _maxLength );

	// reallocate
	if ( _actualLength > 0 )
	{
		_pData = new char[_actualLength];
		if ( _pData == 0 ) throw vdbMemoryException();
	}

	// read em in
	if ( _actualLength > 0 )
		is.read( _pData, _actualLength );

	// read and discard any extra
	if ( _actualLength < dataLength )
	{
		char* pDummy = new char[dataLength - _actualLength];
		is.read( pDummy, dataLength - _actualLength );
	}
}


//------------------------------------------
//^^ ostream << operator writes the object to the output stream
//
std::ostream& operator<< ( std::ostream& os, const vdbBinaryObject& obj )
{
	obj.Put( os );
	return os;
}


//------------------------------------------
//^^ istream >> operator reads the object from the input stream.
//
std::istream& operator>> ( std::istream& is, vdbBinaryObject& obj )
{
	obj.Get( is );
	return is;
}


