//=============================================================================
//
// CLASS:           vdbAppRegistry
//
// FILE NAME:       vdbAppRegistry.h
//
// PURPOSE:         Registry access for an application with default values
//					supplied upon first usage.  The first usage of this class
//                  in an application must occur after vdbApp::g_sRegistrationRoot
//                  has been set.
//
// INITIAL DATE:    12/8/2003
//
// COPYRIGHT:       Copyright © 2003 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbAppRegistry.h"
#include "vdbApp.h"
#include "vdbRegistrationKey.h"
#include "vdbException.h"
#include "vdbUtility.h"

//=============================================================================
// Essential class functions -- Singleton
//=============================================================================

//-------------------------------------
//^^ The vdbAppRegistry constructor
vdbAppRegistry::vdbAppRegistry()
	: _rootkey( HKEY_CURRENT_USER ),
	  _subkey( vdbApp::GetRegistrationRoot() )
{
	if ( _subkey == "" )
		assert( false );	// You must call vdbApp::SetRegistrationRoot() prior to using this
}


//-------------------------------------
//^^ The vdbAppRegistry destructor.
//^ Every application must call delete on the vdbAppRegistry instance
//  created through the Instance function.
vdbAppRegistry::~vdbAppRegistry()
{
	_instance = 0;
}


//-------------------------------------
// copy constructor disabled
vdbAppRegistry::vdbAppRegistry( const vdbAppRegistry& rhs )
{
	assert( false );
}


//-------------------------------------
// assignment operator disabled
vdbAppRegistry& vdbAppRegistry::operator=( const vdbAppRegistry& rhs )
{
	assert( false );
	return *this;
}


//-------------------------------------
// The static member pointer
vdbAppRegistry* vdbAppRegistry::_instance = 0;


//-------------------------------------
//^ The Instance funcion returns a pointer to the vdbAppRegistry
//  instance. All access to the set is through this member function.
//  The first call to the function instantiates the class.  Subsequent
//  calls return a pointer to the same instance.  This mechanism ensures
//  that one and only one instance per application exists.
//
//^ Every application must also call Destroy on the instance created through
//  Instance.
//
vdbAppRegistry* vdbAppRegistry::Instance()
{
	if ( _instance == 0 )
		_instance = new vdbAppRegistry;
	if ( _instance == 0 ) throw vdbMemoryException();

	return _instance;
}


//-------------------------------------
void vdbAppRegistry::Destroy()
{
	if ( _instance != 0 )
	{
		delete _instance; _instance = 0;
	}
}


//=============================================================================
// set up defaults
//=============================================================================

//-------------------------------------
//^ The SetDefault function adds/replaces an entry in the default table for later
//  use by the GetDefault functions and indirectly by the GetValue functions
//> szBranch may be a simple name like "Settings" or a compound name like "Tool\\Settings"
//> szLeaf is the name of the value within the branch
//< szValue is the defult value to set
//
void vdbAppRegistry::SetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, const TCHAR* szValue )
{
	vdbString sBranchLeaf;
	sBranchLeaf.Format( "%s\\%s", szBranch, szLeaf );
	vdbString* p = _defaults[ sBranchLeaf ];
	if ( p == 0 )
	{
		// no default exists, so add one
		_defaults.Add( new vdbString(szValue), sBranchLeaf );
	}
	else
	{
		// replace the existing default
		*p = szValue;
	}
}


//-------------------------------------
void vdbAppRegistry::SetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, int value )
{
	vdbString sValue;
	sValue.Format( "%d", value );
	SetDefault( szBranch, szLeaf, sValue );
}


//-------------------------------------
void vdbAppRegistry::SetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, bool value )
{
	vdbString sValue;
	sValue.Format( "%d", (int) value );
	SetDefault( szBranch, szLeaf, sValue );
}


//=============================================================================
// get defaults
//=============================================================================

//-------------------------------------
//^ The GetDefault function retrieves the previously set default table entry
//> szBranch may be a simple name like "Settings" or a compound name like "Tool\\Settings"
//> szLeaf is the name of the value within the branch
//< szValue is the defult value to get.  Returns an empty string if not found
//< length is the max length of szValue
//< returns true if found or false if not found
//
bool vdbAppRegistry::GetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, TCHAR* szValue, size_t length )
{
	vdbString sBranchLeaf;
	sBranchLeaf.Format( "%s\\%s", szBranch, szLeaf );
	vdbString* p = _defaults[ sBranchLeaf ];
	if ( p == 0 )
	{
		szValue[0] = 0x0;
		return false;
	}
	else
	{
		vdbUtility::SafeCopy( szValue, *p, length );
		return true;
	}
}


