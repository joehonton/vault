//=============================================================================
//
// CLASS:           vdbStatement
//
// ESSENCE:         vdbStatement class
//
// FILE NAME:       vdbStatement.h
//
// INITIAL DATE:    1/26/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef __vdbStatement_h
#define __vdbStatement_h

#include <vdbMessageBox.h>
#include "vdbSQL.h"
#include "boolean.h"
#include "vdbTypedef.h"
#include "vdbTimeStampDB.h"
#include "vdbVarCharLong.h"
#include "vdbBinaryLong.h"

class vdbDatabase;


//=========================================================================

class vdbStatement
{
public:
	// essential class functions
	vdbStatement( vdbDatabase* pDatabase );					// basic constructor
	~vdbStatement();										// destructor

private:
	vdbStatement( const vdbStatement& rhs );				// copy constructor disabled
	vdbStatement& operator=( const vdbStatement& rhs );		// assignment operator disabled 

public:
	inline vdbDatabase* GetDatabase();
	inline RETCODE ReturnCode();

public:
	// ODBC cover functions
	RETCODE Bind( UWORD col, DBBIT& c );
	RETCODE Bind( UWORD col, DBBYTE& c );
	RETCODE Bind( UWORD col, DBSMALL& i );
	RETCODE Bind( UWORD col, DBINT& i );
	RETCODE Bind( UWORD col, DBFLOAT& f );
	RETCODE Bind( UWORD col, DBDOUBLE& f );
	RETCODE Bind( UWORD col, char* s, UWORD size );
	RETCODE Bind( UWORD col, DBDATE& d );
	RETCODE Bind( UWORD col, DBTIME& t );
	RETCODE Bind( UWORD col, DBTIMESTAMP& t );
	RETCODE Bind( UWORD col, vdbTimeStampDB& t );
	RETCODE Bind( UWORD col, OID& i );
	RETCODE BindBinary( UWORD col, void* p, UWORD actualLength );
	RETCODE BindVarBinary( UWORD col, void* p, UWORD maxLength, SDWORD* actualLength );
	RETCODE BindCol(UWORD icol, SWORD fCType, PTR rgbValue, SDWORD cbValueMax, SDWORD* pcbValue );
	RETCODE BindParameter( UWORD ipar, SWORD fParamType, SWORD fCType, SWORD fSqlType, UDWORD cbColDef, SWORD ibScale, PTR rgbValue, SDWORD cbValueMax, SDWORD* pcbValue );
	RETCODE Cancel();
	RETCODE Close();
	RETCODE ColAttributes( UWORD icol, UWORD fDescType, SDWORD* pfDesc );
	RETCODE ColAttributes( UWORD icol, UWORD fDescType, PTR rgbDesc, SWORD cbDescMax, SWORD* pcbDesc );
	RETCODE ColAttributes( UWORD icol, UWORD fDescType, PTR rgbDesc, SWORD cbDescMax, SWORD* pcbDesc, SDWORD* pfDesc );
	RETCODE ColumnPrivileges( LPCSTR szTableQualifier, LPCSTR szTableOwner, LPCSTR szTableName, LPCSTR szColumnName);
	RETCODE ColumnPrivileges( LPCSTR szTableQualifier, SWORD cbTableQualifier, LPCSTR szTableOwner, SWORD cbTableOwner, LPCSTR szTableName, SWORD cbTableName,	LPCSTR szColumnName, SWORD cbColumnName );
	RETCODE Columns( LPCSTR szTableQualifier, LPCSTR szTableOwner,	LPCSTR szTableName,	LPCSTR szColumnName);
	RETCODE Columns( LPCSTR szTableQualifier, SWORD cbTableQualifier, LPCSTR szTableOwner, SWORD cbTableOwner,	LPCSTR szTableName, SWORD cbTableName, LPCSTR szColumnName, SWORD cbColumnName );
	RETCODE DescribeCol( UWORD icol, LPSTR szColName, SWORD cbColNameMax, SWORD* pcbColName, SWORD* pfSqlType, UDWORD* pcbColDef, SWORD* pibScale, SWORD* pfNullable );
	RETCODE DescribeParam( UWORD ipar, SWORD* pfSqlType, UDWORD* pcbColDef, SWORD* pibScale, SWORD* pfNullable);
	RETCODE Error( int functionNumber );
	RETCODE Error( LPSTR szSqlState, SDWORD* pfNativeError, LPSTR szErrorMsg, SWORD cbErrorMsgMax, SWORD* pcbErrorMsg);
	RETCODE Execute( const char* s );
	RETCODE ExecDirect( LPCSTR szSqlStr, SDWORD cbSqlStr );
	RETCODE Execute();
	RETCODE ExtendedFetch( UWORD fFetchType, SDWORD irow, UWORD* rgfRowStatus );
	RETCODE ExtendedFetch( UWORD fFetchType, SDWORD irow, UDWORD* pcrow, UWORD* rgfRowStatus );
	RETCODE Fetch();
	RETCODE Fetch( UWORD fFetchType, SDWORD irow, UWORD* rgfRowStatus );
	RETCODE Fetch( UWORD fFetchType, SDWORD irow, UDWORD* pcrow, UWORD* rgfRowStatus );
	RETCODE ForeignKeys( LPCSTR szPkTableQualifier, LPCSTR szPkTableOwner, LPCSTR szPkTableName, LPCSTR szFkTableQualifier, LPCSTR szFkTableOwner, LPCSTR szFkTableName);
	RETCODE ForeignKeys( LPCSTR szPkTableQualifier, SWORD cbPkTableQualifier, LPCSTR szPkTableOwner, SWORD  cbPkTableOwner, LPCSTR szPkTableName, SWORD  cbPkTableName,
											LPCSTR szFkTableQualifier, SWORD cbFkTableQualifier, LPCSTR szFkTableOwner, SWORD  cbFkTableOwner,	LPCSTR szFkTableName, SWORD cbFkTableName );
	RETCODE FreeStmt( UWORD fOption );
	RETCODE GetCursorName( LPSTR szCursor,	SWORD cbCursorMax, SWORD* pcbCursor );
	RETCODE GetData( UWORD icol, SWORD fCType,	PTR rgbValue, SDWORD cbValueMax, SDWORD* pcbValue );
	RETCODE GetStmtOption( UWORD fOption, PTR pvParam );
	RETCODE GetTypeInfo( SWORD fSqlType );
	RETCODE MoreResults();
	RETCODE NumParams( SWORD* pcpar );
	RETCODE NumResultCols( SWORD* pccol );
	RETCODE Parameter( UWORD col, DBBIT& c );
	RETCODE Parameter( UWORD col, DBBYTE& c );
	RETCODE Parameter( UWORD col, DBSMALL& i );
	RETCODE Parameter( UWORD col, DBINT& i );
	RETCODE ParameterDecimal( UWORD col, const char* s, UDWORD precision, SWORD scale );
	RETCODE Parameter( UWORD col, DBFLOAT& f );
	RETCODE Parameter( UWORD col, DBDOUBLE& f );
	RETCODE ParameterChar( UWORD col, const char* s );
	RETCODE ParameterVarChar( UWORD col, const char* s );
	RETCODE ParameterLongChar( UWORD col, vdbVarCharLong& textItem );
	RETCODE Parameter( UWORD col, DBDATE& d );
	RETCODE Parameter( UWORD col, DBTIME& t );
	RETCODE Parameter( UWORD col, DBTIMESTAMP& t, UDWORD precision, SWORD scale );
	RETCODE Parameter( UWORD col, vdbTimeStampDB& t );
	RETCODE Parameter( UWORD col, OID& i );
	RETCODE ParameterBinary( UWORD col, void* p, UWORD actualLength );
	RETCODE ParameterVarBinary( UWORD col, void* p, UWORD maxLength, SDWORD* actualLength );
	RETCODE ParameterLongBinary( UWORD col, vdbBinaryLong& image );
	RETCODE ParamData( PTR* prgbValue );
	RETCODE ParamOptions( UDWORD crow, UDWORD* pirow );
	RETCODE Prepare( LPCSTR szSqlStr, SDWORD cbSqlStr );
	RETCODE PrimaryKeys( LPCSTR szTableQualifier, LPCSTR szTableOwner,	LPCSTR szTableName );
	RETCODE PrimaryKeys( LPCSTR szTableQualifier, SWORD cbTableQualifier, LPCSTR szTableOwner,	SWORD cbTableOwner,	LPCSTR szTableName, SWORD cbTableName);
// Dec-01-2009	RETCODE ProcedureColumns( LPCSTR szProcQualifier, LPCSTR szProcOwner, LPCSTR szProcName, LPCSTR szColumnName);
	RETCODE ProcedureColumns( LPCSTR szProcQualifier, SWORD cbProcQualifier, LPCSTR szProcOwner, SWORD cbProcOwner, LPCSTR szProcName, SWORD cbProcName, LPCSTR  szColumnName, SWORD cbColumnName );
	RETCODE Procedures(LPCSTR szProcQualifier, LPCSTR szProcOwner, LPCSTR szProcName );
	RETCODE Procedures(LPCSTR szProcQualifier,	SWORD cbProcQualifier, LPCSTR szProcOwner, SWORD cbProcOwner, LPCSTR szProcName, SWORD cbProcName );
	RETCODE PutData(PTR rgbValue, SDWORD cbValue);
	RETCODE ResetParams();
	RETCODE RowCount(SDWORD* pcrow);
	RETCODE SetCursorName(LPCSTR szCursor, SWORD cbCursor );
	RETCODE SetPos(UWORD irow,	BOOL fRefresh, BOOL fLock);
	RETCODE SetScrollOptions(UWORD fConcurrency, SDWORD crowKeyset, UWORD crowRowset);
	RETCODE SetStmtOption( UWORD fOption, UDWORD vParam );
	RETCODE SpecialColumns( UWORD fColType, LPCSTR szTableQualifier, LPCSTR szTableOwner, LPCSTR szTableName, UWORD fScope, UWORD fNullable );
	RETCODE SpecialColumns(UWORD fColType, LPCSTR szTableQualifier, SWORD cbTableQualifier, LPCSTR szTableOwner, SWORD cbTableOwner, LPCSTR szTableName, SWORD cbTableName, UWORD fScope, UWORD fNullable );
	RETCODE Statistics( LPCSTR  szTableQualifier, LPCSTR szTableOwner,	LPCSTR szTableName,	UWORD fUnique, UWORD fAccuracy );
	RETCODE Statistics(LPCSTR  szTableQualifier, SWORD  cbTableQualifier, LPCSTR szTableOwner,	SWORD cbTableOwner, LPCSTR szTableName, SWORD  cbTableName,	UWORD  fUnique,	UWORD  fAccuracy );
	RETCODE TablePrivileges(LPCSTR szTableQualifier, LPCSTR szTableOwner, LPCSTR szTableName );
	RETCODE TablePrivileges(LPCSTR szTableQualifier, SWORD cbTableQualifier, LPCSTR szTableOwner, SWORD  cbTableOwner,	LPCSTR szTableName,	SWORD  cbTableName);
	RETCODE Tables( LPCSTR szTableQualifier, LPCSTR szTableOwner, LPCSTR szTableName, LPCSTR szTableType);
	RETCODE Tables( LPCSTR szTableQualifier, SWORD cbTableQualifier, LPCSTR szTableOwner, SWORD cbTableOwner, LPCSTR szTableName, SWORD  cbTableName, LPCSTR szTableType, SWORD  cbTableType );
	RETCODE Unbind();
	
protected:
	// member data
	RETCODE _rc;            	// ODBC return code
	HSTMT _hstmt;				// ODBC statment
	vdbDatabase* _pDatabase;	// the ODBC environment and database

	RETCODE stmtException( int functionNumber );
};


//==========================================================================================
// inlines
//==========================================================================================

inline vdbDatabase* vdbStatement::GetDatabase()
{
	return _pDatabase;
}


inline RETCODE vdbStatement::ReturnCode()
{
	return _rc;
}


#endif // __vdbStatement_h
