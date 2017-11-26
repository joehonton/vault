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
// INITIAL DATE:    2/3/95
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifdef USE_PRE_HDR
	#include "vdbHeader.h"
#else
	#include "vdbQuery.h"
	#include "vdbPool.h"
	#include <vdbAssert.h>
	#include "vdbStatement.h"
	#include "vdbDatabase.h"
	#include "vdbException.h"
	#include "vdbUtility.h"
#endif

#include <stdio.h>


//=============================================================================
//  Essential class functions
//=============================================================================

//--------------------------------------------
//^^ vdbQuery basic constructor
template<class T>
vdbQuery<T>::vdbQuery( HANDLE_DATABASE hDB )
	: _hDB( hDB ),
	  _pDatabase( vdbPool::Instance()->GetDatabase( hDB ) ),
	  _numSelectionCriteria( 0 ),
	  _numSortCriteria( 0 ),
	  _isFrozen ( false ),
	  _sql( 0 ),
	  _pAccessToClassT( 0 ),
	  _maxRecords( -1 )
{
	_pAccessToClassT = NewObject();
	_osSelect << " WHERE ";
	_osSort << " ORDER BY ";
}
	

//--------------------------------------------
//^^ vdbQuery alternate constructor
template<class T>
vdbQuery<T>::vdbQuery( vdbDatabase* pDatabase )
	: _hDB( vdbPool::Instance()->GetHandle( pDatabase ) ),
	  _pDatabase( pDatabase ),
	  _numSelectionCriteria( 0 ),
	  _numSortCriteria( 0 ),
	  _isFrozen ( false ),
	  _sql( 0 ),
	  _pAccessToClassT( 0 ),
	  _maxRecords( -1 )
{
	assert( _hDB != -1 );

	_pAccessToClassT = NewObject();
	_osSelect << " WHERE ";
	_osSort << " ORDER BY ";
}
	

//--------------------------------------------
//^^ vdbQuery destructor
template<class T>
vdbQuery<T>::~vdbQuery()
{
	delete _pAccessToClassT; _pAccessToClassT = 0;
	if ( _isFrozen )
		delete[] _sql;    
}


//--------------------------------------------
//^^ vdbQuery copy constructor
//^ The copy constructor is disabled because there is no copy constructor
//  available for ostrstream objects.  Copying ostrstream objects that are
//  frozen is possible; however, attempting to copy ostrstream objects that
//  are not frozen is not possible for objects using dynamic string allocation.
template<class T>
vdbQuery<T>::vdbQuery( const vdbQuery& rhs )
{
	assert( false );  // copying vdbQuery objects is not allowed
}


//--------------------------------------------
//^^ vdbQuery assignment operator
//^  Disabled. See discussion for copy constructor.
template<class T>
void vdbQuery<T>::operator=( const vdbQuery& rhs )
{
	assert( false );  // assignment of vdbQuery objects is not allowed
}


//=============================================================================
// Criteria specification
//=============================================================================

//--------------------------------------------
//^^ SelectionCriteria for OID
//^ The SelectionCriteria adds a selection criteria of type OID to the query
//> szColumnName is the name of the column to use in this restrictive clause.
//> szOperation is the comparision operator for the clause.  Valid values are: 
//  "=", "<>", "<", "<=", ">" and ">=".
//> oid is the value of the restrictive clause
//> connector is either vdbNames::OR or vdbNames::AND.  The default is AND.
//< Returns DBS_COLUMN_NOT_FOUND if the column name is invalid and _DEBUG is
//  defined for the compilation.  Returns DBS_ERROR if the query has already been
//  executed.  Returns DBS_OK if the criteria was successfully added.
//
template<class T>
RETCODE vdbQuery<T>::SelectionCriteria( const char* szColumnName, const char* szOperation, OID oid, vdbNames::connectorType connector )
{
	RETCODE rc;
	if ( rc = DoSelectionCriteria( szColumnName, szOperation, connector ) != DBS_OK )
		return rc;
	
	_osSelect << " " << oid;
	return DBS_OK;
}


