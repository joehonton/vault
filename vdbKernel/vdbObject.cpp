//=============================================================================
//
// CLASS:           vdbObject
//
// ESSENCE:         database object base class 
//
// FILE NAME:       vdbObject.cpp
//
// INITIAL DATE:    11/27/94 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1994 by Joe Honton
//
//=============================================================================

//^ The behavior of a vdbObject that is created but never modified is to be
//  not persistent.  Thus an object which is created, then which has subjects
//  that are created and saved, but the client is never modified directly,
//  that client is not persistent.
//

#ifdef USE_PRE_HDR
	#include "vdbHeader.h"
#else
	#include <vdbMessageBox.h>
	#include <vdbAssert.h>
	#include <vdbString.h>
	#include "vdbDatabase.h"
	#include "vdbTable.h"
	#include "vdbStatement.h"
	#include "vdbSet.h"
	#include "vdbObject.h"
	#include "vdbException.h"
#endif

#include <strstream>
#include <string.h>
#include "vdbVersion.h"
#include "vdbUnique.h"
#include "vdbSetOfVersions.h"

//warning C4996: 'sprintf': This function or variable may be unsafe. Consider using sprintf_s instead.
//warning C4996: 'sscanf': This function or variable may be unsafe. Consider using sscanf_s instead. 
#pragma warning( disable : 4996 )

//=============================================================================
//  Essential class functions
//=============================================================================

//------------------------------------------
//^^ Basic constructor
//^ if oid == 0 assign an object identifier and return
//^ if oid != 0 attempt to lookup and load members from database columns
//
vdbObject::vdbObject( OID oid )
	  :	_pDatabase( 0 ),
	  	_pTable( 0 ),
		_oid( oid ),
		_bModified( false ),
		_bPersistent( false )
{
	// _pDatabase and _pTable are set in the call to Initialize.
	// Therefore the constructor for all derived classes must call Initialize
	// before using _pDatabase or _pTable
}


//------------------------------------------
//^^ The destructor
vdbObject::~vdbObject()
{
}


//------------------------------------------
//^^ The Copy constructor
//^ This will not assign a new OID to the new object.
//  The state of persistence and modification remain the same.
// 
vdbObject::vdbObject( const vdbObject& rhs )
	  :	_pDatabase( rhs._pDatabase ),
	  	_pTable( rhs._pTable ),
		_oid ( rhs._oid ),
		_bModified( rhs._bModified ),
		_bPersistent( rhs._bPersistent )
{
}


//------------------------------------------
//^^ The assignment operator
//^ Do not change the table or database pointers.
//  Because every object has a valid OID through its
//  constructor, there is no need to get a new OID.
//  Assignment of the derived member data definately
//  constitutes modification so modified is set to true.
//  Whether or not this object is persistent does not change.
//
vdbObject& vdbObject::operator=( const vdbObject& rhs )
{
	if ( this == &rhs )
		return *this;
	
	Modified( true );
	return *this;
}


//=============================================================================
//  Required virtual functions
//=============================================================================

//------------------------------------------
//^ The Initialize function will either get a new object identifier
//  if the user passed in an OID = 0, or retrieve the object
//  from the database and initialize the member data.
//^ If an invalid OID is passed in and the object cannot be retrieved
//  the _oid is set to 0, the _bPersistent flag remains false and a
//	vdbException is thrown.
//^ Always override this function in the derived class and simply call
//  this base class function.
//^ This cannot be called from vdbObject's constructor because
//  of the polymorphic call to Bind.
//< returns the value from PreRetrieveTrigger if not zero.
//< returns the value from PostRetrieveTrigger otherwise.
//
RETCODE vdbObject::Initialize( vdbTable* pTable )
{
	_pTable = pTable;
	assert( _pTable != 0 );

	_pDatabase = pTable->GetDatabase();
	assert( _pDatabase != 0 );

	// special OID for use by vdbQuery only
	// which will perform its own initialization of the object
	if ( _oid == 1 )
		return DBS_OK;

	RETCODE rc;
	if ( rc = PreRetrieveTrigger() )
		return rc;

	// assign an object identifier
	else if ( _oid == 0 )
		GetNextObjectID();

	// attempt to look up valid OID's
	else
	{
		// assumes index 0 is the OID and OID is a single column
		int size = 14 + strlen( FullyQualifiedTableName() ) + 7 + strlen( Index(0).ColumnName(0) ) + 3 + 10 /* GetOID() */ + 1;
		char* sql = new char[size];
		if ( sql == 0 ) throw vdbMemoryException();
		std::ostrstream os( sql, size );
		os << "SELECT * FROM " << FullyQualifiedTableName() << " WHERE " << Index(0).ColumnName(0) << " = " << GetOID() << std::ends;
		vdbStatement stmt( _pDatabase );																
	 	stmt.Execute( sql );
		Bind( stmt );
		if ( stmt.Fetch() == SQL_SUCCESS )
			_bPersistent = true;
		else  // SQL_NO_DATA_FOUND or an error
		{
			_oid = 0;
			delete[] sql; sql = 0;
			throw vdbException( "vdbObject constructor failed during Initialize.  No such OID" );
		}
		delete[] sql; sql = 0;
	}

	return PostRetrieveTrigger();
}


