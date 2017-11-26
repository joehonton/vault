/* ------------------------------------------------------------------------- *
 *                                                                           *
 * FILE NAME:       vdbDynamicQuery.h                                        *
 *                                                                           *
 * CONTENTS:        vdbDynamicQuery c                                        *
 *                                                                           *
 * DEVELOPER:       Joe Honton                                               *
 *                                                                           *
 * INITIAL DATE:    2/14/95                                                  *
 *                                                                           *
 * COPYRIGHT:       Joe Honton (c) 1995  All rights reserved.                *
 *                                                                           *
 * ------------------------------------------------------------------------- */
#ifndef __vdbDynamicQuery_h
#define __vdbDynamicQuery_h


#include "vdbQuery.h"
#include "vdbStatement.h"

//=============================================================================
template <class T>
class vdbDynamicQuery : public vdbQuery<T>
{
public:
	// essential class functions
	vdbDynamicQuery( int setSize = 32 );						// basic constructor
	virtual ~vdbDynamicQuery();									// destructor

private:
	vdbDynamicQuery( const vdbDynamicQuery& rhs );				// copy constructor disabled
	void operator=( const vdbDynamicQuery& rhs );				// assignment operator disabled

public:
	// dynamic access functions
	T* First();
	T* Last();
	T* Next();
	T* Prev();
	inline T* Current();

protected:
	// query execution	
	virtual RETCODE Execute();

protected:
	// member data
	vdbStatement _stmt;				// the statement used by Execute() and the dynamic acces functions.
	bool _isReady;					// Is this ready for dynamic access?  Has Execute() been called?

private:
	// member data
	vdbSet<T>* _pSet;				// the set of objects returned through the dynamic access functions.
	int _currentPosition;			// The zero-based index into the Set of the most recently accessed object.
	bool _eos;						// Has the end-of-set been reached by the Fetch() routine?
};


//==========================================================================================
// inlines

template<class T>
inline T* vdbDynamicQuery<T>::Current()
{
	if ( _currentPosition == -1 )
		return (T*) 0;
	else
		return &(*_pSet)[_currentPosition];
}


#endif	// __vdbDynamicQuery_h
