//=============================================================================
//
// FILE NAME:       vdbFilename.cpp
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

#include "vdbWindows.h"
#include "vdbFilename.h"
#include <stdio.h>
#include "vdbUtility.h"
#include "vdbTimeStamp.h"
#include <stdlib.h>
#include <direct.h>
#include <vdbSet.h>
#include <vdbMessageBox.h>

// warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
#pragma warning( disable : 4800 )

//warning C4996: 'fopen': This function or variable may be unsafe. Consider using fopen_s instead.
//warning C4996: 'getenv': This function or variable may be unsafe. Consider using _dupenv_s instead.
#pragma warning( disable : 4996 )

//=============================================================================
// Constructors
//=============================================================================

//---------------------------------------
//^^ default constructor
vdbFilename::vdbFilename()
	: vdbString()
{
}


//---------------------------------------
//^^ constructor taking vdbString
vdbFilename::vdbFilename( const vdbString& str )
	: vdbString( str )
{
}


//---------------------------------------
//^^ copy constructor
vdbFilename::vdbFilename( const vdbFilename& rhs )
	: vdbString( (const vdbString&) rhs )
{
}


//=============================================================================
// Assignment
//=============================================================================

//---------------------------------------
//^^ assignment from vdbFilename
const vdbFilename& vdbFilename::operator= ( const vdbFilename& rhs )
{
	vdbString::operator=( (const vdbString&) rhs );
	return *this;
}


//---------------------------------------
//^^ assignment from vdbString
const vdbFilename& vdbFilename::operator= ( const vdbString& rhs )
{
	vdbString::operator=( rhs );
	return *this;
}


//---------------------------------------
//^^ assignment from const TCHAR*
const vdbFilename& vdbFilename::operator= ( const TCHAR* sz )
{
	vdbString::operator=( sz );
	return *this;
}


//------------------------------------------
//^^ const TCHAR* cast operator
vdbFilename::operator const TCHAR*() const
{
	return vdbString::operator const TCHAR*();
}


//=============================================================================
// Modification	functions
//=============================================================================

//---------------------------------------
//^ The Append function appends a directory or filename to the existing
//  contents of the string.
//> szPath is a directory or filename
//
void vdbFilename::Append( const TCHAR* szPath )
{
	int len = Length();
	if ( len != 0 )
		if ( (*this)[len - 1] != '\\' )
			*this = *this + "\\";
	*this = *this + szPath;
}


//---------------------------------------
//^ The Extension function appends or replaces the filename extension
//  to the existing contents of the string.
//> szNewExtension is the new extension to use
//
void vdbFilename::Extension( const TCHAR* szNewExtension )
{
	int lastDot = ReverseFind( '.' );
	if ( lastDot == -1 )
		*this = *this + _T(".");
	else
	{		
		vdbString strWithoutExt = First( lastDot + 1 );
		*this = strWithoutExt;
	}

	*this = *this + szNewExtension;
}


//---------------------------------------
//^ The ReplaceFilename function appends or replace the base name
//  and extension to the existing contents of the string.
//> szNewFilename is the new name of the filename.
//
void vdbFilename::ReplaceFilename( const TCHAR* szNewFilename )
{
	int lastSlash = ReverseFind( '\\' );
	
	if ( lastSlash == -1 )
		lastSlash = ReverseFind( ':' ); 

	if ( lastSlash == -1 )
	{
		*this = szNewFilename;
	}
	else
	{
		vdbString strJustPath = First( lastSlash + 1 );
		*this = strJustPath + szNewFilename;
	}
}


//=============================================================================
// Access functions
//=============================================================================

//---------------------------------------
//^ The Drive function returns a pointer to the drive letter of this filename
//  including the colon and the backslash.
//
const TCHAR* vdbFilename::Drive() const
{
	static TCHAR szDrive[4];

	vdbUtility::SafeCopy( szDrive, (const TCHAR*) First(3), 4 );

	return szDrive;
}


//---------------------------------------
//^ The Directory function returns a pointer to the file's drive and directory
//  without the filename and without the trailing backslash.  This function
//  uses a static variable for the return string which can be no more than _MAX_PATH
//  characters.
//^ If no period is found in the name, the entire name is assumed to be the directory.
//  If the filename has no extension, a period must still be present.
//< returns the directory with drive but without filename or extension.
//
const TCHAR* vdbFilename::Directory() const
{
	static TCHAR szDirectory[_MAX_PATH];

	// if there is no filename, just return this (without trailing slash)
	int lastDot = ReverseFind( '.' );
	if ( lastDot == -1 )
	{
		// restrict to character length
		int len = min( Length(), sizeof(szDirectory) - 1 );

		if ( len == 0 )
			{ szDirectory[0] = 0;	return szDirectory; }

		// strip trailing slash at end
		if ( Get(len-1) == '\\' )
			len--; 
		vdbUtility::SafeCopy( szDirectory, (const TCHAR*) *this, len+1 );
		return szDirectory;
	}
		
	int lastSlash = ReverseFind( '\\' );
	
	if ( lastSlash == -1 )
		lastSlash = ReverseFind( ':' ); 

	if ( lastSlash  < 1 )
		szDirectory[0] = 0;
	else
		vdbUtility::SafeCopy( szDirectory, (const TCHAR*) First( lastSlash ), sizeof(szDirectory) );

	return szDirectory;
}


