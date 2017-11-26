//=============================================================================
//
// CLASS:           vdbProfile
//
// FILE NAME:       vdbProfile.cpp
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

#include <vdbWindows.h>
#include "vdbProfile.h"
#include <vdbUtility.h>
#include "vdbAssert.h"
#include <stdio.h>
#include <vdbException.h>
#include <strstream>
#include <vdbMessageBox.h>
#include "boolean.h"

//warning C4996: 'sscanf': This function or variable may be unsafe. Consider using sscanf_s instead. 
//warning C4996: 'strcpy': This function or variable may be unsafe. Consider using strcpy_s instead.
#pragma warning( disable : 4996 )


//=============================================================================
// Essential Class functions
//=============================================================================
//-------------------------------------
//^^ The default constructor
vdbProfile::vdbProfile()
	: _filename( "" ),
	  _branchName( "" ),
	  _leafName( "" ),
	  _value( "" ),
	  _comment( "" )
{
}


//-------------------------------------
//^^ The (const char*, const char*, const char*) constructor
vdbProfile::vdbProfile( const char* filename, const char* branchName, const char* leafName )
	: _filename( filename ),
	  _branchName( branchName ),
	  _leafName( leafName ),
	  _value( "" ),
	  _comment( "" )
{
}


//=============================================================================
// Member manipulation
//=============================================================================

//-------------------------------------
//^ The SetFilename function changes the value of the filename.  It must be
//  called if the default constructor is used.
//> filename is the name of the INI file.  If a full path is not specified
//  the file is assumed to be in the WINDOWS\SYSTEM directory
//
void vdbProfile::SetFilename( const char* filename )
{
	_filename = filename;
}


//-------------------------------------
//^ The SetBranch function changes the value of the branchName.  It must be called
//  if the default constructor is used.
//> branchName is the section name to manipulate.  For example "[Settings]".
//  Do not supply the brackets, they will be implied by the member functions
//
void vdbProfile::SetBranch( const char* branchName )
{
	_branchName = branchName;
}


//-------------------------------------
//^ The SetLeaf function changes the value of the branchName.  It must be called
//  if the default constructor is used.
//> leafName is the leaf to manipulate.  For example "ClientWindowPos"
//
void vdbProfile::SetLeaf( const char* leafName )
{
	_leafName = leafName;
}


//-------------------------------------
//^ The GetFilename function returns the value of the filename.
//
const char* vdbProfile::GetFilename() const
{
	return _filename;
}


//-------------------------------------
//^ The GetBranch function returns the value of the branchName.
//
const char* vdbProfile::GetBranch() const
{
	return _branchName;
}


//-------------------------------------
//^ The GetLeaf function returns the value of the branchName.
//
const char* vdbProfile::GetLeaf() const
{
	return _leafName;
}


//=============================================================================
// Branch enumeration functions
//=============================================================================

//-------------------------------------
//^ The EnumBranches function obtains all of the branches of the current file
//  in the form of a set of vdbStrings.
//^ Because this function uses the EnumBranches(vdbString) function, the length
//  of all branch names is limited to 2024 bytes.
//> allBranches is the set of branch names in the file.  If any strings exist
//  in the list upon calling this function they are not removed.  Thus the set
//  is concatenated.
//< returns true on success.  Returns false if the last branch name was truncated
//  because the string buffer was too short.
//
bool vdbProfile::EnumBranches( vdbSet<vdbString>& allBranches )
{
	if ( _filename.IsNull() )
		return false;

	vdbString sBranches;
	bool rc = EnumBranches( sBranches );

	int sos = 0;
	int eos = sBranches.Find( '|' );
	while ( eos != -1 )
	{
		allBranches.Add( new vdbString( sBranches.Middle( sos, eos - sos ) ) );
		sos = eos + 1;
		eos = sBranches.FindNext( '|', sos );
	}

	return rc;
}


