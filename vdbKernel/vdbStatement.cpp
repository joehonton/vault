//=============================================================================
//
// CLASS:           vdbStatement
//
// ESSENCE:         vdbStatement class
//
// FILE NAME:       vdbStatement.cpp
//
// INITIAL DATE:    1/26/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifdef USE_PRE_HDR
	#include "vdbHeader.h"
#else
	#include <vdbAssert.h>
	#include "vdbStatement.h"
	#include "vdbDatabase.h"
	#include "vdbException.h"
#endif

#include <strstream>
#include "vdbSafeString.h"


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
// basic constructor
vdbStatement::vdbStatement( vdbDatabase* pDatabase )
	: _pDatabase( pDatabase )
{
	EnterCriticalSection( _pDatabase->StatementGuard( ) );
	assert( _pDatabase->GetHdbc( ) != SQL_NULL_HDBC );
	_hstmt = SQL_NULL_HSTMT;
    _rc    = ::SQLAllocStmt( _pDatabase->GetHdbc( ), &_hstmt );
	stmtException( SQL_API_SQLALLOCSTMT );
}


//-------------------------------------
// destructor
vdbStatement::~vdbStatement( )
{
	if (  _hstmt != SQL_NULL_HSTMT )
	{
		_rc = ::SQLFreeStmt( _hstmt, SQL_DROP );
		stmtException( SQL_API_SQLFREESTMT );
	}
	LeaveCriticalSection( _pDatabase->StatementGuard( ) );
}


//=========================================================================
// ODBC cover functions
// Refer to ODBC documentation for details
//=========================================================================

//-------------------------------------
// Bind a 1-bit column
RETCODE vdbStatement::Bind( UWORD col, DBBIT& c )
{
	static SDWORD nullIndicator = 0;
	return BindCol( col, SQL_C_BIT, &c, 1, &nullIndicator );
}


//-------------------------------------
// Bind a 1-byte signed integer column
RETCODE vdbStatement::Bind( UWORD col, DBBYTE& c )
{
	static SDWORD nullIndicator = 0;
	return BindCol( col, SQL_C_STINYINT, &c, 1, &nullIndicator );
}


//-------------------------------------
// Bind a 2-byte signed integer column
RETCODE vdbStatement::Bind( UWORD col, DBSMALL& i )
{
	static SDWORD nullIndicator = 0;
	return BindCol( col, SQL_C_SSHORT, &i, 2, &nullIndicator );
}


//-------------------------------------
// Bind a 4-byte signed integer column
RETCODE vdbStatement::Bind( UWORD col, DBINT& i )
{
	static SDWORD nullIndicator = 0;
	return BindCol( col, SQL_C_SLONG, &i, 4, &nullIndicator );
}


//-------------------------------------
// Bind a 4-byte float column
RETCODE vdbStatement::Bind( UWORD col, DBFLOAT& f )
{
	static SDWORD nullIndicator = 0;
	return BindCol( col, SQL_C_FLOAT, &f, 4, &nullIndicator );
}


//-------------------------------------
// Bind a 8-byte float column
RETCODE vdbStatement::Bind( UWORD col, DBDOUBLE& f )
{
	static SDWORD nullIndicator = 0;
	return BindCol( col, SQL_C_DOUBLE, &f, 8, &nullIndicator );
}


//-------------------------------------
// Bind a character null-terminated string column.  Use this for char, varchar, and decimal columns
RETCODE vdbStatement::Bind( UWORD col, char* s, UWORD size )
{
	static SDWORD pcbValue;
	pcbValue = size - 1;
	return BindCol( col, SQL_C_CHAR, s, size, &pcbValue );
}


//-------------------------------------
// Bind a date column
RETCODE vdbStatement::Bind( UWORD col, DBDATE& d )
{
	static SDWORD nullIndicator = 0;
	return BindCol( col, SQL_C_DATE, &d, sizeof( DATE_STRUCT ), &nullIndicator );
}


//-------------------------------------
// Bind a time column
RETCODE vdbStatement::Bind( UWORD col, DBTIME& t )
{
	static SDWORD nullIndicator = 0;
	return BindCol( col, SQL_C_TIME, &t, sizeof( TIME_STRUCT ), &nullIndicator );
}