//--------------------------------------------
//^^ SelectionCriteria for DBBYTE
//^ The SelectionCriteria adds a selection criteria of type OID to the query
//> szColumnName is the name of the column to use in this restrictive clause.
//> szOperation is the comparision operator for the clause.  Valid values are: 
//  "=", "<>", "<", "<=", ">", and ">=".
//> i is the value of the restrictive clause
//> connector is either vdbNames::OR or vdbNames::AND.  The default is AND.
//< Returns DBS_COLUMN_NOT_FOUND if the column name is invalid and _DEBUG is
//  defined for the compilation.  Returns DBS_ERROR if the query has already been
//  executed.  Returns DBS_OK if the criteria was successfully added.
//
template<class T>
RETCODE vdbQuery<T>::SelectionCriteria( const char* szColumnName, const char* szOperation, DBBYTE i, vdbNames::connectorType connector )
{
	RETCODE rc;
	if ( rc = DoSelectionCriteria( szColumnName, szOperation, connector ) != DBS_OK )
		return rc;
	
	_osSelect << " '" << i << "'";
	return DBS_OK;
}


//--------------------------------------------
//^^ SelectionCriteria for DBSMALL
//^ The SelectionCriteria adds a selection criteria of type DBSMALL to the query
//> szColumnName is the name of the column to use in this restrictive clause.
//> szOperation is the comparision operator for the clause.  Valid values are: 
//  "=", "<>", "<", "<=", ">", and ">=".
//> i is the value of the restrictive clause
//> connector is either vdbNames::OR or vdbNames::AND.  The default is AND.
//< Returns DBS_COLUMN_NOT_FOUND if the column name is invalid and _DEBUG is
//  defined for the compilation.  Returns DBS_ERROR if the query has already been
//  executed.  Returns DBS_OK if the criteria was successfully added.
//
template<class T>
RETCODE vdbQuery<T>::SelectionCriteria( const char* szColumnName, const char* szOperation, DBSMALL i, vdbNames::connectorType connector )
{
	RETCODE rc;
	if ( rc = DoSelectionCriteria( szColumnName, szOperation, connector ) != DBS_OK )
		return rc;
	
	_osSelect << " " << i;
	return DBS_OK;
}


//--------------------------------------------
//^^ SelectionCriteria for DBINT
//^ The SelectionCriteria adds a selection criteria of type DBINT to the query
//> szColumnName is the name of the column to use in this restrictive clause.
//> szOperation is the comparision operator for the clause.  Valid values are: 
//  "=", "<>", "<", "<=", ">", and ">=".
//> i is the value of the restrictive clause
//> connector is either vdbNames::OR or vdbNames::AND.  The default is AND.
//< Returns DBS_COLUMN_NOT_FOUND if the column name is invalid and _DEBUG is
//  defined for the compilation.  Returns DBS_ERROR if the query has already been
//  executed.  Returns DBS_OK if the criteria was successfully added.
//
template<class T>
RETCODE vdbQuery<T>::SelectionCriteria( const char* szColumnName, const char* szOperation, DBINT i, vdbNames::connectorType connector )
{
	RETCODE rc;
	if ( rc = DoSelectionCriteria( szColumnName, szOperation, connector ) != DBS_OK )
		return rc;
	
	_osSelect << " " << i;
	return DBS_OK;
}


//--------------------------------------------
//^^ SelectionCriteria for DBFLOAT
//^ The SelectionCriteria adds a selection criteria of type DBFLOAT to the query
//> szColumnName is the name of the column to use in this restrictive clause.
//> szOperation is the comparision operator for the clause.  Valid values are: 
//  "=", "<>", "<", "<=", ">", and ">=".
//> f is the value of the restrictive clause
//> connector is either vdbNames::OR or vdbNames::AND.  The default is AND.
//< Returns DBS_COLUMN_NOT_FOUND if the column name is invalid and _DEBUG is
//  defined for the compilation.  Returns DBS_ERROR if the query has already been
//  executed.  Returns DBS_OK if the criteria was successfully added.
//
template<class T>
RETCODE vdbQuery<T>::SelectionCriteria( const char* szColumnName, const char* szOperation, DBFLOAT f, vdbNames::connectorType connector )
{
	RETCODE rc;
	if ( rc = DoSelectionCriteria( szColumnName, szOperation, connector ) != DBS_OK )
		return rc;
	
	_osSelect << " " << f;
	return DBS_OK;
}


