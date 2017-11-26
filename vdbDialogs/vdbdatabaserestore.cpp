//=============================================================================
//
// CLASS:           vdbDatabaseRestore
//
// ESSENCE:         Restore a database backup and create a new ODBC data source
//
// FILE NAME:       vdbDatabaseRestore.cpp
//
// INITIAL DATE:    6/12/00
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#include <vdbWindows.h>
#include "vdbDatabaseRestore.h"
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
#include <vdbFileDialog.h>
#include <VaultResourceInstance.h>
#include <vdbApp.h>
#include <vdbTimeStamp.h>


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
vdbDatabaseRestore::vdbDatabaseRestore( vdbDatabase* pDB, CWnd* pParent )
	: vdbDLLDialog(vdbDatabaseRestore::IDD, pParent),
	  _pDB( pDB )
{
	assert( _pDB != 0 );

	m_ctlSelectBackupFile.SetBitmaps(IDB_VDB_DIRECTORY_UP, IDB_VDB_DIRECTORY_DOWN, IDB_VDB_DIRECTORY_FOCUS, IDB_VDB_DIRECTORY_DISABLED );
	m_ctlSelectDataDirectory.SetBitmaps(IDB_VDB_DIRECTORY_UP, IDB_VDB_DIRECTORY_DOWN, IDB_VDB_DIRECTORY_FOCUS, IDB_VDB_DIRECTORY_DISABLED );

	//{{AFX_DATA_INIT(vdbDatabaseRestore)
	//}}AFX_DATA_INIT
}


//-------------------------------------
vdbDatabaseRestore::~vdbDatabaseRestore()
{
}


//-------------------------------------
void vdbDatabaseRestore::DoDataExchange(CDataExchange* pDX)
{
	vdbDLLDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(vdbDatabaseRestore)
	DDX_Control(pDX, IDC_VDB_CHOOSE_RECENT, m_ctlChooseRecent);
	DDX_Control(pDX, IDC_VDB_CHOOSE_ALTERNATE, m_ctlChooseAlternate);
	DDX_Control(pDX, IDC_VDB_RECENT_BACKUPS, m_ctlRecentBackups);
	DDX_Control(pDX, IDC_VDB_BACKUP_FILE, m_ctlBackupFile);
	DDX_Control(pDX, IDC_VDB_SELECT_BACKUP_FILE, m_ctlSelectBackupFile);
	DDX_Control(pDX, IDC_VDB_DATA_DIRECTORY, m_ctlDataDirectory);
	DDX_Control(pDX, IDC_VDB_SELECT_DIRECTORY, m_ctlSelectDataDirectory);
	DDX_Control(pDX, IDC_VDB_DATASOURCE_NAME, m_ctlDataSourceName);
	DDX_Control(pDX, IDC_VDB_DESCRIPTION, m_ctlDescription);
	DDX_Control(pDX, IDC_VDB_DATABASE_NAME, m_ctlDatabaseName);
	DDX_Control(pDX, IDC_VDB_AUTHORIZATION_ID, m_ctlAuthorizationID);
	DDX_Control(pDX, IDC_VDB_AUTHORIZATION_PASSWORD, m_ctlAuthorizationPassword);
	DDX_Control(pDX, IDOK, m_ctlOK);
	//}}AFX_DATA_MAP
}