//-------------------------------------
//^ The EnumBranches function obtains all of the branches of the current file
//  in the form of a single vdbString where each branch name is separated by a
//  '|' character and the entire string ends in a single null byte.
//> value is the string which contains the branch names
//< returns true on success.  Returns false if the last branch name was truncated
//  because the string buffer was too short.
//
bool vdbProfile::EnumBranches( vdbString& value )
{
	if ( _filename.IsNull() )
		return false;

	char szDefault[] = "";
	char sz[MAX_ENUM_LEN];
	int len = ::GetPrivateProfileString( NULL, NULL, szDefault, sz, MAX_ENUM_LEN, _filename );

	for ( int i = 0; i < len; i++ )
		if ( sz[i] == 0x0 )
			sz[i] = '|';

	value = sz;
	
	if ( len == MAX_ENUM_LEN - 2 )
		return false;
	else
		return true;
}


//-------------------------------------
//^ The EnumBranches function obtains all of the branches of the current file
//  in the form of a character array where each branch name is separated by a
//  '|' and the entire string ends in a single null byte.
//> szValue is the character array to fill
//> length is the length of the character array
//< returns true on success.  Returns false if the last branch name was truncated
//  because the string buffer was too short.
//
bool vdbProfile::EnumBranches( char* szValue, size_t length )
{
	if ( _filename.IsNull() )
		return false;

	char szDefault[] = "";
	int len = ::GetPrivateProfileString( NULL, NULL, szDefault, szValue, length, _filename );

	for ( int i = 0; i < len; i++ )
		if ( szValue[i] == 0x0 )
			szValue[i] = '|';

	if ( len == length - 2 )
		return false;
	else
		return true;
}


//=============================================================================
// Leaf enumeration functions
//=============================================================================

//-------------------------------------
//^ The EnumLeaves function obtains all of the leaves of the current branch
//  in the form of a set of vdbStrings.
//^ Because this function uses the EnumLeaves(vdbString) function, the length
//  of all leaf names is limited to 2024 bytes.
//> allLeaves is the set of leaf names in the file.  If any strings exist
//  in the list upon calling this function they are not removed.  Thus the set
//  is concatenated.
//< returns true on success.  Returns false if the last leaf name was truncated
//  because the string buffer was too short.
//
bool vdbProfile::EnumLeaves( vdbSet<vdbString>& allLeaves )
{
	if ( _filename.IsNull() )
		return false;
	if ( _branchName.IsNull() )
		return false;

	vdbString sLeaves;
	bool rc = EnumLeaves( sLeaves );

	int sos = 0;
	int eos = sLeaves.Find( '|' );
	while ( eos != -1 )
	{
		allLeaves.Add( new vdbString( sLeaves.Middle( sos, eos - sos ) ) );
		sos = eos + 1;
		eos = sLeaves.FindNext( '|', sos );
	}

	return rc;
}


//-------------------------------------
//^ The EnumLeaves function obtains all of the leaves of the current branch
//  in the form of a single vdbString where each leaf name is separated by a
//  '|' character and the entire string ends in a single null byte.
//> value is the string which contains the leaf names
//< returns true on success.  Returns false if the last leaf name was truncated
//  because the string buffer was too short.
//
bool vdbProfile::EnumLeaves( vdbString& value )
{
	if ( _filename.IsNull() )
		return false;
	if ( _branchName.IsNull() )
		return false;

	char szDefault[] = "";
	char sz[MAX_ENUM_LEN];
	int len = ::GetPrivateProfileString( _branchName, NULL, szDefault, sz, MAX_ENUM_LEN, _filename );

	for ( int i = 0; i < len; i++ )
		if ( sz[i] == 0x0 )
			sz[i] = '|';

	value = sz;
	
	if ( len == MAX_ENUM_LEN - 2 )
		return false;
	else
		return true;
}


