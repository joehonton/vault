//=============================================================================
//
// CLASS:           vdbDatabaseAttach
//
// ESSENCE:         Attach database MDF and LDF files
//                  and create a new ODBC data source
//
// FILE NAME:       vdbDatabaseAttach.h
//
// INITIAL DATE:    7/30/01
//
// COPYRIGHT:		Copyright © 2001 by Joe Honton
//
//=============================================================================

#include <vdbWindows.h>
#include "vdbDatabaseAttach.h"
#include <vdbDatabase.h>
#include <vdbUtility.h>
#include <vdbFilename.h>
#include <ODBCinst.h>
#include "vdbStatusWindow.h"
#include <vdbMicrosoftSQLServer.h>
#include <vdbRegistrationKey.h>
#include <vdbAssert.h>
#include <vdbString.h>
#include <vdbFileDialog.h>
#include <VaultResourceInstance.h>


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
vdbDatabaseAttach::vdbDatabaseAttach( vdbDatabase* pDB, CWnd* pParent )
	: vdbDLLDialog(vdbDatabaseAttach::IDD, pParent),
	  _pDB( pDB )
{
	assert( _pDB != 0 );

	m_ctlSelectDataFile.SetBitmaps(IDB_VDB_DIRECTORY_UP, IDB_VDB_DIRECTORY_DOWN, IDB_VDB_DIRECTORY_FOCUS, IDB_VDB_DIRECTORY_DISABLED );
	m_ctlSelectLogFile.SetBitmaps(IDB_VDB_DIRECTORY_UP, IDB_VDB_DIRECTORY_DOWN, IDB_VDB_DIRECTORY_FOCUS, IDB_VDB_DIRECTORY_DISABLED );

	//{{AFX_DATA_INIT(vdbDatabaseAttach)
	//}}AFX_DATA_INIT
}


//-------------------------------------
vdbDatabaseAttach::~vdbDatabaseAttach()
{
}


//-------------------------------------
void vdbDatabaseAttach::DoDataExchange(CDataExchange* pDX)
{
	vdbDLLDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(vdbDatabaseAttach)
	DDX_Control(pDX, IDC_VDB_DATA_FILE, m_ctlDataFile);
	DDX_Control(pDX, IDC_VDB_SELECT_DATA_FILE, m_ctlSelectDataFile);
	DDX_Control(pDX, IDC_VDB_LOG_FILE, m_ctlLogFile);
	DDX_Control(pDX, IDC_VDB_SELECT_LOG_FILE, m_ctlSelectLogFile);
	DDX_Control(pDX, IDC_VDB_DATASOURCE_NAME, m_ctlDataSourceName);
	DDX_Control(pDX, IDC_VDB_DESCRIPTION, m_ctlDescription);
	DDX_Control(pDX, IDC_VDB_DATABASE_NAME, m_ctlDatabaseName);
	DDX_Control(pDX, IDC_VDB_AUTHORIZATION_ID, m_ctlAuthorizationID);
	DDX_Control(pDX, IDC_VDB_AUTHORIZATION_PASSWORD, m_ctlAuthorizationPassword);
	DDX_Control(pDX, IDOK, m_ctlOK);
	//}}AFX_DATA_MAP
}


