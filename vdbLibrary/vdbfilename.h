//=============================================================================
//
// FILE NAME:       vdbFilename.h
//
// ESSENCE:         String class with semantic information about filenames
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    4/29/95
//
// COPYRIGHT:       Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef __vdbFilename_h
#define __vdbFilename_h

#ifndef __vdbString_h
	#include "vdbString.h"
#endif

#ifndef _UNICODE
	#ifndef _IOSTREAM_
		#include <iostream>
	#endif
#endif

class vdbTimeStamp;

class vdbFilename : public vdbString
{
public:
	// CopyFileCallback is a callback function receiving a user-defined item and an unsigned long amount
	// and returning a bool of true to continue or false to stop copying
	typedef bool (*CopyFileCallback)( void*, unsigned long );

public:
	// constructors
	vdbFilename();
	vdbFilename( const vdbString& str );
	vdbFilename( const vdbFilename& rhs );

	// assignment
	const vdbFilename& operator= ( const vdbFilename& rhs );
	const vdbFilename& operator= ( const vdbString& rhs );
	const vdbFilename& operator= ( const TCHAR* sz );
	operator const TCHAR*() const;

	// modification	functions
	void Append( const TCHAR* szPath );
	void Extension( const TCHAR* szNewExtension );
	void ReplaceFilename( const TCHAR* szNewFilename );
	
	// access functions
	const TCHAR* Drive() const;
	const TCHAR* Directory() const;
	const TCHAR* ParentDirectory() const;
	const TCHAR* OutermostSubDirectory() const;
	const TCHAR* Basename() const;
	const TCHAR* Extension() const;
	const TCHAR* FilenameWithExtension() const;

	// file system functions
	#ifndef _WIN32_WCE
		void SetCurrentDirectory() const;
	#endif _WIN32_WCE
	bool CreateDir();
	bool Exists() const;
	bool IsDirectory() const;
	bool IsEmpty() const;
	bool ReadOnly() const;
	unsigned long Attributes() const;
	bool Attributes( unsigned long attr );
	bool AttributesDir( unsigned long attr, bool bRecurseSubDirectories );
	bool GetCreationTime( vdbTimeStamp& ts ) const;
	bool GetLastWriteTime( vdbTimeStamp& ts ) const;
	const TCHAR* GetExecutableVersion() const;
	bool Delete( bool bDeleteReadOnly = false );
	bool DeleteDir( bool bDeleteSubDirectories = false, bool bPromptUser = true );
	bool Rename( vdbString& sNewName );
	bool Copy( vdbFilename& sDestinationFilename, bool bReplace, CopyFileCallback pCallback = NULL, void* pCallbackCookie = NULL );
	bool CopyDir( vdbFilename& sDestinationDir, bool bCopySubDirectories, bool bReplace );
	unsigned long Size( unsigned long* highOrder = 0 );
	unsigned long SizeDir( bool bIncludeSubDirectories );

	// special 
	void RemoveDoubleQuotes();
	void AddDoubleQuotes();
	bool ReplaceEnvironmentVariables();
	bool ReplaceInvalidCharacters();
	const TCHAR* RelativePath( const TCHAR* szReferenceDir ) const;
	const TCHAR* AbsolutePath( const TCHAR* szReferenceDir ) const;

	// Stream operators
	#ifndef _UNICODE
		friend std::ostream& operator<< ( std::ostream& os, const vdbFilename& s );
		friend std::istream& operator>> ( std::istream& is, vdbFilename& s );
	#endif
};

// GetExecutableVersion needs to link with version.lib
#pragma comment(linker,"/defaultlib:version.lib")


#endif // __vdbFilename_h

