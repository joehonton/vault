//=============================================================================
//
// CLASS:           vdbProfile
//
// FILE NAME:       vdbProfile.h
//
// PURPOSE:         A class for enumerating and accessing the sections and keys
//                  of an application defined INI file.  The function naming
//                  scheme is patterned after vdbRegistrationKey
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    12/23/97
//
// COPYRIGHT:       Copyright © 1997 by Joe Honton
//
//=============================================================================

#ifndef __vdbProfile_h
#define __vdbProfile_h

#ifndef __vdbString_h
	#include <vdbString.h>
#endif

#ifndef __vdbSet_h
	#include <vdbSet.h>
#endif


class vdbProfile
{
public:
	// essential class functions
	vdbProfile();
	vdbProfile( const char* filename, const char* branchName, const char* leafName );

public:
	// member manipulation
	void SetFilename( const char* filename );
	void SetBranch( const char* branchName );
	void SetLeaf( const char* leafName );
	const char* GetFilename() const;
	const char* GetBranch() const;
	const char* GetLeaf() const;

public:
	// branch enumeration
	bool EnumBranches( vdbSet<vdbString>& allBranches );
	bool EnumBranches( vdbString& value );
	bool EnumBranches( char* szValue, size_t length );

public:
	// leaf enumeration
	bool EnumLeaves( vdbSet<vdbString>& allLeaves );
	bool EnumLeaves( vdbString& value );
	bool EnumLeaves( char* szValue, size_t length );
	 
public:
	// branch manipulation
	bool DeleteBranch();

public:
	// leaf manipulation
	bool DeleteLeaf();
	bool LeafExists();

public:
	// access to leaf value
	const char* GetValue();
	const vdbString& GetValueString();
	int GetValueInt();
	unsigned int GetValueUnsignedInt();
	long GetValueLong();
	unsigned long GetValueUnsignedLong();
	bool GetValueBool();
	int GetValueHex();
	void SetValue( const char* value );
	void SetValue( const vdbString& value );
	void SetValue( const int value );
	void SetValue( const unsigned int value );
	void SetValue( const long value );
	void SetValue( const unsigned long value );
	void SetValue( const double value );
	void SetValue( const bool value );
	void SetValueHex( const int value );

	const char* GetComment();
	const vdbString& GetCommentString();
	void SetComment( const char* value );

public:
	bool Read();
	bool Write();

public:
	// convenience functions
	void Update( const char* szLeaf, const char* value );
	void Update( const char* szLeaf, const vdbString& value );
	void Update( const char* szLeaf, const int value );
	void Update( const char* szLeaf, const unsigned int value );
	void Update( const char* szLeaf, const long value );
	void Update( const char* szLeaf, const unsigned long value );
	void Update( const char* szLeaf, const double value );
	void Update( const char* szLeaf, const bool value );

	bool ReadParameter( const char* szLeaf, char* szValue, int size, bool bOptional = false );
	bool ReadParameter( const char* szLeaf, short& value, bool bOptional = false );
	bool ReadParameter( const char* szLeaf, int& value, bool bOptional = false );
	bool ReadParameter( const char* szLeaf, double& value, bool bOptional = false );
	bool ReadParameterHex( const char* szLeaf, int& value, bool bOptional = false );

	bool ValidateParameter( int actual, int low, int high );
	bool ValidateParameter( double actual, double low, double high );
	bool ValidateParameter( vdbString* sValidValues, int* replacementValues, int countValidValues, const char* szActualValue, int& actualValue );
	bool ValidateParameterHex( int actual, int low, int high );
	bool ValidateParameterNotNegative( double& actual );

private:
	enum { MAX_ENUM_LEN = 2048 };
	enum { MAX_VALUE_LEN = 81 };
	enum { MAX_COMMENT_LEN = 512 };

private:
	void SplitComment( const char* szCombinedString );
	vdbString CombineComment();
	 
private:
	// members
	vdbString _filename;	// for example "myapp.ini"
	vdbString _branchName;	// for example "[Settings]"
	vdbString _leafName;	// for example "ClientWindowPos"
	vdbString _value;		// for example "0,0"
	vdbString _comment;		// everything following the semicolon
};

#endif  // __vdbProfile_h

