//=============================================================================
//
// CLASS:           vdbRegistrationKey
//
// FILE NAME:       vdbRegistrationKey.cpp
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

#include "vdbWindows.h"
#include "vdbRegistrationKey.h"
#include "vdbUtility.h"
#include "vdbAssert.h"
#include <stdio.h>
#include "vdbException.h"

// warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
#pragma warning( disable : 4800 )

//warning C4996: 'sprintf': This function or variable may be unsafe. Consider using sprintf_s instead.
#pragma warning( disable : 4996 )


//=============================================================================
// Essential Class functions
//=============================================================================

//-------------------------------------
//^^ The default constructor
vdbRegistrationKey::vdbRegistrationKey()
	: _rootkey( 0 ),
	  _subkey( _T("") ),
	  _leafName( _T("") )
{
}


//-------------------------------------
//^^ The (vdbString, vdbString) constructor
vdbRegistrationKey::vdbRegistrationKey( vdbHKEY rootkey, vdbString& subkey, vdbString& leafName )
	: _rootkey( rootkey ),
	  _subkey( subkey ),
	  _leafName( leafName )
{
}


//-------------------------------------
//^^ The (vdbString, const TCHAR*) constructor
vdbRegistrationKey::vdbRegistrationKey( vdbHKEY rootkey, vdbString& subkey, const TCHAR* leafName )
	: _rootkey( rootkey ),
	  _subkey( subkey ),
	  _leafName( leafName )
{
}


//-------------------------------------
//^^ The (const TCHAR*, vdbString) constructor
vdbRegistrationKey::vdbRegistrationKey( vdbHKEY rootkey, const TCHAR* subkey, vdbString& leafName )
	: _rootkey( rootkey ),
	  _subkey( subkey ),
	  _leafName( leafName )
{
}


//-------------------------------------
//^^ The (const TCHAR*, const TCHAR*) constructor
vdbRegistrationKey::vdbRegistrationKey( vdbHKEY rootkey, const TCHAR* subkey, const TCHAR* leafName )
	: _rootkey( rootkey ),
	  _subkey( subkey ),
	  _leafName( leafName )
{
}


//=============================================================================
// Member manipulation
//=============================================================================

//-------------------------------------
//^ The SetRootkey function changes the value of the rootkey.  It must be
//  called if the default constructor is used.
//> rootkey is the root value, for example HKEY_LOCAL_MACHINE or HKEY_CURRENT_USER
//
void vdbRegistrationKey::SetRootkey( vdbHKEY rootkey )
{
	_rootkey = rootkey;
}


//-------------------------------------
//^ The SetSubkey function changes the value of the subkey.  It must be called
//  if the default constructor is used.
//> subkey is the subkey to manipulate.  For example "// for example "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\winword.exe"
//
void vdbRegistrationKey::SetSubkey( vdbString& subkey )
{
	_subkey = subkey;
}


//-------------------------------------
//^ The SetSubkey function changes the value of the subkey.  It must be called
//  if the default constructor is used.
//> subkey is the subkey to manipulate.  For example "// for example "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\winword.exe"
//
void vdbRegistrationKey::SetSubkey( const TCHAR* subkey )
{
	_subkey = subkey;
}


//-------------------------------------
//^ The SetLeaf function changes the value of the subkey.  It must be called
//  if the default constructor is used.
//> leafName is the leaf to manipulate.  For example "Path" or use _T("") to obtain the (Default)
//
void vdbRegistrationKey::SetLeaf( vdbString& leafName )
{
	_leafName = leafName;
}


//-------------------------------------
//^ The SetLeaf function changes the value of the subkey.  It must be called
//  if the default constructor is used.
//> leafName is the leaf to manipulate.  For example "Path" or use _T("") to obtain the (Default)
//
void vdbRegistrationKey::SetLeaf( const TCHAR* leafName )
{
	_leafName = leafName;
}


//-------------------------------------
//^ The GetRootkey function returns the value of the rootkey.
//
vdbHKEY vdbRegistrationKey::GetRootkey() const
{
	return _rootkey;
}


//-------------------------------------
//^ The GetSubkey function returns the value of the subkey.
//
const TCHAR* vdbRegistrationKey::GetSubkey() const
{
	return _subkey;
}


