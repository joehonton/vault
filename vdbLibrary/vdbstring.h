//=============================================================================
//
// FILE NAME:       vdbString.h
//
// ESSENCE:         dynamically resizeable string class
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    1/31/96
//
// COPYRIGHT:       Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __vdbString_h
#define __vdbString_h

#ifndef _INC_TCHAR
	#include <tchar.h>
#endif

#include <ostream>
#include <istream>


class vdbString
{
public:
	enum capitalization { IS_NULL = 0, ALL_LOWERCASE = 1, ALL_UPPERCASE = 2, FIRST_LETTER_ONLY_CAPS = 3, MIXED_CAPS = 4 };
	
public:
	//  Constructors and destructor
	vdbString();
	vdbString( const vdbString& rhs );
	vdbString( const TCHAR* sz );
	vdbString( TCHAR ch );
	~vdbString();

	//  Assignment operators
	const vdbString& operator= ( const vdbString& rhs );
	const vdbString& operator= ( const TCHAR* sz );
	const vdbString& operator= ( TCHAR ch );
	
	// Concatenation operators
	const vdbString& operator+= ( const vdbString& rhs );
	const vdbString& operator+= ( const TCHAR* sz );
	const vdbString& operator+= ( TCHAR ch );

	// Non-member concatenation operators
	friend vdbString operator+ ( const vdbString& string1, const vdbString& string2 );
	friend vdbString operator+ ( const vdbString& string, TCHAR ch );
	friend vdbString operator+ ( TCHAR ch, const vdbString& string );
	friend vdbString operator+ ( const vdbString& string, const TCHAR* sz );
	friend vdbString operator+ ( const TCHAR* sz, const vdbString& string );

	// Character access
	operator const TCHAR*() const;
	TCHAR& operator[] ( int index );
	const TCHAR& operator[] ( int index ) const;
	TCHAR Get( int index ) const;
	void Set( int index, TCHAR ch );
	void Set( const TCHAR* pBuffer, int length );
	vdbString First( int numChars ) const;
	vdbString Middle( int index, int numChars ) const;
	vdbString Middle( int index ) const;
	vdbString Last( int numChars ) const;

	// Comparison
	bool operator== ( const vdbString& rhs ) const;
	bool operator== ( const TCHAR* sz ) const;
	bool operator== ( TCHAR ch ) const;
	bool operator!= ( const vdbString& rhs ) const;
	bool operator!= ( const TCHAR* sz ) const;
	bool operator!= ( TCHAR ch ) const;
	bool operator< ( const vdbString& rhs );
	bool operator< ( const TCHAR* sz ) const;
	bool operator< ( TCHAR ch ) const;
	bool operator> ( const vdbString& rhs );
	bool operator> ( const TCHAR* sz ) const;
	bool operator> ( TCHAR ch ) const;
	bool Compare( const vdbString& rhs, int numChars = 0 ) const;
	bool Compare( const TCHAR* sz, int numChars = 0 ) const;
	bool CompareNoCase( const vdbString& rhs, int numChars = 0 ) const;
	bool CompareNoCase( const TCHAR* sz, int numChars = 0 ) const;
	bool CompareNoCase( TCHAR ch ) const;
	capitalization TypeOfCapitalization();

	// Information functions
	int Length() const;
	bool IsNull() const;
	bool IsNotNull() const;

	// String modification routines
	void ToUpper();
	void ToLower();
	void StripTrailingSpaces();
	void StripLeadingSpaces();
	void StripTrailingWhitespace();
	void StripLeadingWhitespace();
	void Replace( TCHAR oldValue, TCHAR newValue );

	// Sub-string matching
	int Find( TCHAR ch ) const;
	int FindNext( TCHAR ch, int startingPosition ) const;
	int Find( const TCHAR* sz ) const;
	int FindNext( const TCHAR* sz, int startingPosition ) const;
	int ReverseFind( TCHAR ch ) const;
	int ReverseFindNext( TCHAR ch, int endingPosition ) const;
	int FindOneOf( const TCHAR* szCharSet ) const;
	int FindOneOfNext( const TCHAR* szCharSet, int startingPosition ) const;
	int FindInverseOf( const TCHAR* szCharSet ) const;
	int FindInverseOfNext( const TCHAR* szCharSet, int startingPosition ) const;

	// String formatting
	void Format( const TCHAR* szFormat, ... );

	// Stream operators
	#ifndef UNICODE
		void Put( std::ostream& os ) const;
		void Get( std::istream& is );
		void write( std::ostream& os, const int sizeofLength = 4 ) const;
		void read( std::istream& is, const int sizeofLength = 4 );

		friend std::ostream& operator<< ( std::ostream& os, const vdbString& obj );
		friend std::istream& operator>> ( std::istream& is, vdbString& obj );
	#endif

private:
	TCHAR* _s;
	long _length;		// not including the null terminating byte;

	friend class vdbVarCharLong;
};


#endif  // __vdbString_h