//-------------------------------------
//^ The EnumLeaves function obtains all of the leaves of the current branch
//  in the form of a character array where each leaf name is separated by a
//  '|' and the entire string ends in a single null byte.
//> szValue is the character array to fill
//> length is the length of the character array
//< returns true on success.  Returns false if the last leaf name was truncated
//  because the string buffer was too short.
//
bool vdbProfile::EnumLeaves( char* szValue, size_t length )
{
	if ( _filename.IsNull() )
		return false;
	if ( _branchName.IsNull() )
		return false;

	char szDefault[] = "";
	int len = ::GetPrivateProfileString( _branchName, NULL, szDefault, szValue, length, _filename );

	for ( int i = 0; i < len; i++ )
		if ( szValue[i] == 0x0 )
			szValue[i] = '|';

	if ( len == length - 2 )
		return false;
	else
		return true;
}


//=============================================================================
// Branch manipulation
//=============================================================================

//-------------------------------------
//^ The DeleteBranch function deletes the branch including all its leaves
//< The return value is true on success and false on failure.
//
bool vdbProfile::DeleteBranch()
{
	if ( _filename.IsNull() )
		return false;
	if ( _branchName.IsNull() )
		return false;

	BOOL rc = ::WritePrivateProfileString( _branchName, NULL, NULL, _filename );

	if ( rc == TRUE )
		return true;
	else
		return false;
}


//=============================================================================
// Leaf manipulation
//=============================================================================

//-------------------------------------
//^ The DeleteLeaf function deletes the current leaf
//< The return value is true on success and false on failure.
//
bool vdbProfile::DeleteLeaf()
{
	if ( _filename.IsNull() )
		return false;
	if ( _branchName.IsNull() )
		return false;
	if ( _leafName.IsNull() )
		return false;

	BOOL rc = ::WritePrivateProfileString( _branchName, _leafName, NULL, _filename );

	return ( rc == TRUE ? true : false );
}


//-------------------------------------
//^ The LeafExists function determines whether or not the current leaf exists.
//< The return value is true if it exists, false if it does not.
//
bool vdbProfile::LeafExists()
{
	if ( _filename.IsNull() )
		return false;
	if ( _branchName.IsNull() )
		return false;
	if ( _leafName.IsNull() )
		return false;

	char szDefault[] = "";
	char szValue[2];
	::GetPrivateProfileString( _branchName, _leafName, szDefault, szValue, 2, _filename );
	
	if ( szValue[0] != 0x0 )
		return true;
	else
		return false;
}


//=============================================================================
// Access to leaf values
//=============================================================================

//-------------------------------------
//^ The GetValue function gets the value of the current branch and leaf.
//
const char* vdbProfile::GetValue()
{
	return (const char*) _value;
}


//-------------------------------------
//^ The GetValueString function gets the value of the current branch and leaf.
//
const vdbString& vdbProfile::GetValueString()
{
	return _value;
}


//-------------------------------------
//^ The GetValueInt function gets the value of the current branch and leaf.
//
int vdbProfile::GetValueInt()
{
	return atoi(_value);
}


//-------------------------------------
//^ The GetValueUnsignedInt function gets the value of the current branch and leaf.
//
unsigned int vdbProfile::GetValueUnsignedInt()
{
	return atoi(_value);
}


//-------------------------------------
//^ The GetValueLong function gets the value of the current branch and leaf.
//
long vdbProfile::GetValueLong()
{
	return atol(_value);
}


//-------------------------------------
//^ The GetValueUnsignedLong function gets the value of the current branch and leaf.
//
unsigned long vdbProfile::GetValueUnsignedLong()
{
	return atol(_value);
}


//-------------------------------------
//^ The GetValueBool function gets the value of the current branch and leaf.
//> value is filled in by the routine
//
bool vdbProfile::GetValueBool()
{
	return (bool) atoi(_value);	// false is 0, true is 1;
}