//------------------------------------------
//^ The Bind function is called to bind the member data variables
//  with the DBMS columns during execution of SQL SELECT statements.
//  The base class function must be called first to bind
//  column 1 with the _oid member.  The derived class must supply
//  stmt.Bind function calls for each DBMS column / object data member pair.
//  The first argument to stmt.Bind must be the DBMS column index
//  (starting with 1) associated with the data member.
//
void vdbObject::Bind( vdbStatement& stmt )
{
	stmt.Bind( 1, _oid );
}


//------------------------------------------
//^ The Parameter function is called to bind the member data variables
//  with the DBMS columns during execution of SQL UPDATE and INSERT
//  commands.  The base class function must be called first to bind
//  column 1 with the _oid member.  The derived class must supply
//  stmt.Parameter function calls for each DBMS column / object data member pair.
//  The first argument to stmt.Parameter must be the DBMS column index
//  (starting with 1) associated with the data member.
//> bInsert is true when calling this from the Insert function
//> bInsert is false when calling this from the Update function
//
void vdbObject::Parameter( vdbStatement& stmt, bool bInsert )
{
	stmt.Parameter( 1, _oid );
}


//-----------------------------------------------
//^ The ClientIDColumn function is used by vdbLinkSet to obtain the name of
//  the DBMS column which this object uses to hold the object identifier of
//  its client owner.
//> pClient is the object which is the client of this.
//< The return value is the character string containing the column name which
//  is suitable for use in a call to vdbQuery::Selection.
//< The return value is an empty string if the client passed as the argument
//  is not a valid owner of this.
//
const char* vdbObject::ClientIDColumn( vdbObject* pClient )
{
	vdbString s;
	s.Format( "This client is not a valid client for %s", TableNameCS() );
	GetDatabase()->Trouble( s, pClient->TableNameCS() );
	return "";
}


//=============================================================================
// functions related to "long varchar" and "long varbinary" data types
//=============================================================================

#define TOKEN_ID		12345
#define BUFFER_SIZE		1024

//-------------------------------------
vdbVarCharLong& vdbObject::ReadText( const char* szColumnName, vdbVarCharLong& textValue )
{
	// already retrieved from database, just return
	if ( textValue.IsInMemory() )
		return textValue;

	// assemble the SQL statement
	int size = 7 + strlen(szColumnName) + 6 + strlen( FullyQualifiedTableName() ) + 7 + strlen( Index(0).ColumnName(0) ) + 3 + 10 /* GetOID() */ + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "SELECT " << szColumnName;
	os << " FROM " << FullyQualifiedTableName();
	os << " WHERE " << Index(0).ColumnName(0) << " = " << GetOID() << std::ends;

	vdbStatement stmt( _pDatabase );																
	stmt.Execute( sql );
	delete[] sql; sql = 0;

	RETCODE rc = stmt.Fetch();
	if ( rc != SQL_SUCCESS )
		return textValue;

	// clear the user's text prior to calling GetData
	textValue.SetData( "" );
	SDWORD bytesRetrieved = 0;
	char dataBuffer[BUFFER_SIZE];
	rc = SQL_SUCCESS_WITH_INFO;

	while ( rc == SQL_SUCCESS_WITH_INFO )
	{
		rc = stmt.GetData( 1, SQL_C_CHAR, dataBuffer, BUFFER_SIZE, &bytesRetrieved );
		if ( bytesRetrieved == SQL_NULL_DATA )
			break;
		else
			textValue.Append( dataBuffer, min(BUFFER_SIZE - 1, bytesRetrieved) );  
	}

	textValue.Modified( false );
	textValue.InMemory( true );

	return textValue;
}


