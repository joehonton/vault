//=============================================================================
//
// CLASS:           vdbFileDialogSort
//
// OVERVIEW:		The vdbFileDialogSort class is a light-weight class used
//                  by the vdbFileDialog class to sort files.
//
// FILE NAME:       vdbFileDialogSort.h
//
// INITIAL DATE:    10/31/98
//
// COPYRIGHT:		Copyright © 1998 by Joe Honton
//
//=============================================================================

#ifndef __vdbFileDialogSort_h
#define __vdbFileDialogSort_h

#ifndef __vdbString_h
	#include <vdbString.h>
#endif

//=============================================================================
class vdbFileDialogSort
{
public:
	// default constructor require by pphSort::ShellSort but not used
	vdbFileDialogSort()	{}

	// constructor for use by pphSort::SelectionSort
	vdbFileDialogSort( const char* szFilename, bool bIsDirectory )
	{
		_sFilename = szFilename;
		_bIsDirectory = bIsDirectory;
		if ( _bIsDirectory )
			_sSortString = "0";
		else
			_sSortString = "1";
		vdbString sUppercase( szFilename );
		sUppercase.ToUpper();
		_sSortString += sUppercase;
	}
	inline bool operator< ( const vdbFileDialogSort& rhs );
	inline bool operator> ( const vdbFileDialogSort& rhs );

public:
	// data members
	vdbString _sFilename;
	bool _bIsDirectory;
	vdbString _sSortString;
};


//=============================================================================
// inline functions
//=============================================================================

bool vdbFileDialogSort::operator< ( const vdbFileDialogSort& rhs )
{
	// nothing to do?
	if ( this == &rhs )
		return false;

	return (_sSortString < rhs._sSortString);
}


bool vdbFileDialogSort::operator> ( const vdbFileDialogSort& rhs )
{
	// nothing to do?
	if ( this == &rhs )
		return false;

	return (_sSortString > rhs._sSortString);
}

#endif  // __vdbFileDialogSort_h