//-------------------------------------
//^ The GetValueHex function gets the value of the current branch and leaf.
//> value is filled in by the routine
//
int vdbProfile::GetValueHex()
{
	int hexValue;
	sscanf( _value, "%x", &hexValue );
	return hexValue;
}


//-------------------------------------
//^ The SetValue function saves the supplied value to the current branch and leaf.
//> value is the text to save
//
void vdbProfile::SetValue( const char* value )
{
	_value = value;
	_value.StripTrailingSpaces();
}


//-------------------------------------
//^ The SetValue function saves the supplied value to the current branch and leaf.
//> value is the text to save
//
void vdbProfile::SetValue( const vdbString& value )
{
	_value = value;
	_value.StripTrailingSpaces();
}


//-------------------------------------
//^ The SetValue function saves the supplied value to the current branch and leaf.
//> value is the text to save
//
void vdbProfile::SetValue( const int value )
{
	char szValue[18];
	_itoa( value, szValue, 10 );
	_value = szValue;
}


//-------------------------------------
//^ The SetValue function saves the supplied value to the current branch and leaf.
//> value is the text to save
//
void vdbProfile::SetValue( const unsigned int value )
{
	char szValue[18];
	_itoa( value, szValue, 10 );
	_value = szValue;
}


//-------------------------------------
//^ The SetValue function saves the supplied value to the current branch and leaf.
//> value is the text to save
//
void vdbProfile::SetValue( const long value )
{
	char szValue[34];
	_ltoa( value, szValue, 10 );
	_value = szValue;
}


//-------------------------------------
//^ The SetValue function saves the supplied value to the current branch and leaf.
//> value is the text to save
//
void vdbProfile::SetValue( const unsigned long value )
{
	char szValue[34];
	_ltoa( value, szValue, 10 );
	_value = szValue;
}


//-------------------------------------
//^ The SetValue function saves the supplied value to the current branch and leaf.
//> value is the text to save
//
void vdbProfile::SetValue( const double value )
{
	char szValue[15];
	vdbUtility::SafeFormat( szValue, sizeof(szValue), "%f", value );
	_value = szValue;
}


//-------------------------------------
//^ The SetValue function saves the supplied value to the current branch and leaf.
//> value is the text to save
//
void vdbProfile::SetValue( const bool value )
{
	char szValue[2];
	if ( value == true )
		strcpy( szValue, "1" );
	else
		strcpy( szValue, "0" );
	_value = szValue;
}


//-------------------------------------
//^ The SetValueHex function saves the supplied value to the current branch and leaf.
//> value is the text to save
//
void vdbProfile::SetValueHex( const int value )
{
	char szValue[7];
	vdbUtility::SafeFormat( szValue, sizeof(szValue), "%x", value );
	_value = szValue;
}


//-------------------------------------
//^ The GetComment function gets the comment associated with the current branch and leaf.
//
const char* vdbProfile::GetComment()
{
	return (const char*) _comment;
}


//-------------------------------------
//^ The GetComment function gets the comment associated with the current branch and leaf.
//
const vdbString& vdbProfile::GetCommentString()
{
	return _comment;
}


//-------------------------------------
//^ The SetComment function sets the comment associated with the current branch and leaf.
//^ The comment is saved with the value, so you must call one of the overloaded SetValue
//  functions after calling this function.
//> value is the text of the comment
//
void vdbProfile::SetComment( const char* value )
{
	_comment = value;
}


//=============================================================================
// Read and Write functions
//=============================================================================

//-------------------------------------
//^ The Read function reads the value and comment for the current branch and leaf.  Use
//  the GetValue and GetComment functions to access the returned values.
//> returns true on success.  Returns false if the item does not exist or the value was truncated.
//
bool vdbProfile::Read()
{
	if ( _filename.IsNull() )
		return false;
	if ( _branchName.IsNull() )
		return false;
	if ( _leafName.IsNull() )
		return false;

	char szDefault[] = "";
	char szValue[MAX_VALUE_LEN + MAX_COMMENT_LEN];
	int len = ::GetPrivateProfileString( _branchName, _leafName, szDefault, szValue, MAX_VALUE_LEN + MAX_COMMENT_LEN, _filename );
	SplitComment( szValue );

	if ( len == (MAX_VALUE_LEN + MAX_COMMENT_LEN) - 1 || len == 0 )
		return false;
	else
		return true;
}