//-------------------------------------
// Bind a timestamp column
RETCODE vdbStatement::Bind( UWORD col, DBTIMESTAMP& t )
{
	static SDWORD nullIndicator = 0;
	return BindCol( col, SQL_C_TIMESTAMP, &t, sizeof( TIMESTAMP_STRUCT ), &nullIndicator );
}


//-------------------------------------
// Bind a timestamp column
RETCODE vdbStatement::Bind( UWORD col, vdbTimeStampDB& t )
{
	static SDWORD nullIndicator = 0;
	return BindCol( col, SQL_C_TIMESTAMP, t.GetDataPointer(), sizeof( DBTIMESTAMP ), &nullIndicator );
}


//-------------------------------------
// Bind an object identifier column (4 byte unsigned integer)
RETCODE vdbStatement::Bind( UWORD col, OID& i )
{
	static SDWORD nullIndicator = 0;
	return BindCol( col, SQL_C_ULONG, &i, 4, &nullIndicator );
}


//-------------------------------------
// Bind a fixed-length binary column
RETCODE vdbStatement::BindBinary( UWORD col, void* p, UWORD actualLength )
{
	static SDWORD pcbValue;
	pcbValue = actualLength;
	return BindCol( col, SQL_C_BINARY, p, actualLength, &pcbValue );
}


//-------------------------------------
// Bind a variable-length binary column
RETCODE vdbStatement::BindVarBinary( UWORD col, void* p, UWORD maxLength, SDWORD* actualLength )
{
	*actualLength = maxLength;
	return BindCol( col, SQL_C_BINARY, p, maxLength, actualLength );
}


//--------------------------------------------
// Bind a C address to an SQL column prior to a Fetch 
RETCODE vdbStatement::BindCol( UWORD icol, SWORD fCType, PTR rgbValue, SDWORD cbValueMax, SDWORD* pcbValue )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLBindCol( _hstmt, icol, fCType, rgbValue, cbValueMax, pcbValue );
	return stmtException( SQL_API_SQLBINDCOL );
}


//--------------------------------------------
// Bind an application value to a SQL column prior to an INSERT or UPDATE
RETCODE vdbStatement::BindParameter( UWORD ipar, SWORD fParamType, SWORD fCType, SWORD fSqlType, UDWORD cbColDef, SWORD ibScale, PTR rgbValue, SDWORD cbValueMax, SDWORD* pcbValue )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLBindParameter( _hstmt, ipar, fParamType, fCType, fSqlType, cbColDef, ibScale, rgbValue, cbValueMax, pcbValue );
	return stmtException( SQL_API_SQLBINDPARAMETER );
}


//--------------------------------------------
RETCODE vdbStatement::Cancel( )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLCancel( _hstmt );
	return stmtException( SQL_API_SQLCANCEL );
}


//--------------------------------------------
RETCODE vdbStatement::Close( )
{
	return FreeStmt( SQL_CLOSE );
}


//--------------------------------------------
RETCODE vdbStatement::ColAttributes( UWORD icol,	UWORD fDescType, SDWORD* pfDesc )
{
	return ColAttributes( icol, fDescType, NULL, 0, NULL, pfDesc );
}


//--------------------------------------------
RETCODE vdbStatement::ColAttributes( UWORD icol,	UWORD fDescType, PTR rgbDesc, SWORD cbDescMax, SWORD* pcbDesc )
{
	return ColAttributes( icol, fDescType, rgbDesc, cbDescMax, pcbDesc, NULL );
}


//--------------------------------------------
RETCODE vdbStatement::ColAttributes( UWORD icol, UWORD fDescType, PTR rgbDesc, SWORD cbDescMax, SWORD* pcbDesc, SDWORD* pfDesc )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLColAttributes( _hstmt, icol, fDescType, rgbDesc, cbDescMax, pcbDesc, pfDesc );
	return stmtException( SQL_API_SQLCOLATTRIBUTES );
}


//--------------------------------------------
RETCODE vdbStatement::ColumnPrivileges( LPCSTR szTableQualifier, LPCSTR szTableOwner, LPCSTR szTableName, LPCSTR szColumnName )
{
	return ColumnPrivileges( szTableQualifier, SQL_NTS,	szTableOwner, SQL_NTS, szTableName, SQL_NTS, szColumnName, SQL_NTS );
}


