//=============================================================================
//
// CLASS:           vdbObject
//
// ESSENCE:         database object base class 
//
// FILE NAME:       vdbObject.h
//
// INITIAL DATE:    11/27/94 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1994 by Joe Honton
//
//=============================================================================

#ifndef __vdbObject_h
#define __vdbObject_h

#include <vdbTypedef.h>
#include <vdbSet.h>
#include <vdbUtility.h>
#include <vdbBinaryVar.h>
#include <vdbBinaryFixed.h>
#include <vdbBinaryLong.h>
#include <vdbVarCharLong.h>
#include <boolean.h>
#include "vdbTable.h"

class vdbStatement;
class vdbDatabase;
class xmlItem;

//=============================================================================
class vdbObject
{
public:
	// essential class functions
	vdbObject( OID oid );							// basic constructor
	virtual ~vdbObject();							// destructor
	vdbObject( const vdbObject& rhs );				// copy constructor 
	vdbObject& operator=( const vdbObject& rhs );	// assignment operator 
	
protected:
	// required virtual functions
	virtual RETCODE Initialize( vdbTable* pTable ) = 0;
	virtual void Bind( vdbStatement& stmt ) = 0;
	virtual void Parameter( vdbStatement& stmt, bool bInsert ) = 0;
	virtual const char* ClientIDColumn( vdbObject* pClient ) = 0;

protected:
	// optional virtual functions
	virtual bool ReadXML( xmlItem* pTable );
	virtual void SetGenericColumn( const char* szColumnName, const char* szValue );
	virtual xmlItem* WriteXML();
	virtual xmlItem* WriteXML( vdbSet<vdbString>& columnNames );
	virtual void GetGenericColumn( const char* szColumnName, vdbString& sValue );
	static xmlItem* WriteXMLSchema();

public:
	// generic accessor functions
	void SetGeneric( char* dest, int sizeofDest, const char* szSource );
	void SetGeneric( OID& dest, const char* szSource );
	void SetGeneric( DBBIT& dest, const char* szSource );
	void SetGeneric( DBBYTE& dest, const char* szSource );
	void SetGeneric( DBSMALL& dest, const char* szSource );
	void SetGeneric( DBINT& dest, const char* szSource );
	void SetGeneric( DBFLOAT& dest, const char* szSource );
	void SetGeneric( DBDOUBLE& dest, const char* szSource );
	void SetGeneric( vdbTimeStampDB& dest, const char* szSource );
	void SetGeneric( vdbBinaryFixed& dest, const char* szSource );
	void SetGeneric( vdbBinaryVar& dest, const char* szSource );
	void SetGeneric( vdbBinaryLong& dest, const char* szSource );
	void SetGeneric( vdbVarCharLong& dest, const char* szSource );

	void GetGeneric( const char* source, vdbString& sDest );
	void GetGeneric( OID& source, vdbString& sDest );
	void GetGeneric( DBBIT& source, vdbString& sDest );
	void GetGeneric( DBBYTE& source, vdbString& sDest );
	void GetGeneric( DBSMALL& source, vdbString& sDest );
	void GetGeneric( DBINT& source, vdbString& sDest );
	void GetGeneric( DBFLOAT& source, vdbString& sDest );
	void GetGeneric( DBDOUBLE& source, vdbString& sDest );
	void GetGeneric( vdbTimeStampDB& source, vdbString& sDest );
	void GetGeneric( vdbBinaryFixed& source, vdbString& sDest );
	void GetGeneric( vdbBinaryVar& source, vdbString& sDest );
	void GetGeneric( vdbBinaryLong& source, vdbString& sDest );
	void GetGeneric( vdbVarCharLong& source, vdbString& sDest );

public:
	// member access functions
	inline vdbDatabase* GetDatabase();
	inline const char* TableName();
	inline const char* TableNameCS();
	inline const char* FullyQualifiedTableName();
	inline const int ColumnCount();
	inline vdbColumn& Column( int which );
	inline const int IndexCount();
	inline vdbIndex& Index( int which );
	inline OID GetOID() const;
	inline void Modified( bool b = true );
	inline bool IsModified();
	inline void Persistent( bool b = true );
	inline bool IsPersistent();

protected:
	inline void SetOID( OID oid );
	
protected:
	// functions related to "long varchar" and "long varbinary" data types
	vdbVarCharLong& ReadText( const char* szColumnName, vdbVarCharLong& textValue );
	vdbBinaryLong& ReadImage( const char* szColumnName, vdbBinaryLong& image );
	void WriteText( const char* szColumnName, vdbVarCharLong& textValue );
	void WriteImage( const char* szColumnName, vdbBinaryLong& image );

public:
	// SQL encapsulation
	RETCODE Save();
	RETCODE Delete();
	RETCODE Update();
	RETCODE Insert();
	int Extent();
	int CountChildren( const char* szChildTableName, const char* szChildForeignKey );
	int CountChildren( const char* szChildTableName, const char* szChildForeignKey1, OID oid1, const char* szChildForeignKey2, OID oid2 );

public:
	// optional virtual member functions
	virtual void GetNextObjectID();
	virtual RETCODE PreRetrieveTrigger();
	virtual RETCODE PostRetrieveTrigger();
	virtual RETCODE PreInsertTrigger();
	virtual RETCODE PostInsertTrigger();
	virtual RETCODE PreUpdateTrigger();
	virtual RETCODE PostUpdateTrigger();
	virtual RETCODE PreDeleteTrigger();
	virtual RETCODE PostDeleteTrigger();
	virtual OID* ClientID( vdbObject* pClient );

protected:
	vdbTable* _pTable;				// the table to insert objects of this type into

private:
	// member data
	OID  _oid;						// the unique object identifier of this object
	vdbDatabase* _pDatabase;		// the ODBC environment and database
    bool _bModified;				// has any data member been changed and not saved?
    bool _bPersistent;				// does this row exist in the database?
};


//==========================================================================================
// inlines
//==========================================================================================

inline vdbDatabase* vdbObject::GetDatabase()
{
	return _pDatabase;
}


inline const char* vdbObject::TableName()
{
	return _pTable->TableName();
}


inline const char* vdbObject::TableNameCS()
{
	return _pTable->TableNameCS();
}


inline const char* vdbObject::FullyQualifiedTableName()
{
	return _pTable->FullyQualifiedTableName();
}


inline const int vdbObject::ColumnCount()
{
	return _pTable->ColumnCount();
}


inline vdbColumn& vdbObject::Column( int which )
{
	return _pTable->Column( which );
}


inline const int vdbObject::IndexCount()
{
	return _pTable->IndexCount();
}


inline vdbIndex& vdbObject::Index( int which )
{
	return _pTable->Index( which );
}


inline OID vdbObject::GetOID() const
{
	return _oid;
}


inline void vdbObject::SetOID( OID oid )
{
	_oid = oid;
}


inline void vdbObject::Modified( bool b )
{
	_bModified = b;
}


inline bool vdbObject::IsModified()
{
	return _bModified;
}


inline void vdbObject::Persistent( bool b )
{
	_bPersistent = b;
}


inline bool vdbObject::IsPersistent()
{
	return _bPersistent;
}



#endif // __vdbObject_h

