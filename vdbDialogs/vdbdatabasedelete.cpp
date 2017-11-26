//=============================================================================
//
// CLASS:           vdbDatabaseDelete
//
// ESSENCE:         Drop a SQL Server Desktop Engine database, delete the data and
//                  log devices, and remove the ODBC data source.
//
// FILE NAME:       vdbDatabaseDelete.cpp
//
// INITIAL DATE:    3/7/00
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#include <vdbWindows.h>
#include "vdbDatabaseDelete.h"
#include <vdbDatabase.h>
#include <vdbUtility.h>
#include <vdbFilename.h>
#include <ODBCinst.h>
#include "vdbStatusWindow.h"
#include <vdbMicrosoftSQLServer.h>
#include <vdbRegistrationKey.h>


//=============================================================================
// Essential class functions
//=============================================================================

//-------------------------------------
vdbDatabaseDelete::vdbDatabaseDelete( vdbDatabase* pDB, const char* szDataSourceName, CWnd* pParent )
	: vdbDLLDialog(vdbDatabaseDelete::IDD, pParent),
	  _pDB( pDB ),
	  _sDataSource( szDataSourceName ),
	  _bValid( false )
{
	//{{AFX_DATA_INIT(vdbDatabaseDelete)
	//}}AFX_DATA_INIT
}


//-------------------------------------
vdbDatabaseDelete::~vdbDatabaseDelete()
{
}


//-------------------------------------
void vdbDatabaseDelete::DoDataExchange(CDataExchange* pDX)
{
	vdbDLLDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(vdbDatabaseDelete)
	DDX_Control(pDX, IDOK, m_ctlOK);
	DDX_Control(pDX, IDC_VDB_DATABASE_NAME, m_ctlDatabaseName);
	DDX_Control(pDX, IDC_VDB_DATASOURCE_NAME, m_ctlDataSourceName);
	//}}AFX_DATA_MAP
}


//-------------------------------------
BEGIN_MESSAGE_MAP(vdbDatabaseDelete, vdbDLLDialog)
	//{{AFX_MSG_MAP(vdbDatabaseDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



//=============================================================================
// Window creation and intialization
//=============================================================================

//-------------------------------------
BOOL vdbDatabaseDelete::OnInitDialog() 
{
	vdbDLLDialog::OnInitDialog();
	
	_bValid = true;
	vdbString sSubkey;
	sSubkey.Format( "Software\\ODBC\\ODBC.INI\\%s", (const char*) _sDataSource );
	
	// get the database name associated with this data source
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

	m_ctlDataSourceName.SetWindowText( _sDataSource );
	m_ctlDatabaseName.SetWindowText( _sDatabaseName );
		
	return TRUE;
}


//=============================================================================
// Functions to create and initialize the database
//=============================================================================

//-------------------------------------
void vdbDatabaseDelete::OnOK() 
{
	if ( _bValid == false )
		return;

	if ( Execute() == true )
		vdbDLLDialog::OnOK();
	else
		vdbDLLDialog::OnCancel();
}


//-------------------------------------
//^ The Execute function is called by OnOK to execute the complete script.
//< returns true if successful.  Returns false otherwise.
//
bool vdbDatabaseDelete::Execute()
{
	bool rc = true;
	vdbStatusWindow* pStatus = new vdbStatusWindow( "Delete database" );
	pStatus->ShowWindow( SW_SHOW );

	// These database command generate alot of noise, run quietly,
	_pDB->QuenchErrors( true );

	vdbMicrosoftSQLServer* pServer = 0;
	pStatus->Message( "Connecting to server." );
	pServer = ConnectAsAdministrator( pStatus );
	if ( pServer == 0 )
		rc = false;

	if ( rc == true )
	{
		pStatus->Message( "Deleting database" );
		rc = DropDatabase( pServer, pStatus );
	}
	
	if ( rc == true )
	{
		pStatus->Message( "Deleting ODBC data source" );
		DeleteDataSource( pStatus );
	}

	_pDB->QuenchErrors( false );
	if( rc == false )
	{
		pStatus->ShowCloseButton();
	}
	else
	{
		pStatus->Message( "Database deleted" );
		pStatus->DestroyWindow();
		delete pStatus; pStatus = 0;
	}

	_pDB->Disconnect();
	return rc;
}


//-------------------------------------
//^ The ConnectAsAdministrator function.
//< returns a pointer to the SQL Server Desktop Engine instance if successful.
//< returns a null pointer if it fails.
//
vdbMicrosoftSQLServer* vdbDatabaseDelete::ConnectAsAdministrator( vdbStatusWindow* pStatus )
{
	if ( _pDB->ConnectAsAdministrator() != DBS_OK )
	{
		pStatus->Message( "Unable to connect to SQL Server Desktop Engine.  Make sure that the Desktop Engine has been installed and that the \"SQL Server service\" is running." );
		return NULL;
	}

	return (vdbMicrosoftSQLServer*) _pDB->Server();
}



//-------------------------------------
//^ The DropDatabase function is called to drop the database.
//< returns true if successfully dropped.  Returns false otherwise.
//
bool vdbDatabaseDelete::DropDatabase( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus )
{
	assert( pStatus != 0 );
	assert( pServer != 0 );

	// must be in the Master to drop
	if ( pServer->Use( "master" ) != DBS_OK )
	{
		pStatus->Message( "Unable USE master" );
		return false;
	}
 	
	// drop the database
	if ( pServer->DropDatabase( _sDatabaseName ) != DBS_OK )
	{
		pStatus->Message( "Unable to delete the database.  Check to make sure that the database is not in use.\r\n\r\nIf all else fails use the Control Panel ODBC Manager to remove the data source, then use the Explorer to delete the database files." );
		return false;
	}

	return true;
}


//-------------------------------------
//^ The DeleteDataSource function is called to delete an ODBC Data Source.
//< returns true if data source successfully deleted.  Returns false otherwise.
//
bool vdbDatabaseDelete::DeleteDataSource( vdbStatusWindow* pStatus )
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