//--------------------------------------------
RETCODE vdbStatement::ColumnPrivileges( LPCSTR szTableQualifier, SWORD cbTableQualifier, LPCSTR szTableOwner, SWORD cbTableOwner, LPCSTR szTableName, SWORD cbTableName,	LPCSTR szColumnName, SWORD cbColumnName )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLColumnPrivileges( _hstmt, ( UCHAR* ) szTableQualifier, cbTableQualifier, ( UCHAR* ) szTableOwner, cbTableOwner,	( UCHAR* ) szTableName, cbTableName, ( UCHAR* ) szColumnName, cbColumnName );
	return stmtException( SQL_API_SQLCOLUMNPRIVILEGES );
}


//--------------------------------------------
RETCODE vdbStatement::Columns( LPCSTR szTableQualifier, LPCSTR szTableOwner, LPCSTR szTableName, LPCSTR szColumnName )
{
	return Columns( szTableQualifier, SQL_NTS, szTableOwner, SQL_NTS, szTableName, SQL_NTS, szColumnName, SQL_NTS );
}


//--------------------------------------------
RETCODE vdbStatement::Columns( LPCSTR szTableQualifier, SWORD cbTableQualifier, LPCSTR szTableOwner, SWORD cbTableOwner,	LPCSTR szTableName, SWORD cbTableName, LPCSTR szColumnName, SWORD cbColumnName )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLColumns( _hstmt, ( UCHAR* ) szTableQualifier, cbTableQualifier, ( UCHAR* ) szTableOwner, cbTableOwner,	( UCHAR* ) szTableName, cbTableName, ( UCHAR* ) szColumnName, cbColumnName );
	return stmtException( SQL_API_SQLCOLUMNS );
}


//--------------------------------------------
RETCODE vdbStatement::DescribeCol( UWORD icol, LPSTR szColName, SWORD cbColNameMax, SWORD* pcbColName, SWORD* pfSqlType, UDWORD* pcbColDef, SWORD* pibScale, SWORD* pfNullable )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLDescribeCol( _hstmt, icol, ( UCHAR* ) szColName, cbColNameMax, pcbColName, pfSqlType, pcbColDef, pibScale, pfNullable );
	return stmtException( SQL_API_SQLDESCRIBECOL );
}


//--------------------------------------------
RETCODE vdbStatement::DescribeParam( UWORD ipar, SWORD* pfSqlType, UDWORD* pcbColDef, SWORD* pibScale, SWORD* pfNullable )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLDescribeParam( _hstmt, ipar, pfSqlType, pcbColDef,	pibScale, pfNullable );
	return stmtException( SQL_API_SQLDESCRIBEPARAM );
}


//--------------------------------------------
RETCODE vdbStatement::Error( LPSTR szSqlState, SDWORD* pfNativeError, LPSTR szErrorMsg, SWORD cbErrorMsgMax, SWORD* pcbErrorMsg )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	return ::SQLError( SQL_NULL_HENV, SQL_NULL_HDBC, _hstmt, ( UCHAR* ) szSqlState, pfNativeError, ( UCHAR* ) szErrorMsg, cbErrorMsgMax, pcbErrorMsg );
}