//-------------------------------------
vdbBinaryLong& vdbObject::ReadImage( const char* szColumnName, vdbBinaryLong& image )
{
	// already retrieved from database, just return
	if ( image.IsInMemory() )
		return image;

	// assemble the SQL statement
	int size = 7 + strlen(szColumnName) + 6 + strlen( FullyQualifiedTableName() ) + 7 + strlen( Index(0).ColumnName(0) ) + 3 + 10 /* GetOID() */ + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "SELECT " << szColumnName;
	os << " FROM " << FullyQualifiedTableName();
	os << " WHERE " << Index(0).ColumnName(0) << " = " << GetOID() << std::ends;

	vdbStatement stmt( _pDatabase );																
	stmt.Execute( sql );
	delete[] sql; sql = 0;

	RETCODE rc = stmt.Fetch();
	if ( rc != SQL_SUCCESS )
		return image;

	// clear the user's text prior to calling GetData
//	image.SetData( NULL, 0 );
	SDWORD bytesRetrieved = 0;
	char dataBuffer[BUFFER_SIZE];

	// this first call to GetData will return the first portion of the binary data
	// but more importantly will return the total length of the data
	rc = stmt.GetData( 1, SQL_C_BINARY, dataBuffer, BUFFER_SIZE, &bytesRetrieved );
	if ( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO )
	{
		if ( bytesRetrieved == SQL_NULL_DATA || bytesRetrieved <= 0 )
		{
			image.Initialize( 0 );
			image.Modified( false );
			image.InMemory( true );
			return image;
		}
		else // ( bytesRetrieved > 0 )
		{
			image.Initialize( bytesRetrieved );		// set the total length of the binary data
			image.SetDataAt( dataBuffer, min(BUFFER_SIZE, bytesRetrieved), 0 );
		}
	}

	long index = min(BUFFER_SIZE, bytesRetrieved);
	while ( rc == SQL_SUCCESS_WITH_INFO )
	{
		rc = stmt.GetData( 1, SQL_C_BINARY, dataBuffer, BUFFER_SIZE, &bytesRetrieved );
		if ( bytesRetrieved == SQL_NULL_DATA )
			break;
		else
		{
			image.SetDataAt( dataBuffer, min(BUFFER_SIZE, bytesRetrieved), index );  
			index += min(BUFFER_SIZE, bytesRetrieved);
		}
	}

	image.Modified( false );
	image.InMemory( true );

	return image;
}


//-----------------------------------------------
// Perform the delayed write now
void vdbObject::WriteText( const char* szColumnName, vdbVarCharLong& textValue )
{
	// nothing to write
	if ( !textValue.IsInMemory() )
		return;

	// nothing changed
	if ( !textValue.IsModified() )
		return;

    // assemble the SQL statement
	int size = 7 + strlen( FullyQualifiedTableName() ) + 5 + strlen(szColumnName) + 7 + strlen( Index(0).ColumnName(0) ) + 4 + 10 /* GetOID() */ + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "UPDATE " << FullyQualifiedTableName();
	os << " SET " << szColumnName << " = ?";
	os << " WHERE " << Index(0).ColumnName(0) << " = " << GetOID() << std::ends;

	SDWORD cbValue = SQL_DATA_AT_EXEC;
	SDWORD token = TOKEN_ID;
	SDWORD* pToken;

	// bind the parameter values and execute																					
	vdbStatement stmt( _pDatabase );
	RETCODE rc = stmt.BindParameter( 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_LONGVARCHAR, 0, 0, &token, 0, &cbValue );
	rc = stmt.Execute( sql );
	delete[] sql; sql = 0;
	if ( rc != SQL_NEED_DATA )
		return;

	rc = stmt.ParamData( (void**) &pToken );
	if ( rc == SQL_NEED_DATA && *pToken == TOKEN_ID )
	{
		char* pText = textValue.GetDataPointer();

		// if the textValue data pointer is NULL then set the column to NULL
		if ( pText == NULL )
		{
			rc = stmt.PutData( pText, SQL_NULL_DATA );
			stmt.ParamData( (void**) &pToken );		// extra call to ParamData needed only for function sequencing purposes
			textValue.Modified( false );
			return;
		}

		int index = 0;
		SDWORD textLen = textValue.Length();
		while ( index < textLen )
		{
			SDWORD len = min( BUFFER_SIZE, textLen - index );
			rc = stmt.PutData( pText, len );
			if ( rc != SQL_SUCCESS )
				break;
			pText += len;
			index += len;
		}
	}
	stmt.ParamData( (void**) &pToken );		// extra call to ParamData needed only for function sequencing purposes
	
	textValue.Modified( false );
	return;
}


//-----------------------------------------------
void vdbObject::WriteImage( const char* szColumnName, vdbBinaryLong& image )
{
	// nothing to write
	if ( !image.IsInMemory() )
		return;

	// nothing changed
	if ( !image.IsModified() )
		return;

    // assemble the SQL statement
	int size = 7 + strlen( FullyQualifiedTableName() ) + 5 + strlen(szColumnName) + 7 + strlen( Index(0).ColumnName(0) ) + 4 + 10 /* GetOID() */ + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "UPDATE " << FullyQualifiedTableName();
	os << " SET " << szColumnName << " = ?";
	os << " WHERE " << Index(0).ColumnName(0) << " = " << GetOID() << std::ends;

	SDWORD cbValue = SQL_DATA_AT_EXEC;
	SDWORD token = TOKEN_ID;
	SDWORD* pToken;

	// bind the parameter values and execute																					
	vdbStatement stmt( _pDatabase );
	RETCODE rc = stmt.BindParameter( 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_LONGVARBINARY, 0, 0, &token, 0, &cbValue );
	rc = stmt.Execute( sql );
	delete[] sql; sql = 0;
	if ( rc != SQL_NEED_DATA )
		return;

	rc = stmt.ParamData( (void**) &pToken );
	if ( rc == SQL_NEED_DATA && *pToken == TOKEN_ID )
	{
		char* pData = (char*) image.GetDataPointer();

		// if the image data pointer is NULL then set the column to NULL
		if ( pData == NULL )
		{
			rc = stmt.PutData( pData, SQL_NULL_DATA );
			stmt.ParamData( (void**) &pToken );		// extra call to ParamData needed only for function sequencing purposes
			image.Modified( false );
			return;
		}

		int index = 0;
		SDWORD imageLen = image.ActualLength();
		while ( index < imageLen )
		{
			SDWORD len = min( BUFFER_SIZE, imageLen - index );
			rc = stmt.PutData( pData, len );
			if ( rc != SQL_SUCCESS )
				break;
			pData += len;
			index += len;
		}
	}
	stmt.ParamData( (void**) &pToken );		// extra call to ParamData needed only for function sequencing purposes
	
	image.Modified( false );
	return;
}


