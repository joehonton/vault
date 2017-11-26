//=============================================================================
//
// CLASS:           vdbUnique
//
// OVERVIEW:		The vdbUnique class is for managing vdbUnique objects stored
//                  in the database.
//
// FILE NAME:       vdbUnique.cpp
//
// INITIAL DATE:    01/14/96 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#include <vdbHeader.h>
#include "vdbUnique.h"
#include "vdbDatabase.h"
#include "vdbException.h"


//=============================================================================
// Essential class functions
//=============================================================================

//------------------------------------------
//^^ vdbUnique basic constructor
//
//> oid is the object identifier of the vdbUnique object to retrieve
//  from the database.  The oid value should be zero to create a new
//  vdbUnique object.  New objects have a unique object ID assigned by
//  the base class vdbObject constructor.  This oid is available immediately.
//> If a non-zero oid is supplied and the object was not found in the database,
//  the oid is set to zero and the persistent switch is set to false.
//  See the IsPersistent function for details.
//
vdbUnique::vdbUnique( OID oid, HANDLE_DATABASE hDB )
    : vdbObject( oid )
	, _hDB( hDB )
	//{{MemberInitialization
	, nextOID( 0 )
	, lastOID( 0 )
	, cacheIndicator( 0 )
	//}}
{
	vdbDatabase* pDatabase = vdbPool::Instance()->GetDatabase( hDB );
	assert( pDatabase != 0 );

	//{{MemberConstruction
	Initialize( pDatabase->UniqueTable() );
	//}}
}


//------------------------------------------
//^^ vdbUnique alternate constructor
//
//> oid is the object identifier of the vdbUnique object to retrieve
//  from the database.  The oid value should be zero to create a new
//  vdbUnique object.  New objects have a unique object ID assigned by
//  the base class vdbObject constructor.  This oid is available immediately.
//> If a non-zero oid is supplied and the object was not found in the database,
//  the oid is set to zero and the persistent switch is set to false.
//  See the IsPersistent function for details.
//
vdbUnique::vdbUnique( OID oid, vdbDatabase* pDatabase )
    : vdbObject( oid )
	//{{MemberInitialization
	, nextOID( 0 )
	, lastOID( 0 )
	, cacheIndicator( 0 )
	//}}
{
	assert( pDatabase != 0 );

	_hDB = vdbPool::Instance()->GetHandle( pDatabase );
	assert( _hDB != -1 );

	//{{MemberConstruction
	Initialize( pDatabase->UniqueTable() );
	//}}
}


//------------------------------------------
//^^ vdbUnique destructor
//^ The vdbUnique destructor saves the current data member values to the
//  persistent store.  Persistent objects are saved to the database using
//  Update.  Non-persistent objects are saved to the database using Insert.
//  See the IsPersistent function for details.
//^ No attempt to save is made for objects which have not been modified.
//  See the IsModified function for details.
//
vdbUnique::~vdbUnique()
{
	Save();
}


//------------------------------------------
//^^ vdbUnique copy constructor
//^ The copy constructor for the vdbUnique class.  Data member values are
//  copied from the source to the target object including the object ID.
//  The state of persistence and modification remain the same.
//
vdbUnique::vdbUnique( const vdbUnique& rhs )
	: vdbObject( (vdbObject&) rhs )
	, _hDB( rhs._hDB )
{
	//{{MemberCopy
	nextOID = rhs.GetNextOID();
	lastOID = rhs.GetLastOID();
	cacheIndicator = rhs.GetCacheIndicator();
	//}}
}


//------------------------------------------
//^^ vdbUnique assignment operator
//^ The assignment operator for the vdbUnique class.  Data members are
//  copied from the source to the target.  The object ID is not copied to the
//  target;  the target's oid is retained.  The target's persistence state is
//  also retained (not copied from the source.)  The target's modified state
//  is set to true.
//
vdbUnique& vdbUnique::operator=( const vdbUnique& rhs )
{
	if ( this == &rhs )
		return *this;
	
	vdbObject::operator=( rhs );

	_hDB = rhs._hDB;
	//{{MemberCopy
	nextOID = rhs.GetNextOID();
	lastOID = rhs.GetLastOID();
	cacheIndicator = rhs.GetCacheIndicator();
	//}}

	return *this;
}