//------------------------------------------
RETCODE vdbStatement::Error( int functionNumber )
{
	// If we were told to ignore all errors, just return
	if (  GetDatabase( )->QuenchErrors( ) )
		return _rc;

	char szSqlState[50];
	char szErrorMsg[SQL_MAX_MESSAGE_LENGTH];
	SDWORD pfNativeError;
	SWORD pcbErrorMsg;
	Error( szSqlState, &pfNativeError, szErrorMsg, SQL_MAX_MESSAGE_LENGTH - 1, &pcbErrorMsg );

	// SQLGetData returns with "string data truncated" and pfNativeError 0
	// Suppress this message and pass it back to the caller
	if ( functionNumber == SQL_API_SQLGETDATA && _rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS_WITH_INFO;

/*
	// Mask "Cannot create a row of size N which is greater than the allowable maximum of 8060." message from Microsoft SQL Server
	if (  pfNativeError == 511 )
	{
		_rc = SQL_SUCCESS;
		return;
	}
*/

	std::ostrstream os;
	os << "A problem occured while accessing the database.\n\n";
//	os << "Sql State: " << szSqlState << "\n\nNative Error:  " << pfNativeError << "\n\nFunction Number: " << functionNumber << "\n\nError Message: " << szErrorMsg;
	os << "Error Message: " << szErrorMsg;
	os << "\n\nPress OK to continue anyway or CANCEL to quit the application." << std::ends;
    vdbSafeString msg( os.str( ) );

	if (  vdbMessageBox( NULL, msg, "Statement Error", MB_SETFOREGROUND | MB_OKCANCEL ) == IDCANCEL )
	{
		GetDatabase( )->QuenchErrors( true );
		throw vdbException( "Application cancelled" );
	}
	return _rc;
}


//--------------------------------------------
RETCODE vdbStatement::Execute( const char* s )
{
	return ExecDirect( s, strlen( s ) );
}


//--------------------------------------------
RETCODE vdbStatement::ExecDirect( LPCSTR szSqlStr, SDWORD cbSqlStr )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLExecDirect( _hstmt, ( UCHAR* ) szSqlStr, cbSqlStr );
	return stmtException( SQL_API_SQLEXECDIRECT );
} 


//--------------------------------------------
RETCODE vdbStatement::Execute( )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLExecute( _hstmt );
	return stmtException( SQL_API_SQLEXECUTE );
}


//--------------------------------------------
RETCODE vdbStatement::ExtendedFetch( UWORD fFetchType, SDWORD irow, UWORD* rgfRowStatus )
{
	return ExtendedFetch( fFetchType,irow, NULL, rgfRowStatus );
}


//--------------------------------------------
RETCODE vdbStatement::ExtendedFetch( UWORD fFetchType, SDWORD irow, UDWORD* pcrow, UWORD* rgfRowStatus )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLExtendedFetch( _hstmt, fFetchType, irow, pcrow, rgfRowStatus );
	return stmtException( SQL_API_SQLEXTENDEDFETCH );
}


//--------------------------------------------
RETCODE vdbStatement::Fetch( )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLFetch( _hstmt );
	return stmtException( SQL_API_SQLFETCH );
}


//--------------------------------------------
RETCODE vdbStatement::Fetch( UWORD fFetchType, SDWORD irow, UWORD* rgfRowStatus )
{
	return ExtendedFetch( fFetchType, irow, NULL, rgfRowStatus );
}


//--------------------------------------------
RETCODE vdbStatement::Fetch( UWORD fFetchType, SDWORD irow, UDWORD* pcrow, UWORD* rgfRowStatus )
{
	return ExtendedFetch( fFetchType, irow, pcrow, rgfRowStatus );
}


//--------------------------------------------
RETCODE vdbStatement::ForeignKeys( LPCSTR szPkTableQualifier, LPCSTR szPkTableOwner, LPCSTR szPkTableName, LPCSTR szFkTableQualifier, LPCSTR szFkTableOwner, LPCSTR szFkTableName )
{
	return ForeignKeys( szPkTableQualifier, SQL_NTS,	szPkTableOwner, SQL_NTS, szPkTableName, SQL_NTS, szFkTableQualifier, SQL_NTS, szFkTableOwner, SQL_NTS, szFkTableName, SQL_NTS );
}


//--------------------------------------------
RETCODE vdbStatement::ForeignKeys( LPCSTR szPkTableQualifier, SWORD cbPkTableQualifier, LPCSTR szPkTableOwner, SWORD  cbPkTableOwner, LPCSTR szPkTableName, SWORD  cbPkTableName,
											LPCSTR szFkTableQualifier, SWORD cbFkTableQualifier, LPCSTR szFkTableOwner, SWORD  cbFkTableOwner,	LPCSTR szFkTableName, SWORD cbFkTableName )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLForeignKeys( _hstmt, ( UCHAR* ) szPkTableQualifier, cbPkTableQualifier, ( UCHAR* ) szPkTableOwner, cbPkTableOwner, ( UCHAR* ) szPkTableName, cbPkTableName,
										( UCHAR* ) szFkTableQualifier, cbFkTableQualifier, ( UCHAR* ) szFkTableOwner, cbFkTableOwner, ( UCHAR* ) szFkTableName, cbFkTableName );
	return stmtException( SQL_API_SQLFOREIGNKEYS );
}