//-------------------------------------
//^ The GetLeaf function returns the value of the subkey.
//
const TCHAR* vdbRegistrationKey::GetLeaf() const
{
	return _leafName;
}


//=============================================================================
// Subkey manipulation
//=============================================================================

//-------------------------------------
//^ The RenameSubkey function creates a new subkey with the new name, copies
//  all the values from the existing subkey, then deletes the existing subkey
//  and all its values.	 It is not recursive and does not work on keys which
//  have subkeys.
//> newSubkeyName is the new name of the final part of the subkey; for example,
//  "NewPath".  It may not be more than one level deep.  That is, it must not
//  contain embedded backslashes.
//< returns true if everything went smoothly.
//
bool vdbRegistrationKey::RenameSubkey( vdbString& newSubkeyName )
{
	return RenameSubkey( (const TCHAR*) newSubkeyName);
}


//-------------------------------------
//^ The RenameSubkey function creates a new subkey with the new name, copies
//  all the values from the existing subkey, then deletes the existing subkey
//  and all its values.	 It is not recursive and does not work on keys which
//  have subkeys.
//> newSubkeyName is the new name of the final part of the subkey; for example,
//  "NewPath".  It may not be more than one level deep.  That is, it must not
//  contain embedded backslashes.
//< returns true if everything went smoothly.
//
bool vdbRegistrationKey::RenameSubkey( const TCHAR* newSubkeyName )
{
	// Create a new branch at the same level as the current subkey's parent
	vdbString parentKeyName;
	GetParentName( parentKeyName );
	
	vdbRegistrationKey parentKey( _rootkey, parentKeyName, _T("") );
	parentKey.CreateBranch( newSubkeyName );

	vdbString newFullKeyName( parentKeyName );
	newFullKeyName += '\\';
	newFullKeyName += newSubkeyName;
	//? vdbRegistrationKey newSubkey( _rootkey, newSubkeyName, _T("") );

	// copy all values from the existing subkey to the new subkey
	vdbHKEY hkey;
	vdbHKEY hNewKey;
	LONG rc;

	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_ALL_ACCESS, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegOpenKeyEx( _rootkey, newFullKeyName, NULL, KEY_ALL_ACCESS, &hNewKey );
	if ( rc != ERROR_SUCCESS )
		return false;

	TCHAR classString[50];			// buffer for class string 
	DWORD classStringSize = 50;		// size of class string buffer 
	DWORD reserved = 0;				// reserved 
	DWORD numSubkeys = 0;			// number of subkeys 
	DWORD maxSubKeyLen = 0;			// longest subkey name length  			
	DWORD maxClassLen = 0;			// longest class string length 
    DWORD numValues = 0;			// number of value entries 
    DWORD maxValueNameLen = 0;		// longest value name length 
    DWORD maxValueLen = 0;			// longest value data length 
    DWORD securityDescriptorLen = 0;// security descriptor length 
    FILETIME ftLastWriteTime; 		// last write time 

	rc = RegQueryInfoKey ( hNewKey, classString, &classStringSize, &reserved, &numSubkeys, &maxSubKeyLen,
						   &maxClassLen, &numValues, &maxValueNameLen, &maxValueLen, &securityDescriptorLen, &ftLastWriteTime );
	if ( rc != ERROR_SUCCESS )
		return false;


	//todo


	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
  		
	rc = RegCloseKey( hNewKey );	
	if ( rc != ERROR_SUCCESS )
		return false;
  		
	return true;
}


