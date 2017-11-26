//=============================================================================
//
// CLASS:           vdbColumn
//
// ESSENCE:         database column base class 
//
// FILE NAME:       vdbColumn.h
//
// INITIAL DATE:    2/1/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef __vdbColumn_h
#define __vdbColumn_h

class vdbDatabase;


//=============================================================================
class vdbColumn
{
public:
	// essential class functions
	vdbColumn( vdbDatabase* pDatabase, const char* szColumnName, const char* szColumnType );	// basic constructor
	~vdbColumn();														// destructor
	vdbColumn( const vdbColumn& rhs );									// copy constructor 
	vdbColumn& operator=( const vdbColumn& rhs );						// assignment operator 

public:
	// member access functions
	inline const char* Name();
	const char* Type();
	inline const char* RawType() const;
	inline bool IsDataAtExecution() const;

private:
	// member data
	char* _szColumnName;
	char* _szColumnType;
	vdbDatabase* _pDatabase;
	bool _bDataAtExecution;			// true for "longvarchar" and "longvarbinary"
};


//=============================================================================
// inlines
//=============================================================================

inline const char* vdbColumn::Name()
{
	return _szColumnName;
}


inline const char* vdbColumn::RawType() const
{
	return _szColumnType;
}


inline bool vdbColumn::IsDataAtExecution() const
{
	return _bDataAtExecution;
}


#endif // __vdbColumn_h

