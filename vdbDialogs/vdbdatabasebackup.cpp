//=============================================================================
//
// CLASS:           vdbDatabaseBackup
//
// ESSENCE:         Backup a SQL Server Desktop Engine database
//
// FILE NAME:       vdbDatabaseBackup.cpp
//
// INITIAL DATE:    7/11/00
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#include <vdbWindows.h>
#include "vdbDatabaseBackup.h"
#include <vdbDatabase.h>
#include <vdbUtility.h>
#include <vdbFilename.h>
#include "vdbStatusWindow.h"
#include <vdbMicrosoftSQLServer.h>
#include <vdbAssert.h>
#include <vdbString.h>
#include <vdbDirectoryDialog.h>
#include <pphFormatBytes.h>
#include <vdbRegistrationKey.h>
#include <vdbApp.h>
#include <vdbStatement.h>


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
vdbDatabaseBackup::vdbDatabaseBackup( vdbDatabase* pDB, const char* szDataSource, const char* szBackupFilename, CWnd* pParent )
	: vdbDLLDialog(vdbDatabaseBackup::IDD, pParent),
	  _pDB( pDB ),
	  _sDataSource( szDataSource ),
	  _sBackupFilename( szBackupFilename ),
	  _databaseSize( 0 )
{
	assert( _pDB != 0 );
	assert( _sDataSource.IsNotNull() );
	m_ctlSelectDirectory.SetBitmaps(IDB_VDB_DIRECTORY_UP, IDB_VDB_DIRECTORY_DOWN, IDB_VDB_DIRECTORY_FOCUS, IDB_VDB_DIRECTORY_DISABLED );

	//{{AFX_DATA_INIT(vdbDatabaseBackup)
	//}}AFX_DATA_INIT
}


//-------------------------------------
vdbDatabaseBackup::~vdbDatabaseBackup()
{
}


//-------------------------------------
void vdbDatabaseBackup::DoDataExchange(CDataExchange* pDX)
{
	vdbDLLDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(vdbDatabaseBackup)
	DDX_Control(pDX, IDC_VDB_FILENAME, m_ctlFilename);
	DDX_Control(pDX, IDC_VDB_DIRECTORY, m_ctlSelectDirectory);
	DDX_Control(pDX, IDC_VDB_DESCRIPTION, m_ctlDescription );
	DDX_Control(pDX, IDOK, m_ctlOK);
	//}}AFX_DATA_MAP
}


//-------------------------------------
BEGIN_MESSAGE_MAP(vdbDatabaseBackup, vdbDLLDialog)
	//{{AFX_MSG_MAP(vdbDatabaseBackup)
	ON_BN_CLICKED(IDC_VDB_DIRECTORY, OnSelectDirectory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



//=============================================================================
// Window creation and intialization
//=============================================================================

//-------------------------------------
BOOL vdbDatabaseBackup::OnInitDialog() 
{
	vdbDLLDialog::OnInitDialog();
	
	m_ctlSelectDirectory.SizeToContent();

	m_ctlFilename.SetLimitText( 256 );
	m_ctlFilename.SetWindowText( _sBackupFilename );

	// get the database name associated with this data source
	bool _bValid = true;
	vdbString sSubkey;
	sSubkey.Format( "Software\\ODBC\\ODBC.INI\\%s", (const char*) _sDataSource );
	
	vdbRegistrationKey key1( HKEY_CURRENT_USER, sSubkey, "Database" );
	if ( key1.GetValue( _sDatabaseName ) == false )
	{
		vdbRegistrationKey key2( HKEY_LOCAL_MACHINE, sSubkey, "Database" );
		if ( key2.GetValue( _sDatabaseName ) == false )
		{
			_sDatabaseName = "Unable to determine database name";
			_bValid = false;
			m_ctlOK.EnableWindow( FALSE );
		}
	}

	if ( !_pDB->IsConnected() )
		_pDB->ConnectAsAdministrator( _sDataSource );

	// get the size of the database
	_databaseSize = 0;
	if ( _pDB->IsConnected() )
	{
		vdbString sSQL;
		sSQL.Format( "sp_helpdb %s", (const char*) _sDatabaseName );
		vdbStatement stmt( _pDB );
		RETCODE rc = stmt.Execute( sSQL );
		if ( rc == SQL_SUCCESS )
		{
			char szSize[20];
			stmt.Bind( 2, szSize, sizeof(szSize) );
			rc = stmt.Fetch();																
			if ( rc == SQL_SUCCESS )
				_databaseSize = atoi( szSize );
		}
	}

	if ( _databaseSize != 0 )
	{
		vdbString s;
		s.Format( "NOTE:  You will need to use a disk with approximately %d Mb of available space.", _databaseSize );
		m_ctlDescription.SetWindowText( s );
	}
	else
		m_ctlDescription.SetWindowText( "Unable to determine the estimated size of the backup file." );
		
	return TRUE;
}


//=============================================================================
// Functions to create and initialize the database
//=============================================================================

//-------------------------------------
void vdbDatabaseBackup::OnOK() 
{
	char szBackupFilename[256];
	m_ctlFilename.GetWindowText( szBackupFilename, sizeof(szBackupFilename) );
	_sBackupFilename = szBackupFilename;

	// validate the drive letter
	char szDataDrive[4];
	m_ctlFilename.GetWindowText( szDataDrive, 4 );
	if ( szDataDrive[0] == 0x0)
		{ vdbMessageBox( NULL, "Please supply a location for the backup file.", "Backup Database", MB_OK ); return; }
	if ( isalpha( szDataDrive[0] ) == false )
		{ vdbMessageBox( NULL, "Please respecify the backup file location using a valid drive letter.", "Backup Database", MB_OK ); return; }
	if ( szDataDrive[1] != ':' || szDataDrive[2] != '\\' )
		{ vdbMessageBox( NULL, "Please respecify the backup file location using a colon and a backslash after the drive letter.", "Backup Database", MB_OK ); return; }

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

			if ( megabytesAvailable < _databaseSize )
			{
				vdbString s;
				s.Format( "The %s drive only has %d Mb available.  You may need as much as %d Mb to successfully backup your database.\n\nDo you want to try backing up to this location anyway?", szDataDrive, megabytesAvailable, _databaseSize );
				if ( vdbMessageBox( NULL, (const char*) s, "Backup Database", MB_YESNO ) == IDNO )
					return;
			}
		}
	}

	// now do it
	if ( Execute() == false )
		return;

	vdbDLLDialog::OnOK();
}