//-------------------------------------
//^ The DeleteSubkey function opens the subkey, deletes it, and closes the key.
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::DeleteSubkey()
{
	assert( _rootkey != 0 );
	assert( _subkey != _T("") );

	vdbHKEY hkey;
	LONG rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_ALL_ACCESS, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	// how many values does this have
	DWORD countValues;
	rc = RegQueryInfoKey( hkey, NULL, NULL, NULL, NULL, NULL, NULL, &countValues, NULL, NULL, NULL, NULL );
	if ( rc != ERROR_SUCCESS )
		return false;

	// loop through all values and delete
	for ( int i = countValues - 1; i >= 0; i-- )
	{
		TCHAR sz[100];
		DWORD sizeSZ = 100;
		rc = RegEnumValue( hkey, i, sz, &sizeSZ, NULL, NULL, NULL, NULL );
		if ( rc != ERROR_SUCCESS )
			return false;

		rc = RegDeleteValue( hkey, sz );
		if ( rc != ERROR_SUCCESS )
			return false;
	}
 
	// how many subkeys does this have
	DWORD countSubkeys;
	rc = RegQueryInfoKey( hkey, NULL, NULL, NULL, &countSubkeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL );
	if ( rc != ERROR_SUCCESS )
		return false;

	// loop through all subkeys and delete
	for ( int i = countSubkeys - 1; i >= 0; i-- )
	{
		TCHAR sz[100];
		DWORD sizeSZ = 100;
		rc = RegEnumKeyEx( hkey, i, sz, &sizeSZ, NULL, NULL, NULL, NULL );
		if ( rc != ERROR_SUCCESS )
			return false;

		// recurse
		vdbRegistrationKey k( hkey, sz, _T("") );
		k.DeleteSubkey();
	}

	//  According to the WIN32 API documentation this will delete child keys in
	//  Windows95 but not in Windows NT which is why we have to loop through subkeys
	//  and recurse ourselves.
	rc = RegDeleteKey( hkey, _T("") );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
  		
	return true;
}


//-------------------------------------
//^ The SubkeyExists function determines whether or not the subkey exists.
//< The return value is true if it exists, false if it does not.
//
bool vdbRegistrationKey::SubkeyExists()
{
	vdbHKEY hkey;
	LONG rc;

	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_EXECUTE, &hkey );
	if ( rc == ERROR_SUCCESS )
	{
		RegCloseKey( hkey );
		return true;
	}
	else
		return false;
}


//=============================================================================
// Branch manipulation
//=============================================================================

//-------------------------------------
//^ The CreateBranchRecursive function creates all parent keys for the currently
//  defined branch.  If a parent key has alreay been created it is skipped.
//  For example if the _subkey is "SOFTWARE\\Crescent Bloom\\Catalog\\Setup"
//  and the keys "SOFTWARE" and "Crescent Bloom" have already been created,
//  then "Catalog" and "Setup" will be created.
//
bool vdbRegistrationKey::CreateBranchRecursive()
{
	if ( SubkeyExists() )
		return true;

	vdbString s1;
	this->GetParentName( s1 );
	if ( s1 == _T("") )
		return false;
	vdbRegistrationKey key( _rootkey, s1, _T("") );
	if ( key.CreateBranchRecursive() == false )
		return false;

	vdbString s2;
	this->GetTrailingSubkey( s2 );
	return key.CreateBranch( s2 );
}


//-------------------------------------
//^ The CreateBranch function opens the subkey and creates a new branch with
//  the user-supplied value.
//> newBranchName is the name of the branch to add to the existing subkey.
//  It must be only one level deep; that is it cannot contain embedded backslashes.
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::CreateBranch( vdbString& newBranchName )
{
	return CreateBranch( (const TCHAR*) newBranchName );
}


//-------------------------------------
//^ The CreateBranch function opens the subkey and creates a new branch with
//  the user-supplied value.
//> newBranchName is the name of the branch to add to the existing subkey.
//  It must be only one level deep; that is it cannot contain embedded backslashes.
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::CreateBranch( const TCHAR* newBranchName )
{
	assert( _rootkey != 0 );

	vdbHKEY hParentKey;
	vdbHKEY hBranchKey;
	LONG rc;

	// open parent, create branch, write, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_CREATE_SUB_KEY, &hParentKey );
	if ( rc != ERROR_SUCCESS )
		return false;

    DWORD dwDisposition;
	rc = RegCreateKeyEx( hParentKey, newBranchName, NULL, _T("REG_SZ"), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hBranchKey, &dwDisposition );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hBranchKey );	
	if ( rc != ERROR_SUCCESS )
		return false;
  		
	rc = RegCloseKey( hParentKey );	
	if ( rc != ERROR_SUCCESS )
		return false;
  		
	return true;
}


//=============================================================================
// Leaf manipulation
//=============================================================================