//--------------------------------------------
RETCODE vdbStatement::FreeStmt( UWORD fOption )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	assert( fOption != SQL_DROP );
	_rc = ::SQLFreeStmt( _hstmt, fOption );
	return stmtException( SQL_API_SQLFREESTMT );
}


//--------------------------------------------
RETCODE vdbStatement::GetCursorName( LPSTR szCursor,	SWORD cbCursorMax, SWORD* pcbCursor )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLGetCursorName( _hstmt, ( UCHAR* ) szCursor, cbCursorMax, pcbCursor );
	return stmtException( SQL_API_SQLGETCURSORNAME );
}


//--------------------------------------------
RETCODE vdbStatement::GetData( UWORD icol, SWORD fCType,	PTR rgbValue, SDWORD cbValueMax, SDWORD* pcbValue )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLGetData( _hstmt, icol, fCType, rgbValue, cbValueMax, pcbValue );
	return stmtException( SQL_API_SQLGETDATA );
}


//--------------------------------------------
RETCODE vdbStatement::GetStmtOption( UWORD fOption, PTR pvParam )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLGetStmtOption( _hstmt, fOption, pvParam );
	return stmtException( SQL_API_SQLGETSTMTOPTION );
}


//--------------------------------------------
RETCODE vdbStatement::GetTypeInfo( SWORD fSqlType )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLGetTypeInfo( _hstmt, fSqlType );
	return stmtException( SQL_API_SQLGETINFO );
}


//--------------------------------------------
RETCODE vdbStatement::MoreResults( )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLMoreResults( _hstmt );
	return stmtException( SQL_API_SQLMORERESULTS );
}


//--------------------------------------------
RETCODE vdbStatement::NumParams( SWORD* pcpar )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLNumParams( _hstmt, pcpar );
	return stmtException( SQL_API_SQLNUMPARAMS );
}


//--------------------------------------------
RETCODE vdbStatement::NumResultCols( SWORD* pccol )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLNumResultCols( _hstmt, pccol );
	return stmtException( SQL_API_SQLNUMRESULTCOLS );
}


//-------------------------------------
// Bind a 1-bit integer column
RETCODE vdbStatement::Parameter( UWORD col, DBBIT& c )
{
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT, 0, 0, &c, 0, NULL ); 																
}


//-------------------------------------
// Bind a 1-byte integer column
RETCODE vdbStatement::Parameter( UWORD col, DBBYTE& c )
{
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_STINYINT, SQL_TINYINT, 0, 0, &c, 0, NULL ); 																
}																					


//-------------------------------------
// Bind a 2-byte integer column
RETCODE vdbStatement::Parameter( UWORD col, DBSMALL& i )
{
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &i, 0, NULL ); 																
}


//-------------------------------------
// Bind a 4-byte integer column
RETCODE vdbStatement::Parameter( UWORD col, DBINT& i )
{
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &i, 0, NULL ); 																
}


//-------------------------------------
// Bind a decimal column
RETCODE vdbStatement::ParameterDecimal( UWORD col, const char* s, UDWORD precision, SWORD scale )
{
	static long length;
	length = strlen(s);
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_DECIMAL, precision, scale, (void*) s, length, NULL );
}


//-------------------------------------
// Bind a 4-byte float column
RETCODE vdbStatement::Parameter( UWORD col, DBFLOAT& f )
{
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL, 0, 0, &f, 0, NULL ); 																
}


//-------------------------------------
// Bind a 8-byte float column
RETCODE vdbStatement::Parameter( UWORD col, DBDOUBLE& f )
{
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_FLOAT, 0, 0, &f, 0, NULL ); 																
}


//-------------------------------------
// Bind a fixed length null-terminated character string column
RETCODE vdbStatement::ParameterChar( UWORD col, const char* s )
{
	static long length;
	length = strlen(s);
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, length, 0, (void*) s, length, NULL );
}


//-------------------------------------
// Bind a variable length null-terminated character string column
RETCODE vdbStatement::ParameterVarChar( UWORD col, const char* s )
{
	static long length;
	length = strlen(s);
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, length, 0, (void*) s, length, NULL );
}