//-------------------------------------
BEGIN_MESSAGE_MAP(vdbDatabaseAttach, vdbDLLDialog)
	//{{AFX_MSG_MAP(vdbDatabaseAttach)
	ON_BN_CLICKED(IDC_VDB_SELECT_DATA_FILE, OnSelectDataFile)
	ON_BN_CLICKED(IDC_VDB_SELECT_LOG_FILE, OnSelectLogFile)
	ON_EN_CHANGE(IDC_VDB_DATA_FILE, OnChangeDataFile)
	ON_EN_CHANGE(IDC_VDB_LOG_FILE, OnChangeDataFile)
	ON_EN_CHANGE(IDC_VDB_DATASOURCE_NAME, OnChangeDataSourceName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
// Window creation and intialization
//=============================================================================

//-------------------------------------
BOOL vdbDatabaseAttach::OnInitDialog() 
{
	vdbDLLDialog::OnInitDialog();
	
	m_ctlSelectDataFile.SizeToContent();
	m_ctlSelectLogFile.SizeToContent();

	m_ctlDataSourceName.SetLimitText( SQL_MAX_DSN_LENGTH );
	m_ctlDescription.SetLimitText( 40 );
	m_ctlDatabaseName.SetLimitText( 128 );
	
	m_ctlDataSourceName.SetWindowText( _pDB->_defaults._sDataSourceName );
	m_ctlDescription.SetWindowText( _pDB->_defaults._sDescription );
	m_ctlDatabaseName.SetWindowText( _pDB->_defaults._sDatabaseName );
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

	m_ctlDataFile.SetWindowText( "C:\\Database\\" );
	m_ctlLogFile.SetWindowText( "C:\\Database\\" );

	EnableControls( false );

	return TRUE;
}


//=============================================================================
// User interface callbacks
//=============================================================================

//-------------------------------------
//^ The OnSelectDataFile function is called by the user to select which MDF
//  file to attach
void vdbDatabaseAttach::OnSelectDataFile()
{
	char sz[256];
	m_ctlDataFile.GetWindowText( sz, sizeof(sz) );
	vdbFilename sFilename( sz );

	vdbFileDlgData data( m_hWnd, "Select Database data file", "Select the data file to attach", (const char*) sFilename.Directory(), (const char*) sFilename.FilenameWithExtension(), false );
	data.AddFileFilter( "Data files (*.mdf)", "mdf", true );
	data.AddFileFilter( "All files (*.*)", "*", false );

	if ( vdbFileDialog( data ) == true )
	{
		sFilename = data.GetDirectory();
		sFilename.Append( data.GetSelectedFilename() );
		m_ctlDataFile.SetWindowText( sFilename );

		// help the user by getting the log file, if it can be found
		int index = sFilename.Find( "Data.mdf" );
		vdbFilename sLogFilename = sFilename.First( index );
		sLogFilename += "Log.ldf";
		if ( sLogFilename.Exists() )
			m_ctlLogFile.SetWindowText( sLogFilename );
	}
}


//-------------------------------------
//^ The OnSelectLogFile function is called by the user to select which LDF
//  file to attach
void vdbDatabaseAttach::OnSelectLogFile()
{
	char sz[256];
	m_ctlLogFile.GetWindowText( sz, sizeof(sz) );
	vdbFilename sFilename( sz );

	vdbFileDlgData data( m_hWnd, "Select Database log file", "Select the log file to attach", (const char*) sFilename.Directory(), (const char*) sFilename.FilenameWithExtension(), false );
	data.AddFileFilter( "Log files (*.ldf)", "ldf", true );
	data.AddFileFilter( "All files (*.*)", "*", false );

	if ( vdbFileDialog( data ) == true )
	{
		sFilename = data.GetDirectory();
		sFilename.Append( data.GetSelectedFilename() );
		m_ctlLogFile.SetWindowText( sFilename );
	}
}


//-------------------------------------
//^ The OnChangeDataFile function is triggered every time the MDF filename
//  control is modified.
//
void vdbDatabaseAttach::OnChangeDataFile() 
{
	char sz[256];
	m_ctlDataFile.GetWindowText( sz, sizeof(sz) );
	vdbFilename sDataFilename( sz );

	m_ctlLogFile.GetWindowText( sz, sizeof(sz) );
	vdbFilename sLogFilename( sz );

	// enable the lower half controls if these are real files
	if ( sDataFilename.Exists() && sLogFilename.Exists() )
		if ( !sDataFilename.IsDirectory() && !sLogFilename.IsDirectory() )
		{
			EnableControls( true );
			return;
		}

	EnableControls( false );
}


//-------------------------------------
//^ The OnChangeDatasourceName function is triggered every time the data source
//  name control is modified.
//
void vdbDatabaseAttach::OnChangeDataSourceName() 
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


//-------------------------------------
//^ The EnableControls function is called to enable or disable the controls in
//  the lower half of the dialog
void vdbDatabaseAttach::EnableControls( bool b )
{
	m_ctlDataSourceName.EnableWindow( b );
	m_ctlDescription.EnableWindow( b );
	m_ctlDatabaseName.EnableWindow( b );
	m_ctlOK.EnableWindow( b );

	if ( _pDB->_defaults._bEnableAuthorization == true )
	{
		m_ctlAuthorizationID.EnableWindow( b );
		m_ctlAuthorizationPassword.EnableWindow( b );
	}
}


//=============================================================================
// Functions to attach the database files
//=============================================================================

//-------------------------------------
void vdbDatabaseAttach::OnOK() 
{
	// get the data file name
	char sz[256];
	m_ctlDataFile.GetWindowText( sz, sizeof(sz) );
	_sDataFile = sz;
	if ( !_sDataFile.Exists() )
	{
		vdbString s;
		s.Format( "The file %s doesn't exist.  Please choose a different file to attach.", (const char*) _sDataFile );
		vdbMessageBox( NULL, s, "Attach database", MB_OK );
		return;
	}

	// make sure the user has specified a fixed disk drive
	char szDataDrive[4];
	vdbUtility::SafeCopy( szDataDrive, _sDataFile, 4 );
	UINT driveType = GetDriveType( szDataDrive );
	if ( driveType == DRIVE_CDROM )
		{ vdbMessageBox( NULL, "Sorry, CD-ROM drives can't be used for the database.\n\nPlease copy to a fixed disk first.", "Attach Database", MB_OK ); return; }
	else if ( driveType == DRIVE_REMOTE )
		{ vdbMessageBox( NULL, "Sorry, network drives can't be used for the database.\n\nPlease copy to a local disk first.", "Attach Database", MB_OK ); return; }
	else if ( driveType == DRIVE_REMOVABLE )
		{ vdbMessageBox( NULL, "Sorry, removable drives can't be used for the database.\n\nPlease copy to a fixed disk firt.", "Attach Database", MB_OK ); return; }
	else if ( driveType != DRIVE_FIXED )
		{ vdbMessageBox( NULL, "Sorry, only local fixed disks can be used for the database.\n\nPlease copy to a local/fixed drive.", "Attach Database", MB_OK ); return; }

	// get the log file name
	m_ctlLogFile.GetWindowText( sz, sizeof(sz) );
	_sLogFile = sz;
	if ( !_sLogFile.Exists() )
	{
		vdbString s;
		s.Format( "The file %s doesn't exist.  Please choose a different file to attach.", (const char*) _sLogFile );
		vdbMessageBox( NULL, s, "Attach database", MB_OK );
		return;
	}
	// make sure the user has specified a fixed disk drive
	char szLogDrive[4];
	vdbUtility::SafeCopy( szLogDrive, _sLogFile, 4 );
	driveType = GetDriveType( szLogDrive );
	if ( driveType == DRIVE_CDROM )
		{ vdbMessageBox( NULL, "Sorry, CD-ROM drives can't be used for the database.\n\nPlease copy to a fixed disk first.", "Attach Database", MB_OK ); return; }
	else if ( driveType == DRIVE_REMOTE )
		{ vdbMessageBox( NULL, "Sorry, network drives can't be used for the database.\n\nPlease copy to a local disk first.", "Attach Database", MB_OK ); return; }
	else if ( driveType == DRIVE_REMOVABLE )
		{ vdbMessageBox( NULL, "Sorry, removable drives can't be used for the database.\n\nPlease copy to a fixed disk firt.", "Attach Database", MB_OK ); return; }
	else if ( driveType != DRIVE_FIXED )
		{ vdbMessageBox( NULL, "Sorry, only local fixed disks can be used for the database.\n\nPlease copy to a local/fixed drive.", "Attach Database", MB_OK ); return; }

	// validate the datasource name
	char szDataSource[SQL_MAX_DSN_LENGTH];
	m_ctlDataSourceName.GetWindowText( szDataSource, SQL_MAX_DSN_LENGTH );
	vdbString sDataSource( szDataSource );
	if ( sDataSource.IsNull() )
		{ vdbMessageBox( NULL, "Please supply a name for the data source and try again.", "Attach Database", MB_OK ); return; }
	if ( sDataSource.FindOneOf( "[]{}(),;?*=!@\\" ) != -1 )
		{ vdbMessageBox( NULL, "Data source names cannot contain any of the letters []{}(),;?*=!@\\.\n\nPlease change the name and try again.", "Attach Database", MB_OK ); return; }

	// make sure that there is no Data Source with this name
	vdbString sSubkey( "Software\\ODBC\\ODBC.INI\\ODBC Data Sources" );
	vdbRegistrationKey key1( HKEY_CURRENT_USER, sSubkey, szDataSource );
	if ( key1.LeafExists() )
		{ vdbMessageBox( NULL, "There is already a Data Source with this name.\n\nPlease choose a unique name.", "Attach Database", MB_OK ); return; }

	vdbRegistrationKey key2( HKEY_LOCAL_MACHINE, sSubkey, szDataSource );
	if ( key2.LeafExists() )
		{ vdbMessageBox( NULL, "There is already a Data Source with this name.\n\nPlease choose a unique name.", "Attach Database", MB_OK ); return; }

	// validate the database name
	char szDatabaseName[129];
	m_ctlDatabaseName.GetWindowText( szDatabaseName, 129 );
	vdbString sDatabaseName( szDatabaseName );
	if ( sDatabaseName.IsNull() )
		{ vdbMessageBox( NULL, "Please supply a name for the database and try again.", "Attach Database", MB_OK ); return; }
	if ( sDatabaseName.FindOneOf( "[]{}(),;?*=+!@-\\" ) != -1 )
		{ vdbMessageBox( NULL, "Database names cannot contain any of the letters []{}(),;?*=+!@-\\.\n\nPlease change the name and try again.", "Attach Database", MB_OK ); return; }


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
bool vdbDatabaseAttach::Execute()
{
	bool rc = true;
	vdbStatusWindow* pStatus = new vdbStatusWindow( "Attach database" );
	pStatus->ShowWindow( SW_SHOW );

	// These database commands generate alot of noise, run quietly,
	_pDB->QuenchErrors( true );

	vdbMicrosoftSQLServer* pServer = 0;
	pStatus->Message( "Connecting to SQL Server Desktop Engine." );
	pServer = ConnectAsAdministrator( pStatus );
	if ( pServer == 0 )
		rc = false;

	if ( rc == true )
	{
		pStatus->Message( "Attaching database" );
		rc = AttachDatabase( pServer, pStatus );
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

	_pDB->QuenchErrors( false );
	if( rc == false )
	{
		pStatus->ShowCloseButton();
	}
	else
	{
		pStatus->Message( "Database attach successful" );
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
bool vdbDatabaseAttach::CreateDataSource( vdbStatusWindow* pStatus )
{
	char szDataSource[SQL_MAX_DSN_LENGTH];
	m_ctlDataSourceName.GetWindowText( szDataSource, SQL_MAX_DSN_LENGTH );
	char szDescription[41];
	m_ctlDescription.GetWindowText( szDescription, 41 );
	char szDatabaseName[129];
	m_ctlDatabaseName.GetWindowText( szDatabaseName, 129 );

	return CreateDataSource( pStatus, szDataSource, szDescription, szDatabaseName );
}


//-------------------------------------
//^ The CreateDataSource function is called to create an ODBC Data Source.
//< returns true if data source successfully created.  Returns false otherwise.
//
bool vdbDatabaseAttach::CreateDataSource( vdbStatusWindow* pStatus, const char* szDataSource, const char* szDescription, const char* szDatabaseName )
{
	char szComputerName[MAX_COMPUTERNAME_LENGTH + 1];		// 31 + 1
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
//^ The ConnectAsAdministrator function.
//< returns a pointer to the SQL Server Desktop Engine instance if successful.
//< returns a null pointer if it fails.
//
vdbMicrosoftSQLServer* vdbDatabaseAttach::ConnectAsAdministrator( vdbStatusWindow* pStatus )
{
	if ( _pDB->ConnectAsAdministrator() != DBS_OK )
	{
		pStatus->Message( "Unable to connect to SQL Server Desktop Engine.  Make sure that the Desktop Engine has been installed and that the \"SQL Server service\" is running." );
		return NULL;
	}

	return (vdbMicrosoftSQLServer*) _pDB->Server();
}


//-------------------------------------
//^ The AttachDatabase function is called to attach the database.
//< returns true if successfully created.  Returns false otherwise.
//
bool vdbDatabaseAttach::AttachDatabase( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus )
{
	char szDatabaseName[129];
	m_ctlDatabaseName.GetWindowText( szDatabaseName, 129 );

	return AttachDatabase( pServer, pStatus, szDatabaseName );
}


//-------------------------------------
//^ The AttachDatabase function is called to create the database.
//< returns true if successfully created.  Returns false otherwise.
//
bool vdbDatabaseAttach::AttachDatabase( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus, const char* szDatabaseName )
{
	assert( pStatus != 0 );
	assert( pServer != 0 );

	// make sure that there isn't a database with this name already
	if ( pServer->DatabaseExists( szDatabaseName ) )
	{
		pStatus->Message( "There is already a database with this name.\n\nPlease choose a unique database name." );
		return false;
	}

	// Attach the database
	if ( pServer->AttachDatabase( _sDataFile, _sLogFile, szDatabaseName ) != DBS_OK )
	{
		pStatus->Message( "Unable to attach the database." );
		return false;
	}

	return true;
}


//-------------------------------------
//^ The CreateAuthorization function is called to create the authorization login.
//< returns true if successfully created.  Returns false otherwise.
//
bool vdbDatabaseAttach::CreateAuthorization( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus )
{
	char szAuthorizationID[31];
	m_ctlAuthorizationID.GetWindowText( szAuthorizationID, 31 );

	char szAuthorizationPassword[31];
	m_ctlAuthorizationPassword.GetWindowText( szAuthorizationPassword, 31 );

	char szDatabaseName[129];
	m_ctlDatabaseName.GetWindowText( szDatabaseName, 129 );

	return CreateAuthorization( pServer, pStatus, szAuthorizationID, szAuthorizationPassword, szDatabaseName );
}


//-------------------------------------
//^ The CreateAuthorization function is called to create the authorization login.
//< returns true if successfully created.  Returns false otherwise.
//
bool vdbDatabaseAttach::CreateAuthorization( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus, const char* szAuthorizationID, const char* szAuthorizationPassword, const char* szDatabaseName )
{
	assert( pStatus != 0 );
	assert( pServer != 0 );

	// add a login id and assign the Attach Database as its default login database
	// failure to AddLogin is not fatal if the login already exists
	pStatus->Message( "Creating login ID" );
	pServer->AddLogin( szAuthorizationID, szAuthorizationPassword, "master" );
	
	// use the Attach Database
	if ( pServer->Use( szDatabaseName ) != DBS_OK )
	{
		pStatus->Message( "Unable to USE the attached Database." );
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