//=============================================================================
//  SQL cover functions
//=============================================================================

//-----------------------------------------------------------------
//^ The Save function calls either Insert or Update. 
//^ Since this function is normally called by the destructor, it is typically
//  not necessary to call it explicitly.
//^ One possible exception would be global and static pointers to persistent objects.
//  Because the database is probabably disconnected by the time such destructors
//  are called, it is necesary to explictly Save() such objects.
//^ Rules for overridding this virtual function:  The functionality must always
//  be provided.  Either make a call to the base class function from the derived
//  class function or copy the code entirely and make your customizations.
//
//< Returns SQL_SUCCESS immediately if this is not modified.
//< Returns the value from Insert or Update if they were called.
//
RETCODE vdbObject::Save()
{						
	if ( _bModified == false )
		return SQL_SUCCESS;
	
	if ( _bPersistent == true )
		return Update();
	else
		return Insert();
}


//-----------------------------------------------------------------
//^ The Insert function adds a row to the table using the member current values.
//^ Preconditions:  The object must not already be persistent and the oid must not be 0.
//^ Rules for overridding this virtual function:  If your derived class has
//  associations with other vdbObject-derived objects, you will want
//  to override this function to call their Insert functions after calling the
//  base class Insert.  This is so that DBMS's with built-in relational-integrity
//  constraints do give you grief about missing foreign keys.
//
//< Returns the ODBC return code from SQLExecute if there was a problem.
//< Returns the value from PreInsertTrigger if not 0.
//< Returns the value from PostInsertTrigger in all other cases.
//
RETCODE vdbObject::Insert()
{						
	RETCODE rc;
	if ( rc = PreInsertTrigger() )
		return rc;
    
	if ( _bPersistent == true )															
		return _pDatabase->Trouble( "Attempt to Insert() when already persistent", FullyQualifiedTableName() );															
	if ( GetOID() == 0 )															
		return _pDatabase->Trouble( "Attempt to Insert() with OID = 0", FullyQualifiedTableName() );															

    // assemble the SQL statement
	int size = 12 + strlen( FullyQualifiedTableName() ) + 10 + (ColumnCount() * 5) + 2 + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "INSERT INTO " << FullyQualifiedTableName() << " VALUES ( ";
	for ( int i = 0; i < ColumnCount(); i++ )
	{
		if ( i != 0 )
			os << ", ";

		if ( Column(i).IsDataAtExecution() )
		{
			if ( !strncmp( Column(i).RawType(), "longvarchar", 11 ) )
				os << "''";
			else // ( !strncmp( Column(i).RawType(), "longvarbinary", 11 ) )
				os << "0x00";
		}
		else
			os << "?";
	}
    os << " )" << std::ends;
	
	// bind the parameter values and execute																					
	vdbStatement stmt( _pDatabase );
	Parameter( stmt, true );
	rc = stmt.Execute( sql );														
	if ( rc == SQL_SUCCESS )
	{
		_bPersistent = true;
    	_bModified = false;
    }

	delete[] sql; sql = 0;
	if ( rc )
		return rc;

	return PostInsertTrigger();
}
															
	
//-----------------------------------------------------------------
//^ The Update function updates a row of the table based on the members' current values.
//^ Preconditions:  The object must already be persistent and the oid must not be 0.
//^ Rules for overridding this virtual function:  If your derived class has
//  associations with other vdbObject-derived objects, you will probably want
//  to override this function to call their Update functions as well as this
//  base class Update.  Whether to call the base class before or after the
//  other class's Updates is situation dependent.
//
//< Returns the ODBC return code from SQLExecute if there was a problem.
//< Returns the value from PreUpdateTrigger if not 0.
//< Returns the value from PostUpdateTrigger in all other cases.
//
RETCODE vdbObject::Update()
{																						
	RETCODE rc;
	if ( rc = PreUpdateTrigger() )
		return rc;

	if ( _bPersistent == false )															
		return _pDatabase->Trouble( "Attempt to Update() when not persistent", FullyQualifiedTableName() );															
	if ( GetOID() == 0 )															
		return _pDatabase->Trouble( "Attempt to Update() with OID = 0", FullyQualifiedTableName() );															

    // assemble the SQL statement
	// assumes index 0 is the OID and OID is a single column
	int size = 7 + strlen( FullyQualifiedTableName() ) + 5 + ( ColumnCount() * (32 /* max sizeof column name */ + 4) ) + 7 + strlen( Index(0).ColumnName(0) ) + 4 + 10 /* GetOID() */ + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "UPDATE " << FullyQualifiedTableName() << " SET ";
	for ( int i = 0; i < ColumnCount(); i++ )
	{
		if ( !Column(i).IsDataAtExecution() )
		{
			if ( i != 0 )
				os << ", ";
			os << Column(i).Name() << " = ?";
		}
	}
	os << " WHERE " << Index(0).ColumnName(0) << " = " << GetOID() << std::ends;

	// bind the parameter values and execute																					
	vdbStatement stmt( _pDatabase );
	Parameter( stmt, false );
	rc = stmt.Execute( sql );
	if ( rc == SQL_SUCCESS )
    	_bModified = false;

	delete[] sql; sql = 0;
	if ( rc )
		return rc;

	return PostUpdateTrigger();
}																						