//-------------------------------------
BEGIN_MESSAGE_MAP(vdbDatabaseRestore, vdbDLLDialog)
	//{{AFX_MSG_MAP(vdbDatabaseRestore)
	ON_BN_CLICKED(IDC_VDB_CHOOSE_RECENT, OnChooseRecent)
	ON_NOTIFY(LVN_SELCHANGED, IDC_VDB_RECENT_BACKUPS, OnSelchangedRecentBackups)
	ON_BN_CLICKED(IDC_VDB_CHOOSE_ALTERNATE, OnChooseAlternate)
	ON_EN_CHANGE(IDC_VDB_BACKUP_FILE, OnChangeBackupFile)
	ON_BN_CLICKED(IDC_VDB_SELECT_BACKUP_FILE, OnSelectBackupFile)
	ON_EN_CHANGE(IDC_VDB_DATASOURCE_NAME, OnChangeDataSourceName)
	ON_BN_CLICKED(IDC_VDB_SELECT_DIRECTORY, OnSelectDataDirectory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
// Window creation and intialization
//=============================================================================

//-------------------------------------
BOOL vdbDatabaseRestore::OnInitDialog() 
{
	vdbDLLDialog::OnInitDialog();
	
	m_ctlSelectBackupFile.SizeToContent();
	m_ctlSelectDataDirectory.SizeToContent();

	m_ctlDataSourceName.SetLimitText( SQL_MAX_DSN_LENGTH );
	m_ctlDescription.SetLimitText( 40 );
	m_ctlDatabaseName.SetLimitText( 128 );
	m_ctlDataDirectory.SetLimitText( 120 );
	
	m_ctlDataSourceName.SetWindowText( _pDB->_defaults._sDataSourceName );
	m_ctlDescription.SetWindowText( _pDB->_defaults._sDescription );
	m_ctlDatabaseName.SetWindowText( _pDB->_defaults._sDatabaseName );
	m_ctlDataDirectory.SetWindowText( _pDB->_defaults._sDirectoryLocation );

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

	// set up the list control
	HINSTANCE hInst = VaultResourceInstance::Handle();
	HBITMAP hBitmap = ::LoadBitmap( hInst, MAKEINTRESOURCE(IDB_VDB_DATABASE) );
	_listBitmap.Attach( hBitmap );
	_listImage.Create( 16, 16, FALSE, 3, 3 );
	_listImage.Add( &_listBitmap, (CBitmap*) NULL );   
	m_ctlRecentBackups.SetImageList( &_listImage, LVSIL_SMALL );

	RECT rect;
	m_ctlRecentBackups.GetClientRect( &rect );
	int columnWidth0 = 20;
	int columnWidth1 = static_cast<int>( ( rect.right - columnWidth0 ) * 0.65 );
	int columnWidth2 = rect.right - ( columnWidth0 + columnWidth1 + GetSystemMetrics(SM_CXVSCROLL) );

	m_ctlRecentBackups.PostCreate();
	m_ctlRecentBackups.ChangeColors( "SOFTWARE\\ProgrammersHeaven\\1.0\\Vault\\Colors" );
	m_ctlRecentBackups.InsertColumn( 0, "", LVCFMT_LEFT, columnWidth0, 0 );
	m_ctlRecentBackups.InsertColumn( 1, "Filename", LVCFMT_LEFT, columnWidth1, 1 );
	m_ctlRecentBackups.InsertColumn( 2, "Date", LVCFMT_LEFT, columnWidth2, 2 );

	// fill the list of recent backups as found in the registry
	int num = 0;
	while ( num < 100 )
	{
		vdbString sLeaf;
		sLeaf.Format( "Backup%02d", ++num );
		vdbRegistrationKey key( HKEY_CURRENT_USER,	vdbApp::GetDatabaseRegistrationRoot(), sLeaf );
	
		if ( key.LeafExists() )
		{
			vdbFilename sBackupFilename;
			key.GetValue( sBackupFilename );

			if ( sBackupFilename.Exists() )
			{
				int where = m_ctlRecentBackups.InsertItem( 0, "", 0 );
				m_ctlRecentBackups.SetItemText( where, 1, sBackupFilename );
				
				vdbTimeStamp ts;
				sBackupFilename.GetCreationTime( ts );
				
				char szDate[50];
				ts.Format( szDate, sizeof(szDate), "%b %d, %Y  %I:%M %p" );
				m_ctlRecentBackups.SetItemText( where, 2, szDate );
			}
			// remove the registry entry if the file is no longer there
			else
				key.DeleteLeaf();
		}
	}

	m_ctlBackupFile.SetWindowText( "C:\\" );

	m_ctlChooseRecent.SetCheck( 1 );
	OnChooseRecent();
	EnableControls( false );

	return TRUE;
}


//=============================================================================
// User interface callbacks
//=============================================================================

//-------------------------------------
//^ The OnChooseRecent function
void vdbDatabaseRestore::OnChooseRecent()
{
	m_ctlRecentBackups.EnableWindow( TRUE );
	m_ctlBackupFile.EnableWindow( FALSE );
	m_ctlSelectBackupFile.EnableWindow( FALSE );

	EnableControls( false );
}


//-------------------------------------
//^ The OnChooseAlternate function
void vdbDatabaseRestore::OnChooseAlternate()
{
	int count = m_ctlRecentBackups.GetItemCount();
	for ( int i = 0; i < count; i++ )
		m_ctlRecentBackups.SetItemState( i, 0x0000, LVIS_SELECTED );

	m_ctlRecentBackups.EnableWindow( FALSE );
	m_ctlBackupFile.EnableWindow( TRUE );
	m_ctlSelectBackupFile.EnableWindow( TRUE );
	OnChangeBackupFile();
}


//-------------------------------------
//^ The OnSelchangedRecentBackups file is a callback triggered by selecting an
//  item in the recent backup list
void vdbDatabaseRestore::OnSelchangedRecentBackups(NMHDR* pNMHDR, LRESULT* pResult)
{
	int which = m_ctlRecentBackups.GetNextItem( -1 , LVNI_SELECTED );
	if ( which == -1 )
	{
		EnableControls( false );
		return;
	}
	else
	{
		char sz[256];
		m_ctlRecentBackups.GetItemText( which, 1, sz, sizeof(sz) );
		vdbFilename sFilename( sz );
		if ( sFilename.Exists() )
		{
			EnableControls( true );
		
			// provide a good default value for the restored database
			m_ctlDataSourceName.SetWindowText( sFilename.Basename() );
		}
		else
			EnableControls( false );
	}

}


//-------------------------------------
//^ The OnChangeBackupFile function is triggered every time the backup file name
//  control is modified.
//
void vdbDatabaseRestore::OnChangeBackupFile() 
{
	char sz[256];
	m_ctlBackupFile.GetWindowText( sz, sizeof(sz) );

	// enable the lower half controls if this is a real file
	vdbFilename sFilename( sz );
	if ( sFilename.Exists() )
		if ( !sFilename.IsDirectory() )
		{
			EnableControls( true );

			// provide a good default value for the restored database
			m_ctlDataSourceName.SetWindowText( sFilename.Basename() );

			return;
		}

	EnableControls( false );
}


//-------------------------------------
//^ The OnSelectBackupFile function is called by the user to select which alternate
//  backup file to use
void vdbDatabaseRestore::OnSelectBackupFile()
{
	char sz[256];
	m_ctlBackupFile.GetWindowText( sz, sizeof(sz) );
	vdbFilename sFilename( sz );

	vdbFileDlgData data( m_hWnd, "Select alternate backup file", "Select the backup file to restore", (const char*) sFilename.Directory(), (const char*) sFilename.FilenameWithExtension(), false );
	data.AddFileFilter( "Backup files (*.bak)", "bak", true );
	data.AddFileFilter( "All files (*.*)", "*", false );

	if ( vdbFileDialog( data ) == true )
	{
		sFilename = data.GetDirectory();
		sFilename.Append( data.GetSelectedFilename() );
		m_ctlBackupFile.SetWindowText( sFilename );
	}
}


//-------------------------------------
//^ The OnChangeDatasourceName function is triggered every time the data source
//  name control is modified.
//
void vdbDatabaseRestore::OnChangeDataSourceName() 
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
void vdbDatabaseRestore::OnSelectDataDirectory() 
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


//-------------------------------------
//^ The EnableControls function is called to enable or disable the controls in
//  the lower half of the dialog
void vdbDatabaseRestore::EnableControls( bool b )
{
	m_ctlDataSourceName.EnableWindow( b );
	m_ctlDescription.EnableWindow( b );
	m_ctlDatabaseName.EnableWindow( b );
	m_ctlDataDirectory.EnableWindow( b );
	m_ctlSelectDataDirectory.EnableWindow( b );
	m_ctlOK.EnableWindow( b );

	if ( _pDB->_defaults._bEnableAuthorization == true )
	{
		m_ctlAuthorizationID.EnableWindow( b );
		m_ctlAuthorizationPassword.EnableWindow( b );
	}
}


//=============================================================================
// Functions to create and initialize the database
//=============================================================================

//-------------------------------------
void vdbDatabaseRestore::OnOK() 
{
	// get the backup file name
	if ( m_ctlChooseRecent.GetCheck() == 1 )
	{
		int which = m_ctlRecentBackups.GetNextItem( -1 , LVNI_SELECTED );
		if ( which == -1 )
			return;
		else
		{
			char sz[256];
			m_ctlRecentBackups.GetItemText( which, 1, sz, sizeof(sz) );
			_sBackupFile = sz;
		}
	}
	else // ( m_ctlChooseAlternate.GetCheck() == 1 )
	{
		char sz[256];
		m_ctlBackupFile.GetWindowText( sz, sizeof(sz) );
		_sBackupFile = sz;
	}

	if ( !_sBackupFile.Exists() )
	{
		vdbString s;
		s.Format( "The file %s doesn't exist.  Please choose a different file to restore.", (const char*) _sBackupFile );
		vdbMessageBox( NULL, s, "Restore Backup", MB_OK );
		return;
	}

	// verify that the source file is on a fixed disk
	char szDataDrive[4];
	vdbUtility::SafeCopy( szDataDrive, _sBackupFile, sizeof(szDataDrive) );
	UINT driveType = GetDriveType( szDataDrive );
	if ( driveType == DRIVE_CDROM )
		{ vdbMessageBox( NULL, "Sorry, the backup file can't be located on a CD-ROM drive.\n\nPlease copy this file to a fixed disk and try again.", "Restore Database", MB_OK ); return; }
	else if ( driveType == DRIVE_REMOTE )
		{ vdbMessageBox( NULL, "Sorry, the backup file can't be located on a network drive.\n\nPlease copy this file to a fixed disk and try again.", "Restore Database", MB_OK ); return; }

	// validate the datasource name
	char szDataSource[SQL_MAX_DSN_LENGTH];
	m_ctlDataSourceName.GetWindowText( szDataSource, SQL_MAX_DSN_LENGTH );
	vdbString sDataSource( szDataSource );
	if ( sDataSource.IsNull() )
		{ vdbMessageBox( NULL, "Please supply a name for the data source and try again.", "Restore Database", MB_OK ); return; }
	if ( sDataSource.FindOneOf( "[]{}(),;?*=!@\\" ) != -1 )
		{ vdbMessageBox( NULL, "Data source names cannot contain any of the letters []{}(),;?*=!@\\.\n\nPlease change the name and try again.", "Restore Database", MB_OK ); return; }

	// make sure that there is no Data Source with this name
	vdbString sSubkey( "Software\\ODBC\\ODBC.INI\\ODBC Data Sources" );
	vdbRegistrationKey key1( HKEY_CURRENT_USER, sSubkey, szDataSource );
	if ( key1.LeafExists() )
		{ vdbMessageBox( NULL, "There is already a Data Source with this name.\n\nPlease choose a unique name.", "Restore Database", MB_OK ); return; }

	vdbRegistrationKey key2( HKEY_LOCAL_MACHINE, sSubkey, szDataSource );
	if ( key2.LeafExists() )
		{ vdbMessageBox( NULL, "There is already a Data Source with this name.\n\nPlease choose a unique name.", "Restore Database", MB_OK ); return; }

	// validate the database name
	char szDatabaseName[129];
	m_ctlDatabaseName.GetWindowText( szDatabaseName, 129 );
	vdbString sDatabaseName( szDatabaseName );
	if ( sDatabaseName.IsNull() )
		{ vdbMessageBox( NULL, "Please supply a name for the database and try again.", "Restore Database", MB_OK ); return; }
	if ( sDatabaseName.FindOneOf( "[]{}(),;?*=+!@-\\" ) != -1 )
		{ vdbMessageBox( NULL, "Database names cannot contain any of the letters []{}(),;?*=+!@-\\.\n\nPlease change the name and try again.", "Restore Database", MB_OK ); return; }

	// make sure the user has specified a fixed disk drive
	m_ctlDataDirectory.GetWindowText( szDataDrive, 4 );
	if ( szDataDrive[0] == 0x0)
		{ vdbMessageBox( NULL, "Please supply the location for the database directory.", "Restore Database", MB_OK ); return; }
	if ( isalpha( szDataDrive[0] ) == false )
		{ vdbMessageBox( NULL, "Please respecify the database directory location using a valid drive letter.", "Restore Database", MB_OK ); return; }
	if ( szDataDrive[1] != ':' || szDataDrive[2] != '\\' )
		{ vdbMessageBox( NULL, "Please respecify the database directory location using a colon and a backslash after the drive letter.", "Restore Database", MB_OK ); return; }

	driveType = GetDriveType( szDataDrive );
	if ( driveType == DRIVE_CDROM )
		{ vdbMessageBox( NULL, "Sorry, CD-ROM drives can't be used for the database.\n\nPlease specify a fixed disk.", "Restore Database", MB_OK ); return; }
	else if ( driveType == DRIVE_REMOTE )
		{ vdbMessageBox( NULL, "Sorry, network drives can't be used for the database.\n\nPlease specify a local disk.", "Restore Database", MB_OK ); return; }
	else if ( driveType == DRIVE_REMOVABLE )
		{ vdbMessageBox( NULL, "Sorry, removable drives can't be used for the database.\n\nPlease specify a fixed disk.", "Restore Database", MB_OK ); return; }
	else if ( driveType != DRIVE_FIXED )
		{ vdbMessageBox( NULL, "Sorry, only local fixed disks can be used for the database.\n\nPlease specify a different drive.", "Restore Database", MB_OK ); return; }

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
			long megabytesAvailable = static_cast<long>( ma );

			unsigned long highOrder;		
			unsigned long lowOrder = _sBackupFile.Size( &highOrder );
			DWORDLONG mn = lowOrder /K /K;
			long megabytesNeeded = static_cast<long>( mn );

			if ( megabytesAvailable < megabytesNeeded )
			{
				vdbString s;
				s.Format( "Sorry, the database to be restored requires %d Mb, but the %s drive only has %d Mb available.\n\nEither reduced the size of the database or select a different drive with more space.", megabytesNeeded, szDataDrive, megabytesAvailable );
				vdbMessageBox( NULL, (const char*) s, "Restore Database", MB_OK );
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
bool vdbDatabaseRestore::Execute()
{
	bool rc = true;
	vdbStatusWindow* pStatus = new vdbStatusWindow( "Restore database" );
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
		pStatus->Message( "Restoring database" );
		rc = RestoreDatabase( pServer, pStatus );
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
		pStatus->Message( "Database restore successful" );
		pStatus->DestroyWindow();
		delete pStatus; pStatus = 0;
	}

	_pDB->Disconnect();
	return rc;
}


//-------------------------------------
//^ The SilentRestore function can be called instead of DoModal to quietly
//  restore a backup file and create the ODBC data source entry.
bool vdbDatabaseRestore::SilentRestore( const char* szDataSource, const char* szDatabaseName, const char* szDescription, const char* szDestinationDir, const char* szBackupFile, const char* szAuthorizationID, const char* szAuthorizationPassword )
{
	_sDataSource = szDataSource;
	_sDescription = szDescription;
	_sBackupFile = szBackupFile;

	bool rc = true;
	vdbStatusWindow* pStatus = new vdbStatusWindow( "Restore database" );
	pStatus->ShowWindow( SW_HIDE );

	_pDB->QuenchErrors( true );

	vdbMicrosoftSQLServer* pServer = 0;
	pServer = ConnectAsAdministrator( pStatus );
	if ( pServer == 0 )
		rc = false;

	if ( rc == true )
	{
		pStatus->Message( "Restoring database" );
		rc = RestoreDatabase( pServer, pStatus, szDatabaseName, szDestinationDir );
	}
	
	if ( rc == true )
	{
		pStatus->Message( "Creating authorization" );
		rc = CreateAuthorization( pServer, pStatus, szAuthorizationID, szAuthorizationPassword, szDatabaseName );
	}
	
	if ( rc == true )
	{
		pStatus->Message( "Creating ODBC data source" );
		rc = CreateDataSource( pStatus, szDataSource, szDescription, szDatabaseName );
	}

	pStatus->DestroyWindow();
	delete pStatus; pStatus = 0;

	_pDB->QuenchErrors( false );
	_pDB->Disconnect();
	
	return rc;
}


//-------------------------------------
//^ The CreateDataSource function is called to create an ODBC Data Source.
//< returns true if data source successfully created.  Returns false otherwise.
//
bool vdbDatabaseRestore::CreateDataSource( vdbStatusWindow* pStatus )
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
bool vdbDatabaseRestore::CreateDataSource( vdbStatusWindow* pStatus, const char* szDataSource, const char* szDescription, const char* szDatabaseName )
{
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
//^ The ConnectAsAdministrator function.
//< returns a pointer to the SQL Server Desktop Engine instance if successful.
//< returns a null pointer if it fails.
//
vdbMicrosoftSQLServer* vdbDatabaseRestore::ConnectAsAdministrator( vdbStatusWindow* pStatus )
{
	if ( _pDB->ConnectAsAdministrator() != DBS_OK )
	{
		pStatus->Message( "Unable to connect to SQL Server Desktop Engine.  Make sure that the Desktop Engine has been installed and that the \"SQL Server service\" is running." );
		return NULL;
	}

	return (vdbMicrosoftSQLServer*) _pDB->Server();
}


//-------------------------------------
//^ The RestoreDatabase function is called to restore the database.
//< returns true if successfully created.  Returns false otherwise.
//
bool vdbDatabaseRestore::RestoreDatabase( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus )
{
	char szDatabaseName[129];
	m_ctlDatabaseName.GetWindowText( szDatabaseName, 129 );

	char szDataDirectory[121];
	m_ctlDataDirectory.GetWindowText( szDataDirectory, 121 );

	return RestoreDatabase( pServer, pStatus, szDatabaseName, szDataDirectory );
}


//-------------------------------------
//^ The RestoreDatabase function is called to restore the database.
//< returns true if successfully created.  Returns false otherwise.
//
bool vdbDatabaseRestore::RestoreDatabase( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus, const char* szDatabaseName, const char* szDestinationDir )
{
	assert( pStatus != 0 );
	assert( pServer != 0 );

	// make sure that there isn't a database with this name already
	if ( pServer->DatabaseExists( szDatabaseName ) )
	{
		pStatus->Message( "There is already a database with this name.\n\nPlease choose a unique database name." );
		return false;
	}

	// restore the database
	if ( pServer->RestoreDatabase( _sBackupFile, szDatabaseName, szDestinationDir ) != DBS_OK )
	{
		pStatus->Message( "Unable to restore the database." );
		return false;
	}

	return true;
}


//-------------------------------------
//^ The CreateAuthorization function is called to create the authorization login.
//< returns true if successfully created.  Returns false otherwise.
//
bool vdbDatabaseRestore::CreateAuthorization( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus )
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
bool vdbDatabaseRestore::CreateAuthorization( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus, const char* szAuthorizationID, const char* szAuthorizationPassword, const char* szDatabaseName )
{
	assert( pStatus != 0 );
	assert( pServer != 0 );

	// add a login id and assign the Restore Database as its default login database
	// failure to AddLogin is not fatal if the login already exists
	pStatus->Message( "Creating login ID" );
	pServer->AddLogin( szAuthorizationID, szAuthorizationPassword, "master" );
	
	// use the Restore Database
	if ( pServer->Use( szDatabaseName ) != DBS_OK )
	{
		pStatus->Message( "Unable to USE the restored Database." );
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


