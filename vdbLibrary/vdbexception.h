//=============================================================================
//
// CLASS:           vdbException
//
// ESSENCE:         Vault Class Library exception class
//
// FILE NAME:       vdbException.cpp
//
// INITIAL DATE:    11/12/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef __vdbException_h
#define __vdbException_h

#ifndef _INC_TCHAR
	#include <tchar.h>
#endif

//=============================================================================
// macro definitions
//=============================================================================

#ifdef _WIN32_WCE

	#define vdbThrowException( szDescription )							AfxThrowUserException()
	#define vdbThrowMemoryException()									AfxThrowMemoryException()
	#define vdbThrowAssertException( szAssertion, szFile, iLine )		AfxThrowUserException()
	#define vdbThrowRangeException( index, maxSubscript, szClassName )	AfxThrowUserException()
	#define vdbThrowFileException( szFilename )							AfxThrowArchiveException( CArchiveException::generic, szFilename )

#else // _WIN32_CE

	#define vdbThrowException( szDescription )							throw vdbException( szDescription )
	#define vdbThrowMemoryException()									throw vdbMemoryException()
	#define vdbThrowAssertException( szAssertion, szFile, iLine )		throw vdbAssertException( szAssertion, szFile, iLine )
	#define vdbThrowRangeException( index, maxSubscript, szClassName )	throw vdbRangeException( index, maxSubscript, szClassName )
	#define vdbThrowFileException( szFilename )							throw vdbFileException( szFilename )

#endif // _WIN32_CE


//=============================================================================
// character string exception
//=============================================================================

class vdbException
{
public:
	// essential class functions
	vdbException( const TCHAR* szDescription = 0 );				// basic constructor
	~vdbException();											// destructor
	vdbException( const vdbException& rhs );					// copy constructor
	const vdbException& operator= ( const vdbException& rhs );	// assignment

public:
	// member access functions
	inline const TCHAR* Description();

private:
	// member data
	TCHAR* _szDescription;
};


//=============================================================================
// memory exception
//=============================================================================

class vdbMemoryException : public vdbException
{
public:
	// essential class functions
	vdbMemoryException();													// basic constructor
	~vdbMemoryException();													// destructor
	vdbMemoryException( const vdbMemoryException& rhs );					// copy constructor
	const vdbMemoryException& operator= ( const vdbMemoryException& rhs );	// assignment
};


//=============================================================================
// assertion exception
//=============================================================================

class vdbAssertException : public vdbException
{
public:
	// essential class functions
	vdbAssertException( const TCHAR* szAssertion, const TCHAR* szFile, int iLine );// basic constructor
	~vdbAssertException();													// destructor
	vdbAssertException( const vdbAssertException& rhs );					// copy constructor
	const vdbAssertException& operator= ( const vdbAssertException& rhs );	// assignment

public:
	// member access functions
	inline const TCHAR* GetAssertion();
	inline const TCHAR* GetFile();
	inline int GetLine();

private:
	// member data
	TCHAR* _szAssertion;
	TCHAR* _szFile;
	int _iLine;
};


//=============================================================================
// range exception
//=============================================================================

class vdbRangeException : public vdbException
{
public:
	// essential class functions
	vdbRangeException( int index, int maxSubscript, const TCHAR* szClassName );					// basic constructor
	~vdbRangeException();												// destructor
	vdbRangeException( const vdbRangeException& rhs );					// copy constructor
	const vdbRangeException& operator= ( const vdbRangeException& rhs );// assignment

public:
	// member access functions
	inline int GetIndex();
	inline int GetMaxSubscript();
	inline const TCHAR* GetClassName();

private:
	// member data
	int _iIndex;
	int _iMaxSubscript;
	TCHAR* _szClassName;
};


//=============================================================================
// file exception
//=============================================================================

class vdbFileException : public vdbException
{
public:
	// essential class functions
	vdbFileException( const TCHAR* szFilename = 0 );						// basic constructor
	~vdbFileException();												// destructor
	vdbFileException( const vdbFileException& rhs );					// copy constructor
	const vdbFileException& operator= ( const vdbFileException& rhs );	// assignment
};


//=============================================================================
// inlines
//=============================================================================

inline const TCHAR* vdbException::Description()
{
	if ( _szDescription == 0 )
		return _T("");
	else
		return _szDescription;
}


inline const TCHAR* vdbAssertException::GetAssertion()
{
	if ( _szAssertion == 0 )
		return _T("");
	else
		return _szAssertion;
}


inline const TCHAR* vdbAssertException::GetFile()
{
	if ( _szFile == 0 )
		return _T("");
	else
		return _szFile;
}


inline int vdbAssertException::GetLine()
{
	return _iLine;
}


inline int vdbRangeException::GetIndex()
{
	return _iIndex;
}


inline int vdbRangeException::GetMaxSubscript()
{
	return _iMaxSubscript;
}


inline const TCHAR* vdbRangeException::GetClassName()
{
	if ( _szClassName == 0 )
		return ( _T("") );
	else
		return _szClassName;
}


#endif // __vdbException_h