//--------------------------------------------
//^^ SelectionCriteria for DBDOUBLE
//^ The SelectionCriteria adds a selection criteria of type DBDOUBLE to the query
//> szColumnName is the name of the column to use in this restrictive clause.
//> szOperation is the comparision operator for the clause.  Valid values are: 
//  "=", "<>", "<", "<=", ">", and ">=".
//> f is the value of the restrictive clause
//> connector is either vdbNames::OR or vdbNames::AND.  The default is AND.
//< Returns DBS_COLUMN_NOT_FOUND if the column name is invalid and _DEBUG is
//  defined for the compilation.  Returns DBS_ERROR if the query has already been
//  executed.  Returns DBS_OK if the criteria was successfully added.
//
template<class T>
RETCODE vdbQuery<T>::SelectionCriteria( const char* szColumnName, const char* szOperation, DBDOUBLE f, vdbNames::connectorType connector )
{
	RETCODE rc;
	if ( rc = DoSelectionCriteria( szColumnName, szOperation, connector ) != DBS_OK )
		return rc;
	
	_osSelect << " " << f;
	return DBS_OK;
}


//--------------------------------------------
//^^ SelectionCriteria for const char* NULL-terminated
//^ The SelectionCriteria adds a selection criteria of type const char* to the query
//> szColumnName is the name of the column to use in this restrictive clause.
//> szOperation is the comparision operator for the clause.  Valid values are: 
//  "=", "<>", "<", "<=", ">", ">=", "LIKE" and "NOT LIKE".
//> sz is the value of the restrictive clause
//> connector is either vdbNames::OR or vdbNames::AND.  The default is AND.
//< Returns DBS_COLUMN_NOT_FOUND if the column name is invalid and _DEBUG is
//  defined for the compilation.  Returns DBS_ERROR if the query has already been
//  executed.  Returns DBS_OK if the criteria was successfully added.
//
template<class T>
RETCODE vdbQuery<T>::SelectionCriteria( const char* szColumnName, const char* szOperation, const char* sz, vdbNames::connectorType connector )
{
	RETCODE rc;
	if ( rc = DoSelectionCriteria( szColumnName, szOperation, connector ) != DBS_OK )
		return rc;
	
	_osSelect << " '";
	WriteWithEscapedQuotes( sz );
	_osSelect << "'";

	return DBS_OK;
}


//--------------------------------------------
//^^ SelectionCriteria for const char* non-NULL-terminated
//^ The SelectionCriteria adds a selection criteria of type const char* to the query
//> szColumnName is the name of the column to use in this restrictive clause.
//> szOperation is the comparision operator for the clause.  Valid values are: 
//  "=", "<>", "<", "<=", ">", ">=", "LIKE" and "NOT LIKE".
//> sz is the value of the restrictive clause
//> size is the length of the value sz.
//> connector is either vdbNames::OR or vdbNames::AND.  The default is AND.
//< Returns DBS_COLUMN_NOT_FOUND if the column name is invalid and _DEBUG is
//  defined for the compilation.  Returns DBS_ERROR if the query has already been
//  executed.  Returns DBS_OK if the criteria was successfully added.
//
template<class T>
RETCODE vdbQuery<T>::SelectionCriteria( const char* szColumnName, const char* szOperation, const char* sz, int size, vdbNames::connectorType connector )
{
	char* psz = new char[size+1];
	if ( psz == NULL )
		throw vdbMemoryException();
	vdbUtility::SafeCopy( psz, sz, size+1 );

	RETCODE rc = SelectionCriteria( szColumnName, szOperation, psz, connector );

	delete[] psz; psz = 0;
	return rc;
}


//--------------------------------------------
//^ This SelectionCriteria variant builds a select statement criteria something like:
//  "(column1 = value1 AND column2 = value2)"
//> connector1 is used as the connector for the two phrases inside the parenthesis
//> connector2 is used as the connector between this compound phase and the
//  previous phrase.
//
template<class T>
RETCODE vdbQuery<T>::SelectionCriteria( const char* szColumnName1, const char* szOperation1, const char* sz1, vdbNames::connectorType connector1, const char* szColumnName2, const char* szOperation2, const char* sz2, vdbNames::connectorType connector2 )
{
	if ( _isFrozen )
		return DBS_ERROR; 			// can't add additional criteria

	if ( CheckColumnName( szColumnName1 ) == false )
		return DBS_ERROR;
	if ( CheckColumnName( szColumnName2 ) == false )
		return DBS_ERROR;
	
	if ( _numSelectionCriteria == 0 )
		_osSelect << "(";
	else // if ( _numSelectionCriteria != 0 )
	{
		if ( connector2 == vdbNames::AND )
			_osSelect << ") AND (";
		else
			_osSelect << " OR ";
	}

	_osSelect << "(";
	_osSelect << szColumnName1 << " " << szOperation1;
	_osSelect << " '";
	WriteWithEscapedQuotes( sz1 );
	_osSelect << "'";

	if ( connector1 == vdbNames::AND )
		_osSelect << " AND ";
	else
		_osSelect << " OR ";

	_osSelect << szColumnName2 << " " << szOperation2;
	_osSelect << " '";
	WriteWithEscapedQuotes( sz2 );
	_osSelect << "'";
	_osSelect << ")";


	_numSelectionCriteria++;
	return DBS_OK;
}


