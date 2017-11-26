//=============================================================================
//
// CLASS:           vdbDatabaseNew
//
// ESSENCE:         Create a new ODBC data source and SQL Server Desktop Engine
//                  devices and tables
//
// FILE NAME:       vdbDatabaseNew.cpp
//
// INITIAL DATE:    3/6/00
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#include <vdbWindows.h>
#include "vdbDatabaseNew.h"
#include <vdbDatabase.h>
#include <vdbUtility.h>
#include <vdbFilename.h>
#include <ODBCinst.h>
#include "vdbStatusWindow.h"
#include <vdbMicrosoftSQLServer.h>
#include <vdbRegistrationKey.h>
#include <vdbAssert.h>
#include <vdbString.h>
#include <vdbDirectoryDialog.h>


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
vdbDatabaseNew::vdbDatabaseNew( vdbDatabase* pDB, CWnd* pParent )
	: vdbDLLDialog(vdbDatabaseNew::IDD, pParent),
	  _pDB( pDB )
{
	assert( _pDB != 0 );

	m_ctlSelectDirectory.SetBitmaps(IDB_VDB_DIRECTORY_UP, IDB_VDB_DIRECTORY_DOWN, IDB_VDB_DIRECTORY_FOCUS, IDB_VDB_DIRECTORY_DISABLED );

	//{{AFX_DATA_INIT(vdbDatabaseNew)
	//}}AFX_DATA_INIT
}


//-------------------------------------
vdbDatabaseNew::~vdbDatabaseNew()
{
}


//-------------------------------------
void vdbDatabaseNew::DoDataExchange(CDataExchange* pDX)
{
	vdbDLLDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(vdbDatabaseNew)
	DDX_Control(pDX, IDC_VDB_DATA_DIRECTORY, m_ctlDataDirectory);
	DDX_Control(pDX, IDC_VDB_LOG_SIZE, m_ctlLogSize);
	DDX_Control(pDX, IDC_VDB_AUTHORIZATION_PASSWORD, m_ctlAuthorizationPassword);
	DDX_Control(pDX, IDC_VDB_AUTHORIZATION_ID, m_ctlAuthorizationID);
	DDX_Control(pDX, IDC_VDB_DATA_SIZE, m_ctlDataSize);
	DDX_Control(pDX, IDC_VDB_DATABASE_NAME, m_ctlDatabaseName);
	DDX_Control(pDX, IDC_VDB_DESCRIPTION, m_ctlDescription);
	DDX_Control(pDX, IDC_VDB_DATASOURCE_NAME, m_ctlDataSourceName);
	DDX_Control(pDX, IDC_VDB_DIRECTORY, m_ctlSelectDirectory);
	//}}AFX_DATA_MAP
}


