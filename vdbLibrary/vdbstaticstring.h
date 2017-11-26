//=============================================================================
//
// FILE NAME:       vdbStaticString.h
//
// CONTENTS:        A class for storing null terminated strings
//
//=============================================================================

#ifndef __VDB_STATIC_STRING_H
#define __VDB_STATIC_STRING_H

#include <string.h>

//------------------------------------------
// The vdbStaticString Class holds a null terminated character array.
// This may not be modified after construction.
// The constructor makes a copy of the string and stores it internally.
// The destructor deallocates the memory.
//
class vdbStaticString
{
public:
	vdbStaticString( const char* szString )	{ _pString = new char[strlen(szString) + 1];
											  strcpy( _pString, szString ); }
	~vdbStaticString()						{ delete[] _pString; _pString = 0; }
	inline operator const char* ()			{ return _pString; }

private:
	char* _pString;
};


#endif  // __VDB_STATIC_STRING_H