//--------------------------------------------
//^ This SelectionCriteria variant is for adding a full SQL phrase to the
//  Select string
//^ The caller must remember to do things like escaping quote characters and
//  ensuring valid column names.
template<class T>
RETCODE vdbQuery<T>::SelectionCriteria( const char* szSQL, vdbNames::connectorType connector = vdbNames::AND )
{
	if ( _isFrozen )
		return DBS_ERROR; 			// can't add additional criteria

	if ( _numSelectionCriteria == 0 )
		_osSelect << "(";
	else // if ( _numSelectionCriteria != 0 )
	{
		if ( connector == vdbNames::AND )
			_osSelect << ") AND (";
		else
			_osSelect << " OR ";
	}

	_osSelect << szSQL;
		
	_numSelectionCriteria++;
	return DBS_OK;
}


//--------------------------------------------
//^ The SortCriteria function adds a sort criteria on the column name passed.
//  The sort direction need not be specified to use the database default sorting
//  order (usually ASCENDING).  More than one sort criteria can be added.  Just
//  call this routine multiple times.
//> szColumnName is the name of the column to sort on
//> direction is vdbNames::DEFAULT, vdbNames::ASCENDING, or
//  vdbNames::DESCENDING
//< Returns DBS_COLUMN_NOT_FOUND if the column name is invalid and _DEBUG is
//  defined for the compilation.  Returns DBS_ERROR if the query has already been
//  executed.  Returns DBS_OK if the sort criteria was successfully added.
//
template<class T>
RETCODE vdbQuery<T>::SortCriteria( const char* szColumnName, vdbNames::sortOrder direction )
{
	if ( _isFrozen )
		return DBS_ERROR;			// can't add additional criteria

	if ( CheckColumnName( szColumnName ) == false )
		return DBS_ERROR;
	
	if ( _numSortCriteria != 0 )
		_osSort << ", ";

	_osSort << szColumnName;

	if ( direction == vdbNames::ASCENDING )
		_osSort << " ASC ";
	else if ( direction == vdbNames::DESCENDING )
		_osSort << " DESC ";

	_numSortCriteria++;
	return DBS_OK;
}


//--------------------------------------------
//^ The LimitCriteria function restricts the number of records fetched through
//  the Execute command
//> maxRecords can be -1 to allow unrestricted retrieval or any positive number
//  to restrict retrieval to a limited number.
//< Returns DBS_OK
//
template<class T>
RETCODE vdbQuery<T>::LimitCriteria( int maxRecords )
{
	_maxRecords = maxRecords;
	return DBS_OK;
}


//=============================================================================
// Query execution
//=============================================================================

//--------------------------------------------
//^ The Execute function gets the set of objects matching the selection criteria.
//> set is the collection to add objects to.  This set of objects of type T must
//  be created before calling this routine.  Upon completion, the set will
//  contain all objects which match the selection criteria.  The set is not
//  cleared by the routine.  Thus you may call this with a non-empty set to 
//  add objects.
//< Returns DBS_OK if successful or the return code from the ODBC statement if
//  there was a problem.
//
template<class T>
RETCODE vdbQuery<T>::Execute( vdbSet<T>& set )
{
	const char* sql = GetSQLSelect();

	vdbStatement stmt( GetDatabase() );																
	RETCODE rc = stmt.Execute( sql );														
	
	// bind the parameter values and fetch until done
	int count = 0;
	while ( rc == SQL_SUCCESS  )
	{
		if ( _maxRecords > 0 && count >= _maxRecords )
			break;

		T* row = NewObject();
		row->Bind( stmt );																
		rc = stmt.Fetch();																
		if ( rc == SQL_NO_DATA_FOUND )													
		{																				
			delete row;
			row = 0;
			break;																		
		}																				
		else if ( rc != SQL_SUCCESS )													
		{																				
			delete row;
			row = 0;
			return stmt.Error(0);	
		}
		
		// add object to the set                                                                                
		row->Persistent();																
		row->PostRetrieveTrigger();
		set.Add( row );
		count++;
	}                                                                                   
	return DBS_OK;
}