//-------------------------------------
BEGIN_MESSAGE_MAP(vdbDatabaseNew, vdbDLLDialog)
	//{{AFX_MSG_MAP(vdbDatabaseNew)
	ON_EN_CHANGE(IDC_VDB_DATASOURCE_NAME, OnChangeDataSourceName)
	ON_BN_CLICKED(IDC_VDB_DIRECTORY, OnSelectDirectory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



//=============================================================================
// Window creation and intialization
//=============================================================================

//-------------------------------------
BOOL vdbDatabaseNew::OnInitDialog() 
{
	vdbDLLDialog::OnInitDialog();
	
	m_ctlSelectDirectory.SizeToContent();

	m_ctlDataSourceName.SetLimitText( SQL_MAX_DSN_LENGTH );
	m_ctlDescription.SetLimitText( 40 );
	m_ctlDatabaseName.SetLimitText( 128 );
	m_ctlDataDirectory.SetLimitText( 120 );
	
	m_ctlDataSourceName.SetWindowText( _pDB->_defaults._sDataSourceName );
	m_ctlDescription.SetWindowText( _pDB->_defaults._sDescription );
	m_ctlDatabaseName.SetWindowText( _pDB->_defaults._sDatabaseName );
	m_ctlDataDirectory.SetWindowText( _pDB->_defaults._sDirectoryLocation );

	vdbString s;
	s.Format( "%d", _pDB->_defaults._dataSize );
	m_ctlDataSize.SetWindowText( s );
	s.Format( "%d", _pDB->_defaults._logSize );
	m_ctlLogSize.SetWindowText( s );

	m_ctlAuthorizationID.SetWindowText( _pDB->_defaults._sAuthorizationID );
	m_ctlAuthorizationPassword.SetWindowText( _pDB->_defaults._sAuthorizationPassword );
	if ( _pDB->_defaults._bEnableAuthorization == true )
	{
		m_ctlAuthorizationID.EnableWindow( TRUE );
		m_ctlAuthorizationPassword.EnableWindow( TRUE );
	}
	else
	{
		m_ctlAuthorizationID.EnableWindow( FALSE );
		m_ctlAuthorizationPassword.EnableWindow( FALSE );
	}
		
	return TRUE;
}


//=============================================================================
// User interface callbacks
//=============================================================================

//-------------------------------------
//^ The OnChangeDatasourceName function is triggered every time the data source
//  name control is modified.
//
void vdbDatabaseNew::OnChangeDataSourceName() 
{
	char szName[SQL_MAX_DSN_LENGTH];
	m_ctlDataSourceName.GetWindowText( szName, sizeof(szName) );

	vdbFilename sDB( szName );
	sDB += "_Database";
	sDB.ReplaceInvalidCharacters();
	sDB.Replace( ' ', '_' );
	sDB.Replace( '-', '_' );
	sDB.Replace( '+', '_' );
	sDB.Replace( '(', '_' );
	sDB.Replace( ')', '_' );
	sDB.Replace( '{', '_' );
	sDB.Replace( '}', '_' );
	sDB.Replace( '[', '_' );
	sDB.Replace( ']', '_' );
	m_ctlDatabaseName.SetWindowText( sDB );
}


//=============================================================================
// Functions to create and initialize the database
//=============================================================================

//-------------------------------------
void vdbDatabaseNew::OnOK() 
{
	// validate the datasource name
	char szDataSource[SQL_MAX_DSN_LENGTH];
	m_ctlDataSourceName.GetWindowText( szDataSource, SQL_MAX_DSN_LENGTH );
	vdbString sDataSource( szDataSource );
	if ( sDataSource.IsNull() )
		{ vdbMessageBox( NULL, "Please supply a name for the data source and try again.", "New Database", MB_OK ); return; }
	if ( sDataSource.FindOneOf( "[]{}(),;?*=!@\\" ) != -1 )
		{ vdbMessageBox( NULL, "Data source names cannot contain any of the letters []{}(),;?*=!@\\.\n\nPlease change the name and try again.", "New Database", MB_OK ); return; }

	// make sure that there is no Data Source with this name
	vdbString sSubkey( "Software\\ODBC\\ODBC.INI\\ODBC Data Sources" );
	vdbRegistrationKey key1( HKEY_CURRENT_USER, sSubkey, szDataSource );
	if ( key1.LeafExists() )
		{ vdbMessageBox( NULL, "There is already a Data Source with this name.\n\nPlease choose a unique name.", "New Database", MB_OK ); return; }

	vdbRegistrationKey key2( HKEY_LOCAL_MACHINE, sSubkey, szDataSource );
	if ( key2.LeafExists() )
		{ vdbMessageBox( NULL, "There is already a Data Source with this name.\n\nPlease choose a unique name.", "New Database", MB_OK ); return; }

	// validate the database name
	char szDatabaseName[129];
	m_ctlDatabaseName.GetWindowText( szDatabaseName, 129 );
	vdbString sDatabaseName( szDatabaseName );
	if ( sDatabaseName.IsNull() )
		{ vdbMessageBox( NULL, "Please supply a name for the database and try again.", "New Database", MB_OK ); return; }
	if ( sDatabaseName.FindOneOf( "[]{}(),;?*=+!@-\\" ) != -1 )
		{ vdbMessageBox( NULL, "Database names cannot contain any of the letters []{}(),;?*=+!@-\\.\n\nPlease change the name and try again.", "New Database", MB_OK ); return; }


	// make sure the user has specified a fixed disk drive
	char szDataDrive[4];
	m_ctlDataDirectory.GetWindowText( szDataDrive, 4 );
	if ( szDataDrive[0] == 0x0)
		{ vdbMessageBox( NULL, "Please supply the location for the database directory.", "New Database", MB_OK ); return; }
	if ( isalpha( szDataDrive[0] ) == false )
		{ vdbMessageBox( NULL, "Please respecify the database directory location using a valid drive letter.", "New Database", MB_OK ); return; }
	if ( szDataDrive[1] != ':' || szDataDrive[2] != '\\' )
		{ vdbMessageBox( NULL, "Please respecify the database directory location using a colon and a backslash after the drive letter.", "New Database", MB_OK ); return; }

	UINT driveType = GetDriveType( szDataDrive );
	if ( driveType == DRIVE_CDROM )
		{ vdbMessageBox( NULL, "Sorry, CD-ROM drives can't be used for the database.\n\nPlease specify a fixed disk.", "New Database", MB_OK ); return; }
	else if ( driveType == DRIVE_REMOTE )
		{ vdbMessageBox( NULL, "Sorry, network drives can't be used for the database.\n\nPlease specify a local disk.", "New Database", MB_OK ); return; }
	else if ( driveType == DRIVE_REMOVABLE )
		{ vdbMessageBox( NULL, "Sorry, removable drives can't be used for the database.\n\nPlease specify a fixed disk.", "New Database", MB_OK ); return; }
	else if ( driveType != DRIVE_FIXED )
		{ vdbMessageBox( NULL, "Sorry, only local fixed disks can be used for the database.\n\nPlease specify a different drive.", "New Database", MB_OK ); return; }

	typedef BOOL (WINAPI *pGDFS)(LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);
	pGDFS pFunction = (pGDFS) GetProcAddress( GetModuleHandle( "kernel32.dll" ), "GetDiskFreeSpaceExA" );
	if ( pFunction != NULL )
	{
		// make sure there is enough disk space
		ULARGE_INTEGER freeBytesAvailableToCaller;
		ULARGE_INTEGER totalNumberOfBytes;
		ULARGE_INTEGER totalNumberOfFreeBytes;

		// if ( GetDiskFreeSpaceEx( szDataDrive, &freeBytesAvailableToCaller, &totalNumberOfBytes, &totalNumberOfFreeBytes ) == TRUE ) 		// tested and works on NT/2000
		if ( (*pFunction)( szDataDrive, &freeBytesAvailableToCaller, &totalNumberOfBytes, &totalNumberOfFreeBytes ) == TRUE )
		{
			DWORDLONG K = 1024;
			DWORDLONG ma = freeBytesAvailableToCaller.QuadPart / K / K;
			unsigned long megabytesAvailable = static_cast<unsigned long>( ma );

			char szDataSize[5];
			m_ctlDataSize.GetWindowText( szDataSize, 5 );
			unsigned int dataSize = atoi( szDataSize );
			if ( dataSize < 5 )
				{ vdbMessageBox( NULL, "The database data file must be at least 5 Mb in size.  Please respecify.", "New Database", MB_OK ); return; }

			char szLogSize[5];
			m_ctlLogSize.GetWindowText( szLogSize, 5 );
			unsigned int logSize = atoi( szLogSize );
			if ( logSize < 5 )
				{ vdbMessageBox( NULL, "The database log file must be at least 5 Mb in size.  Please respecify.", "New Database", MB_OK ); return; }

			if ( megabytesAvailable < dataSize + logSize )
			{
				vdbString s;
				s.Format( "Sorry, you've requested a total database size of %d Mb, but the %s drive only has %d Mb available.\n\nEither reduced the size of the database or select a different drive with more space.", dataSize + logSize, szDataDrive, megabytesAvailable );
				vdbMessageBox( NULL, (const char*) s, "New Database", MB_OK );
				return;
			}
		}
	}

	char szDescription[41];
	m_ctlDescription.GetWindowText( szDescription, 41 );

	// now do it
	if ( Execute() == false )
		return;

	// pass back the data source name so that the caller can connect
	_sDataSource = szDataSource;
	_sDescription = szDescription;

	vdbDLLDialog::OnOK();
}


//-------------------------------------
//^ The Execute function is called by OnOK to execute the complete script.
//< returns true if successful.  Returns false otherwise.
//
bool vdbDatabaseNew::Execute()
{
	bool rc = true;
	vdbStatusWindow* pStatus = new vdbStatusWindow( "Create database" );
	pStatus->ShowWindow( SW_SHOW );

	// These database command generate alot of noise, run quietly,
	_pDB->QuenchErrors( true );

	vdbMicrosoftSQLServer* pServer = 0;
	pStatus->Message( "Connecting to SQL Server Desktop Engine." );
	pServer = ConnectAsAdministrator( pStatus );
	if ( pServer == 0 )
		rc = false;

	if ( rc == true )
	{
		pStatus->Message( "Creating database" );
		rc = CreateDatabase( pServer, pStatus );
	}
	
	if ( rc == true )
	{
		pStatus->Message( "Creating authorization" );
		rc = CreateAuthorization( pServer, pStatus );
	}
	
	if ( rc == true )
	{
		pStatus->Message( "Creating ODBC data source" );
		rc = CreateDataSource( pStatus );
	}

	if ( rc == true )
	{
		pStatus->Message( "Initializing Database tables" );
		rc = InitializeDatabase( pStatus );
		if ( rc == false )
			DeleteDataSource( pStatus );
	}

	_pDB->QuenchErrors( false );
	if( rc == false )
	{
		pStatus->ShowCloseButton();
	}
	else
	{
		pStatus->Message( "Database creation successful" );
		pStatus->DestroyWindow();
		delete pStatus; pStatus = 0;
	}

	_pDB->Disconnect();
	return rc;
}


//-------------------------------------
//^ The CreateDataSource function is called to create an ODBC Data Source.
//< returns true if data source successfully created.  Returns false otherwise.
//
bool vdbDatabaseNew::CreateDataSource( vdbStatusWindow* pStatus )
{
	char szDataSource[SQL_MAX_DSN_LENGTH];
	m_ctlDataSourceName.GetWindowText( szDataSource, SQL_MAX_DSN_LENGTH );
	char szDescription[41];
	m_ctlDescription.GetWindowText( szDescription, 41 );
	char szDatabaseName[129];
	m_ctlDatabaseName.GetWindowText( szDatabaseName, 129 );

	char szComputerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD len = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerName( szComputerName, &len );

	vdbString sServerInstance;
	if ( _pDB->_defaults._sDesktopInstance.IsNull() )
		sServerInstance = szComputerName;
	else
		sServerInstance.Format( "%s\\%s", szComputerName, (const char*) _pDB->_defaults._sDesktopInstance );

	vdbString sParam;
	sParam.Format( "DSN=%s|SERVER=%s|DATABASE=%s|DESCRIPTION=%s||", szDataSource, (const char*) sServerInstance, szDatabaseName, szDescription );
	sParam.Replace( '|', '\0' );

	if ( SQLConfigDataSource( NULL, ODBC_ADD_DSN, "SQL Server", (const char*) sParam ) == TRUE )
	{
		vdbString sSubkey( "Software\\ODBC\\ODBC.INI\\" );
		sSubkey += szDataSource;
		vdbRegistrationKey key( HKEY_CURRENT_USER, sSubkey, "LastUser" );
		key.SetValue( "sa" );
		return true;
	}
	else
	{
		pStatus->Message( "Unable to create the ODBC Data source." );
		return false;
	}
}


//-------------------------------------
//^ The DeleteDataSource function is called to delete an ODBC Data Source.
//< returns true if data source successfully deleted.  Returns false otherwise.
//
bool vdbDatabaseNew::DeleteDataSource( vdbStatusWindow* pStatus )
{
	char szDataSource[SQL_MAX_DSN_LENGTH];
	m_ctlDataSourceName.GetWindowText( szDataSource, SQL_MAX_DSN_LENGTH );

	// assemble the attribute string
	char szDSN[45];
	vdbUtility::SafeFormat( szDSN, sizeof(szDSN), "DSN=%s", szDataSource ); 
	char szAttributes[100];
	szAttributes[0] = 0x0;
	vdbUtility::SafeCatWithNullTerminator( szAttributes, szDSN, sizeof(szAttributes) );

	if ( SQLConfigDataSource( NULL, ODBC_REMOVE_DSN, "SQL Server", szAttributes ) == TRUE )
		return true;
	else
		return false;
}


//-------------------------------------
//^ The ConnectAsAdministrator function.
//< returns a pointer to the SQL Server Desktop Engine instance if successful.
//< returns a null pointer if it fails.
//
vdbMicrosoftSQLServer* vdbDatabaseNew::ConnectAsAdministrator( vdbStatusWindow* pStatus )
{
	if ( _pDB->ConnectAsAdministrator() != DBS_OK )
	{
		pStatus->Message( "Unable to connect to SQL Server Desktop Engine.  Make sure that the Desktop Engine has been installed and that the \"SQL Server service\" is running." );
		return NULL;
	}

	return (vdbMicrosoftSQLServer*) _pDB->Server();
}


//-------------------------------------
//^ The CreateDatabase function is called to create the database.
//< returns true if successfully created.  Returns false otherwise.
//
bool vdbDatabaseNew::CreateDatabase( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus )
{
	assert( pStatus != 0 );
	assert( pServer != 0 );

	char szDatabaseName[129];
	m_ctlDatabaseName.GetWindowText( szDatabaseName, 129 );

	char szDataDirectory[121];
	m_ctlDataDirectory.GetWindowText( szDataDirectory, 121 );

	char szDataSize[5];
	m_ctlDataSize.GetWindowText( szDataSize, 5 );
	unsigned int dataSize = atoi( szDataSize );

	char szLogSize[5];
	m_ctlLogSize.GetWindowText( szLogSize, 5 );
	unsigned int logSize = atoi( szLogSize );

	// make sure that there isn't a database with this name already
	if ( pServer->DatabaseExists( szDatabaseName ) )
	{
		pStatus->Message( "There is already a database with this name.\n\nPlease choose a unique database name." );
		return false;
	}

	// create the database
	if ( pServer->CreateDatabase( szDatabaseName, szDataDirectory, dataSize, logSize ) != DBS_OK )
	{
		pStatus->Message( "Unable to create the database." );
		return false;
	}

	return true;
}


//-------------------------------------
//^ The CreateAuthorization function is called to create the authorization login.
//< returns true if successfully created.  Returns false otherwise.
//
bool vdbDatabaseNew::CreateAuthorization( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus )
{
	assert( pStatus != 0 );
	assert( pServer != 0 );

	char szDatabaseName[129];
	m_ctlDatabaseName.GetWindowText( szDatabaseName, 129 );

	char szAuthorizationPassword[31];
	m_ctlAuthorizationPassword.GetWindowText( szAuthorizationPassword, 31 );

	char szAuthorizationID[31];
	m_ctlAuthorizationID.GetWindowText( szAuthorizationID, 31 );

	// add a login id and assign the new database as its default login database
	// failure to AddLogin is not fatal if the login already exists
	pStatus->Message( "Creating login ID" );
	pServer->AddLogin( szAuthorizationID, szAuthorizationPassword, "master" );
	
	// use the new database
	if ( pServer->Use( szDatabaseName ) != DBS_OK )
	{
		pStatus->Message( "Unable to USE the new database." );
		return false;
	}

	// Add this user to the database
	// failure is not fatal if the user is already there
	pServer->AddUser( szAuthorizationID );

	// add the login to the sysadmin fixed server role
	if ( pServer->AddServerRoleMember( szAuthorizationID, "sysadmin" ) != DBS_OK )
	{
		pStatus->Message( "Unable to add 'sysadmin' server role to the new login ID." );
		return false;
	}

	return true;
}


//-------------------------------------
//^ The InitializeDatabase function is called to create the data tables.
//< returns true if successfully created.  Returns false otherwise.
//
bool vdbDatabaseNew::InitializeDatabase( vdbStatusWindow* pStatus )
{
	assert( pStatus != 0 );
	char szAuthorizationPassword[31];
	m_ctlAuthorizationPassword.GetWindowText( szAuthorizationPassword, 31 );

	char szAuthorizationID[31];
	m_ctlAuthorizationID.GetWindowText( szAuthorizationID, 31 );

	char szDataSource[SQL_MAX_DSN_LENGTH];
	m_ctlDataSourceName.GetWindowText( szDataSource, SQL_MAX_DSN_LENGTH );

	if ( _pDB->Connect( szDataSource, szAuthorizationID, szAuthorizationPassword ) != DBS_OK )
	{
		pStatus->Message( "Unable to connect to SQL Server Desktop Engine using new login ID." );
		return false;
	}

	bool rc = true;
	pStatus->Message( "Initializing system tables." );
	_pDB->CreateSystemTables();

	// disconnect and reconnect so that the new vdbUnique entry will be used for
	// all of the remaining objects to be created in this function
	_pDB->Disconnect();
	if ( _pDB->Connect( szDataSource, szAuthorizationID, szAuthorizationPassword ) != DBS_OK )
	{
		pStatus->Message( "Unable to connect to SQL Server Desktop Engine using new login ID." );
		return false;
	}
	_pDB->CreateUserTables();

	return true;
}


//-------------------------------------
void vdbDatabaseNew::OnSelectDirectory() 
{
	char sz[120];
	m_ctlDataDirectory.GetWindowText( sz, sizeof(sz) );

	// show directory dialog box
	vdbDirectoryDlgData data;
	data.SetDirectory( sz );

	if ( vdbDirectoryDialog( data ) == false )
		return;

	m_ctlDataDirectory.SetWindowText( data.GetDirectory() );	
}