//-----------------------------------------------------------------
//^ The Delete function removes a row from a table based on the key value.
//^ Preconditions:  The oid must not be 0.
//^ Rules for overridding this virtual function:  If performing cascading
//  deletes of related records, you will probably want to do those deletes
//  prior to calling this base class functionality.  This is so that DBMS's with 
//  built-in relational-integrity constraints do give you grief about orphans.
//^ State changes:  _modified becomes false.  _persistent becomes false.  _oid becomes 0.  
//
//< If the object was not persistent prior to the call, returns DBS_NOT_FOUND.
//< Returns the ODBC return code from SQLExecute if there was a problem.
//< Returns the value from PreDeleteTrigger if not 0.
//< Returns the value from PostDeleteTrigger in all other cases.
//
RETCODE vdbObject::Delete()
{																						
	RETCODE rc;
	if ( rc = PreDeleteTrigger() )
		return rc;

	if ( _bPersistent == false )
	{
		// do this so that Save() does not attempt to Insert() after all our efforts
		// we may have gotten here if we attempt to Delete() soon after birth (before Insert)
		_bModified = false;
		_oid = 0;
		return DBS_NOT_FOUND;															
	}
	if ( GetOID() == 0 )															
		return _pDatabase->Trouble( "Attempt to Delete() with OID = 0", FullyQualifiedTableName() );															

    // assemble the SQL statement
	// assumes index 0 is the OID and OID is a single column
	int size = 12 + strlen( FullyQualifiedTableName() ) + 7 + strlen( Index(0).ColumnName(0) ) + 3 + 10 /* GetOID() */ + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "DELETE FROM " << FullyQualifiedTableName() << " WHERE " << Index(0).ColumnName(0) << " = " << GetOID() << std::ends;
    
	vdbStatement stmt( _pDatabase );																
	rc = stmt.Execute( sql );														
	if ( rc == SQL_SUCCESS )
	{
		_bPersistent = false;
    	_bModified = false;
		_oid = 0;
    }

	delete[] sql; sql = 0;
	if ( rc )
		return rc;

	return PostDeleteTrigger();
}																						


//-----------------------------------------------------------------
//^ The Extent function finds the number of records in this table.
//
//< The return value is the number of records in this table (the extent.)
//
int vdbObject::Extent()
{						
	int size = 21 + strlen( FullyQualifiedTableName() ) + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "SELECT count(*) FROM " << FullyQualifiedTableName() << std::ends;
	
	vdbStatement stmt( _pDatabase );																
	stmt.Execute( sql );														
	
	DBINT rowCount;
	stmt.Bind( 1, rowCount );
	stmt.Fetch();																

	delete[] sql; sql = 0;
	return rowCount;
}


//-----------------------------------------------------------------
//^ The CountChildren function finds the number of child records
//  associated with this parent record.
//  This will work for child records which have a foreign key whose value
//  is the same as this parent record's OID.
//
//> szChildTableName is the name of the table whose record's are to be counted.
//> szChildForeignKey is the name of the child table's "parentID" columns.
//
//< The return value is the number of records in the child table whose
//  foreign key matches this object's OID.
//
int vdbObject::CountChildren( const char* szChildTableName, const char* szChildForeignKey )
{						
	int size = 21 + strlen( _pDatabase->Owner() ) + 1 + strlen( szChildTableName ) + 7 + strlen( szChildForeignKey ) + 3 + 10 /* GetOID() */ + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "SELECT count(*) FROM " << _pDatabase->Owner() << "." << szChildTableName << " WHERE " << szChildForeignKey << " = " << GetOID() << std::ends;
	
	vdbStatement stmt( _pDatabase );																
	stmt.Execute( sql );														
	
	DBINT rowCount;
	stmt.Bind( 1, rowCount );
	stmt.Fetch();																

	delete[] sql; sql = 0;
	return rowCount;
}


