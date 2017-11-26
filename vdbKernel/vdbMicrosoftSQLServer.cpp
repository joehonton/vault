//=============================================================================
//
// CLASS:           vdbMicrosoftSQLServer
//
// ESSENCE:         vdbDatabase mix in functions specific to
//                  Microsoft SQL Server 4.21
//
// FILE NAME:       vdbMicrosoftSQLServer.cpp
//
// INITIAL DATE:    3/28/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright � 1995 by Joe Honton
//
//=============================================================================

#ifdef USE_PRE_HDR
	#include "vdbHeader.h"
#else
	#include "vdbMessageBox.h"
	#include "vdbStatement.h"
	#include "vdbDatabase.h"
	#include "vdbUtility.h"
	#include "vdbUnique.h"
	#include "vdbVersion.h"
	#include "vdbQuery.h"
#endif

#include <strstream>
#include "vdbMicrosoftSQLServer.h"
#include "vdbException.h"
#include "vdbFilename.h"
#include "wn95scm.h"			// SQL Server Service Control Manager
#include "vdbStatusWindow.h"
#include <vdbApp.h>
#include <vdbRegistrationKey.h>


//=============================================================================
// essential class functions
//=============================================================================

//-------------------------------------------------
//^^ Constructor for vdbMicrosoftSQLServer
vdbMicrosoftSQLServer::vdbMicrosoftSQLServer( HANDLE_DATABASE hDB )
	: vdbServer( hDB )
{
}


//-------------------------------------------------
//^^ Destructor for vdbMicrosoftSQLServer
vdbMicrosoftSQLServer::~vdbMicrosoftSQLServer()
{
}


//=============================================================================
//  required virtual functions
//=============================================================================

//-------------------------------------------------
//^ The ColumnType function returns column types suitable for use in table creation
//
const char* vdbMicrosoftSQLServer::ColumnType( const char* szType )
{
	if ( !strcmp( szType, "OID" ) )
		return "int";
	else if ( !strcmp( szType, "DBBIT" ) )
		return "bit";
	else if ( !strcmp( szType, "DBBYTE" ) )
		return "tinyint";
	else if ( !strcmp( szType, "DBSMALL" ) )
		return "smallint";
	else if ( !strcmp( szType, "DBINT" ) )
		return "int";
	else if ( !strcmp( szType, "DBFLOAT" ) )
		return "real";
	else if ( !strcmp( szType, "DBDOUBLE" ) )
		return "float";
	else if ( !strcmp( szType, "DBTIMESTAMP" ) )
		return "datetime";
	else if ( !strncmp( szType, "decimal", 7 ) )
		return szType;
	else if ( !strncmp( szType, "char", 4 ) )
		return szType;
	else if ( !strncmp( szType, "varchar", 7 ) )
		return szType;
	else if ( !strncmp( szType, "longvarchar", 11 ) )
		return "text";
	else if ( !strncmp( szType, "binary", 6 ) )
		return szType;
	else if ( !strncmp( szType, "varbinary", 9 ) )
		return szType;
	else if ( !strncmp( szType, "longvarbinary", 13 ) )
		return "image";
	else
	{		
		GetDatabase()->Trouble( "unidentified column type in vdbMicrosoftSQLServer::ColumnType()" ); 
		return "";
	}

}


//-------------------------------------------------
// Drops an index, using table name prequalifiers
//
RETCODE vdbMicrosoftSQLServer::DropIndex( vdbTable* pTable, int which )
{
    // assemble the SQL statement
	int size = 11 + strlen( pTable->FullyQualifiedTableName() ) + 1 + strlen( pTable->Index(which).IndexName() ) + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
 	os << "DROP INDEX " << pTable->FullyQualifiedTableName() << "." << pTable->Index(which).IndexName() << std::ends;
	
	// bind the parameter values and execute																					
	vdbStatement stmt( GetDatabase() );
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;

	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}


//--------------------------------
//^ The AdminAuthentication function obtains the default administrator userID and password.
//
//> szUID is the default administrator ID.
//> sizeUID is the maximum size of the string used for szUID.
//> szAuthStr is the password of the default administrator ID
//> sizeAuthStr is the maximum size of the string used for szAuthStr.
//
void vdbMicrosoftSQLServer::AdminAuthentication( char* szUID, int sizeUID, char* szAuthStr, int sizeAuthStr )
{
	vdbUtility::SafeCopy( szUID, "sa", sizeUID );
	vdbUtility::SafeCopy( szAuthStr, "sa", sizeAuthStr );
}


