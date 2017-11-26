//=============================================================================
//
// CLASS:           SimpleDatabase
//
// OVERVIEW:		A simple database derivation for use when table and
//                  column access is all done through ODBC
//
// FILE NAME:       SimpleDatabase.h
//
// INITIAL DATE:    12/26/99
//
// COPYRIGHT:		Copyright © 1999 by Joe Honton
//
//=============================================================================

#ifndef __SimpleDatabase_h
#define __SimpleDatabase_h

#include "vdbDatabase.h"

class SimpleDatabase : public vdbDatabase 
{
public:
	SimpleDatabase();
	virtual ~SimpleDatabase();

private:
	SimpleDatabase( const SimpleDatabase& rhs );
	SimpleDatabase& operator=( const SimpleDatabase& rhs );
};									


#endif // __SimpleDatabase_h