//-----------------------------------------------------------------
//^ The CountChildren function finds the number of child records associated with this
//  parent record.
//
//> szChildTableName is the name of the table whose record's are to be counted.
//> szChildForeignKey1 is the name of one of the child table's columns.
//> oid1 is the value to match for the first column
//> szChildForeignKey2 is the name of another of the child table's columns.
//> oid2 is the value to match for the second column
//
//< The return value is the number of records in the child table whose two
//  foreign keys match the two OIDs.
//
int vdbObject::CountChildren( const char* szChildTableName, const char* szChildForeignKey1, OID oid1, const char* szChildForeignKey2, OID oid2 )
{						
	int size = 21 + strlen( _pDatabase->Owner() ) + 1 + strlen( szChildTableName ) + 7 + strlen( szChildForeignKey1 ) + 3 + 10 /* GetOID() */ + 5 + strlen( szChildForeignKey2 ) + 3 + 10 /* GetOID() */ + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "SELECT count(*) FROM " << _pDatabase->Owner() << "." << szChildTableName << " WHERE " << szChildForeignKey1 << " = " << oid1;
	os << " AND " << szChildForeignKey2 << " = " << oid2 << std::ends;

	vdbStatement stmt( _pDatabase );																
	stmt.Execute( sql );														
	
	DBINT rowCount;
	stmt.Bind( 1, rowCount );
	stmt.Fetch();																

	delete[] sql; sql = 0;
	return rowCount;
}


//=============================================================================
// virtual member functions
//=============================================================================

//------------------------------------------
//^ The GetNextObjectID function assigns
//  unique object identifiers.  This implementation
//  uses the cached value in the vdbDatabase instance.
//^ Rules for overridding this virtual function:  Simply supply
//  this object's _oid data member with a unique value.
//
void vdbObject::GetNextObjectID()
{
	_oid = _pDatabase->GetNextOID();
}


//------------------------------------------
//^ The PreRetrieveTrigger function is called during the Initialize
//  function, prior to any assignment of OIDs or retrieval of
//  database columns.
//^ Rules for overriding this virtual:  Do not call the base class function
//  because it has no functionality.
//
//< Return DBS_OK to continue with the retrieval process.
//< Return any other value to interrupt processing.  The return code
//  will be passed back as the return code from Initialize.
//
RETCODE vdbObject::PreRetrieveTrigger()
{
	return DBS_OK;
}


//------------------------------------------
//^ The PostRetrieveTrigger is called at the completion
//  of the initialization process as well as by the vdbQuery
//  Execute functions.  At this point all data members have
//  been initialized with default values or initialized with
//  data from the database columns.
//^ Rules for overriding this virtual:  Do not call the base class function
//  because it has no functionality.
//< The return code of this trigger is always passed back as the
//  return code of Initialize.
//
RETCODE vdbObject::PostRetrieveTrigger()
{
	return DBS_OK;
}


//------------------------------------------
//^ The PreInsertTrigger function is called during the Insert
//  function, prior to all DBMS interaction.
//^ Rules for overriding this virtual:  Do not call the base class function
//  because it has no functionality.
//
//< Return DBS_OK to continue with the insert process.
//< Return any other value to interrupt processing.  The return code
//  will be passed back as the return code from Insert.
//
RETCODE vdbObject::PreInsertTrigger()
{
	return DBS_OK;
}


//------------------------------------------
//^ The PostInsertTrigger is called at the completion
//  of the Insert function after a successful DBMS insertion.
//  If the insert failed, this trigger is never called.
//^ Rules for overriding this virtual:  Do not call the base class function
//  because it has no functionality.
//< The return code of this trigger is always passed back as the
//  return code of Insert.
//
RETCODE vdbObject::PostInsertTrigger()
{
	return DBS_OK;
}


//------------------------------------------
//^ The PreUpdateTrigger function is called during the Update
//  function, prior to all DBMS interaction.
//^ Rules for overriding this virtual:  Do not call the base class function
//  because it has no functionality.
//
//< Return DBS_OK to continue with the update process.
//< Return any other value to interrupt processing.  The return code
//  will be passed back as the return code from Update.
//
RETCODE vdbObject::PreUpdateTrigger()
{
	return DBS_OK;
}


//------------------------------------------
//^ The PostUpdateTrigger is called at the completion
//  of the Update function after a successful DBMS insertion.
//  If the update failed, this trigger is never called.
//^ Rules for overriding this virtual:  Do not call the base class function
//  because it has no functionality.
//< The return code of this trigger is always passed back as the
//  return code of Update.
//
RETCODE vdbObject::PostUpdateTrigger()
{
	return DBS_OK;
}