//--------------------------------------------
//^ The Execute function gets the map of objects matching the selection criteria.
//  Each entry in the map will be added using the key value returned from the
//  function GetKeyName.  Thus you must provide a suitable specialization for
//  this function. 
//> map is the collection to add objects to.  This map of objects of type T must
//  be created before calling this routine.  Upon completion, the map will
//  contain all objects which match the selection criteria.  The map is not
//  cleared by the routine.  Thus you may call this with a non-empty map to 
//  add objects.
//< Returns DBS_OK if successful or the return code from the ODBC statement if
//  there was a problem.
//
template<class T>
RETCODE vdbQuery<T>::Execute( vdbMap<T>& map )
{
	const char* sql = GetSQLSelect();

	vdbStatement stmt( GetDatabase() );																
	RETCODE rc = stmt.Execute( sql );														
	
	// bind the parameter values and fetch until done
	int count = 0;
	while ( rc == SQL_SUCCESS  )
	{
		if ( _maxRecords != -1 && count >= _maxRecords )
			break;

		T* row = NewObject();
		row->Bind( stmt );																
		rc = stmt.Fetch();																
		if ( rc == SQL_NO_DATA_FOUND )													
		{																				
			delete row;
			row = 0;
			break;																		
		}																				
		else if ( rc != SQL_SUCCESS )													
		{																				
			delete row;
			row = 0;
			return stmt.Error(0);	
		}
		
		// add object to the set                                                                                
		row->Persistent();																
		row->PostRetrieveTrigger();
		map.Add( row, GetKeyName( *row ) ); 														
		count++;
	}                                                                                   
	return DBS_OK;
}


//--------------------------------------------
//^ The Execute function gets one row from the database matching the selection
//  criteria.  If no matching row is found, returns NULL.  If more than one
//  matching row exists, it returns the first one found.
//< Returns a pointer to the object retrieved or NULL if none.
//
template<class T>
T* vdbQuery<T>::Execute()
{
	RETCODE rc;
	return Execute( rc );
}


//--------------------------------------------
//^ The Execute function gets one row from the database matching the selection
//  criteria.  If no matching row is found, returns NULL.  If more than one
//  matching row exists, it returns the first one found.
//> rc is the return code from the ODBC statement or fetch if there was a
//  problem.  rc is SQL_SUCCESS if a row was found. 
//< Returns a pointer to the object retrieved or NULL if none.
//
template<class T>
T* vdbQuery<T>::Execute( RETCODE& rc )
{
	const char* sql = GetSQLSelect();

	vdbStatement stmt( GetDatabase() );																
	rc = stmt.Execute( sql );														
	if ( rc != SQL_SUCCESS )													
		return 0;	
	
	// bind the parameter values and fetch first row only
	T* row = NewObject();
	row->Bind( stmt );																
	rc = stmt.Fetch();																
	if ( rc != SQL_SUCCESS )													
	{		
		delete row;
		return 0;	
	}
		
	row->Persistent();
	row->PostRetrieveTrigger();
	return row;
}

//--------------------------------------------
//^ The ExecuteCount function returns a count of the number of rows in the database
//  matching the selection criteria
//
template<class T>
int vdbQuery<T>::ExecuteCount()
{
	if ( !_isFrozen )
	{
    	// close off the two streams
		if ( _numSelectionCriteria > 0 )
			_osSelect << ")";
		_osSelect << std::ends;
	    char* sqlSelect = _osSelect.str();

		// combine the streams together
		int size = 21 + strlen( GetTable()->FullyQualifiedTableName() ) + strlen( sqlSelect ) + 1;
		_sql = new char[size];
		if ( _sql == 0 ) throw vdbMemoryException();
		std::ostrstream _osCombined( _sql, size );
		_osCombined << "SELECT count(*) FROM ";
		_osCombined << GetTable()->FullyQualifiedTableName();

		if ( _numSelectionCriteria > 0 )
			_osCombined << sqlSelect;
		
		_osCombined << std::ends;	

		// clean up
		delete[] sqlSelect; sqlSelect = 0;

		_isFrozen = true;
	}

	vdbStatement stmt( _pDatabase );																
	stmt.Execute( _sql );														
	
	DBINT rowCount;
	stmt.Bind( 1, rowCount );
	stmt.Fetch();																

	return rowCount;
}


//=============================================================================
// Protected helpers
//=============================================================================

