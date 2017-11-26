//=============================================================================
//
// FILE NAME:       vdbMicrosoftSQLServer.h
//
// ESSENCE:         vdbDatabase mix in functions specific to Microsoft SQL Server 4.21
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    3/28/95
//
// COPYRIGHT:       Copyright © 1995 by Joe Honton
//
//=============================================================================

#ifndef __vdbMicrosoftSQLServer_h
#define __vdbMicrosoftSQLServer_h

#include "vdbServer.h"
#include "vdbSysDevices.h"
#include "boolean.h"

class vdbTable;


//=============================================================================
class vdbMicrosoftSQLServer : public vdbServer 
{
public:
	// essential class functions
	vdbMicrosoftSQLServer( HANDLE_DATABASE hDB = 0 );						// basic constructor
	virtual ~vdbMicrosoftSQLServer();										// destructor

private:
	vdbMicrosoftSQLServer( const vdbMicrosoftSQLServer& rhs );				// copy constructor disabled
	vdbMicrosoftSQLServer& operator=( const vdbMicrosoftSQLServer& rhs );	// assignment operator disabled

public:
	// virtuals
	virtual const char* ColumnType( const char* szType );
	virtual RETCODE DropIndex( vdbTable* pTable, int which );
	virtual void AdminAuthentication( char* szUID, int sizeUID, char* szAuthStr, int sizeAuthStr );
	bool ServiceControlManager( const char* szService, int action );
	virtual bool IsEngineRunning();
	virtual bool StartEngine();
	virtual bool StopEngine();

public:
	RETCODE AddLogin( const char* szLoginID, const char* szPassword = NULL, const char* szDefaultDB = NULL );
	RETCODE DefaultDB( const char* szLoginID, const char* szDefaultDB );
	RETCODE AddUser( const char* szLoginID );
	RETCODE AddServerRoleMember( const char* szLoginID, const char* szRoleName );
	RETCODE BackupDatabase( const char* szDatabaseName, const char* szBackupFilename, bool bReplace = true );
	RETCODE ChangeOwner( const char* szDatabaseName, const char* szNewOwner );
	RETCODE CreateDatabase( const char* szDatabaseName, const char* szDirectory, unsigned int dataSizeInMegabytes, unsigned int logSizeInMegabytes );
	RETCODE DropDatabase( const char* szDatabaseName );
	RETCODE DumpTransaction( const char* szDatabaseName );
	RETCODE GrantAll( const char* szLoginID );
	RETCODE RestoreFilelist( const char* szBackupFilename, vdbString& sLogicalDataName, vdbString& sLogicalLogName );
	RETCODE RestoreDatabase( const char* szBackupFilename, const char* szNewDatabaseName, const char* szNewDirectory );
	RETCODE AttachDatabase( const char* szDataFilename, const char* szLogFilename, const char* szDatabaseName );
	RETCODE TruncateLogOnCheckpoint( const char* szDatabaseName );
	RETCODE Use( const char* szDatabaseName );
	bool DatabaseExists( const char* szDatabaseName );
};

// SQLSCMLocalServiceControl is in w95scm.dll
#pragma comment(linker,"/defaultlib:w95scm.lib")


#endif // __vdbMicrosoftSQLServer_h