//-------------------------------------
// Bind a long variable length null-terminated character string column
RETCODE vdbStatement::ParameterLongChar( UWORD col, vdbVarCharLong& textItem )
{
	static long length;
	length = textItem.Length();
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_LONGVARCHAR, length, 0, (void*) textItem.GetDataPointer(), length, &length );
}


//-------------------------------------
// Bind a date column
RETCODE vdbStatement::Parameter( UWORD col, DBDATE& d )
{
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_DATE, SQL_DATE, 0, 0, &d, 0, NULL ); 																
}


//-------------------------------------
// Bind a time column
RETCODE vdbStatement::Parameter( UWORD col, DBTIME& t )
{
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_TIME, SQL_TIME, 0, 0, &t, 0, NULL ); 																
}


//-------------------------------------
// Bind a timestamp column
// Use precision 16 scale 0 for "yyyy-mm-dd hh:mm"
// Use precision 23 scale 3 for "yyyy-mm-dd hh:mm:ss.fff"
//
RETCODE vdbStatement::Parameter( UWORD col, DBTIMESTAMP& t, UDWORD precision, SWORD scale )
{
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, precision, scale, &t, 0, NULL );
}


//-------------------------------------
// Bind a timestamp column
// Use precision 16 scale 0 for "yyyy-mm-dd hh:mm"
// Use precision 23 scale 3 for "yyyy-mm-dd hh:mm:ss.fff"
//
RETCODE vdbStatement::Parameter( UWORD col, vdbTimeStampDB& t )
{
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, t.TotalLength(), t.FractionalDigits(), t.GetDataPointer(), 0, NULL );
}


//-------------------------------------
// Bind an object identifier column
RETCODE vdbStatement::Parameter( UWORD col, OID& i )
{
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &i, 0, NULL ); 																
}


//-------------------------------------
// Bind a fixed length binary object
RETCODE vdbStatement::ParameterBinary( UWORD col, void* p, UWORD actualLength )
{
	static long length;
	length = actualLength;
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, actualLength, 0, p, length, &length );
}


//-------------------------------------
// Bind a variable length binary object
RETCODE vdbStatement::ParameterVarBinary( UWORD col, void* p, UWORD maxLength, SDWORD* actualLength )
{
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, maxLength, 0, p, *actualLength, actualLength );
}


//-------------------------------------
// Bind a long variable length binary object
RETCODE vdbStatement::ParameterLongBinary( UWORD col, vdbBinaryLong& image )
{
	static long length;
	length = image.ActualLength();
	return BindParameter( col, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_LONGVARBINARY, length, 0, image.GetDataPointer(), length, &length );
}


//-------------------------------------
RETCODE vdbStatement::ParamData( PTR* prgbValue )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLParamData( _hstmt, prgbValue );
	return stmtException( SQL_API_SQLPARAMDATA );
}


//-------------------------------------
RETCODE vdbStatement::ParamOptions( UDWORD crow, UDWORD* pirow )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLParamOptions( _hstmt, crow, pirow );
	return stmtException( SQL_API_SQLPARAMOPTIONS );
}


//-------------------------------------
RETCODE vdbStatement::Prepare( LPCSTR szSqlStr, SDWORD cbSqlStr )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLPrepare( _hstmt, ( UCHAR* ) szSqlStr, cbSqlStr );
	return stmtException( SQL_API_SQLPREPARE );
}


//-------------------------------------
RETCODE vdbStatement::PrimaryKeys( LPCSTR szTableQualifier, LPCSTR szTableOwner, LPCSTR szTableName )
{
	return PrimaryKeys( szTableQualifier, SQL_NTS, szTableOwner, SQL_NTS, szTableName, SQL_NTS );
}


//-------------------------------------
RETCODE vdbStatement::PrimaryKeys( LPCSTR szTableQualifier, SWORD cbTableQualifier, LPCSTR szTableOwner, SWORD cbTableOwner, LPCSTR szTableName, SWORD cbTableName )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLPrimaryKeys( _hstmt, ( UCHAR* ) szTableQualifier, cbTableQualifier, ( UCHAR* ) szTableOwner, cbTableOwner, ( UCHAR* ) szTableName, cbTableName );
	return stmtException( SQL_API_SQLPRIMARYKEYS );
}


