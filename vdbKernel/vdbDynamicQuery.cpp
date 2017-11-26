/* ------------------------------------------------------------------------- *
 *                                                                           *
 * FILE NAME:       vdbDynamicQuery.cpp                                      *
 *                                                                           *
 * CONTENTS:        Dynamic Query template.  Template argument must be a     *
 *                  vdbObject derived object.  Include this file in the      *
 *                  .cpp file for template instantiations.                   *
 *                                                                           *
 * DEVELOPER:       Joe Honton                                               *
 *                                                                           *
 * INITIAL DATE:    2/24/95                                                  *
 *                                                                           *
 * COPYRIGHT:       Joe Honton (c) 1995                                      *
 *                                                                           *
 * ------------------------------------------------------------------------- */

#ifdef USE_PRE_HDR
	#include "vdbHeader.h"
#else
	#include <vdbAssert.h>
	#include "vdbStatement.h"
	#include "vdbDatabase.h"
	#include "vdbException.h"
#endif

#include "vdbDynamicQuery.h"


//=============================================================================
//  Essential class functions
//=============================================================================

//--------------------------------------------
// basic constructor
template<class T>
vdbDynamicQuery<T>::vdbDynamicQuery( int setSize )
	: vdbQuery<T>(),
	  _isReady( false ),
	  _currentPosition( -1 ),
	  _eos( false ),
  	  _stmt( GetDatabase() )
{
	_pSet = new vdbSet<T>( setSize );
	if ( _pSet == 0 ) throw vdbMemoryException();
}
	

//--------------------------------------------
// destructor
template<class T>
vdbDynamicQuery<T>::~vdbDynamicQuery()
{
	delete _pSet;    
}


//--------------------------------------------
// copy constructor
// Disable because there is no copy constructor available for vdbQuery objects.
template<class T>
vdbDynamicQuery<T>::vdbDynamicQuery( const vdbDynamicQuery& rhs )
	: vdbQuery<T>(),
	  _stmt( GetDatabase() )
{
	assert( "copying vdbDynamicQuery objects is not allowed" );
}


//--------------------------------------------
// assignment operator
// Disable this too.
template<class T>
void vdbDynamicQuery<T>::operator=( const vdbDynamicQuery& rhs )
{
	assert( "assignment of vdbDynamicQuery objects is not allowed" );
}


//=============================================================================
// Dynamic access functions
//=============================================================================

//--------------------------------------------
// Get the first database object matching the
// selection criteria.  May return a NULL pointer.
//
template<class T>
T* vdbDynamicQuery<T>::First()
{
	if ( !_isReady )
		Execute();

	// return the first member of set if it has already been retrieved
	if ( _pSet->Count() > 0 )
	{
		_currentPosition = 0;
		return &(*_pSet)[0];
	}
	
	// otherwise retrieve it
	T* row = NewObject();
	row->Bind( _stmt );
	RETCODE rc = _stmt.Fetch();
	if ( rc == SQL_NO_DATA_FOUND )
	{
		delete row;
		row = 0;
		_eos = true;
		_currentPosition = -1;
		return (T*) 0;
	}
	else if ( rc != SQL_SUCCESS )
	{
		delete row;
		row = 0;
		_currentPosition = -1;
		return (T*) 0;
	}

	// add object to the set
	row->Persistent();
	_pSet->Add( row );
	_currentPosition = 0;
	return row;
}


//--------------------------------------------
// Get the last database object matching the
// selection criteria.  May return a NULL pointer.
//
template<class T>
T* vdbDynamicQuery<T>::Last()
{
	if ( !_isReady )
		Execute();

	// retrieve all remaining objects
	if ( _eos == false )
	{
		RETCODE rc = SQL_SUCCESS;
		while ( rc == SQL_SUCCESS )
		{
			T* row = NewObject();
			row->Bind( _stmt );
			rc = _stmt.Fetch();
			if ( rc == SQL_NO_DATA_FOUND )
			{
				delete row;
				row = 0;
				_eos = true;
				break;
			}
			else if ( rc != SQL_SUCCESS )
			{
				delete row;
				row = 0;
				_currentPosition = -1;
				return (T*) 0;
			}
		
			// add object to the set
			row->Persistent();
			_pSet->Add( row );
		}
	}

	// now return the last one
	if ( _pSet->Count() > 0 )
	{
		_currentPosition = _pSet->Count() - 1;
		return &(*_pSet)[_currentPosition];
	}
	else
	{
		_currentPosition = -1;
		return (T*) 0;
	}
}


//--------------------------------------------
// Get the next database object matching the
// selection criteria.  May return a NULL pointer.
//
template<class T>
T* vdbDynamicQuery<T>::Next()
{
	if ( !_isReady )
		Execute();

	// return the next member of set if it has already been retrieved
	if ( (_pSet->Count() - 1) > _currentPosition )
	{
		_currentPosition++;
		return &(*_pSet)[_currentPosition];
	}

	// if there are no more, return NULL 
	if ( _eos == true )
	{
		_currentPosition = -1;
		return (T*) 0;
	}

	// otherwise retrieve it
	T* row = NewObject();
	row->Bind( _stmt );
	RETCODE rc = _stmt.Fetch();
	if ( rc == SQL_NO_DATA_FOUND )
	{
		delete row;
		row = 0;
		_eos = true;
		_currentPosition = -1;
		return (T*) 0;
	}
	else if ( rc != SQL_SUCCESS )
	{
		delete row;
		row = 0;
		_currentPosition = -1;
		return (T*) 0;
	}

	// add object to the set
	row->Persistent();
	_pSet->Add( row );
	_currentPosition++;
	return row;
}


//--------------------------------------------
// Get the previous database object matching the
// selection criteria.  Returns a NULL pointer when
// attempting to access beyond the beginning of the set.
//
template<class T>
T* vdbDynamicQuery<T>::Prev()
{
	// return the previous member of set if it has already been retrieved
	if ( _currentPosition > 0 )
	{
		_currentPosition--;
		return &(*_pSet)[_currentPosition];
	}

	// if we are already at the beginning, return NULL 
	else
	{
		_currentPosition = -1;
		return (T*) 0;
	}
}



//=============================================================================
// Query execution
//=============================================================================

//--------------------------------------------
// Execute the SQL Select and prepare for 
// dynamic access.
//
template<class T>
RETCODE vdbDynamicQuery<T>::Execute()
{
	if ( !_isReady )
	{
		const char* sql = GetSQLSelect();
		RETCODE rc = _stmt.Execute( sql );														
		_isReady = true;
	}
	return DBS_OK;
}



