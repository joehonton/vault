//=============================================================================
//
// FILE NAME:       vdbWideChar.h
//
// ESSENCE:         Conversion between ANSI and UNICODE
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    2/13/2002
//
// COPYRIGHT:       Copyright © 2002 by Joe Honton
//
//=============================================================================

#ifndef __vdbWideChar_h
#define __vdbWideChar_h


class vdbWideChar
{
public:
	// essential class functions
	vdbWideChar();
	vdbWideChar( const char* sz, int length = -1 );
	vdbWideChar( const wchar_t* sz, int length = -1 );
	~vdbWideChar();
	vdbWideChar( const vdbWideChar& rhs );
	const vdbWideChar& operator= ( const vdbWideChar& rhs );

	// access functions
	operator const char*();
	operator const wchar_t*();
	const char* GetAnsi();
	const wchar_t* GetUnicode();
	void SetAnsi( const char* sz, int length = -1 );
	void SetUnicode( const wchar_t* sz, int length = -1 );
	int Length();

private:
	void AnsiToUnicode();
	void UnicodeToAnsi(); 

protected:
	char* _pszAnsi;			// single byte string
	wchar_t* _pszUnicode;	// UNICODE string
	bool _bIsAnsiValid;		// true if the _pszAnsi string is ready to be retrieved
	bool _bIsUnicodeValid;	// true if the _pszUnicode string is ready to be retrieved
};

#endif // __vdbWideChar_h

