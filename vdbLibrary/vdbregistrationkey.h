//=============================================================================
//
// CLASS:           vdbRegistrationKey
//
// FILE NAME:       vdbRegistrationKey.h
//
// PURPOSE:         Registration key query, rename, delete, and set value.
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    10/1/96
//
// COPYRIGHT:       Copyright © 1996 by Joe Honton
//
//=============================================================================

#ifndef __vdbRegistrationKey_h
#define __vdbRegistrationKey_h

#include <winreg.h>
#include "vdbString.h"
#include "vdbSet.h"

#ifdef USE_WIN
	typedef void * vdbHKEY;
#elif USE_AFX
	struct HKEY__;
	typedef struct HKEY__ *HKEY;
	typedef HKEY vdbHKEY;
#else
	#error USE_AFX or USE_WIN must be defined
#endif


// proceed with caution.  No test driver written for this yet
// some items not implemented fully.

class vdbRegistrationKey
{
public:
	// essential class functions
	vdbRegistrationKey();
	vdbRegistrationKey( vdbHKEY rootkey, vdbString& subkey, vdbString& leafName );
	vdbRegistrationKey( vdbHKEY rootkey, vdbString& subkey, const TCHAR* leafName );
	vdbRegistrationKey( vdbHKEY rootkey, const TCHAR* subkey, vdbString& leafName );
	vdbRegistrationKey( vdbHKEY rootkey, const TCHAR* subkey, const TCHAR* leafName );

public:
	// member manipulation
	void SetRootkey( vdbHKEY rootkey );
	void SetSubkey( vdbString& subkey );
	void SetSubkey( const TCHAR* subkey );
	void SetLeaf( vdbString& leafName );
	void SetLeaf( const TCHAR* leafName );
	vdbHKEY GetRootkey() const;
	const TCHAR* GetSubkey() const;
	const TCHAR* GetLeaf() const;

public:
	// subkey manipulation
	bool RenameSubkey( vdbString& newSubkeyName );
	bool RenameSubkey( const TCHAR* newSubkeyName );	  // not done yet
	bool DeleteSubkey();
	bool SubkeyExists();

public:
	// branch manipulation
	bool CreateBranchRecursive();
	bool CreateBranch( vdbString& newBranchName);
	bool CreateBranch( const TCHAR* newBranchName );

public:
	// leaf manipulation
	bool RenameLeaf( vdbString& newLeafName );
	bool RenameLeaf( const TCHAR* newLeafName );
	bool DeleteLeaf();
	bool LeafExists();

public:
	// access to leaf value
	bool GetValue( TCHAR* value, size_t length );
	bool GetValue( vdbString& value );
	bool GetValue( int& value );
	bool GetValue( unsigned int& value );
	bool GetValue( long& value );
	bool GetValue( unsigned long& value );
	bool GetValue( bool& value );
	bool GetValue( double& value );
	bool SetValue( const TCHAR* value );
	bool SetValue( const TCHAR* value, DWORD type );
	bool SetValue( const vdbString value );
	bool SetValue( const int value );
	bool SetValue( const unsigned int value );
	bool SetValue( const long value );
	bool SetValue( const unsigned long value );
	bool SetValue( const bool value );
	bool SetValue( const double value );

public:
	// access to subkey
	void GetParentName( vdbString& parentName );
	void GetTrailingSubkey( vdbString& keyName );

public:
	// enumerate members
	bool EnumValues( vdbSet<vdbString>& values );
	bool EnumSubkeys( vdbSet<vdbString>& subkeys );

private:
	// members
	vdbHKEY _rootkey;		// for example HKEY_LOCAL_MACHINE or HKEY_CURRENT_USER
	vdbString _subkey;		// for example "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\winword.exe"
	vdbString _leafName;	// for example "Path" or use "" to obtain the (Default)
};

#endif  // __vdbRegistrationKey_h

