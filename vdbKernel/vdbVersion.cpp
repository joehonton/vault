//=============================================================================
//
// CLASS:           vdbVersion
//
// OVERVIEW:		The vdbVersion class is for managing vdbVersion
//                  objects stored in the database.
//
// FILE NAME:       vdbVersion.cpp
//
// INITIAL DATE:    01/14/96 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

#include <vdbHeader.h>
#include "vdbVersion.h"
#include "vdbDatabase.h"


//=============================================================================
// Essential class functions
//=============================================================================

//------------------------------------------
//^^ vdbVersion basic constructor
//
//> oid is the object identifier of the vdbVersion object to retrieve
//  from the database.  The oid value should be zero to create a new
//  vdbVersion object.  New objects have a unique object ID assigned by
//  the base class vdbObject constructor.  This oid is available immediately.
//> If a non-zero oid is supplied and the object was not found in the database,
//  the oid is set to zero and the persistent switch is set to false.
//  See the IsPersistent function for details.
//
vdbVersion::vdbVersion( OID oid, HANDLE_DATABASE hDB )
    : vdbObject( oid )
	//{{MemberInitialization
	, tableID( 0 )
	, version( 0 )
	//}}
{
	vdbDatabase* pDatabase = vdbPool::Instance()->GetDatabase( hDB );
	assert( pDatabase != 0 );

	//{{MemberConstruction
	tableName[0] = 0;
	Initialize( pDatabase->VersionTable() );
	//}}
}


//------------------------------------------
//^^ vdbVersion destructor
//^ The vdbVersion destructor saves the current data member values to the
//  persistent store.  Persistent objects are saved to the database using
//  Update.  Non-persistent objects are saved to the database using Insert.
//  See the IsPersistent function for details.
//^ No attempt to save is made for objects which have not been modified.
//  See the IsModified function for details.
//
vdbVersion::~vdbVersion()
{
	Save();
}


//------------------------------------------
//^^ vdbVersion copy constructor
//^ The copy constructor for the vdbVersion class.  Data member values are
//  copied from the source to the target object including the object ID.
//  The state of persistence and modification remain the same.
//
vdbVersion::vdbVersion( const vdbVersion& rhs )
	: vdbObject( (vdbObject&) rhs )
{
	//{{MemberCopy
	tableID = rhs.GetTableID();
	vdbUtility::SafeCopy( tableName, rhs.GetTableName(), sizeof(tableName) );
	version = rhs.GetVersion();
	//}}
}


//------------------------------------------
//^^ vdbVersion assignment operator
//^ The assignment operator for the vdbVersion class.  Data members are
//  copied from the source to the target.  The object ID is not copied to the
//  target;  the target's oid is retained.  The target's persistence state is
//  also retained (not copied from the source.)  The target's modified state
//  is set to true.
//
vdbVersion& vdbVersion::operator=( const vdbVersion& rhs )
{
	if ( this == &rhs )
		return *this;
	
	vdbObject::operator=( rhs );

	//{{MemberCopy
	tableID = rhs.GetTableID();
	vdbUtility::SafeCopy( tableName, rhs.GetTableName(), sizeof(tableName) );
	version = rhs.GetVersion();
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
//  Initialize belongs to vdbVersion because constructors are not
//  polymorphic.
//
RETCODE vdbVersion::Initialize( vdbTable* pTable )
{
	return vdbObject::Initialize( pTable );
}


//------------------------------------------
//^ The Bind function is called prior to a query's execution in order to bind
//  the vdbVersion's data members to the associated database columns.
//  Always call vdbObject's Bind function first to bind the _oid data member
//  to the oid column.
//^ The order and data type of the calls to stmt.Bind must match the order
//  and data type of the columns in the associated SQL table.
//
//> stmt is a reference to the ODBC statement which will be subsequently used
//  to execute an SQL SELECT statement.
//
void vdbVersion::Bind( vdbStatement& stmt )
{
	vdbObject::Bind( stmt );
	//{{MemberBind
	stmt.Bind( 2, tableID );
	stmt.Bind( 3, tableName, sizeof(tableName) );
	stmt.Bind( 4, version );
	//}}
}


//------------------------------------------
//^ The Parameter function is called prior to an Insert or Update statement
//  in order to bind the vdbVersion's data members to the associated
//  database columns.  Always call vdbObject's Parameter function first to
//  associate the _oid data member to the oid column.
//^ The order and data type of the calls to stmt.Parameter must match the
//  order and data type of the columns in the associated SQL table.
//> stmt is a reference to the ODBC statement which will be subsequently used
//  to execute an SQL INSERT or UPDATE statement.
//
void vdbVersion::Parameter( vdbStatement& stmt, bool bInsert )
{
	vdbObject::Parameter( stmt, bInsert );
	//{{MemberParameter
	stmt.Parameter( 2, tableID );
	stmt.ParameterChar( 3, tableName );
	stmt.Parameter( 4, version );
	//}}
}


//-----------------------------------------------
//^ The ClientIDColumn function is used by vdbLinkSet to obtain the name of
//  the DBMS column which cdbAcceptableRoutes uses to hold the object identifier of
//  its client owner.
//
const char* vdbVersion::ClientIDColumn( vdbObject* pClient )
{
	//{{ClientIDColumnComparisons
	//}}

	// if pClient is not valid, call the base class which will report the problem
	return vdbObject::ClientIDColumn( pClient );
}


//-----------------------------------------------
vdbVersionTable* vdbVersion::GetTable()
{
	return reinterpret_cast<vdbVersionTable*>( _pTable );
}


//=============================================================================
// Optional virtual function overrides
//=============================================================================

//{{VirtualFunctionDefinitions}}
//------------------------------------------
//^ The PostRetrieveTrigger is called at the completion of the initialization
//  process as well as by the vdbQuery Execute functions.  At this point all
//  data members have been initialized with default values or initialized with
//  data from the database columns.
//^ Rules for overriding this virtual:  Do not call the base class function
//  because it has no functionality.
//< The return code of this trigger is always passed back as the return code
//  of Initialize.
//
RETCODE vdbVersion::PostRetrieveTrigger()
{
	//{{StripTrailingSpaces
	vdbUtility::StripTrailingSpaces( tableName );
	//}}
	return DBS_OK;
}

