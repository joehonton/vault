//=============================================================================
//
// CLASS:           vdbRowColumn
//
// OVERVIEW:		The vdbRowColumn class is used to manage a two-dimensional table
//                  of object pointers.  The number of columns and rows must be
//                  set at construction time.  The default pointer for each entry
//                  is NULL.  Use the Adopt function to place an item into the
//                  table.  All objects of type T must be created on the heap.
//                  The vdbRowColumn destructor deletes the adopted objects.
//
// FILE NAME:       vdbRowColumn.h
//
// INITIAL DATE:    2/18/00
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __vdbRowColumn_h
#define __vdbRowColumn_h

#ifndef _ISTREAM_ 
	#include <istream>
#endif

#ifndef _OSTREAM_ 
	#include <ostream>
#endif

#ifndef __vdbStreamHeader_h
	#include <vdbStreamHeader.h>
#endif

#ifndef _INC_MALLOC
	#include <malloc.h>
#endif

#ifndef __vdbAssert_h
	#include <vdbAssert.h>
#endif


//=============================================================================
template <class T> class vdbRowColumn
{
public:
	// essential class functions
	vdbRowColumn( int rowCount, int columnCount );			// basic constructor
	vdbRowColumn();                                         // constructor for use with istream operator
	~vdbRowColumn();										// destructor
	vdbRowColumn( const vdbRowColumn<T>& rhs );					// copy constructor 
	vdbRowColumn<T>& operator=( const vdbRowColumn<T>& rhs );	// assignment operator 

public:
	// public member functions
	bool Adopt( int row, int column, T* pItem );
	T* Get( int row, int column ) const;
	T* Orphan( int row, int column );
	void ClearEntries();
	inline const int CountRows() const;
	inline const int CountColumns() const;

private:
	void AllocateTable( int rowCount, int columnCount );
	void DeleteTable();

private:
	// member data
	T*** _pObjects;
	int _rowCount;
	int _columnCount;

	// io stream
	friend std::ostream& operator<< ( std::ostream& os, const vdbRowColumn<T>& obj );
	friend std::istream& operator>> ( std::istream& is, vdbRowColumn<T>& obj );
};


//=============================================================================
// inline functions
//=============================================================================

//--------------------------------------------
//^ The CountRows function returns a count of the number of rows
//
template<class T>
inline
const int vdbRowColumn<T>::CountRows() const
{
	return _rowCount;
}


//--------------------------------------------
//^ The CountColumns function returns a count of the number of columns
//
template<class T>
inline
const int vdbRowColumn<T>::CountColumns() const
{
	return _columnCount;
}


//=============================================================================
// non-member stream functions
//=============================================================================

//------------------------------------------
//^^ ostream << operator
template<class T>
std::ostream& operator<< ( std::ostream& os, const vdbRowColumn<T>& obj )
{
	char sep = ',';

	os << obj._rowCount << sep << obj._columnCount << endl;
	for ( int i = 0; i < obj._rowCount; i++ )
		for ( int j = 0; j < obj._columnCount; j++ )
		{
			T* pItem = obj.Get(i,j);
			if ( pItem == 0 )
				os << "0" << endl;
			else
				os << "1" << endl << *pItem;
		}

	return os;
}


//------------------------------------------
//^^ istream >> operator
//^ All classes of type T must have a default constructor. 
//^ The input stream operator reallocates the table of object pointers thus
//  wiping out any existing table entries.  In general use the constructor with
//  no arguments to create a 0 x 0 table, then call this function.
//
template<class T>
std::istream& operator>> ( std::istream& is, vdbRowColumn<T>& obj )
{
	char sep;

	if ( is.fail() )
		return is;

	// delete the table of pointers
	obj.DeleteTable();

	// allocate the new table of pointers
	int rowCount;
	int	columnCount;
	is >> rowCount >> sep >> columnCount >> ws;
	assert( rowCount > 0 );
	assert( columnCount > 0 );
	obj.AllocateTable( rowCount, columnCount );

	for ( int i = 0; i < rowCount; i++ )
		for ( int j = 0; j < columnCount; j++ )
		{
			int flag;
			is >> flag >> ws;

			if ( flag == 1 )
			{
				T* pItem = new T;
				is >> *pItem;
				if ( is.fail() )
					{ delete pItem; pItem = 0; break; }
				obj.Adopt( i, j, pItem );
			}
		}

	return is;
}



#endif  // __vdbRowColumn_h