//-------------------------------------
//^ The Execute function is called by OnOK to execute the complete script.
//< returns true if successful.  Returns false otherwise.
//
bool vdbDatabaseBackup::Execute()
{
	bool rc = true;
	vdbStatusWindow* pStatus = new vdbStatusWindow( "Backup database" );
	pStatus->ShowWindow( SW_SHOW );

	// These database commands generate alot of noise, run quietly,
	_pDB->QuenchErrors( true );

	pStatus->Message( "Connecting to SQL Server Desktop Engine." );
	if ( _pDB->ConnectAsAdministrator( _sDataSource ) != DBS_OK )
	{
		rc = false;
		_pDB->QuenchErrors( false );
		pStatus->Message( "Could not connect to database." );
		pStatus->ShowCloseButton();
	}

	vdbMicrosoftSQLServer* pServer = 0;
	if ( rc == true )
	{
		pServer = (vdbMicrosoftSQLServer*) _pDB->Server();
		if ( pServer == 0 )
			rc = false;
	}

	pStatus->Message( "Backing up database." );
	rc = Backup( pServer, pStatus );

	_pDB->QuenchErrors( false );
	if ( rc == false )
	{
		pStatus->ShowCloseButton();
	}
	else
	{
		pStatus->Message( "Database backup successful" );
		pStatus->DestroyWindow();
		delete pStatus; pStatus = 0;
	}

	_pDB->Disconnect();
	return rc;
}


//-------------------------------------
//^ The Backup function is called to backup the database.
//< returns true if successful.  Returns false otherwise.
//
bool vdbDatabaseBackup::Backup( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus )
{
	assert( pStatus != 0 );
	assert( pServer != 0 );

	if ( pServer->BackupDatabase( _sDatabaseName, _sBackupFilename, true ) != DBS_OK )
	{
		pStatus->Message( "Unable to backup the database." );
		return false;
	}

	// save the backup filename to the registry if it is different from any previous name
	int num = 1;
	bool bMatchExisting = false;	
	vdbRegistrationKey key( HKEY_CURRENT_USER,	vdbApp::GetDatabaseRegistrationRoot(), "Backup01" );
	while ( key.LeafExists() && bMatchExisting == false )
	{
		// get the name of the backup under the existing key
		vdbString sExistingName;
		key.GetValue( sExistingName );

		if ( _sBackupFilename == sExistingName )
			bMatchExisting = true;
		else
		{
			vdbString sLeaf;
			sLeaf.Format( "Backup%02d", ++num );
			key.SetLeaf( sLeaf );
		}
	}
	if ( bMatchExisting == false )
		key.SetValue( _sBackupFilename );
	
	return true;
}


//-------------------------------------
void vdbDatabaseBackup::OnSelectDirectory() 
{
	char sz[256];
	m_ctlFilename.GetWindowText( sz, sizeof(sz) );
	vdbFilename sFullname( sz );

	vdbFilename sDirOnly( sFullname.Directory() );
	vdbFilename sFileOnly( sFullname.FilenameWithExtension() );

	// show directory dialog box
	vdbDirectoryDlgData data( m_hWnd, "Select the backup directory", sDirOnly, true );
	if ( vdbDirectoryDialog( data ) == false )
		return;

	sFullname = data.GetDirectory();
	sFullname.Append( sFileOnly );
	m_ctlFilename.SetWindowText( sFullname );	
}