//-------------------------------------
//^ The ServiceControlManager function
//> szService is "MSSQLServer" or something like "MSSQL$CompleatBotanica"
//> action 1 = start
//         3 = is running
//         6 = stop
//< true on success
bool vdbMicrosoftSQLServer::ServiceControlManager( const char* szService, int action )
{
	int scmVersion = 0;
	vdbFilename sLocation;
	vdbRegistrationKey key( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Microsoft SQL Server\\80\\Tools\\ClientSetup", "SQLPath" );
	if ( key.GetValue( sLocation ) == false )
	{
		vdbRegistrationKey key2( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\MSSQLServer\\Setup", "SQLPath" );	// version 7.0
		if ( key2.GetValue( sLocation ) == false )
		{
			vdbTextBox( "Unable to locate the SQL Server Desktop Engine.  The Desktop Engine must be reinstalled.", MB_OK );
			return false;
		}
		else
			scmVersion = 7;
	}
	else
		scmVersion = 8;

	sLocation.Append( "Binn\\SCM.exe" );

	// start the SQL Server service
	vdbString sFileWithOptions;
	sFileWithOptions.Format( "\"%s\" -Action %d -Silent 1 -Service %s", (const char*) sLocation, action, szService );

	char szFileWithOptions[MAX_PATH + 40];
	vdbUtility::SafeCopy( szFileWithOptions, sFileWithOptions, sizeof(szFileWithOptions) );

	STARTUPINFO si;
	memset( &si, 0, sizeof(si) );
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	memset( &pi, 0, sizeof(pi) );

	if ( CreateProcess( NULL, szFileWithOptions, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi ) == 0 )
		return false;

	// wait until finished starting
	WaitForSingleObject( pi.hProcess, INFINITE );

	DWORD exitCode;
	GetExitCodeProcess( pi.hProcess, &exitCode );

	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	
	if ( action == 3 )	// status
	{
		if ( scmVersion == 7 )
		{
			if ( exitCode == 0 )
				return true;
			else // ( exitCode == -1 )
				return false;
		}
		else // if ( scmVersion == 8 )
		{
			if ( exitCode == 1056 )
				return true;
			else // ( exitCode == 1062 )
				return false;
		}
	}
	
	else		// start and stop
	{
		if ( exitCode == 0 )
			return true;
		else
			return false;
	}
}


//-------------------------------------
bool vdbMicrosoftSQLServer::IsEngineRunning()
{
	vdbString& sDesktopInstance = GetDatabase()->_defaults._sDesktopInstance;
	if ( sDesktopInstance.IsNull() )
	{
		if ( ServiceControlManager( "MSSQLServer", 3 ) == true )
			return true;
		else
			return false;
	}
	else
	{
		vdbString s;
		s.Format( "MSSQL$%s", (const char*)  sDesktopInstance );
		char sz[30];
		vdbUtility::SafeCopy( sz, s, sizeof(sz) );
		if ( ServiceControlManager( sz, 3 ) == true )
			return true;
		else
			return false;
	}
}


//-------------------------------------
bool vdbMicrosoftSQLServer::StartEngine()
{
	if ( IsEngineRunning() )
		return true;
	else
	{
		bool b = false;
		vdbStatusWindow* pStatus = new vdbStatusWindow( vdbApp::GetMessageTitle() );
		pStatus->ShowWindow( SW_SHOW );
		pStatus->Message( "One moment please.\n\nStarting SQL Server Desktop Engine. . ." );

		vdbString& sDesktopInstance = GetDatabase()->_defaults._sDesktopInstance;
		if ( sDesktopInstance.IsNull() )
		{
			if ( ServiceControlManager( "MSSQLServer", 1 ) == true )
			{
				Sleep( 5000 );
				b = true;
			}
			else
				b = false;
		}
		else
		{
			vdbString s;
			s.Format( "MSSQL$%s", (const char*)  sDesktopInstance );
			char sz[30];
			vdbUtility::SafeCopy( sz, s, sizeof(sz) );

			if ( ServiceControlManager( sz, 1 ) == true )
			{
				Sleep( 5000 );
				b = true;
			}
			else
				b = false;
		}

		pStatus->DestroyWindow();
		delete pStatus; pStatus = 0;
		return b;
	}
}


//-------------------------------------
bool vdbMicrosoftSQLServer::StopEngine()
{
	if ( !IsEngineRunning() )
		return true;
	else
	{
		vdbString& sDesktopInstance = GetDatabase()->_defaults._sDesktopInstance;
		if ( sDesktopInstance.IsNull() )
			return ServiceControlManager( "MSSQLServer", 6 );
		else
		{
			vdbString s;
			s.Format( "MSSQL$%s", (const char*)  sDesktopInstance );
			char sz[30];
			vdbUtility::SafeCopy( sz, s, sizeof(sz) );
			return ServiceControlManager( sz, 6 );
		}
	}
}


//=============================================================================
//  public functions
//=============================================================================

//--------------------------------
// "sp_addlogin szLoginID, szPassword, szDefaultDB"
// allows a user to connect to SQL Server
//
RETCODE vdbMicrosoftSQLServer::AddLogin( const char* szLoginID, const char* szPassword, const char* szDefaultDB )
{
    // assemble the SQL statement
	int size = 12 + strlen(szLoginID);
	if ( szPassword != 0 )
		size = size + 2 + strlen(szPassword);
	if ( szDefaultDB != 0 )
		size = size + 2 + strlen(szDefaultDB);
	size += 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "sp_addlogin " << szLoginID;
	if ( szPassword != 0 )
	{
		os << ", " << szPassword;
		if ( szDefaultDB != 0 )
			os << ", " << szDefaultDB;
    }
	os << std::ends;

	// execute statement
	vdbStatement stmt( GetDatabase() );																
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;    

	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}


//--------------------------------
// "sp_defaultdb szLoginID, szDefaultDB"
// changes a user's default database
//
RETCODE vdbMicrosoftSQLServer::DefaultDB( const char* szLoginID, const char* szDefaultDB )
{
    // assemble the SQL statement
	int size = 13 + strlen(szLoginID) + 2 + strlen(szDefaultDB) + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "sp_defaultdb " << szLoginID << ", " << szDefaultDB;
	os << std::ends;

	// execute statement
	vdbStatement stmt( GetDatabase() );																
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;    

	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}


//--------------------------------
// "sp_adduser szLoginID"
//  Allows a user to connect to the current database
//
RETCODE vdbMicrosoftSQLServer::AddUser( const char* szLoginID )
{
    // assemble the SQL statement
	int size = 11 + strlen(szLoginID) + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "sp_adduser " << szLoginID << std::ends;

	// execute statement
	vdbStatement stmt( GetDatabase() );																
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;   

	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}


//-------------------------------------------------
//^ The AddServerRoleMember function adds a fixed system role to the specified user
//^ "sp_addsrvrolemember szLoginID, szRoleName"
//> szLoginID is the login to modify
//> szRoleName is usually "sysadmin"
//
RETCODE vdbMicrosoftSQLServer::AddServerRoleMember( const char* szLoginID, const char* szRoleName )
{
    // assemble the SQL statement
	int size = 20 + strlen(szLoginID) + 2 + strlen(szRoleName) + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "sp_addsrvrolemember " << szLoginID << ", " << szRoleName << std::ends;

	// execute statement
	vdbStatement stmt( GetDatabase() );																
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;   

	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}


//-------------------------------------------------
//^ The BackupDatabase function backs up a database
//  The database can be opened and in use during backup.
//> szDatabaseName is the name of the database.
//> szBackupFilename is the full drive, directory, filename, and extension to use
//> bReplace is true to replace any existing file with the same name
//< returns a database error on failure
//
RETCODE vdbMicrosoftSQLServer::BackupDatabase( const char* szDatabaseName, const char* szBackupFilename, bool bReplace )
{
	assert( szDatabaseName != 0 );
	assert( szBackupFilename != 0 );

	vdbFilename sFilename( szBackupFilename );
	sFilename.ReplaceInvalidCharacters();

	// delete any previous backup with the same name
	if ( sFilename.Exists() )
	{
		if ( bReplace == false )
		{
			vdbString s;
			s.Format( "The file \"%s\" already exists.  Do you want to replace it with this new backup?", (const char*) sFilename );
			if ( vdbMessageBox( NULL, s, "Backup database", MB_YESNO ) == IDNO )
				return DBS_FAILED;
		}

		// delete the existing backup even if it is read-only
		if ( sFilename.Delete( true ) == false )
		{
			vdbString s;
			s.Format( "The file \"%s\" already exists and cannot be deleted.  Choose a different filename, or manually delete the file andt try again.", (const char*) sFilename );
			vdbMessageBox( NULL, s, "Backup database", MB_OK );
			return DBS_FAILED;
		}
	}

	// create the directory if it doesn't exist
	else
		sFilename.CreateDir();

    // assemble the SQL statement
	std::ostrstream os;
	os << "BACKUP DATABASE " << szDatabaseName;
	os << " TO DISK = '" << (const char*) sFilename << "'";
	os << std::ends;
	char* sql = os.str();
	if ( sql == 0 ) throw vdbMemoryException();
	    
	// execute statement
	vdbStatement stmt( GetDatabase() );																
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;    

	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}


//-------------------------------------------------
//^ The ChangeOwner function changes the owner of the database
//> szDatabaseName is the new name of the database.
//> szNewOwner is the name of the owner for the restored database
//< returns a database error on failure
//
RETCODE vdbMicrosoftSQLServer::ChangeOwner( const char* szDatabaseName, const char* szNewOwner )
{
	assert( szDatabaseName != 0 );
	assert( szNewOwner != 0 );

	char* sql;
	RETCODE rc;
	vdbStatement stmt( GetDatabase() );																

	// Use the new database
	std::ostrstream os3;
	os3 << "USE " << szDatabaseName << std::ends;
	sql = os3.str();
	if ( sql == 0 )
		throw vdbMemoryException();
	rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;    
	if ( rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO )
		return rc;

	// change the owner of the new database
	std::ostrstream os4;
	os4 << "sp_changedbowner '" << szNewOwner << "'" << std::ends;
	sql = os4.str();
	if ( sql == 0 )
		throw vdbMemoryException();
	rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;    

	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}


//-------------------------------------------------
//^ Creates a new database using SQL Serer version 7.0 syntax
//> szDatabaseName is the name of the database.
//> szDirectory is the location where the data and log devices will be placed
//> dataSizeInMegabytes
//> logSizeInMegabytes
//< returns a database error on failure
//
RETCODE vdbMicrosoftSQLServer::CreateDatabase( const char* szDatabaseName, const char* szDirectory, unsigned int dataSizeInMegabytes, unsigned int logSizeInMegabytes )
{
	assert( dataSizeInMegabytes > 0 );
	assert( logSizeInMegabytes > 0 );

	// use the vdbFilename class to easily remove troublesome stuff
	vdbFilename sDatabaseName( szDatabaseName );
	sDatabaseName.ReplaceInvalidCharacters();

	vdbString sDataName( sDatabaseName );
	sDataName += "Data";

	vdbString sLogName( sDatabaseName );
	sLogName += "Log";

	// create the directory for the data and log files
	vdbFilename sDirectory( szDirectory );
	sDirectory.ReplaceInvalidCharacters();
	sDirectory.CreateDir();

	vdbFilename sDataFile( sDirectory );
	sDataFile.Append( sDataName );
	sDataFile.Extension( "mdf" );
	if ( sDataFile.Exists() )
	{
		vdbString s;
		s.Format( "The new database \"%s\" will place it's data in the file \"%s\".\n\nThis file already exists.  Do you want to delete the existing file?", (const char*) sDatabaseName, (const char*) sDataFile );
		if ( vdbMessageBox( NULL, s, "Create database", MB_YESNO ) == IDYES )
		{
			if ( sDataFile.Delete() == false )
			{
				vdbMessageBox( NULL, "The file could not be deleted.\n\nDatabase not created.", "Create database", MB_OK );
				return DBS_FAILED;
			}
		}
		else
			return DBS_FAILED;
	}

	vdbFilename sLogFile( sDirectory );
	sLogFile.Append( sLogName );
	sLogFile.Extension( "ldf" );
	if ( sLogFile.Exists() )
	{
		vdbString s;
		s.Format( "The new database \"%s\" will place it's transaction log in the file \"%s\".\n\nThis file already exists.  Do you want to delete the existing file?", (const char*) sDatabaseName, (const char*) sLogFile );
		if ( vdbMessageBox( NULL, s, "Create database", MB_YESNO ) == IDYES )
		{
			if ( sLogFile.Delete() == false )
			{
				vdbMessageBox( NULL, "The file could not be deleted.\n\nDatabase not created.", "Create database", MB_OK );
				return DBS_FAILED;
			}
		}
		else
			return DBS_FAILED;
	}

    // assemble the SQL statement
	std::ostrstream os;
	os << "CREATE DATABASE " << (const char*) sDatabaseName;
	
	os << " ON (NAME = '" << (const char*) sDataName << "',";
	os << " FILENAME = '" << (const char*) sDataFile << "',";
	os << " SIZE = " << dataSizeInMegabytes << ")";
	
	os << " LOG ON (NAME = '" << (const char*) sLogName << "',";
	os << " FILENAME = '" << (const char*) sLogFile << "',";
	os << " SIZE = " << logSizeInMegabytes << ")";

	os << std::ends;
	char* sql = os.str();
	if ( sql == 0 ) throw vdbMemoryException();
	    
	// execute statement
	vdbStatement stmt( GetDatabase() );
	RETCODE rc = stmt.Execute( sql );
	delete[] sql; sql = 0;

	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}

  
//-------------------------------------------------
//^ The DropDatabase function drops a database and deletes the data and log device
//  Be sure to USE( "master" ) before calling this function
//
RETCODE vdbMicrosoftSQLServer::DropDatabase( const char* szDatabaseName )
{
    // assemble the SQL statement
	int size = 14 + strlen(szDatabaseName) + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "DROP DATABASE " << szDatabaseName << std::ends;
    
	// execute statement
	vdbStatement stmt( GetDatabase() );																
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;    

	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}


//-------------------------------------------------
//^ The DumpTransaction function removes the inactive part 
//  of the transaction using the WITH NO_LOG option.
//  This is used if you have run out of space in the database.
//  After the transaction log has been dumped the changes
//  recorded in the log are not recoverable.
//
RETCODE vdbMicrosoftSQLServer::DumpTransaction( const char* szDatabaseName )
{
    // assemble the SQL statement
	int size = 17 + strlen(szDatabaseName) + 12 + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "DUMP TRANSACTION " << szDatabaseName << " WITH NO_LOG" << std::ends;
    
	// execute statement
	vdbStatement stmt( GetDatabase() );																
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;

	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}


//--------------------------------
// "grant all to qsar"
//
RETCODE vdbMicrosoftSQLServer::GrantAll( const char* szLoginID )
{
    // assemble the SQL statement
	int size = 13 + strlen(szLoginID) + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "GRANT ALL TO " << szLoginID;
	os << std::ends;
    
	// execute statement
	vdbStatement stmt( GetDatabase() );																
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;

	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}


//-------------------------------------------------
//^ The RestoreFilelist function gets the names of the data and log devices
//  embedded in a backup file
//> szBackupFilename is the file to examine
//> sLogicalDataName is the internal name of the data device for the backup
//> sLogicalLogName is the internal name of the log device for the backup
//
RETCODE vdbMicrosoftSQLServer::RestoreFilelist( const char* szBackupFilename, vdbString& sLogicalDataName, vdbString& sLogicalLogName )
{
	assert( szBackupFilename != 0 );

	// make sure the backup file exists
	vdbFilename sBackupFilename( szBackupFilename );
	if ( !sBackupFilename.Exists() )
		return DBS_FAILED;
	
	// get the logical names of the data and log devices in the backup file
	char szLogicalDataName[256];
	char szLogicalLogName[256];
	
	std::ostrstream os;
	os << "RESTORE FILELISTONLY FROM DISK = '";
	os << szBackupFilename << "'";
	os << std::ends;
	char* sql = os.str();
	if ( sql == 0 )
		throw vdbMemoryException();
	vdbStatement stmt( GetDatabase() );																
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;    
	if ( rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO )
		return rc;
	else
	{
		// This should return exactly two names, each in its own row
		// One of them should end in 'Data' and one should end in 'Log' but it doesn't
		// really matter as long as we have exactly two names.
		stmt.Bind( 1, szLogicalDataName, sizeof(szLogicalDataName) );
		rc = stmt.Fetch();
		if ( rc != SQL_SUCCESS )
			return rc;

		stmt.Bind( 1, szLogicalLogName, sizeof(szLogicalLogName) );
		rc = stmt.Fetch();
		if ( rc != SQL_SUCCESS )
			return rc;
	}

	sLogicalDataName = szLogicalDataName;
	sLogicalLogName = szLogicalLogName;
	
	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}


//-------------------------------------------------
//^ The RestoreDatabase function restores a database from backup.
//  Note that no one can be using the database.  Thus you should connect using
//  the system administrator id for the duration of the process.
//> szBackupFilename is the location of the backup file to restore
//> szNewDatabaseName is the new name of the database.
//> szNewDirectory is the new location for the database devicefiles
//< returns a database error on failure
//
RETCODE vdbMicrosoftSQLServer::RestoreDatabase( const char* szBackupFilename, const char* szNewDatabaseName, const char* szNewDirectory )
{
	assert( szBackupFilename != 0 );
	assert( szNewDatabaseName != 0 );
	assert( szNewDirectory != 0 );

	char* sql;
	RETCODE rc;

	// make sure the backup file exists
	vdbFilename sBackupFilename( szBackupFilename );
	if ( !sBackupFilename.Exists() )
		return DBS_FAILED;

	// It's possible for DatabaseExists to return false when a database actually
	// exists and is marked as suspect.  If that happens, the caller might end up
	// here and the rest of this will fail.  As a precaution, just drop any
	// possible suspect database now.
	DropDatabase( szNewDatabaseName ); 

	// get the logical data and log names embedded in the backup file
	vdbString sLogicalDataName;
	vdbString sLogicalLogName;
	
	rc = RestoreFilelist( szBackupFilename, sLogicalDataName, sLogicalLogName );
	if ( rc != SQL_SUCCESS )
		return rc;

	// Assemble the data and log file names
	vdbFilename sDatabaseName( szNewDatabaseName );
	sDatabaseName.ReplaceInvalidCharacters();
	vdbString sDataName( sDatabaseName );
	sDataName += "Data";
	vdbString sLogName( sDatabaseName );
	sLogName += "Log";

	// create the directory for the restored data and log files
	vdbFilename sDirectory( szNewDirectory );
	sDirectory.CreateDir();

	vdbFilename sDataFile( sDirectory );
	sDataFile.Append( sDataName );
	sDataFile.Extension( "mdf" );
	if ( sDataFile.Exists() )
	{
		vdbString s;
		s.Format( "The database \"%s\" will place it's DATA in the file \"%s\".\n\nThis file already exists.  Do you want to replace the existing file?", (const char*) sDatabaseName, (const char*) sDataFile );
		if ( vdbMessageBox( NULL, s, "Restore database", MB_YESNO ) == IDYES )
		{
			if ( sDataFile.Delete( true ) == false )
			{
				vdbMessageBox( NULL, "The file could not be deleted.\n\nDatabase not restored.", "Restore database", MB_OK );
				return DBS_FAILED;
			}
		}
		else
			return DBS_FAILED;
	}

	vdbFilename sLogFile( sDirectory );
	sLogFile.Append( sLogName );
	sLogFile.Extension( "ldf" );
	if ( sLogFile.Exists() )
	{
		vdbString s;
		s.Format( "The database \"%s\" will place it's TRANSACTION LOG in the file \"%s\".\n\nThis file already exists.  Do you want to replace the existing file?", (const char*) sDatabaseName, (const char*) sLogFile );
		if ( vdbMessageBox( NULL, s, "Create database", MB_YESNO ) == IDYES )
		{
			if ( sLogFile.Delete( true ) == false )
			{
				vdbMessageBox( NULL, "The file could not be deleted.\n\nDatabase not restored.", "Restore database", MB_OK );
				return DBS_FAILED;
			}
		}
		else
			return DBS_FAILED;
	}

	// Restore the database
	std::ostrstream os;
	os << "RESTORE DATABASE " << szNewDatabaseName;
	os << " FROM DISK = '" << szBackupFilename << "'";
	os << " WITH RECOVERY, REPLACE,";
	os << " MOVE '" << (const char*) sLogicalDataName << "' TO '" << (const char*) sDataFile << "',";
	os << " MOVE '" << (const char*) sLogicalLogName << "' TO '" << (const char*) sLogFile << "'";
	os << std::ends;
	sql = os.str();
	if ( sql == 0 )
		throw vdbMemoryException();
	vdbStatement stmt( GetDatabase() );																
	rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;    

	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}


//--------------------------------
RETCODE vdbMicrosoftSQLServer::AttachDatabase( const char* szDataFilename, const char* szLogFilename, const char* szDatabaseName )
{
    // assemble the SQL statement
	std::ostrstream os;
	os << "sp_attach_db ";
	os << "'" << szDatabaseName << "', ";
	os << "'" << szDataFilename << "', ";
	os << "'" << szLogFilename << "'";
	os << std::ends;
	char* sql = os.str();
	if ( sql == 0 )
		throw vdbMemoryException();

	// execute statement
	vdbStatement stmt( GetDatabase() );																
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;

	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}


//--------------------------------
//^ The TruncateLogOnCheckpoint function issues this stored	procedure:
//  "sp_dboption szDatabaseName, trunc.log on chkpt, true".  Everytime the
//  CHECKPOINT processing occurs (which is about twice a minute), the roll
//  forward/recovery log is truncated so that all commited transactions are
//  removed from the transaction log.  This prevents the SYSLOGS table from
//  eventually overflowing.  Use this when you don't have a	system
//  administrator and you want to run without issuing DUMP TRANSACTION
//  periodically.
//^ You must be logged into the 'master' database to issue this.
//  If you make this change to the 'model' database, all new
//  databases will also have this option set.
//
RETCODE vdbMicrosoftSQLServer::TruncateLogOnCheckpoint( const char* szDatabaseName )
{
    // assemble the SQL statement
	int size = 12 + strlen(szDatabaseName) + 30 + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "sp_dboption " << szDatabaseName << ", 'trunc. log on chkpt.', true" << std::ends;

	// execute statement
	vdbStatement stmt( GetDatabase() );																
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;

	// now execute the CHECKPOINT command
	if ( rc == DBS_OK )
		rc = stmt.Execute( "CHECKPOINT" );

	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}


//-------------------------------------------------
//^ The Use function uses the specified database. If the user has connected
//  to a different database upon startup, vdbUnique's nextOID needs to be
//  saved, so this function will reinitialize the _nextOID and the version
//  cache.
//^ returns DBS_OK on success.
//
RETCODE vdbMicrosoftSQLServer::Use( const char* szDatabaseName )
{
	vdbDatabase* pDB = GetDatabase();

	// save the _nextOID in the existing vdbUnique table
	if ( pDB->TableExists( "vdbUnique" ) )
	{
		// reliquish the version cache
		pDB->VersionCache().ClearAll();

		// Save the nextOID/lastOID for the next session
		pDB->SaveCacheOfUniqueIDs();
		pDB->Commit();
	}

    // assemble the SQL statement
	int size = 4 + strlen(szDatabaseName) + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "USE " << szDatabaseName << std::ends;
    
	// execute statement
	vdbStatement stmt( pDB );																
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;

	// Does this new database have the necessary system tables?
	if ( pDB->TableExists( "vdbVersion" ) && pDB->TableExists( "vdbUnique" ) )
	{
		pDB->_hasSystemTables = true;

		// Fill the version cache
		vdbQuery<vdbVersion> q;
		q.Execute( pDB->VersionCache() );

		// Get a cache of unique OID's
		pDB->GetCacheOfUniqueIDs();
	}

	else
		pDB->_hasSystemTables = false;

	if ( rc == SQL_SUCCESS_WITH_INFO )
		return SQL_SUCCESS;
	else
		return rc;
}


//-------------------------------------------------
//^ The DatabaseExists function returns true if there is a database with
//  the given name
bool vdbMicrosoftSQLServer::DatabaseExists( const char* szDatabaseName )
{
	vdbDatabase* pDB = GetDatabase();

    // assemble the SQL statement
	int size = 10 + strlen(szDatabaseName) + 1;
	char* sql = new char[size];
	if ( sql == 0 ) throw vdbMemoryException();
	std::ostrstream os( sql, size );
	os << "sp_helpdb " << szDatabaseName << std::ends;
    
	// execute statement
	vdbStatement stmt( pDB );																
	RETCODE rc = stmt.Execute( sql );														
	delete[] sql; sql = 0;

	if ( rc == DBS_OK )
		return true;
	else
		return false;
}