//-------------------------------------
//^ The RenameLeaf function obtains the current value of the current leaf and 
//  deletes the leaf.  A new leaf with the new name is created and the saved
//  value is assigned to the new leaf.
//> newValueName is the name of the new value.
//< returns true if everything went smoothly.
//
bool vdbRegistrationKey::RenameLeaf( const TCHAR* newLeafName )
{
	vdbString szCurrentValue;
	if ( GetValue( szCurrentValue ) == false )
		return false;

	if ( DeleteLeaf() == false )
		return false;

	_leafName = newLeafName;

	// SetValue will create the new leaf and set the value
	if ( SetValue( szCurrentValue ) == false )
		return false;

	return true;
}


//-------------------------------------
//^ The DeleteLeaf function opens the subkey and deletes the leaf of that key
//  referenced by the current value of _leafName.
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::DeleteLeaf()
{
	vdbHKEY hkey;
	LONG rc;

	// open, delete, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_ALL_ACCESS, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegDeleteValue( hkey, _leafName );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
  		
	return true;
}


//-------------------------------------
//^ The LeafExists function returns true if the leaf exists and false if it does not.
//
bool vdbRegistrationKey::LeafExists()
{
	vdbHKEY hkey;
	LONG rc;
	DWORD valueType = REG_NONE;
	TCHAR szValue[256];
	DWORD valueSize = 256 * sizeof(TCHAR);

	// open, read, close
	rc = RegOpenKeyEx( _rootkey, (const TCHAR*) _subkey, NULL, KEY_QUERY_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	bool bExists = false;
	rc = RegQueryValueEx( hkey, _leafName, NULL, &valueType, (BYTE*) szValue, &valueSize );
	if ( rc != ERROR_SUCCESS )
		return false;

//	if ( valueSize > 0 )			<-- works on NT/2000 but not 95/98
	if ( valueType != REG_NONE )
		bExists = true;
	else
		bExists = false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
	
	return bExists;
}


//=============================================================================
// Access to leaf values
//=============================================================================

//-------------------------------------
//^ The GetValue function opens the subkey, obtains its value and closes the
//  entry.
//> value is filled in by the routine if the operation was successful and the 
//  type of value is REG_SZ or REG_EXPAND_SZ or REG_MULTI_SZ or REG_DWORD.
//  Otherwise it is null.
//> length is the maximum length of the character array.
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::GetValue( TCHAR* value, size_t length )
{
	vdbHKEY hkey;
	LONG rc;
	DWORD valueType;
	TCHAR szValue[160];
	DWORD valueSize = 160 * sizeof(TCHAR);

	// initialize value for possible failure	
	value[0] = 0;

	// open, read, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_QUERY_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegQueryValueEx( hkey, _leafName, NULL, &valueType, (BYTE*) szValue, &valueSize );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
	
	// convert value to vdbString
	if ( valueType == REG_SZ )
		vdbUtility::SafeCopy( value, szValue, length);
	else if ( valueType == REG_EXPAND_SZ )
		vdbUtility::SafeCopy( value, szValue, length);
	else if ( valueType == REG_MULTI_SZ )
		vdbUtility::SafeCopy( value, szValue, length);
	else if ( valueType == REG_DWORD )
		_stprintf( value, _T("%d"), szValue );
	else if ( valueType == REG_NONE )
		vdbUtility::SafeCopy( value, szValue, length);
	else
		return false;
		
	return true;
}


//-------------------------------------
//^ The GetValue function opens the subkey, obtains its value and closes the
//  entry.
//> value is filled in by the routine if the operation was successful and the 
//  type of value is REG_SZ or REG_EXPAND_SZ or REG_MULTI_SZ or REG_DWORD.
//  Otherwise it is null.
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::GetValue( vdbString& value )
{
	vdbHKEY hkey;
	LONG rc;
	DWORD valueType;
	TCHAR szValue[160];
	DWORD valueSize = 160 * sizeof(TCHAR);

	// initialize value for possible failure	
	value = _T("");

	// open, read, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_QUERY_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegQueryValueEx( hkey, _leafName, NULL, &valueType, (BYTE*) szValue, &valueSize );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
	
	// convert value to vdbString
	if ( valueType == REG_SZ )
		value = szValue;
	else if ( valueType == REG_EXPAND_SZ )
		value = szValue;
	else if ( valueType == REG_MULTI_SZ )
		value = szValue;
	else if ( valueType == REG_DWORD )
		value.Format( _T("%d"), szValue );
	else if ( valueType == REG_NONE )
		value = szValue;
	else
		return false;
		
	return true;
}


//-------------------------------------
//^ The GetValue function opens the subkey, obtains its value and closes the
//  entry.
//> value is filled in by the routine if the operation was successful and the 
//  type of value is REG_DWORD.  Otherwise it is 0.
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::GetValue( int& value )
{
	vdbHKEY hkey;
	LONG rc;
	DWORD valueType;
	DWORD valueSize = sizeof(int);

	// initialize value for possible failure	
	value = 0;

	// open, read, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_QUERY_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegQueryValueEx( hkey, _leafName, NULL, &valueType, (unsigned char *) &value, &valueSize );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
	
	// convert value to long
	if ( valueType != REG_DWORD )
	{
		value = 0;
		return false;
	}
		
	return true;
}


//-------------------------------------
//^ The GetValue function opens the subkey, obtains its value and closes the
//  entry.
//> value is filled in by the routine if the operation was successful and the 
//  type of value is REG_DWORD.  Otherwise it is 0.
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::GetValue( unsigned int& value )
{
	vdbHKEY hkey;
	LONG rc;
	DWORD valueType;
	DWORD valueSize = sizeof(unsigned int);

	// initialize value for possible failure	
	value = 0;

	// open, read, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_QUERY_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegQueryValueEx( hkey, _leafName, NULL, &valueType, (unsigned char *) &value, &valueSize );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
	
	// convert value to long
	if ( valueType != REG_DWORD )
	{
		value = 0;
		return false;
	}
		
	return true;
}


//-------------------------------------
//^ The GetValue function opens the subkey, obtains its value and closes the
//  entry.
//> value is filled in by the routine if the operation was successful and the 
//  type of value is REG_DWORD.  Otherwise it is 0.
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::GetValue( long& value )
{
	vdbHKEY hkey;
	LONG rc;
	DWORD valueType;
	DWORD valueSize = sizeof(long);

	// initialize value for possible failure	
	value = 0;

	// open, read, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_QUERY_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegQueryValueEx( hkey, _leafName, NULL, &valueType, (unsigned char *) &value, &valueSize );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
	
	// convert value to long
	if ( valueType != REG_DWORD )
	{
		value = 0;
		return false;
	}
		
	return true;
}


//-------------------------------------
//^ The GetValue function opens the subkey, obtains its value and closes the
//  entry.
//> value is filled in by the routine if the operation was successful and the 
//  type of value is REG_DWORD.  Otherwise it is 0.
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::GetValue( unsigned long& value )
{
	vdbHKEY hkey;
	LONG rc;
	DWORD valueType;
	DWORD valueSize = sizeof(unsigned long);

	// initialize value for possible failure	
	value = 0;

	// open, read, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_QUERY_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegQueryValueEx( hkey, _leafName, NULL, &valueType, (unsigned char *) &value, &valueSize );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
	
	// convert value to long
	if ( valueType != REG_DWORD )
	{
		value = 0;
		return false;
	}
		
	return true;
}


//-------------------------------------
//^ The GetValue function opens the subkey, obtains its value and closes the
//  entry.
//> value is filled in by the routine if the operation was successful and the 
//  type of value is REG_DWORD.  Otherwise it is false.
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::GetValue( bool& value )
{
	vdbHKEY hkey;
	LONG rc;
	DWORD valueType;
	DWORD valueSize = 4;
	DWORD dwValue;

	// initialize value for possible failure	
	value = false;

	// open, read, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_QUERY_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegQueryValueEx( hkey, _leafName, NULL, &valueType, (unsigned char *) &dwValue, &valueSize );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
	
	// convert value to bool
	if ( valueType != REG_DWORD )
	{
		value = false;
		return false;
	}
		
	value = (bool) dwValue;	
	return true;
}


//-------------------------------------
//^ The GetValue function opens the subkey, obtains its value and closes the
//  entry.
//> value is filled in by the routine if the operation was successful.  Otherwise it is false.
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::GetValue( double& value )
{
	vdbHKEY hkey;
	LONG rc;
	DWORD valueType;
	TCHAR szValue[23];
	DWORD valueSize = 23 * sizeof(TCHAR);

	// initialize value for possible failure	
	value = 0.0;

	// open, read, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_QUERY_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegQueryValueEx( hkey, _leafName, NULL, &valueType, (BYTE*) szValue, &valueSize );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
	
	// convert value to double
	TCHAR* end;
	value = _tcstod( szValue, &end );
		
	return true;
}


//-------------------------------------
//^ The SetValue function opens the subkey, saves its value and closes the
//  entry.  This is function saves the entry as a REG_SZ.
//> value is the text to place into the key
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::SetValue( const TCHAR* value )
{
	vdbHKEY hkey;
	LONG rc;

	// open, write, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_SET_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegSetValueEx( hkey, _leafName, NULL, REG_SZ, (BYTE*) value, (_tcslen(value) + 1) * sizeof(TCHAR) );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
  		
	return true;
}


//-------------------------------------
//^ The SetValue function opens the subkey, saves its value and closes the
//  entry.  This is function saves the entry to the explicit type specified by the user
//> value is the text to place into the key
//> type is REG_SZ or REG_EXPAND_SZ.
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::SetValue( const TCHAR* value, DWORD type )
{
	vdbHKEY hkey;
	LONG rc;

	// open, write, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_SET_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegSetValueEx( hkey, _leafName, NULL, type, (BYTE*) value, (_tcslen(value) + 1) * sizeof(TCHAR) );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
  		
	return true;
}


//-------------------------------------
//^ The SetValue function opens the subkey, saves its value and closes the
//  entry.  This is function saves the entry as a REG_SZ.
//> value is the value to place into the key
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::SetValue( const vdbString value )
{
	vdbHKEY hkey;
	LONG rc;

	// open, write, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_SET_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	TCHAR* p = new TCHAR[value.Length()+1];
	if ( p == 0 ) vdbThrowMemoryException();
	_tcscpy( p, (const TCHAR*) value );
	rc = RegSetValueEx( hkey, _leafName, NULL, REG_SZ, (BYTE*) p, (value.Length() + 1) * sizeof(TCHAR) );
	delete[] p; p = 0;
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
  		
	return true;
}


//-------------------------------------
//^ The SetValue function opens the subkey, saves its value and closes the
//  entry.  This is function saves the entry as a REG_DWORD.
//> value is the value to place into the key
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::SetValue( const int value )
{
	vdbHKEY hkey;
	LONG rc;

	// open, write, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_SET_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegSetValueEx( hkey, _leafName, NULL, REG_DWORD, (const unsigned char *) &value, 4 );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
  		
	return true;
}


//-------------------------------------
//^ The SetValue function opens the subkey, saves its value and closes the
//  entry.  This is function saves the entry as a REG_DWORD.
//> value is the value to place into the key
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::SetValue( const unsigned int value )
{
	vdbHKEY hkey;
	LONG rc;

	// open, write, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_SET_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegSetValueEx( hkey, _leafName, NULL, REG_DWORD, (const unsigned char *) &value, 4 );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
  		
	return true;
}


//-------------------------------------
//^ The SetValue function opens the subkey, saves its value and closes the
//  entry.  This is function saves the entry as a REG_DWORD.
//> value is the value to place into the key
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::SetValue( const long value )
{
	vdbHKEY hkey;
	LONG rc;

	// open, write, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_SET_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegSetValueEx( hkey, _leafName, NULL, REG_DWORD, (const unsigned char *) &value, 4 );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
  		
	return true;
}


//-------------------------------------
//^ The SetValue function opens the subkey, saves its value and closes the
//  entry.  This is function saves the entry as a REG_DWORD.
//> value is the value to place into the key
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::SetValue( const unsigned long value )
{
	vdbHKEY hkey;
	LONG rc;

	// open, write, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_SET_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegSetValueEx( hkey, _leafName, NULL, REG_DWORD, (const unsigned char *) &value, 4 );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
  		
	return true;
}


//-------------------------------------
//^ The SetValue function opens the subkey, saves its value and closes the
//  entry.  This is function saves the boolean entry as a REG_DWORD.
//> value is the value to place into the key
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::SetValue( const bool value )
{
	vdbHKEY hkey;
	LONG rc;
	DWORD dwValue = value;

	// open, write, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_SET_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegSetValueEx( hkey, _leafName, NULL, REG_DWORD, (const unsigned char *) &dwValue, 4 );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
  		
	return true;
}


//-------------------------------------
//^ The SetValue function opens the subkey, saves its value and closes the
//  entry.  This is function saves the entry as a REG_DWORD.
//> value is the value to place into the key
//< The return value is true on success and false on failure.
//
bool vdbRegistrationKey::SetValue( const double value )
{
	vdbHKEY hkey;
	LONG rc;

	char szValue[23];
	vdbUtility::SafeFormat( szValue, sizeof(szValue), "%f", value );

	// open, write, close
	rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_SET_VALUE, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegSetValueEx( hkey, _leafName, NULL, REG_SZ, (const unsigned char *) szValue, strlen(szValue) + 1 );
	if ( rc != ERROR_SUCCESS )
		return false;

	rc = RegCloseKey( hkey );	
	if ( rc != ERROR_SUCCESS )
		return false;
  		
	return true;
}


//-------------------------------------
//^ The GetParentName function obtains the left-hand part of the current
//  subkey.  This is everything to the left of the final backslash.
//> parentName is filled with the leading part of the subkey by this function.
//
void vdbRegistrationKey::GetParentName( vdbString& parentName )
{
	assert ( _subkey != _T("") );

	int where = _subkey.ReverseFind( '\\' );
	if ( where <= 0 )
		parentName = _T("");
	else
		parentName = _subkey.First( where );
}


//-------------------------------------
//^ The GetTrailingSubkey function obtains the trailing part of the current
//  subkey.  This is everything to the right of the final backslash.
//> keyName is filled with the trailing part of the subkey by this function.
//
void vdbRegistrationKey::GetTrailingSubkey( vdbString& keyName )
{
	assert ( _subkey != _T("") );

	int where = _subkey.ReverseFind( '\\' );
	if ( where <= 0 )
		keyName = _subkey;
	else
		keyName = _subkey.Middle( where + 1 );
}


//-------------------------------------
//^ The EnumValues function queries the key to get the entire set of leaves 
//> values is filled with the leaf names
//< returns true if succeeded, false if failed
//
bool vdbRegistrationKey::EnumValues( vdbSet<vdbString>& values )
{
	assert( _rootkey != 0 );
	assert( _subkey != _T("") );

	vdbHKEY hkey;
	LONG rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_ALL_ACCESS, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	// how many values does this have
	DWORD countValues;
	rc = RegQueryInfoKey( hkey, NULL, NULL, NULL, NULL, NULL, NULL, &countValues, NULL, NULL, NULL, NULL );
	if ( rc != ERROR_SUCCESS )
		return false;
	
	for ( unsigned int i = 0; i < countValues; i++ )
	{
		TCHAR sz[100];
		DWORD sizeSZ = 100;
		rc = RegEnumValue( hkey, i, sz, &sizeSZ, NULL, NULL, NULL, NULL );
		if ( rc != ERROR_SUCCESS )
			return false;
			
		values.Add( new vdbString( sz ) );
	}

	return true;
}


//-------------------------------------
//^ The EnumSubkeys function queries the key to get the entire set of subkeys 
//> subkeys is filled with the leaf names
//< returns true if succeeded, false if failed
//
bool vdbRegistrationKey::EnumSubkeys( vdbSet<vdbString>& subkeys )
{
	assert( _rootkey != 0 );
	assert( _subkey != _T("") );

	vdbHKEY hkey;
	LONG rc = RegOpenKeyEx( _rootkey, _subkey, NULL, KEY_ALL_ACCESS, &hkey );
	if ( rc != ERROR_SUCCESS )
		return false;

	// how many subkeys does this have
	DWORD countKeys;
	rc = RegQueryInfoKey( hkey, NULL, NULL, NULL, &countKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL );
	if ( rc != ERROR_SUCCESS )
		return false;
	
	for ( unsigned int i = 0; i < countKeys; i++ )
	{
		TCHAR sz[100];
		DWORD sizeSZ = 100;
		rc = RegEnumKey( hkey, i, sz, sizeSZ );
		if ( rc != ERROR_SUCCESS )
			return false;
			
		subkeys.Add( new vdbString( sz ) );
	}

	return true;
}