//=============================================================================
// Required virtual functions
//=============================================================================

//------------------------------------------
//^ The Initialize function must call the vdbObject's base class Initialize
//  function to assign a unique object ID for new objects and to retrieve
//  existing objects from the database.  The responsibility for calling
//  Initialize belongs to vdbUnique because constructors are not
//  polymorphic.
//
RETCODE vdbUnique::Initialize( vdbTable* pTable )
{
	return vdbObject::Initialize( pTable );
}


//------------------------------------------
//^ The Bind function is called prior to a query's execution in order to bind
//  the vdbUnique's data members to the associated database columns.
//  Always call vdbObject's Bind function first to bind the _oid data member
//  to the oid column.
//^ The order and data type of the calls to stmt.Bind must match the order
//  and data type of the columns in the associated SQL table.
//
//> stmt is a reference to the ODBC statement which will be subsequently used
//  to execute an SQL SELECT statement.
//
void vdbUnique::Bind( vdbStatement& stmt )
{
	vdbObject::Bind( stmt );
	//{{MemberBind
	stmt.Bind( 2, nextOID );
	stmt.Bind( 3, lastOID );
	stmt.Bind( 4, cacheIndicator );
	//}}
}


//------------------------------------------
//^ The Parameter function is called prior to an Insert or Update statement
//  in order to bind the vdbUnique's data members to the associated
//  database columns.  Always call vdbObject's Parameter function first to
//  associate the _oid data member to the oid column.
//^ The order and data type of the calls to stmt.Parameter must match the
//  order and data type of the columns in the associated SQL table.
//> stmt is a reference to the ODBC statement which will be subsequently used
//  to execute an SQL INSERT or UPDATE statement.
//
void vdbUnique::Parameter( vdbStatement& stmt, bool bInsert )
{
	vdbObject::Parameter( stmt, bInsert );
	//{{MemberParameter
	stmt.Parameter( 2, nextOID );
	stmt.Parameter( 3, lastOID );
	stmt.Parameter( 4, cacheIndicator );
	//}}
}


//-----------------------------------------------
//^ The ClientIDColumn function is used by vdbLinkSet to obtain the name of
//  the DBMS column which cdbAcceptableRoutes uses to hold the object identifier of
//  its client owner.
//
const char* vdbUnique::ClientIDColumn( vdbObject* pClient )
{
	//{{ClientIDColumnComparisons
	//}}

	// if pClient is not valid, call the base class which will report the problem
	return vdbObject::ClientIDColumn( pClient );
}


//-----------------------------------------------
vdbUniqueTable* vdbUnique::GetTable()
{
	return reinterpret_cast<vdbUniqueTable*>( _pTable );
}


//=============================================================================
// Optional virtual function overrides
//=============================================================================

//{{VirtualFunctionDefinitions}}

//------------------------------------------
//^ The PreDeleteTrigger function is called during the Delete function, prior
//  to all DBMS interaction.
//^ Rules for overriding this virtual:  Do not call the base class function
//  because it has no functionality.
//
//< Return DBS_OK to continue with the delete process.
//< Return any other value to interrupt processing.  The return code will be
//  passed back as the return code from Delete.
//
RETCODE vdbUnique::PreDeleteTrigger()
{
	if ( GetOID() == 2 )
		return DBS_NOT_ALLOWED;
	else
		return DBS_OK;
}


//------------------------------------------
//^ The GetNextObjectID function.  Override the default to
//  obtain the next OID for this new unique object by getting
//  it directly from the master record.
//
void vdbUnique::GetNextObjectID()
{
	// get the master vdbUnique record
	vdbPtr<vdbUnique> pMaster;
	pMaster.Adopt( new vdbUnique( 2, _hDB ) );
	if ( pMaster.IsInvalid() )
		throw vdbException( "vdbUnique OID = 2 not found" );

	OID next = pMaster->GetNextOID();
	SetOID( next );
	pMaster->SetNextOID( next + 1 );		
}