/*  Dec-01-2009

//-------------------------------------
RETCODE vdbStatement::ProcedureColumns( LPCSTR szProcQualifier, LPCSTR  szProcOwner,	LPCSTR szProcName, LPCSTR szColumnName )
{
	return ProcedureColumns( szProcQualifier, szProcOwner, szProcName, szColumnName );
}
*/

//-------------------------------------
RETCODE vdbStatement::ProcedureColumns( LPCSTR szProcQualifier, SWORD cbProcQualifier, LPCSTR szProcOwner, SWORD cbProcOwner, LPCSTR szProcName, SWORD cbProcName, LPCSTR szColumnName, SWORD cbColumnName )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLProcedureColumns( _hstmt, ( UCHAR* ) szProcQualifier, cbProcQualifier, ( UCHAR* ) szProcOwner, cbProcOwner, ( UCHAR* ) szProcName, cbProcName, ( UCHAR* ) szColumnName, cbColumnName );
	return stmtException( SQL_API_SQLPROCEDURECOLUMNS );
}


//-------------------------------------
RETCODE vdbStatement::Procedures( LPCSTR szProcQualifier,LPCSTR szProcOwner, LPCSTR szProcName )
{
	return Procedures( szProcQualifier, SQL_NTS,	szProcOwner, SQL_NTS, szProcName, SQL_NTS );
}


//-------------------------------------
RETCODE vdbStatement::Procedures( LPCSTR szProcQualifier,	SWORD cbProcQualifier, LPCSTR szProcOwner, SWORD cbProcOwner, LPCSTR szProcName, SWORD cbProcName )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLProcedures( _hstmt,	( UCHAR* ) szProcQualifier, cbProcQualifier, ( UCHAR* ) szProcOwner, cbProcOwner, ( UCHAR* ) szProcName, cbProcName );
	return stmtException( SQL_API_SQLPROCEDURES );
}


//-------------------------------------
RETCODE vdbStatement::PutData( PTR rgbValue, SDWORD cbValue )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLPutData( _hstmt, rgbValue, cbValue );
	return stmtException( SQL_API_SQLPUTDATA );
}


//-------------------------------------
RETCODE vdbStatement::ResetParams( )
{
	return FreeStmt( SQL_RESET_PARAMS );
}


//-------------------------------------
RETCODE vdbStatement::RowCount( SDWORD* pcrow )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLRowCount( _hstmt, pcrow );
	return stmtException( SQL_API_SQLROWCOUNT );
}


//-------------------------------------
RETCODE vdbStatement::SetCursorName( LPCSTR szCursor, SWORD cbCursor )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLSetCursorName( _hstmt, ( UCHAR* ) szCursor, cbCursor );
	return stmtException( SQL_API_SQLSETCURSORNAME );
}


//-------------------------------------
RETCODE vdbStatement::SetPos( UWORD irow, BOOL fRefresh, BOOL fLock )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLSetPos( _hstmt, irow, fRefresh, fLock );
	return stmtException( SQL_API_SQLSETPOS );
}


//-------------------------------------
RETCODE vdbStatement::SetScrollOptions( UWORD fConcurrency, SDWORD crowKeyset, UWORD crowRowset )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLSetScrollOptions( _hstmt, fConcurrency,	crowKeyset, crowRowset );
	return stmtException( SQL_API_SQLSETSCROLLOPTIONS );
}


//-------------------------------------
RETCODE vdbStatement::SetStmtOption( UWORD fOption, UDWORD vParam )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLSetStmtOption( _hstmt, fOption, vParam );
	return stmtException( SQL_API_SQLSETSTMTOPTION );
}


//-------------------------------------
RETCODE vdbStatement::SpecialColumns( UWORD fColType, LPCSTR szTableQualifier, LPCSTR szTableOwner, LPCSTR szTableName, UWORD fScope, UWORD fNullable )
{
	return SpecialColumns( fColType, szTableQualifier, SQL_NTS, szTableOwner, SQL_NTS, szTableName, SQL_NTS,	fScope, fNullable );
}