//---------------------------------------
//^ The ParentDirectory function returns a pointer to the parent directory.
//  For example if "this" is "C:\AAA\BBB\CCC\File.ext" it will return "C:\AAA\BBB"
//  This function uses a static variable for the return string which can be no
//  more than _MAX_PATH characters.
//
const TCHAR* vdbFilename::ParentDirectory() const
{
	static TCHAR szDirectory[_MAX_PATH];

	// first strip off the filename and any trailing backslash
	vdbString sNoFilename( Directory() );

	int lastSlash = sNoFilename.ReverseFind( '\\' );

	if ( lastSlash < 1 )
		vdbUtility::SafeCopy( szDirectory, (const TCHAR*) sNoFilename, sizeof(szDirectory) );
	else
		vdbUtility::SafeCopy( szDirectory, (const TCHAR*) sNoFilename.First( lastSlash ), sizeof(szDirectory) );
	
	return szDirectory;
}


//---------------------------------------
//^ The OutermostSubDirectory function returns a pointer to the outermost
//  sub-directory.
//  For example if "this" is "C:\AAA\BBB\CCC\File.ext" it will return "CCC"
//  This function uses a static variable for the return string which can be no
//  more than _MAX_DIR characters.
//
const TCHAR* vdbFilename::OutermostSubDirectory() const
{
	static TCHAR szDirectory[_MAX_DIR];

	// first strip off the filename and any trailing backslash
	vdbString sNoFilename( Directory() );

	int lastSlash = sNoFilename.ReverseFind( '\\' );

	if ( lastSlash == -1 )
		szDirectory[0] = 0x0;
	else
		vdbUtility::SafeCopy( szDirectory, (const TCHAR*) sNoFilename.Middle( lastSlash + 1 ), sizeof(szDirectory) );
	
	return szDirectory;
}


//---------------------------------------
//^ The Basename function returns a pointer to the file's base name or NULL
//  if none.  This function uses a static variable for the return string which
//  can be no more than _MAX_FNAME characters.
//< returns the base filename without drive, directory or extension.
//
const TCHAR* vdbFilename::Basename() const
{
	static TCHAR Basename[_MAX_FNAME];

	int lastDot = ReverseFind( '.' );
	int lastSlash = ReverseFind( '\\' );
	
	if ( lastSlash == -1 )
		lastSlash = ReverseFind( ':' ); 

	if ( lastDot == -1 )
		vdbUtility::SafeCopy( Basename, (const TCHAR*) Last( Length() - lastSlash - 1), sizeof(Basename) );
	else
		vdbUtility::SafeCopy( Basename, (const TCHAR*) Middle( lastSlash + 1, lastDot - (lastSlash+1) ), sizeof(Basename) );

	return Basename;
}


//---------------------------------------
//^ The Extension function returns a pointer to the three letter (_MAX_EXT)
//  filename extension or NULL if none.
//
const TCHAR* vdbFilename::Extension() const
{
	static TCHAR Extension[_MAX_EXT];

	int lastDot = ReverseFind( '.' );
	if ( lastDot == -1 )
		Extension[0] = 0;
	else
		vdbUtility::SafeCopy( Extension, (const TCHAR*) Last( Length() - lastDot - 1), sizeof(Extension) );

	return Extension;
}


//---------------------------------------
//^ The FilenameWithExtension function returns a pointer to the 
//  file's base and its extension (3 character) or NULL
//  if none.
const TCHAR* vdbFilename::FilenameWithExtension() const
{
	static TCHAR Basename[_MAX_PATH];

	int lastSlash = ReverseFind( '\\' );
	
	if ( lastSlash == -1 )
		lastSlash = ReverseFind( ':' ); 

	vdbUtility::SafeCopy( Basename, (const TCHAR*) Middle( lastSlash + 1, Length() - (lastSlash+1) ), sizeof(Basename) );

	return Basename;
}


//=============================================================================
// File system functions
//=============================================================================

#ifndef _WIN32_WCE
//---------------------------------------
//^ The SetCurrentDirectory functions uses this object's filename to set the
//  current working directory.
//
void vdbFilename::SetCurrentDirectory() const
{
	::SetCurrentDirectory( (const TCHAR *) *this );
}
#endif // _WIN32_WCE


//------------------------------------------
//^ The CreateDirectory function recursively calls itself to create the directory
//  which it specifies.  This directory should have the drive specification
//  followed by the directory (and subdirectories) to be created.  Any filename,
//  if present, is ignored.
//> directory is the directory to create
//
bool vdbFilename::CreateDir()
{
	// start by just getting the drive and directory
	vdbFilename thisLevel = Directory();

	int lastSlash = thisLevel.ReverseFind( '\\' );
	if ( lastSlash == -1 )
		return true;

	// get the previous level of the directory tree and recurse
	vdbFilename prevLevel = thisLevel.First( lastSlash );
	if ( prevLevel.CreateDir() == false )
		return false;

	// now create the directories, innermost first
	if ( !thisLevel.Exists() )
	{
	    SECURITY_ATTRIBUTES  sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = FALSE;

		if ( ::CreateDirectory( thisLevel, &sa ) == FALSE )
			return false;
	}

	return true;
}


//---------------------------------------
//^ The Exists function determines whether or not this file exists.
//< returns true if it exists or false if it does not.
//
bool vdbFilename::Exists() const
{
    if ( ::GetFileAttributes( (const TCHAR *) *this ) == -1 )
        return false;
    else
        return true;
}


