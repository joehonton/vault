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

#ifndef __vdbAppRegistry_h
#define __vdbAppRegistry_h

#ifndef __vdbRegsitrationKey_h
	#include "vdbRegistrationKey.h"
#endif

#ifndef __vdbMap_h
	#include "vdbMap.h"
#endif

#ifndef __vdbString_h
	#include "vdbString.h"
#endif


class vdbAppRegistry
{
protected:
	// essential class functions
	vdbAppRegistry();
	~vdbAppRegistry();
	vdbAppRegistry( const vdbAppRegistry& rhs );			// copy constructor disabled
	vdbAppRegistry& operator=( const vdbAppRegistry& rhs );	// assignment operator disabled

public:
	// public access to this class
	static vdbAppRegistry* Instance();			// use this function for all access to the class
	static void Destroy();						// use this function before exiting the program

public:
	// set up defaults
	void SetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, const TCHAR* szValue );
	void SetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, const int value );
	void SetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, const unsigned int value );
	void SetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, const long value );
	void SetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, const unsigned long value );
	void SetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, const bool value );
	void SetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, const double value );

public:
	// get defaults
	bool GetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, TCHAR* szValue, size_t length );
	bool GetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, vdbString& sValue );
	bool GetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, int& value );
	bool GetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, unsigned int& value );
	bool GetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, long& value );
	bool GetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, unsigned long& value );
	bool GetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, bool& value );
	bool GetDefault( const TCHAR* szBranch, const TCHAR* szLeaf, double& value );

public:
	// set value
	bool SetValue( const TCHAR* szBranch, const TCHAR* szLeaf, const TCHAR* szValue );
	bool SetValue( const TCHAR* szBranch, const TCHAR* szLeaf, const int value );
	bool SetValue( const TCHAR* szBranch, const TCHAR* szLeaf, const unsigned int value );
	bool SetValue( const TCHAR* szBranch, const TCHAR* szLeaf, const long value );
	bool SetValue( const TCHAR* szBranch, const TCHAR* szLeaf, const unsigned long value );
	bool SetValue( const TCHAR* szBranch, const TCHAR* szLeaf, const bool value );
	bool SetValue( const TCHAR* szBranch, const TCHAR* szLeaf, const double value );

public:
	// get value
	bool GetValue( const TCHAR* szBranch, const TCHAR* szLeaf, TCHAR* szValue, size_t length );
	bool GetValue( const TCHAR* szBranch, const TCHAR* szLeaf, vdbString& sValue );
	bool GetValue( const TCHAR* szBranch, const TCHAR* szLeaf, int& value );
	bool GetValue( const TCHAR* szBranch, const TCHAR* szLeaf, unsigned int& value );
	bool GetValue( const TCHAR* szBranch, const TCHAR* szLeaf, long& value );
	bool GetValue( const TCHAR* szBranch, const TCHAR* szLeaf, unsigned long& value );
	bool GetValue( const TCHAR* szBranch, const TCHAR* szLeaf, bool& value );
	bool GetValue( const TCHAR* szBranch, const TCHAR* szLeaf, double& value );

private:
	vdbHKEY _rootkey;				// for example HKEY_LOCAL_MACHINE or HKEY_CURRENT_USER
	vdbString _subkey;				// for example "SOFTWARE\\CrescentBloom\\Proxy\\1.0"
	vdbMap<vdbString> _defaults;	// the default values to use when no item exists in the Registry

private:
	static vdbAppRegistry* _instance;		// the one and only instance
};

#endif  // __vdbAppRegistry_h