//------------------------------------------
//^ The PreDeleteTrigger function is called during the Delete
//  function, prior to all DBMS interaction.
//^ Rules for overriding this virtual:  Do not call the base class function
//  because it has no functionality.
//
//< Return DBS_OK to continue with the delete process.
//< Return any other value to interrupt processing.  The return code
//  will be passed back as the return code from Delete.
//
RETCODE vdbObject::PreDeleteTrigger()
{
	return DBS_OK;
}


//------------------------------------------
//^ The PostDeleteTrigger is called at the completion
//  of the Delete function after a successful DBMS insertion.
//  If the deletion failed, this trigger is never called.
//^ Rules for overriding this virtual:  Do not call the base class function
//  because it has no functionality.
//< The return code of this trigger is always passed back as the
//  return code of Delete.
//
RETCODE vdbObject::PostDeleteTrigger()
{
	return DBS_OK;
}


//-----------------------------------------------
//^ The ClientID function is used by vdbLinkSet to obtain
//  the address of the member variable which cdbEntry uses to
//  hold the object identifier of its client owner.
//> pClient is the object which is the client of cdbEntry.
//< The return value is the address of the cdbEntry attribute
//  which holds the client's OID.
//< The return value is NULL if the client passed
//  as the argument is not a valid owner of this class.
//
OID* vdbObject::ClientID( vdbObject* pClient )
{
	_pDatabase->Trouble( "No virtual function override has been provided for ClientID()", pClient->TableNameCS() );
	return NULL;
}


//=============================================================================
// XML read and write
//=============================================================================

//-----------------------------------------------
bool vdbObject::ReadXML( xmlItem* pTable )
{
	return false;
}


//-------------------------------------
void vdbObject::SetGenericColumn( const char* szColumnName, const char* szValue )
{
}


//-----------------------------------------------
xmlItem* vdbObject::WriteXML()
{
	return 0;
}


//-----------------------------------------------
xmlItem* vdbObject::WriteXML( vdbSet<vdbString>& columnNames )
{
	return 0;
}


//-----------------------------------------------
void vdbObject::GetGenericColumn( const char* szColumnName, vdbString& sValue )
{
}


//-----------------------------------------------
xmlItem* vdbObject::WriteXMLSchema()
{
	return 0;
}


//=============================================================================
// SetGeneric functions
//=============================================================================

//-----------------------------------------------
void vdbObject::SetGeneric( char* dest, int sizeofDest, const char* szSource )
{
	Modified();
	vdbUtility::SafeCopy( dest, szSource, sizeofDest );
}


//-----------------------------------------------
void vdbObject::SetGeneric( OID& dest, const char* szSource )
{
	Modified();
	dest = atoi( szSource );
}


//-----------------------------------------------
void vdbObject::SetGeneric( DBBIT& dest, const char* szSource )
{
	Modified();
	dest = atoi( szSource );
}


//-----------------------------------------------
void vdbObject::SetGeneric( DBBYTE& dest, const char* szSource )
{
	Modified();
	dest = atoi( szSource );
}


//-----------------------------------------------
void vdbObject::SetGeneric( DBSMALL& dest, const char* szSource )
{
	Modified();
	dest = atoi( szSource );
}


//-----------------------------------------------
void vdbObject::SetGeneric( DBINT& dest, const char* szSource )
{
	Modified();
	dest = atoi( szSource );
}


//-----------------------------------------------
void vdbObject::SetGeneric( DBFLOAT& dest, const char* szSource )
{
	Modified();
	dest = static_cast<DBFLOAT>( atof( szSource ) );
}


//-----------------------------------------------
void vdbObject::SetGeneric( DBDOUBLE& dest, const char* szSource )
{
	Modified();
	dest = atof( szSource );
}


//-----------------------------------------------
//> szSource should be in the form: "%Y-%m-%dT%H:%M:%SZ"
void vdbObject::SetGeneric( vdbTimeStampDB& dest, const char* szSource )
{
	Modified();
	dest.SetFromDatabaseString( szSource );
}


//-----------------------------------------------
//> szSource should be a string of Hex characters where each two characters
//  is translated into a single byte in the destination.  The entire string
//  is null terminated.
//> If the source does not have enough hex characters, pad the trailing bytes
//  of the fixed length string with zeroes.  If the source has too many hex
//  characters, truncate it to the length of the destination.
//
void vdbObject::SetGeneric( vdbBinaryFixed& dest, const char* szSource )
{
	Modified();
	
	int binaryLength = strlen( szSource ) / 2;
	int destLength = dest.MaxLength();
	int minLength = min( binaryLength, destLength );
	
	// convert from hex
	char sz[3];
	int c;
	int offset;
	int i;
	for ( i = 0; i < minLength; i++ )
	{
		offset = i*2;
		sz[0] = szSource[offset];
		sz[1] = szSource[offset+1];
		sscanf( sz, "%x", &c );
		dest[i] = c;
	}
	
	// pad any remainder with zeros
	for ( ; i < destLength; i++ )
		dest[i] = 0x0;
}