//---------------------------------------
//^ The IsDirectory function determines whether or not this is a directory.
//< returns true if it is a directory or false if it is not.
//
bool vdbFilename::IsDirectory() const
{
    unsigned long attr = ::GetFileAttributes( (const TCHAR *) *this );
	if ( attr == -1 )
		return false;

	if ( attr & FILE_ATTRIBUTE_DIRECTORY )
        return true;
    else
        return false;
}


//---------------------------------------
//^ The IsEmpty function checks to see if this directory has files or sub-directories.
//< returns true only if there are no files or subdirectories
//
bool vdbFilename::IsEmpty() const
{
	// this function doesn't work on files
	if ( !IsDirectory() )
		return false;

	// lookup all files and directories
	vdbFilename sAllFiles = *this;
	sAllFiles.Append( _T("*.*") );

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile( sAllFiles, &fd );
	if ( hFind == INVALID_HANDLE_VALUE )					  
		return true;

	BOOL bMore = TRUE;
	while( bMore )
	{
		if ( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
		{
			if ( _tcscmp( fd.cFileName, _T(".") ) )
				if ( _tcscmp( fd.cFileName, _T("..") ) )
					return false;
		}
		else
			return false;

		bMore = FindNextFile( hFind, &fd );
	}
	
	FindClose( hFind );
	return true;
}


//---------------------------------------
//^ The ReadOnly function determines whether or not this file is read-only.
//< returns true if it is read-only or false if it is not.
//< Also returns false if the file does not exist.
//
bool vdbFilename::ReadOnly() const
{
    unsigned long attr = ::GetFileAttributes( (const TCHAR *) *this );
	if ( attr == -1 )
		return false;

	if ( attr & FILE_ATTRIBUTE_READONLY )
        return true;
    else
        return false;
}


//---------------------------------------
//^ The Attributes function get file attributes
//< The return value is the file's attributes; see GetFileAttributes for a
//  description of the possible return values.
// 
unsigned long vdbFilename::Attributes() const
{
    return ::GetFileAttributes( (const TCHAR *) *this );
}


//---------------------------------------
//^ The Attributes function sets a file's attributes
//> attr is the attributes to set.
// FILE_ATTRIBUTE_ARCHIVE   The file is an archive file. Applications use this attribute to mark files for backup or removal. 
// FILE_ATTRIBUTE_HIDDEN    The file is hidden. It is not included in an ordinary directory listing. 
// FILE_ATTRIBUTE_NORMAL    The file has no other attributes set. This attribute is valid only if used alone. 
// FILE_ATTRIBUTE_OFFLINE   The data of the file is not immediately available. Indicates that the file data has been physically moved to offline storage. 
// FILE_ATTRIBUTE_READONLY  The file is read-only. Applications can read the file but cannot write to it or delete it. 
// FILE_ATTRIBUTE_SYSTEM    The file is part of the operating system or is used exclusively by it. 
// FILE_ATTRIBUTE_TEMPORARY The file is being used for temporary storage. File systems attempt to keep all of the data in memory for quicker access rather than flushing the data back to mass storage. A temporary file should be deleted by the application as soon as it is no longer needed. 
//
bool vdbFilename::Attributes( unsigned long attr )
{
    return (bool) ::SetFileAttributes( (const TCHAR *) *this, attr );
}


//---------------------------------------
//^ The AttributesDir function sets the attributes of the directory and all files
//  of the directory.
//> attr is the attributes to set
//> bRecurseSubDirectories is true to set all attributes of subdirectories too
//
bool vdbFilename::AttributesDir( unsigned long attr, bool bRecurseSubDirectories )
{
	// first change the attributes of "this", which should be a directory
	if ( this->Attributes( attr ) == false )
		return false;

	// now change all files
	vdbFilename sAllFiles = *this;
	sAllFiles.Append( _T("*.*") );

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile( sAllFiles, &fd );
	if ( hFind == INVALID_HANDLE_VALUE )					  
		return true;

	BOOL bMore = TRUE;
	while( bMore )
	{
		vdbFilename sFullname( *this );
		sFullname.Append( fd.cFileName );
		if ( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
		{
			if ( bRecurseSubDirectories == true )
				if ( _tcscmp( fd.cFileName, _T(".") ) )
					if ( _tcscmp( fd.cFileName, _T("..") ) )
					{
						if ( sFullname.AttributesDir( attr, true ) == false )
							return false;
					}
		}
		else
		{
			if ( sFullname.Attributes( attr ) == false )
				return false;
		}

		bMore = FindNextFile( hFind, &fd );
	}
	FindClose( hFind );

	return true;
}


//---------------------------------------
//^ The GetCreationTime function gets the date and time the file was created.
//> ts is the timestamp of the file in Greenwich Mean Time (unadjusted for
//  daylight savings time).
//< The return value is true if successful, false if the file could not be opened
// 
bool vdbFilename::GetCreationTime( vdbTimeStamp& ts ) const
{
    HANDLE hFile = CreateFile ( (const TCHAR*) *this, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile == INVALID_HANDLE_VALUE )
		return false;

	FILETIME ft;
	if ( GetFileTime( hFile, &ft, NULL, NULL ) == FALSE )
		return false;

	CloseHandle( hFile );
	ts = vdbTimeStamp( ft );

	// Windows 98 has been observed to return a bad time from GetFileTime without any
	// return code indicator, so perform a sanity check here.
	if ( !ts.IsValid() )
	{
		ts.SetNullValue();
		return false;
	}

	return true;
}

//---------------------------------------
//^ The GetLastWriteTime function gets the date and time the file was last
//  written to.
//> ts is the timestamp of the file in Greenwich Mean Time (unadjusted for
//  daylight savings time).
//< The return value is true if successful, false if the file could not be opened
// 
bool vdbFilename::GetLastWriteTime( vdbTimeStamp& ts ) const
{
    HANDLE hFile = CreateFile ( (const TCHAR*) *this, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile == INVALID_HANDLE_VALUE )
		return false;

	FILETIME ft;
	if ( GetFileTime( hFile, NULL, NULL, &ft ) == FALSE )
		return false;

	CloseHandle( hFile );
	ts = vdbTimeStamp( ft );

	// Windows 98 has been observed to return a bad time from GetFileTime without any
	// return code indicator, so perform a sanity check here.
	if ( !ts.IsValid() )
	{
		ts.SetNullValue();
		return false;
	}

	return true;
}


//-------------------------------------
//^ The GetExecutableVersion function obtains product version info directly from
//  the executable.
//< The return value is a string containing the product version.
//
const TCHAR* vdbFilename::GetExecutableVersion() const
{
	static vdbString sVersion;
	sVersion = _T("");

	// find out if this executable has version info, and if so how large it is.
	DWORD dwHandle;
	TCHAR exeLocation[200];
	vdbUtility::SafeCopy( exeLocation, *this, sizeof(exeLocation) );
	DWORD size = GetFileVersionInfoSize( exeLocation, &dwHandle );
	if ( size == 0 )
		return (const TCHAR*) sVersion;

	// get the version info
	TCHAR* pVersionData = new TCHAR[size];
	if ( GetFileVersionInfo( exeLocation, dwHandle, size, pVersionData ) == FALSE )
	{
		delete[] pVersionData; pVersionData = 0;
		return (const TCHAR*) sVersion;
	}

	// Structure used to store enumerated languages and code pages.
	struct LANGANDCODEPAGE {
	  WORD wLanguage;
	  WORD wCodePage;
	} *pTranslate;
	pTranslate = 0;
	UINT cbTranslate;

	// Read the list of languages and code pages.
	if ( VerQueryValue( pVersionData, _T("\\VarFileInfo\\Translation"), (void**) &pTranslate, &cbTranslate ) == 0 )
	{
		delete[] pTranslate; pTranslate = 0;
		return (const TCHAR*) sVersion;
	}

	// Format the command to get the product version
	TCHAR sz[80];
	vdbUtility::SafeFormat( sz, TSIZEOF(sz), _T("\\StringFileInfo\\%04x%04x\\ProductVersion"), pTranslate->wLanguage, pTranslate->wCodePage );

	// move the version info into the structure
	TCHAR* pProductVersion = 0;
	UINT length;
	if ( VerQueryValue( pVersionData, sz, (void**) &pProductVersion, &length ) == 0 )
	{
		delete[] pVersionData; pVersionData = 0;
		return (const TCHAR*) sVersion;
	}
	
	sVersion = pProductVersion;

	delete[] pVersionData; pVersionData = 0;
	return (const TCHAR*) sVersion;
}


//---------------------------------------
//^ The Delete function deletes this file from the disk.
//> bDeleteReadOnly is true to delete read only files too
//< Returns true if successful
//< Returns false if the file is read only and the bDeleteReadOnly
//  flag is not set to true
//
bool vdbFilename::Delete( bool bDeleteReadOnly )
{
	if ( ReadOnly() == true )
	{
		if ( bDeleteReadOnly == true )
			Attributes( FILE_ATTRIBUTE_NORMAL );
		else
			return false;
	}

	#ifdef _WIN32_WCE
		if ( DeleteFile( (const TCHAR *) *this ) == TRUE )
			return true;
		else
			return false;
	#else
		if ( _tremove( (const TCHAR *) *this ) == 0 )
			return true;
		else
			return false;
	#endif // _WIN32_WCE
}


//---------------------------------------
//^ The DeleteDir function deletes all files of the current directory 
//  and deletes the directory itself.  Read-only files are not deleted.
//^ This routine does not allow deletion of a null directory name or the
//  root directory of a drive.
//> bDeleteSubDirectories if true call recursively to delete all subdirectories too
//> bPromptUser is true to show message box with yes/no prompt
//
bool vdbFilename::DeleteDir( bool bDeleteSubDirectories, bool bPromptUser )
{
	// whoaa!!
	if ( this->IsNull() )
		return false;

	// whoaa!!
	if ( this->Length() <= 3 )	// "C:\"
		return false;

	// prompt user
	if ( bPromptUser == true )
	{
		vdbString s;
		s.Format( "Are you sure you want to delete \"%s\" and all its subdirectories?", (const char*) *this );
		if ( vdbTextBox( s, MB_YESNO ) == IDNO )
			return false;
	}

	// first delete all files
	vdbFilename sAllFiles = *this;
	sAllFiles.Append( _T("*.*") );

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile( sAllFiles, &fd );
	if ( hFind == INVALID_HANDLE_VALUE )					  
		return true;

	BOOL bMore = TRUE;
	while( bMore )
	{
		vdbFilename sFullname( *this );
		sFullname.Append( fd.cFileName );
		if ( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
		{
			if( bDeleteSubDirectories == true )
				if ( _tcscmp( fd.cFileName, _T(".") ) )
					if ( _tcscmp( fd.cFileName, _T("..") ) )
					{
						if ( sFullname.DeleteDir( true, false ) == false )
							return false;
					}
		}
		else
		{
			if ( sFullname.Delete() == false )
				return false;
		}

		bMore = FindNextFile( hFind, &fd );
	}
	FindClose( hFind );

	// now delete the directory
	#ifdef _WIN32_WCE
		if ( RemoveDirectory( (const TCHAR *) *this ) == TRUE )
			return true;
		else
			return false;
	#else
		if ( _trmdir( (const TCHAR *) *this ) == 0 )
			return true;
		else
			return false;
	#endif // _WIN32_CE
}


//---------------------------------------
//^ The Rename function renames this file to a new name.
//> strNewName is the new filename.
//< Returns true if successful and false on failure.
//
bool vdbFilename::Rename( vdbString& sNewName )
{
	#ifdef _WIN32_WCE
		// renaming directories not supported under CE
		if ( IsDirectory() == true )
			return false;

		__try
		{
			CFile::Rename( (const TCHAR *) *this, (const TCHAR *) sNewName );
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
			return false;
		}
		return true;
	#else
		if ( _trename( (const TCHAR *) *this, (const TCHAR *) sNewName ) == 0 )
			return true;
		else
			return false;
	#endif // _WIN32_CE
}


//---------------------------------------
//^ The Copy function copies the contents of the file associated with this
//  vdbFilename object to the requested destination.
//> sDestinationFilename is the destination filename.
//> bReplace is true to replace any existing file (even read-only files),
//  false to fail if the destination already exists.
//> pCallback is the address of the callback function which will be called
//  repeatedly to report the progress of the number of bytes copied.  It can
//  be NULL. This callback function should return true to continue copying
//  or false to stop further copying.
//> pCallbackCookie is a value passed in by the caller of this function and
//  passed back as the first parameter of the callback function.  It may be NULL.
//< Returns true if successful and false on failure.
//
bool vdbFilename::Copy( vdbFilename& sDestinationFilename, bool bReplace, CopyFileCallback pCallback, void* pCallbackCookie )
{
	if ( !Exists() )
		return false;

	if ( sDestinationFilename.Exists() )
	{	
		if ( bReplace == false )
			return false;
		if ( sDestinationFilename.Delete( true ) == false )
			return false;
	}			

	FILE* copyFrom = _tfopen( (const TCHAR*) *this, _T("rb") );
	if ( copyFrom == 0 )
		return false;
	
	FILE* copyTo = _tfopen( (const TCHAR*) sDestinationFilename, _T("wb") );
	if ( copyTo == 0 )
		return false;

	TCHAR buf[4096];
	size_t bytes = 0;
	do
	{
		bytes = fread( buf, sizeof(TCHAR), sizeof(buf), copyFrom );
		size_t actual = fwrite( buf, sizeof(TCHAR), bytes, copyTo );
		if ( bytes != actual )
			return false;			// might have ran out of space

		// report status to caller
		if ( pCallback != 0 )
		{
			bool rc = pCallback( pCallbackCookie, actual*sizeof(TCHAR) );
			
			// user request to cancel remainder of copy
			if ( rc == false )
			{
				fclose( copyFrom );
				fclose( copyTo );
				return false;
			}
		}

	} while ( bytes == sizeof(buf) );

	fclose( copyFrom );
	fclose( copyTo );

	// get the dates of the source file
    HANDLE hFileFrom = CreateFile ( (const TCHAR*) *this, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFileFrom == INVALID_HANDLE_VALUE )
		return false;
	FILETIME ft1, ft2, ft3;
	if ( GetFileTime( hFileFrom, &ft1, &ft2, &ft3 ) == FALSE )
		return false;
	CloseHandle( hFileFrom );

	// set the dates of the new file
    HANDLE hFileTo = CreateFile ( (const TCHAR*) sDestinationFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFileTo == INVALID_HANDLE_VALUE )
		return false;
	if ( SetFileTime( hFileTo, &ft1, &ft2, &ft3 ) == FALSE )
		return false;
	CloseHandle( hFileTo );

	// set the attributes for the new file
	if ( sDestinationFilename.Attributes( this->Attributes() ) == false )
		return false;

	return true;
}


//---------------------------------------
//^ The CopyDir function copies all files of the directory represented by this
//  vdbFilename object to the requested destination.
//> sDestinationDir is the destination directory.  It will be created.
//> bCopySubDirectories is true to call this function recusively.
//> bReplace is true to replace any existing files, false to skip all existing files.
//< Returns true if successful and false on failure.
//
bool vdbFilename::CopyDir( vdbFilename& sDestinationDir, bool bCopySubDirectories, bool bReplace )
{
	// first make sure this is a directory
	if ( IsDirectory() == false )
		return false;

	// create the destination directory
	if ( sDestinationDir.CreateDir() == false )
		return false;

	// lookup all files and directories
	vdbFilename sAllFiles = *this;
	sAllFiles.Append( _T("*.*") );

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile( sAllFiles, &fd );
	if ( hFind == INVALID_HANDLE_VALUE )					  
		return true;

	BOOL bMore = TRUE;
	while( bMore )
	{
		if ( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
		{
			if( bCopySubDirectories == true )
				if ( _tcscmp( fd.cFileName, _T(".") ) )
					if ( _tcscmp( fd.cFileName, _T("..") ) )
					{
						vdbFilename sSourceDir( *this );
						sSourceDir.Append( fd.cFileName );
						
						vdbFilename sDestDir( sDestinationDir );
						sDestDir.Append( fd.cFileName );

						if ( sSourceDir.CopyDir( sDestDir, bCopySubDirectories, bReplace ) == false )
							return false;
					}
		}
		else
		{
			vdbFilename sSourceFile( *this );
			sSourceFile.Append( fd.cFileName );

			vdbFilename sDestFile( sDestinationDir );
			sDestFile.Append( fd.cFileName );

			if ( sSourceFile.Copy( sDestFile, bReplace ) == false )
				return false;
		}

		bMore = FindNextFile( hFind, &fd );
	}
	
	FindClose( hFind );
	return true;
}


//---------------------------------------
//^ The Size function obtains the number of bytes in the file.  If the
//  file is larger than 4 Gigabytes, supply a non-NULL address for highOrder
//  to recieve the high order bytes of the the file size
//> highOrder is the address of the variable to fill with high order size info
//  If highOrder is NULL, the high order info will be disregarded
//< returns the number of bytes or -1 on failure
//
unsigned long vdbFilename::Size( unsigned long* highOrder )
{
 	HANDLE hFile = CreateFile( (const TCHAR *) *this, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	unsigned long lowOrder;
	lowOrder = GetFileSize( hFile, highOrder );
	
	CloseHandle( hFile );
	return lowOrder;
}



//---------------------------------------
//^ The SizeDir function obtains the number of bytes all files of the directory.
//  If the total size is larger than 4 Gigabytes, supply a non-NULL address for
//  highOrder to recieve the high order bytes of the the directory size
//> bIncludeSubDirectories is true to include all sub directories too
//> highOrder is the address of the variable to fill with high order size info
//  If highOrder is NULL, the high order info will be disregarded
//< returns the number of bytes.
//
//  Caution: if Size fails for any one file, the function continues
//  anyway and the total size will be wrong
//  Caution: if the total size of the files is greater than 4 Gb, overflow
//  will occur.  Correcting the problem cannot occur until better API's are available
//  for adding 32 bit values using 64 bits.
//
unsigned long vdbFilename::SizeDir( bool bIncludeSubDirectories )
{
	unsigned long totalBytes = 0;

	// lookup all files and directories
	vdbFilename sAllFiles = *this;
	sAllFiles.Append( _T("*.*") );

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile( sAllFiles, &fd );
	if ( hFind == INVALID_HANDLE_VALUE )					  
		return true;

	BOOL bMore = TRUE;
	while( bMore )
	{
		if ( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
		{
			if( bIncludeSubDirectories == true )
				if ( _tcscmp( fd.cFileName, _T(".") ) )
					if ( _tcscmp( fd.cFileName, _T("..") ) )
					{
						vdbFilename sSourceDir( *this );
						sSourceDir.Append( fd.cFileName );
						
						totalBytes += sSourceDir.SizeDir( true );
					}
		}
		else
		{
			vdbFilename sSourceFile( *this );
			sSourceFile.Append( fd.cFileName );

			totalBytes += sSourceFile.Size( NULL );
		}

		bMore = FindNextFile( hFind, &fd );
	}
	
	FindClose( hFind );
	return totalBytes;
}


//=============================================================================
//  Quote functions
//=============================================================================

//---------------------------------------
void vdbFilename::RemoveDoubleQuotes()
{
	int foundAt = 0;
	
	do
	{
		foundAt = Find( '"' );
		if ( foundAt != -1 )
		{
			int len = Length();
			vdbString temp = Middle( 0, foundAt ) + Middle( foundAt + 1, len - foundAt );
			*this = temp;
		}
	}
	while ( foundAt != -1 );
}


//---------------------------------------
void vdbFilename::AddDoubleQuotes()
{
	if ( Find( '"' ) == -1 )
	{
		vdbString temp;
		temp = _T("\"");
		temp += *this;
		temp += _T("\"");
		*this = temp;
	}
}


//---------------------------------------
//^ The ReplaceEnvironmentVariables function replaces a single environment
//  variable such %systemroot% or %USERPROFILE% with the current system
//  value.  If there are multiple environment variables in the string
//  you should call this function multiple times.
//< returns true if a replacement occured, false is no environment variable
//  was found.
bool vdbFilename::ReplaceEnvironmentVariables()
{
	int firstPercent = Find( '%' );
	if ( firstPercent == -1 )
		return false;

	// split the string into a beginning part and an ending part with the
	// first "%" dropped.
	vdbString sBeginning;
	vdbString sEnding;
	sBeginning = First( firstPercent );
	sEnding = Middle( firstPercent + 1 );
	
	// find the second "%"
	int secondPercent = sEnding.Find( '%' );
	if ( secondPercent == -1 )
		return false;

	// split the string into the environment variable and the ending part
	// with the second "%" dropped.
	vdbString sEnvironment;
	sEnvironment = sEnding.First( secondPercent );
	sEnding = sEnding.Middle( secondPercent + 1 );

	#ifdef _WIN32_WCE
		// not supported on CE
		return false;
	#else
		// find the envirnment variable
		TCHAR* pszValue = _tgetenv( (const TCHAR*) sEnvironment );
		if ( pszValue == NULL )
			return false;

		// now reassemble the three parts
		*this = sBeginning + pszValue;
		*this = *this + sEnding;
		return true;
	#endif // _WIN32_WCE
}


//---------------------------------------
//^ The ReplaceInvalidCharacters function replaces any characters in the string
//  which are not valid filename characters
//< returns true if one or more characters were replaced.
//< returns false if nothing was replaced.
bool vdbFilename::ReplaceInvalidCharacters()
{
	bool bFound = false;
	int where = FindOneOf( _T("<>*?|/\"") );
	while ( where != -1 )
	{
		bFound = true;
		Set( where, '_' );
		where = FindOneOf( _T("<>*?|/\"") );
	}
	return bFound;
}


//---------------------------------------
//^ The RelativePath function returns a pointer to a string representing this
//  object's filename relative to the reference filename.
//^ If szReference is "C:\Ref1\Ref2" then the following results are obtained:
//< uses a static string no more than _MAX_PATH characters long
//
//  Reference                This object               Return string         refCount  thisCount  commonCount
//  --------------   -------------------------------   --------------------  --------  ---------  -----------
//  "C:\Ref1\Ref2"   "D:\File1.ext"                    "D:\File1.ext"            3         2           0
//  "C:\Ref1\Ref2"   ".\File3.ext"                     ".\File3.ext"             3         2           0
//  "C:\Ref1\Ref2"   "..\File3.ext"                     "..\File3.ext"           3         2           0
//  "C:\Ref1\Ref2"   "C:\Ref1\Ref2"                    "."                       3         3           3
//  "C:\Ref1\Ref2"   "C:\Ref1\Ref2\MyDir3\File4.ext"   ".\MyDir3\File4.ext"      3         5           3
//  "C:\Ref1\Ref2"   "C:\Ref1\File2.ext"               "..\File2.ext"            3		   3           2
//  "C:\Ref1\Ref2"   "C:\File1.ext"                    "..\..\File1.ext"         3         2           1
//  "C:\Ref1\Ref2"   "C:\Ref1\MyDir2\File3.ext"        "..\MyDir2\File3.ext"     3         4           2
//
const TCHAR* vdbFilename::RelativePath( const TCHAR* szReference ) const
{
	vdbString sReference( szReference );

	// break the current filename into parts like "C:", "MyDir1", "MyFile2.ext"
	vdbSet<vdbString> thisParts;
	int begin = 0;
	int end = this->Find( "\\" );
	while ( end != -1 )
	{
		int len = end - begin;
		thisParts.Add( new vdbString( this->Middle(begin, len) ) );

		begin = end + 1;
		end = this->FindNext( "\\", begin );
	} 
	if ( end < this->Length() - 1 )		//<-- don't add any trailing backslash as a separate part
		thisParts.Add( new vdbString( this->Middle(begin) ) );
	

	// break the reference filename into parts like "C:", "Ref1", "File2.ext"
	vdbSet<vdbString> referenceParts;
	begin = 0;
	end = sReference.Find( "\\" );
	while ( end != -1 )
	{
		int len = end - begin;
		referenceParts.Add( new vdbString( sReference.Middle(begin, len) ) );

		begin = end + 1;
		end = sReference.FindNext( "\\", begin );
	} 
	if ( end < sReference.Length() - 1 )		//<-- don't add any trailing backslash as a separate part
		referenceParts.Add( new vdbString( sReference.Middle(begin) ) );


	// count the common number of parts
	int countThisParts = thisParts.Count();
	int countReferenceParts = referenceParts.Count();
	int countCommonParts = 0;
	int i = 0;
	while ( i < countThisParts && i < countReferenceParts )
	{
		if ( thisParts[i] == referenceParts[i] )
		{
			countCommonParts++;
			i++;
		}
		else
			break;
	}

	vdbFilename sRelative;

//  Reference                This object               Return string         refCount  thisCount  commonCount
//  --------------   -------------------------------   --------------------  --------  ---------  -----------
//  "C:\Ref1\Ref2"   "D:\File1.ext"                    "D:\File1.ext"            3         2           0
//  "C:\Ref1\Ref2"   ".\File3.ext"                     ".\File3.ext"             3         2           0
//  "C:\Ref1\Ref2"   "..\File3.ext"                     "..\File3.ext"           3         2           0
	if ( countCommonParts == 0 )
		sRelative = *this;

//  Reference                This object               Return string         refCount  thisCount  commonCount
//  --------------   -------------------------------   --------------------  --------  ---------  -----------
//  "C:\Ref1\Ref2"   "C:\Ref1\Ref2"                    "."                       3         3           3
	else if ( countCommonParts == countThisParts && countCommonParts == countReferenceParts )
		sRelative = ".";

//  Reference                This object               Return string         refCount  thisCount  commonCount
//  --------------   -------------------------------   --------------------  --------  ---------  -----------
//  "C:\Ref1\Ref2"   "C:\Ref1\Ref2\MyDir3\File4.ext"   ".\MyDir3\File4.ext"      3         5           3
	else if ( countCommonParts == countReferenceParts )
	{
		sRelative = ".";

		for ( int i = countCommonParts; i < countThisParts; i++ )
		{
			sRelative += "\\";
			sRelative += thisParts[i];
		}
	}

//  Reference                This object               Return string         refCount  thisCount  commonCount
//  --------------   -------------------------------   --------------------  --------  ---------  -----------
//  "C:\Ref1\Ref2"   "C:\Ref1\File2.ext"               "..\File2.ext"            3		   3           2
//  "C:\Ref1\Ref2"   "C:\File1.ext"                    "..\..\File1.ext"         3         2           1
//  "C:\Ref1\Ref2"   "C:\Ref1\MyDir2\File3.ext"        "..\MyDir2\File3.ext"     3         4           2
	else if ( countCommonParts < countReferenceParts )
	{
		int countBacktrack = countReferenceParts - countCommonParts;
		int i;
		for ( i = 0; i < countBacktrack; i++ )
		{
			if ( i > 0 )
				sRelative += "\\";
			sRelative += "..";
		}
		for ( i = countCommonParts; i < countThisParts; i++ )
		{
			sRelative += "\\";
			sRelative += thisParts[i];
		}
	}
	
	else
		assert( false );

	
	static TCHAR szRelative[_MAX_PATH];
	vdbUtility::SafeCopy( szRelative, sRelative, sizeof(szRelative) );
	return szRelative;
}


//---------------------------------------
//^ The AbsolutePath function returns a pointer to a string containing the absolute
//  path of this filename when this object points to a relative path.
//^ If szReference is "C:\Ref1\Ref2" then the following results are obtained:
//< uses a static string no more than _MAX_PATH characters long
//
//  Reference       This object           Return string                   
//  --------------  --------------------  ------------------------------- 
//  "C:\Ref1\Ref2"  "D:\File1.ext"        "D:\File1.ext"                  
//  "C:\Ref1\Ref2"  ".\File3.ext"         "C:\Ref1\Ref2\File3.ext"        
//  "C:\Ref1\Ref2"  "..\File3.ext"        "C:\Ref1\File3.ext"                  
//  "C:\Ref1\Ref2"  "File3.ext"           "C:\Ref1\Ref2\File3.ext"        
//
const TCHAR* vdbFilename::AbsolutePath( const TCHAR* szReference ) const
{
	vdbString sReference( szReference );

	// break the current relative filename into parts like ".", "..", "MyDir", "MyFile2.ext"
	vdbSet<vdbString> thisParts;
	int begin = 0;
	int end = this->Find( "\\" );
	while ( end != -1 )
	{
		int len = end - begin;
		thisParts.Add( new vdbString( this->Middle(begin, len) ) );

		begin = end + 1;
		end = this->FindNext( "\\", begin );
	} 
	if ( end < this->Length() - 1 )		//<-- don't add any trailing backslash as a separate part
		thisParts.Add( new vdbString( this->Middle(begin) ) );
	

	// break the reference filename into parts like "C:", "Ref1", "Ref2"
	vdbSet<vdbString> referenceParts;
	begin = 0;
	end = sReference.Find( "\\" );
	while ( end != -1 )
	{
		int len = end - begin;
		referenceParts.Add( new vdbString( sReference.Middle(begin, len) ) );

		begin = end + 1;
		end = sReference.FindNext( "\\", begin );
	} 
	if ( end < sReference.Length() - 1 )		//<-- don't add any trailing backslash as a separate part
		referenceParts.Add( new vdbString( sReference.Middle(begin) ) );


	vdbFilename sAbsolute;

	if ( thisParts.Count() == 0 )
	{
		// this filename is blank, just return the reference directory
		sAbsolute = sReference;
	}

	//  Reference       This object           Return string                   
	//  --------------  --------------------  ------------------------------- 
	//  "C:\Ref1\Ref2"  "D:\File1.ext"        "D:\File1.ext"                  
	else if ( thisParts[0].Length() > 1 && thisParts[0].Get(1) == ':' )
	{
		// this filename is already absolute, just return it
		sAbsolute = *this;
	}

	//  Reference       This object           Return string                   
	//  --------------  --------------------  ------------------------------- 
	//  "C:\Ref1\Ref2"  "."                   "C:\Ref1\Ref2\File3.ext"        
	//  "C:\Ref1\Ref2"  ".\File3.ext"         "C:\Ref1\Ref2\File3.ext"        
	else if ( thisParts[0] == "." )
	{
		sAbsolute = szReference;
		for ( int i = 1; i < thisParts.Count(); i++ )
			sAbsolute.Append( thisParts[i] );
	}

	//  Reference       This object           Return string                   
	//  --------------  --------------------  ------------------------------- 
	//  "C:\Ref1\Ref2"  "..\File3.ext"        "C:\Ref1\File3.ext"                  
	//  "C:\Ref1\Ref2"  "..\..\File3.ext"     "C:\File3.ext"                  
	else if ( thisParts[0] == ".." )
	{
		int numUpDir = 1;
		int i;
		for ( i = 1; i < thisParts.Count(); i++ )
		{
			if ( thisParts[i] == ".." )
				numUpDir++;
			else
				break;
		}

		int remainingReferenceDir = max( referenceParts.Count() - numUpDir, 0 );

		for ( i = 0; i < remainingReferenceDir; i++ )
			sAbsolute.Append( referenceParts[i] );

		for ( i = numUpDir; i < thisParts.Count(); i++ )
			sAbsolute.Append( thisParts[i] );
	}

	//  Reference       This object           Return string                   
	//  --------------  --------------------  ------------------------------- 
	//  "C:\Ref1\Ref2"  "File3.ext"           "C:\Ref1\Ref2\File3.ext"        
	else
	{
		sAbsolute = szReference;
		for ( int i = 0; i < thisParts.Count(); i++ )
			sAbsolute.Append( thisParts[i] );
	}

	
	static TCHAR szAbsolute[_MAX_PATH];
	vdbUtility::SafeCopy( szAbsolute, sAbsolute, sizeof(szAbsolute) );
	return szAbsolute;
}


#ifndef _UNICODE
//=============================================================================
//  iostream functions
//=============================================================================

//------------------------------------------
//^^ ostream << operator
std::ostream& operator<< ( std::ostream& os, const vdbFilename& s )
{
	const vdbString& string = (const vdbString&) s;
	return operator<< ( os, string );
}


//------------------------------------------
//^^ istream >> operator
std::istream& operator>> ( std::istream& is, vdbFilename& s )
{
	vdbString& string = (vdbString&) s;
	return operator>> ( is, string );
}

#endif // _UNICODE