//-------------------------------------
RETCODE vdbStatement::SpecialColumns( UWORD fColType, LPCSTR szTableQualifier, SWORD cbTableQualifier, LPCSTR szTableOwner, SWORD cbTableOwner, LPCSTR szTableName, SWORD cbTableName, UWORD fScope, UWORD fNullable )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLSpecialColumns( _hstmt, fColType, ( UCHAR* ) szTableQualifier, cbTableQualifier, ( UCHAR* ) szTableOwner, cbTableOwner, ( UCHAR* ) szTableName, cbTableName, fScope, fNullable );
	return stmtException( SQL_API_SQLSPECIALCOLUMNS );
}


//-------------------------------------
RETCODE vdbStatement::Statistics( LPCSTR  szTableQualifier, LPCSTR szTableOwner, LPCSTR szTableName, UWORD fUnique, UWORD fAccuracy )
{
	return Statistics( szTableQualifier, SQL_NTS, szTableOwner, SQL_NTS,	szTableName, SQL_NTS, fUnique, fAccuracy );
}


//-------------------------------------
RETCODE vdbStatement::Statistics( LPCSTR  szTableQualifier, SWORD  cbTableQualifier, LPCSTR szTableOwner,	SWORD cbTableOwner, LPCSTR szTableName, SWORD  cbTableName,	UWORD  fUnique,	UWORD  fAccuracy )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLStatistics( _hstmt,	( UCHAR* ) szTableQualifier, cbTableQualifier, ( UCHAR* ) szTableOwner, cbTableOwner, ( UCHAR* ) szTableName, cbTableName, fUnique, fAccuracy );
	return stmtException( SQL_API_SQLSTATISTICS );
}


//-------------------------------------
RETCODE vdbStatement::TablePrivileges( LPCSTR szTableQualifier, LPCSTR szTableOwner, LPCSTR szTableName )
{
	return TablePrivileges( szTableQualifier, SQL_NTS, szTableOwner, SQL_NTS, szTableName, SQL_NTS );
}


//-------------------------------------
RETCODE vdbStatement::TablePrivileges( LPCSTR szTableQualifier, SWORD cbTableQualifier, LPCSTR szTableOwner, SWORD  cbTableOwner,	LPCSTR szTableName,	SWORD  cbTableName )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLTablePrivileges( _hstmt, ( UCHAR* ) szTableQualifier, cbTableQualifier, ( UCHAR* ) szTableOwner, cbTableOwner, ( UCHAR* ) szTableName, cbTableName );
	return stmtException( SQL_API_SQLTABLEPRIVILEGES );
}


//-------------------------------------
RETCODE vdbStatement::Tables( LPCSTR szTableQualifier, LPCSTR szTableOwner, LPCSTR szTableName, LPCSTR szTableType )
{
	return Tables( szTableQualifier, SQL_NTS, szTableOwner, SQL_NTS, szTableName, SQL_NTS, szTableType, SQL_NTS );
}


//-------------------------------------
RETCODE vdbStatement::Tables( LPCSTR szTableQualifier, SWORD cbTableQualifier, LPCSTR szTableOwner, SWORD cbTableOwner, LPCSTR szTableName, SWORD  cbTableName, LPCSTR szTableType, SWORD  cbTableType )
{
	assert( _hstmt != SQL_NULL_HSTMT );
	_rc = ::SQLTables( _hstmt, ( UCHAR* ) szTableQualifier, cbTableQualifier, ( UCHAR* ) szTableOwner, cbTableOwner, ( UCHAR* ) szTableName, cbTableName, ( UCHAR* ) szTableType, cbTableType );
	return stmtException( SQL_API_SQLTABLES );
}


//-------------------------------------
RETCODE vdbStatement::Unbind( )
{
	return FreeStmt( SQL_UNBIND );
}


//==========================================================================================
//  protected functions
//==========================================================================================

//-------------------------------------
RETCODE vdbStatement::stmtException( int functionNumber )
{
	if ( _rc == SQL_SUCCESS || _rc == SQL_NO_DATA_FOUND || _rc == SQL_NEED_DATA )
		return _rc;

	else if ( _rc == SQL_SUCCESS_WITH_INFO )
	{
		Error( functionNumber );
		return _rc;
	}
	
	else
	{
		Error( functionNumber );
		return _rc;
	}
}