//-----------------------------------------------
//> szSource should be a string of Hex characters where each two characters
//  is translated into a single byte in the destination.  The entire string
//  is null terminated.
//> The length of the destination is reset to be the length of the source.
void vdbObject::SetGeneric( vdbBinaryVar& dest, const char* szSource )
{
	Modified();
	
	int binaryLength = strlen( szSource ) / 2;
	dest.MaxLength( binaryLength );

	// convert from hex
	char sz[3];
	int c;
	int offset;
	for ( int i = 0; i < binaryLength; i++ )
	{
		offset = i*2;
		sz[0] = szSource[offset];
		sz[1] = szSource[offset+1];
		sscanf( sz, "%x", &c );
		dest[i] = c;
	}
}


//-----------------------------------------------
//> szSource should be a string of Hex characters where each two characters
//  is translated into a single byte in the destination.  The entire string
//  is null terminated.
//> The length of the destination is reset to be the length of the source.
void vdbObject::SetGeneric( vdbBinaryLong& dest, const char* szSource )
{
	Modified();
	
	int binaryLength = strlen( szSource ) / 2;
	dest.Initialize( binaryLength );

	// convert from hex
	char sz[3];
	int c;
	int offset;
	for ( int i = 0; i < binaryLength; i++ )
	{
		offset = i*2;
		sz[0] = szSource[offset];
		sz[1] = szSource[offset+1];
		sscanf( sz, "%x", &c );
		dest[i] = c;
	}
}


//-----------------------------------------------
void vdbObject::SetGeneric( vdbVarCharLong& dest, const char* szSource )
{
	Modified();
	
	dest.SetData( szSource );
}


//=============================================================================
// GetGeneric functions
//=============================================================================

//-----------------------------------------------
void vdbObject::GetGeneric( const char* source, vdbString& sDest )
{
	sDest = source;
}


//-----------------------------------------------
void vdbObject::GetGeneric( OID& source, vdbString& sDest )
{
	sDest.Format( "%d", source );
}


//-----------------------------------------------
void vdbObject::GetGeneric( DBBIT& source, vdbString& sDest )
{
	sDest.Format( "%d", source );
}


//-----------------------------------------------
void vdbObject::GetGeneric( DBBYTE& source, vdbString& sDest )
{
	sDest.Format( "%d", source );
}


//-----------------------------------------------
void vdbObject::GetGeneric( DBSMALL& source, vdbString& sDest )
{
	sDest.Format( "%d", source );
}


//-----------------------------------------------
void vdbObject::GetGeneric( DBINT& source, vdbString& sDest )
{
	sDest.Format( "%d", source );
}


//-----------------------------------------------
void vdbObject::GetGeneric( DBFLOAT& source, vdbString& sDest )
{
	sDest.Format( "%f", source );
}


//-----------------------------------------------
void vdbObject::GetGeneric( DBDOUBLE& source, vdbString& sDest )
{
	sDest.Format( "%f", source );
}


//-----------------------------------------------
void vdbObject::GetGeneric( vdbTimeStampDB& source, vdbString& sDest )
{
	char sz[22];
	source.Format( sz, sizeof(sz), "%Y-%m-%dT%H:%M:%SZ" );	// <-- XML standard form
	sDest = sz;
}


//-----------------------------------------------
void vdbObject::GetGeneric( vdbBinaryFixed& source, vdbString& sDest )
{
	int len = source.ActualLength();
	int bytes = (len + 1) * 2;
	
	char* psz = new char[bytes];
	if ( psz == 0 )
		throw vdbMemoryException();

	// convert to hex
	for ( int i = 0; i < len; i++ )
		sprintf( &psz[i*2], "%x", source[i] );

	psz[len*2] = 0x0;
	
	sDest = psz;
	delete[] psz; psz = 0;
}


//-----------------------------------------------
void vdbObject::GetGeneric( vdbBinaryVar& source, vdbString& sDest )
{
	int len = source.ActualLength();
	int bytes = (len + 1) * 2;
	
	char* psz = new char[bytes];
	if ( psz == 0 )
		throw vdbMemoryException();

	// convert to hex
	for ( int i = 0; i < len; i++ )
		sprintf( &psz[i*2], "%x", source[i] );

	psz[len*2] = 0x0;
	
	sDest = psz;
	delete[] psz; psz = 0;
}


//-----------------------------------------------
void vdbObject::GetGeneric( vdbBinaryLong& source, vdbString& sDest )
{
	int len = source.ActualLength();
	int bytes = (len + 1) * 2;
	
	char* psz = new char[bytes];
	if ( psz == 0 )
		throw vdbMemoryException();

	// convert to hex
	for ( int i = 0; i < len; i++ )
		sprintf( &psz[i*2], "%x", source[i] );

	psz[len*2] = 0x0;
	
	sDest = psz;
	delete[] psz; psz = 0;
}


//-----------------------------------------------
void vdbObject::GetGeneric( vdbVarCharLong& source, vdbString& sDest )
{
	sDest = (const char*) source;
}