//-------------------------------------
//^ The Write function writes the value and comment for the current branch and leaf.  Use
//  the SetValue and SetComment functions to set these values before calling this function.
//> returns true on success. false on failure.
//
bool vdbProfile::Write()
{
	if ( _filename.IsNull() )
		return false;
	if ( _branchName.IsNull() )
		return false;
	if ( _leafName.IsNull() )
		return false;

	BOOL rc = ::WritePrivateProfileString( _branchName, _leafName, CombineComment(), _filename );
	
	return ( rc == TRUE ? true : false );
}


//=============================================================================
// Convenience functions
//=============================================================================

//-------------------------------------
//^ The UpdateValue function reads the current value and comment from the file,
//  sets the value to the user-supplied variable, and rewrites the line to the file
//> szLeaf is the item to update.
//> value is the text to save
//
void vdbProfile::Update( const char* szLeaf, const char* value )
{
	_leafName = szLeaf;
	Read();
	_value = value;
	Write();
}


//-------------------------------------
//^ The UpdateValue function reads the current value and comment from the file,
//  sets the value to the user-supplied variable, and rewrites the line to the file
//> szLeaf is the item to update.
//> value is the text to save
//
void vdbProfile::Update( const char* szLeaf, const vdbString& value )
{
	_leafName = szLeaf;
	Read();
	_value = value;
	Write();
}


//-------------------------------------
//^ The UpdateValue function reads the current value and comment from the file,
//  sets the value to the user-supplied variable, and rewrites the line to the file
//> szLeaf is the item to update.
//> value is the text to save
//
void vdbProfile::Update( const char* szLeaf, const int value )
{
	char szValue[18];
	_itoa( value, szValue, 10 );
	_leafName = szLeaf;
	Read();
	_value = szValue;
	Write();
}


//-------------------------------------
//^ The UpdateValue function reads the current value and comment from the file,
//  sets the value to the user-supplied variable, and rewrites the line to the file
//> szLeaf is the item to update.
//> value is the text to save
//
void vdbProfile::Update( const char* szLeaf, const unsigned int value )
{
	char szValue[18];
	_itoa( value, szValue, 10 );
	_leafName = szLeaf;
	Read();
	_value = szValue;
	Write();
}


//-------------------------------------
//^ The UpdateValue function reads the current value and comment from the file,
//  sets the value to the user-supplied variable, and rewrites the line to the file
//> szLeaf is the item to update.
//> value is the text to save
//
void vdbProfile::Update( const char* szLeaf, const long value )
{
	char szValue[34];
	_ltoa( value, szValue, 10 );
	_leafName = szLeaf;
	Read();
	_value = szValue;
	Write();
}


//-------------------------------------
//^ The UpdateValue function reads the current value and comment from the file,
//  sets the value to the user-supplied variable, and rewrites the line to the file
//> szLeaf is the item to update.
//> value is the text to save
//
void vdbProfile::Update( const char* szLeaf, const unsigned long value )
{
	char szValue[34];
	_ltoa( value, szValue, 10 );
	_leafName = szLeaf;
	Read();
	_value = szValue;
	Write();
}


//-------------------------------------
//^ The UpdateValue function reads the current value and comment from the file,
//  sets the value to the user-supplied variable, and rewrites the line to the file
//> szLeaf is the item to update.
//> value is the text to save
//
void vdbProfile::Update( const char* szLeaf, const double value )
{
	char szValue[15];
	vdbUtility::SafeFormat( szValue, sizeof(szValue), "%f", value );
	_leafName = szLeaf;
	Read();
	_value = szValue;
	Write();
}