//--------------------------------------------
// Using the selection criteria and sort criteria
// get the SQL Select statement to be executed.
// Once obtained, the SQL statement is "frozen" and
// further criteria can not be added.
//
template<class T>
const char* vdbQuery<T>::GetSQLSelect()
{
	if ( !_isFrozen )
	{
    	// close off the two streams
		if ( _numSelectionCriteria > 0 )
			_osSelect << ")";
		_osSelect << std::ends;
		_osSort << std::ends;
	    char* sqlSelect = _osSelect.str();
	    char* sqlSort = _osSort.str();

		// combine the streams together
		int size = 14 + strlen( GetTable()->FullyQualifiedTableName() ) + strlen( sqlSelect ) + strlen( sqlSort ) + 1;
		_sql = new char[size];
		if ( _sql == 0 ) throw vdbMemoryException();
		std::ostrstream _osCombined( _sql, size );
		_osCombined << "SELECT * FROM ";
		_osCombined << GetTable()->FullyQualifiedTableName();

		if ( _numSelectionCriteria > 0 )
			_osCombined << sqlSelect;
		
		if ( _numSortCriteria > 0 )
			_osCombined << sqlSort;
	
		_osCombined << std::ends;	

		// clean up
		delete[] sqlSelect; sqlSelect = 0;
		delete[] sqlSort; sqlSort = 0;

		_isFrozen = true;
	}
	
	return _sql;
}


//--------------------------------------------
// construct a new object with OID == 1,
// An OID of 1 means "do not initialize" and is used
// in object constructors only within this query class.
// Create your own specialization of this member function
// if your constructor takes more than just an OID argument.
// 
template<class T>
T* vdbQuery<T>::NewObject()
{
	T* p = new T( 1, _hDB );
	if ( p == 0 ) throw vdbMemoryException();
	return p;
}


//--------------------------------------------
//^ The GetKeyName function
template<class T>
const char* vdbQuery<T>::GetKeyName( T& obj )
{
	// as a stop-gap measure, if no specialization is defined, just make the
	// object ID the key value.
	static char szOID[16];
	vdbUtility::SafeFormat( szOID, sizeof(szOID), "%d", obj.GetOID() );

	return szOID;
}


//=============================================================================
// Private helpers
//=============================================================================

//--------------------------------------------
// Add the column name to the selection criteria
// using the comparison operator passed as the
// second argument.  The connector type defaults to
// "AND".  The Connector argument is ignored for the
// first selection criteria.
//
template<class T>
RETCODE vdbQuery<T>::DoSelectionCriteria( const char* szColumnName, const char* szOperation, vdbNames::connectorType connector )
{
	if ( _isFrozen )
		return DBS_ERROR; 			// can't add additional criteria
	
	if ( CheckColumnName( szColumnName ) == false )
		return DBS_ERROR;
	
	if ( _numSelectionCriteria == 0 )
		_osSelect << "(";
	else // if ( _numSelectionCriteria != 0 )
	{
		if ( connector == vdbNames::AND )
			_osSelect << ") AND (";
		else
			_osSelect << " OR ";
	}

	_osSelect << szColumnName << " " << szOperation;
	_numSelectionCriteria++;
	return DBS_OK;
}


//--------------------------------------------
//^ The WriteWithEscapedQuotes function puts the string to the "select" stream
//  replacing any embedded quotes with two consecutive quotes.
//
template<class T>
void vdbQuery<T>::WriteWithEscapedQuotes( const char* sz )
{
	// if the string contains quote characters, replace them with two consecutive quote characters
	const char* p1 = sz;
	char* p2 = const_cast<char *>( strchr( p1, '\'' ) );
	while ( p2 != NULL )
	{
		p2++;
		_osSelect.write( p1, (p2-p1) );
		_osSelect << "'";
		p1 = p2;
		p2 = const_cast<char *>( strchr( p1, '\'' ) );
	}
	_osSelect << p1;
}


//--------------------------------------------
//^ The CheckColumnName function checks the validity of the column name
//
template<class T>
bool vdbQuery<T>::CheckColumnName( const char* szColumnName )
{
	if ( !GetTable()->IsValidColumn( szColumnName ) )
	{
		#ifdef _DEBUG	
			std::ostrstream os;
			os << "Invalid column '" << szColumnName << "' in vdbQuery" << std::ends;
			char* s = os.str();
			GetDatabase()->Trouble( s, GetTable()->FullyQualifiedTableName() );
			delete[] s;
		#endif  // _DEBUG

		return false;
	}
	return true;
}