//=============================================================================
//
// CLASS:           vdbQuery
//
// OVERVIEW:		The vdbQuery class is used to construct and execute an SQL
//                  query on a type T.  Execution can return a pointer to a
//                  single object, a set of objects or a map of objects.
//                  Class T must have a constructor that takes two arguments
//                  (an OID and a HANDLE_DATABASE).  If no such constructor
//                  exists then you must provide a template specialization for
//                  the function NewObject.
//                  If you want to use the Execute function which uses vdbMap 
//                  you must provide a template specialization for GetKeyName.
//
// FILE NAME:       vdbQuery.cpp
//
// INITIAL DATE:    1/15/95
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef __vdbQuery_h
#define __vdbQuery_h

#ifndef _STRSTREAM_ 
	#include <strstream>
#endif

#ifndef __boolean_h
	#include <boolean.h>
#endif

#ifndef __vdbSet_h
	#include "vdbSet.h"
#endif

#ifndef __vdbMap_h
	#include "vdbMap.h"
#endif

#ifndef __vdbTypedef_h
	#include "vdbTypedef.h"
#endif

class vdbDatabase;
class vdbTable;
class vdbObject;


namespace vdbNames
{
	enum connectorType { AND, OR };
	enum sortOrder { DEFAULT, ASCENDING, DESCENDING };
}

//==========================================================================================
template <class T> class vdbQuery
{
public:
	// essential class functions
	vdbQuery( HANDLE_DATABASE hDB = 0 );			// basic constructor
	vdbQuery( vdbDatabase* pDatabase );				// alternate constructor
	virtual ~vdbQuery();							// destructor

protected:
	vdbQuery( const vdbQuery& rhs );				// copy constructor disabled
	void operator=( const vdbQuery& rhs );			// assignment operator disabled

public:
	// member access
	inline vdbDatabase* GetDatabase();
	inline vdbTable* GetTable() const;
	inline const char* ClientIDColumn( vdbObject* pClient ) const;
	
public:
	// criteria specification
	RETCODE SelectionCriteria( const char* szColumnName, const char* szOperation, OID oid,	vdbNames::connectorType connector = vdbNames::AND );
	RETCODE SelectionCriteria( const char* szColumnName, const char* szOperation, DBBYTE i,	vdbNames::connectorType connector = vdbNames::AND );
	RETCODE SelectionCriteria( const char* szColumnName, const char* szOperation, DBSMALL i, vdbNames::connectorType connector = vdbNames::AND );
	RETCODE SelectionCriteria( const char* szColumnName, const char* szOperation, DBINT i, vdbNames::connectorType connector = vdbNames::AND );
	RETCODE SelectionCriteria( const char* szColumnName, const char* szOperation, DBFLOAT f, vdbNames::connectorType connector = vdbNames::AND );
	RETCODE SelectionCriteria( const char* szColumnName, const char* szOperation, DBDOUBLE f, vdbNames::connectorType connector = vdbNames::AND );
	RETCODE SelectionCriteria( const char* szColumnName, const char* szOperation, const char* sz, vdbNames::connectorType connector = vdbNames::AND );
	RETCODE SelectionCriteria( const char* szColumnName, const char* szOperation, const char* sz, int size, vdbNames::connectorType connector = vdbNames::AND );
	RETCODE SelectionCriteria( const char* szColumnName1, const char* szOperation1, const char* sz1, vdbNames::connectorType connector1, const char* szColumnName2, const char* szOperation2, const char* sz2, vdbNames::connectorType connector2 = vdbNames::AND );
	RETCODE SelectionCriteria( const char* szSQL, vdbNames::connectorType connector = vdbNames::AND );
	RETCODE SortCriteria( const char* szColumnName, vdbNames::sortOrder direction = vdbNames::DEFAULT );
	RETCODE LimitCriteria( int maxRecords );
	
public:
	// query execution	
	RETCODE Execute( vdbSet<T>& set );
	RETCODE Execute( vdbMap<T>& map );
	T* Execute( RETCODE& rc );
	T* Execute();
	int ExecuteCount();

protected:
	// protected helpers
	const char* GetSQLSelect();
	T* NewObject();
	const char* GetKeyName( T& obj );

private:
	// private helpers
	RETCODE DoSelectionCriteria( const char* szColumnName, const char* szOperation, vdbNames::connectorType connector );
	void WriteWithEscapedQuotes( const char* sz );
	bool CheckColumnName( const char* szColumnName );

private:
	// member data
	HANDLE_DATABASE _hDB;			// handle to the database instance
	vdbDatabase* _pDatabase;		// the ODBC environment and database
	int _numSelectionCriteria;		// the number of columns to use in the selection
	int _numSortCriteria;			// the number of columns to use in the sorting 
	char* _sql;						// the SQL statement
	bool _isFrozen;					// the string is frozen upon the first call to GetSQLSelect(); no further additional criteria can be added when frozen
	std::ostrstream _osSelect;		// the output string stream for building the WHERE clause
	std::ostrstream _osSort;		// the output string stream for building the ORDER BY clause
	T* _pAccessToClassT;			// a temporary T used to call T::GetTable()
	int _maxRecords;				// the maximum number of records to fetch with Execute, or -1 for unrestricted
};


//==========================================================================================
// inlines
//==========================================================================================

//-------------------------------------
template<class T>
inline vdbDatabase* vdbQuery<T>::GetDatabase()
{
	return _pDatabase;
}


//-------------------------------------
template<class T>
inline vdbTable* vdbQuery<T>::GetTable() const
{
	assert( _pAccessToClassT != 0 );
	
	return _pAccessToClassT->GetTable();
}


//-------------------------------------
template<class T>
inline const char* vdbQuery<T>::ClientIDColumn( vdbObject* pClient ) const
{
	assert( _pAccessToClassT != 0 );
	
	return _pAccessToClassT->ClientIDColumn( pClient );
}

#endif	// __vdbQuery_h