//-------------------------------------
//^ The UpdateValue function reads the current value and comment from the file,
//  sets the value to the user-supplied variable, and rewrites the line to the file
//> szLeaf is the item to update.
//> value is the text to save
//
void vdbProfile::Update( const char* szLeaf, const bool value )
{
	char szValue[2];
	if ( value == true )
		strcpy( szValue, "1" );
	else
		strcpy( szValue, "0" );
	_leafName = szLeaf;
	Read();
	_value = szValue;
	Write();
}


//=============================================================================
// Parameter reading
//=============================================================================

//-------------------------------------
//^ The ReadParameter function reads a single entry from the initialization file.
//> szLeaf is the item to read.
//> szValue is the value obtained from the initialization file
//> size is the maximum length of the value including the null terminator
//> bOptional is true is this is an optional parameter.  It is false if this is
//  a required parameter
//< returns true if the value was read properly, false if it does not exist
// 
bool vdbProfile::ReadParameter( const char* szLeaf, char* szValue, int size, bool bOptional )
{
	assert( size > 0 );
	_leafName = szLeaf;

	bool rc = Read();
	if ( rc == false && bOptional == false )
	{
		std::ostrstream os;
		os << "The \"";
		os << (const char*) _leafName;
		os << "\" parameter for the \"";
		os << (const char*) _branchName;
		os << "\" section is missing from the ";
		os << (const char*) _filename;
		os << ".  Do you want to continue anyway?" << std::ends;
		char* s = os.str();
		if ( vdbMessageBox( NULL, s, "Initialization Parameter", MB_YESNO ) == IDYES )
			{ delete[] s; s = 0; return true; }
		else
			{ delete[] s; s = 0; return false; }
	}
	else if ( rc == false && bOptional == true )
	{
		szValue[0] = 0;
		return false;
	}
	else
	{
		vdbUtility::SafeCopy( szValue, GetValue(), size-1 );
		return true;
	}
}


//-------------------------------------
//^ The ReadParameter function reads a single entry from the initialization file.
//> szLeaf is the item to read.
//> value is the value obtained from the initialization file
//> bOptional is true is this is an optional parameter.  It is false if this is
//  a required parameter
//< returns true if the value was read properly, false if it does not exist
// 
bool vdbProfile::ReadParameter( const char* szLeaf, int& value, bool bOptional )
{
	char sz[80];
	if ( ReadParameter( szLeaf, sz, sizeof(sz), bOptional ) == false )
		return false;

	value = atoi( sz );
	return true;
}


//-------------------------------------
//^ The ReadParameter function reads a single entry from the initialization file.
//> szLeaf is the item to read.
//> value is the value obtained from the initialization file
//> bOptional is true is this is an optional parameter.  It is false if this is
//  a required parameter
//< returns true if the value was read properly, false if it does not exist
// 
bool vdbProfile::ReadParameter( const char* szLeaf, short& value, bool bOptional )
{
	char sz[80];
	if ( ReadParameter( szLeaf, sz, sizeof(sz), bOptional ) == false )
		return false;

	value = atoi( sz );
	return true;
}


//-------------------------------------
//^ The ReadParameter function reads a single entry from the initialization file.
//> szLeaf is the item to read.
//> value is the value obtained from the initialization file
//> bOptional is true is this is an optional parameter.  It is false if this is
//  a required parameter
//< returns true if the value was read properly, false if it does not exist
// 
bool vdbProfile::ReadParameter( const char* szLeaf, double& value, bool bOptional )
{
	char sz[80];
	if ( ReadParameter( szLeaf, sz, sizeof(sz), bOptional ) == false )
		return false;

	value = atof( sz );
	return true;
}


