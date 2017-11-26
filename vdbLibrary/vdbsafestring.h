//=============================================================================
//
// FILE NAME:       vdbSafeString.h
//
// CONTENTS:        A class for managing the automatic deletion of pointers to
//                  null-terminated strings when the go out of scope.
//
//=============================================================================

#ifndef __vdbSafeString_h
#define __vdbSafeString_h

#include <string.h>

//------------------------------------------
//^ The vdbSafeString Class holds a pointer to a null terminated character array.
// This may not be modified after construction.
// The constructor adopts the string pointer.  It must be a heap-allocated pointer
// to the string.
// The destructor deallocates the memory.
//
class vdbSafeString
{
public:
	vdbSafeString( char* szString ) : _pString( szString ) {}
	~vdbSafeString()						{ delete[] _pString; _pString = 0; }
	inline operator const char* ()			{ return _pString; }

private:
	char* _pString;
};


#endif  // __vdbSafeString_h
