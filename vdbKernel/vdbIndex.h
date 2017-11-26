//=============================================================================
//
// CLASS:           vdbIndex
//
// ESSENCE:         database index base class 
//
// FILE NAME:       vdbIndex.cpp
//
// INITIAL DATE:    2/1/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef __vdbIndex_h
#define __vdbIndex_h

#include "boolean.h"

class vdbIndex
{
public:
	// essential class functions
	vdbIndex( const char* szIndexName, const char* szColumnName1, bool isUnique );
	vdbIndex( const char* szIndexName, const char* szColumnName1, const char* szColumnName2, bool isUnique );
	vdbIndex( const char* szIndexName, const char* szColumnName1, const char* szColumnName2, const char* szColumnName3, bool isUnique );
	~vdbIndex();									// destructor
	vdbIndex( const vdbIndex& rhs );				// copy constructor 
	vdbIndex& operator=( const vdbIndex& rhs );		// assignment operator 
	
public:
	// member access functions
	inline const char* IndexName();
	inline const char* ColumnName( int which );
	inline int ColumnCount();
	inline bool IsUnique();

private:
	// member data
	char* _szIndexName;
	char* _szColumnName[3];
	int _columnCount;
	bool _isUnique;
};


//==========================================================================================
// inlines

inline const char* vdbIndex::IndexName()
{
	return _szIndexName;
}


inline const char* vdbIndex::ColumnName( int which )
{
	return _szColumnName[which];
}


inline int vdbIndex::ColumnCount()
{
	return _columnCount;
}


inline bool vdbIndex::IsUnique()
{
	return _isUnique;
}


#endif // __vdbIndex_h