//-------------------------------------
//^ The ReadParameterHex function reads a single entry from the initialization file.
//> szLeaf is the item to read.
//> value is the value obtained from the initialization file
//> bOptional is true is this is an optional parameter.  It is false if this is
//  a required parameter
//< returns true if the value was read properly, false if it does not exist
// 
bool vdbProfile::ReadParameterHex( const char* szLeaf, int& value, bool bOptional )
{
	char sz[80];
	if ( ReadParameter( szLeaf, sz, sizeof(sz), bOptional ) == false )
		return false;

	sscanf( sz, "%x", &value );
	return true;
}


//=============================================================================
// Parameter validating
//=============================================================================

//-------------------------------------
//^ The ValidateParameter checks to make sure the parameter value is within the requested range
//> actual is the value obtained from the initialization file
//> low is the lowest valid value
//> high is the highest valid value
//< returns true if the value is within range, false if it is not
// 
bool vdbProfile::ValidateParameter( int actual, int low, int high )
{
	if ( actual < low || actual > high )
	{
		std::ostrstream os;
		os << "The \"";
		os << (const char*) _leafName;
		os << "\" parameter for the \"";
		os << (const char*) _branchName;
		os << "\" section of the ";
		os << (const char*) _filename;
		os << " file has a value of ";
		os << actual;
		os << ".  Valid values are in the range from ";
		os << low;
		os << " to ";
		os << high;
		os << ".  Do you want to continue anyway?" << std::ends;
		char* s = os.str();
		if ( vdbMessageBox( NULL, s, "Initialization Parameter", MB_YESNO ) == IDYES )
			{ delete[] s; s = 0; return true; }
		else
			{ delete[] s; s = 0; return false; }
	}
	
	return true;
}


//-------------------------------------
//^ The ValidateParameter checks to make sure the parameter value is within the requested range
//> actual is the value obtained from the initialization file
//> low is the lowest valid value
//> high is the highest valid value
//< returns true if the value is within range, false if it is not
// 
bool vdbProfile::ValidateParameter( double actual, double low, double high )
{
	if ( actual < low || actual > high )
	{
		std::ostrstream os;
		os << "The \"";
		os << (const char*) _leafName;
		os << "\" parameter for the \"";
		os << (const char*) _branchName;
		os << "\" section of the ";
		os << (const char*) _filename;
		os << " file has a value of ";
		os << actual;
		os << ".  Valid values are in the range from ";
		os << low;
		os << " to ";
		os << high;
		os << ".  Do you want to continue anyway?" << std::ends;
		char* s = os.str();
		if ( vdbMessageBox( NULL, s, "Initialization Parameter", MB_YESNO ) == IDYES )
			{ delete[] s; s = 0; return true; }
		else
			{ delete[] s; s = 0; return false; }
	}
	
	return true;
}


//-------------------------------------
//^ The ValidateParameter checks to make sure the parameter value is one of the possible
//  values in the list.
//> sValidValues is the list of valid values
//> replacementValues is the list of replacement values
//> countValidValues is the number of valid values and replacement values in the lists
//> szActualValue is the value obtained from the initialization file
//> actualValue is matching replacement value.  This is supplied by the function.  If the
//  string value passed in is not in the list of valid values, this is set to 0
//< returns true if the value is within the list, false if it is not
// 
bool vdbProfile::ValidateParameter( vdbString* sValidValues, int* replacementValues, int countValidValues, const char* szActualValue, int& actualValue )
{
	for ( int i = 0; i < countValidValues; i++ )
		if ( sValidValues[i].CompareNoCase( szActualValue ) == true )
		{
			actualValue = replacementValues[i];
			return true;
		}	
	
	actualValue = 0;	 
	
	std::ostrstream os;
	os << "The \"";
	os << (const char*) _leafName;
	os << "\" parameter for the \"";
	os << (const char*) _branchName;
	os << "\" section of the ";
	os << (const char*) _filename;
	os << " file has a value of ";
	os << szActualValue;
	os << "." << std::endl << std::endl;
	os << "Valid values are:" << std::endl;
	for ( int i = 0; i < countValidValues; i++ )
	{
		os << "    " << (const char*) sValidValues[i] << std::endl;
	}
	os << std::endl << "Do you want to continue anyway?" << std::ends;
	char* s = os.str();

	int rc = MessageBox( NULL, s, "Initialization Parameter", MB_YESNO );
	delete[] s; s = 0;
	
	if ( rc == IDYES )
		return true;
	else
		return false;
}


