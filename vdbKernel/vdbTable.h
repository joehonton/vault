//=============================================================================
//
// CLASS:           vdbTable
//
// ESSENCE:         database table base class 
//
// FILE NAME:       vdbTable.h
//
// INITIAL DATE:    2/1/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef __vdbTable_h
#define __vdbTable_h

#include <vdbMessageBox.h>
#include "vdbSQL.h"
#include "boolean.h"
#include "vdbTimeStampDB.h"
#include "vdbSet.h"
#include "vdbColumn.h"
#include "vdbIndex.h"

class vdbDatabase;
class vdbObject;


//=============================================================================
class vdbTable
{
public:
	// essential class functions
	vdbTable( vdbDatabase* pDatabase );				// basic constructor
	virtual ~vdbTable();							// destructor
	vdbTable( const vdbTable& rhs );				// copy constructor 
	vdbTable& operator=( vdbTable& rhs );			// assignment operator 
	
public:
	// required virtual functions (not pure because vdbSet copy constructor complains )
	virtual const char* TableNameCS();
	virtual int GetCodeVersion();					// the version in code
	virtual const char* ClientIDColumn( vdbObject* pClient );

public:
	// database, column and index public functions
	const char* TableName();
	const char* FullyQualifiedTableName();
	void ResetFullyQualifiedTableName();
	inline vdbDatabase* GetDatabase();
	inline void AddColumn( vdbColumn* p );
	inline const int ColumnCount();
	inline vdbColumn& Column( int which );
	inline void AddIndex( vdbIndex* p );
	inline const int IndexCount();
	inline vdbIndex& Index( int which = 0 );

public:
	// SQL table create and drop functions
	virtual RETCODE CreateTable();
	virtual void CreateIndexes();
	virtual RETCODE CreateIndex ( int which );
	virtual RETCODE DropTable();
	virtual void DropIndexes();
	virtual RETCODE DropIndex( int which );
	bool TableExists();
	virtual RETCODE AlterTable( vdbColumn& newColumn, const char* szHow );
	virtual RETCODE RenameColumn( const char* szOldName, const char* szNewName );

public:
	// information	
	bool IsValidColumn( const char* szColumnName );
	
public:
	// support functions
	RETCODE AddToVersionTable();
	RETCODE RemoveFromVersionTable();

public:
	// public version and identification utility functions
	int GetDiskVersion();							// the version on the disk	
	int TableID();
	
private:
	// base member data
	vdbDatabase* _pDatabase;						// the ODBC environment and database
	vdbSet<vdbColumn> _columns;						// the columns of the table
	vdbSet<vdbIndex> _indexes;						// the indexes of the table
	char _szTableName[41];							// the properly capitalized table name, assigned the first time TableName() is called.
	char _szFullyQualifiedTableName[71];			// the fully qualified table name, assigned the first time FullyQualifiedTableName() is called.
};


//=============================================================================
// column and index inline functions
//=============================================================================

//------------------------------------------
inline vdbDatabase* vdbTable::GetDatabase()
{
	return _pDatabase;
}


//------------------------------------------
inline void vdbTable::AddColumn( vdbColumn* p )
{
	_columns.Add( p );
}


//------------------------------------------
inline const int vdbTable::ColumnCount()
{
	return _columns.Count();
}


//------------------------------------------
inline vdbColumn& vdbTable::Column( int which )
{
	return _columns[which];
}


//------------------------------------------
inline void vdbTable::AddIndex( vdbIndex* p )
{
	_indexes.Add( p );
}


//------------------------------------------
inline const int vdbTable::IndexCount()
{
	return _indexes.Count();
}


//------------------------------------------
inline vdbIndex& vdbTable::Index( int which )
{
	return _indexes[which];
}


#endif // __vdbTable_h