//-------------------------------------
bool vdbAppRegistry::GetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, vdbString& sValue )
{
	vdbString sBranchLeaf;
	sBranchLeaf.Format( "%s\\%s", szBranch, szLeaf );
	vdbString* p = _defaults[ sBranchLeaf ];
	if ( p == 0 )
	{
		sValue = "";
		return false;
	}
	else
	{
		sValue = *p;
		return true;
	}
}


//-------------------------------------
bool vdbAppRegistry::GetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, int& value )
{
	vdbString sValue;
	bool rc = GetDefault( szBranch, szLeaf, sValue );
	value = atoi( sValue );
	return rc;
}


//-------------------------------------
bool vdbAppRegistry::GetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, bool& value )
{
	vdbString sValue;
	bool rc = GetDefault( szBranch, szLeaf, sValue );
	value = (bool) atoi( sValue );
	return rc;
}


//=============================================================================
// set value
//=============================================================================

//-------------------------------------
//^ The SetValue function sets the current registry value for the given
//  branch relative to the application root branch.
//> szBranch may be a simple name like "Settings" or a compound name like "Tool\\Settings"
//> szLeaf is the name of the value within the branch to obtain
//< szValue is returned.  It may be either the value obtained from the registry
//  or a default value previously defined, or an empty string if neither are available
//< returns true if successful, false if attempting to set a Registry key that doesn't exist
//
bool vdbAppRegistry::SetValue( const TCHAR* szBranch, const TCHAR* szLeaf, const TCHAR* szValue )
{
	vdbString sSubkey;
	sSubkey.Format( "%s\\%s", (const TCHAR*) _subkey, szBranch );

	// try looking in the Registry, then the default table
	vdbRegistrationKey key( _rootkey, sSubkey, szLeaf );
	return key.SetValue( szValue );
}


//-------------------------------------
bool vdbAppRegistry::SetValue( const TCHAR* szBranch, const TCHAR* szLeaf, int value )
{
	vdbString sValue;
	sValue.Format( "%d", value );
	return SetValue( szBranch, szLeaf, sValue );
}


//-------------------------------------
bool vdbAppRegistry::SetValue( const TCHAR* szBranch, const TCHAR* szLeaf, bool value )
{
	vdbString sValue;
	sValue.Format( "%d", (int) value );
	return SetValue( szBranch, szLeaf, sValue );
}


//=============================================================================
// get value
//=============================================================================

//-------------------------------------
//^ The GetValue function obtains the current registry setting for the given
//  branch relative to the application root branch.
//> szBranch may be a simple name like "Settings" or a compound name like "Tool\\Settings"
//> szLeaf is the name of the value within the branch to obtain
//< szValue is returned.  It may be either the value obtained from the registry
//  or a default value previously defined, or an empty string if neither are available
//> length is the max length of szValue
//< returns true if the value was obtained from the Registry.
//< returns true if the default value was returned.
//< returns false only in the case where neither the Registry nor the default was obtainable.
//
bool vdbAppRegistry::GetValue( const TCHAR* szBranch, const TCHAR* szLeaf, TCHAR* szValue, size_t length )
{
	vdbString sSubkey;
	sSubkey.Format( "%s\\%s", (const TCHAR*) _subkey, szBranch );

	// try looking in the Registry, then the default table
	vdbRegistrationKey key( _rootkey, sSubkey, szLeaf );
	if ( key.GetValue( szValue, length ) == false )
		return GetDefault( szBranch, szLeaf, szValue, length ); 
	else		
		return true;
}


//-------------------------------------
bool vdbAppRegistry::GetValue( const TCHAR* szBranch, const TCHAR* szLeaf, vdbString& sValue )
{
	vdbString sSubkey;
	sSubkey.Format( "%s\\%s", (const TCHAR*) _subkey, szBranch );

	// try looking in the Registry, then the default table
	vdbRegistrationKey key( _rootkey, sSubkey, szLeaf );
	if ( key.GetValue( sValue ) == false )
		return GetDefault( szBranch, szLeaf, sValue ); 
	else		
		return true;
}


//-------------------------------------
bool vdbAppRegistry::GetValue( const TCHAR* szBranch, const TCHAR* szLeaf, int& value )
{
	vdbString sValue;
	bool rc = GetValue( szBranch, szLeaf, sValue );
	value = atoi( sValue );
	return rc;
}


//-------------------------------------
bool vdbAppRegistry::GetValue( const TCHAR* szBranch, const TCHAR* szLeaf, bool& value )
{
	vdbString sValue;
	bool rc = GetValue( szBranch, szLeaf, sValue );
	value = (bool) atoi( sValue );
	return rc;
}