//-------------------------------------
//^ The ValidateParameterHex checks to make sure the parameter value is within the requested range
//> actual is the value obtained from the initialization file
//> low is the lowest valid value
//> high is the highest valid value
//< returns true if the value is within range, false if it is not
// 
bool vdbProfile::ValidateParameterHex( int actual, int low, int high )
{
	if ( actual < low || actual > high )
	{
		std::ostrstream os;
		os << "The \"";
		os << (const char*) _leafName;
		os << "\" parameter for the \"";
		os << (const char*) _branchName;
		os << "\" section of the ";
		os << (const char*) _filename;
		os << " file has a value of ";
		os << std::hex << actual;
		os << ".  Valid values are in the range from ";
		os << low;
		os << " to ";
		os << high;
		os << ".  Do you want to continue anyway?" << std::ends;
		char* s = os.str();
		if ( vdbMessageBox( NULL, s, "Initialization Parameter", MB_YESNO ) == IDYES )
			{ delete[] s; s = 0; return true; }
		else
			{ delete[] s; s = 0; return false; }
	}
	
	return true;
}


//-------------------------------------
//^ The ValidateParameterNotNegative checks to make sure the parameter value
//  is greater than or equal to zero.
//> actual is the value obtained from the initialization file
//< returns true if the value is greater than or equal to zero, false if it is not
// 
bool vdbProfile::ValidateParameterNotNegative( double& actual )
{
	if ( actual < 0.0 )
	{
		std::ostrstream os;
		os << "The \"";
		os << (const char*) _leafName;
		os << "\" parameter for the \"";
		os << (const char*) _branchName;
		os << "\" section of the ";
		os << (const char*) _filename;
		os << " file has a value of ";
		os << actual;
		os << ".  This value should be greater than or equal to zero.";
		os << "  Do you want to continue anyway?" << std::ends;
		char* s = os.str();
		if ( vdbMessageBox( NULL, s, "Initialization Parameter", MB_YESNO ) == IDYES )
			{ delete[] s; s = 0; return true; }
		else
			{ delete[] s; s = 0; return false; }
	}
	
	return true;
}


//=============================================================================
// Private functions
//=============================================================================

//-------------------------------------
//^ The SplitComment function splits the combined string into the _value and
//  its associated _comment.  This function is appropriate to call just after
//  reading the string.
//
void vdbProfile::SplitComment( const char* szCombinedString )
{
	vdbString s( szCombinedString );
	int where = s.Find( ';' );

	if ( where != -1 )
	{
		_comment = s.Middle( where + 2 );	// skip the semicolon and the space after it
		_value = s.First( where );
	}
	else
	{
		_comment = "";
		_value = s;
	}

	_value.StripTrailingSpaces();
}


//-------------------------------------
//^ The CombineComment function combines the _value and _comment into a single
//  string.  This function is appropriate to call just before writing the string.
//
vdbString vdbProfile::CombineComment()
{
	// if there is no comment, just return the value
	if ( _comment.IsNull() )
		return _value;

	int len = _value.Length() + _leafName.Length();

	char szPadding[41];
	int i;
	for ( i = 0; i < 40-len; i++ )
		szPadding[i] = ' ';
	szPadding[i] = 0x0;

	vdbString sCombinedString;
	sCombinedString.Format( "%s%s; %s", (const char*) _value, szPadding, (const char*) _comment );
	return sCombinedString;
}


