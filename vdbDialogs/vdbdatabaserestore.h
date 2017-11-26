//=============================================================================
//
// CLASS:           vdbDatabaseRestore
//
// ESSENCE:         Restore a database backup and create a new ODBC data source
//
// FILE NAME:       vdbDatabaseRestore.h
//
// INITIAL DATE:    6/12/00
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __vdbDatabaseRestore_h
#define __vdbDatabaseRestore_h

#include "vdbDLLDialog.h"
#include "..\\vdbResources\\resource.h"

#ifndef __vdbDLLBitmapButton_h
	#include "vdbDLLBitmapButton.h"
#endif

#ifndef __pphListCtrl_h
	#include "pphListCtrl.h"
#endif

#ifndef __vdbFilename_h
	#include <vdbFilename.h>
#endif

class vdbStatusWindow;
class vdbMicrosoftSQLServer;
class vdbDatabase;


class vdbDatabaseRestore : public vdbDLLDialog
{
public:
	vdbDatabaseRestore( vdbDatabase* pDB, CWnd* pParent = NULL );
	virtual ~vdbDatabaseRestore();

	//{{AFX_DATA(vdbDatabaseRestore)
	enum { IDD = IDD_DATABASE_RESTORE };
	CButton	m_ctlChooseRecent;
	CButton	m_ctlChooseAlternate;
	pphListCtrl m_ctlRecentBackups;
	CEdit	m_ctlBackupFile;
	vdbDLLBitmapButton	m_ctlSelectBackupFile;
	CEdit	m_ctlDataDirectory;
	vdbDLLBitmapButton	m_ctlSelectDataDirectory;
	CEdit	m_ctlDataSourceName;
	CEdit	m_ctlDescription;
	CEdit	m_ctlDatabaseName;
	CEdit	m_ctlAuthorizationID;
	CEdit	m_ctlAuthorizationPassword;
	CButton	m_ctlOK;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(vdbDatabaseRestore)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

private:
	//{{AFX_MSG(vdbDatabaseRestore)
	virtual BOOL OnInitDialog();
	afx_msg void OnChooseRecent();
	afx_msg void OnChooseAlternate();
	afx_msg void OnSelchangedRecentBackups(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeBackupFile();
	afx_msg void OnSelectBackupFile();
	afx_msg void OnChangeDataSourceName();
	afx_msg void OnSelectDataDirectory();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void EnableControls( bool b );

public:
	bool SilentRestore( const char* szDataSource, const char* szDatabaseName, const char* szDescription, const char* szBackupDir, const char* szBackupFile, const char* szAuthorizationID, const char* szAuthorizationPassword );

private:
	// restore database
	bool Execute();
	bool CreateDataSource( vdbStatusWindow* pStatus );
	bool CreateDataSource( vdbStatusWindow* pStatus, const char* szDataSource, const char* szDescription, const char* szDatabaseName );
	vdbMicrosoftSQLServer* ConnectAsAdministrator( vdbStatusWindow* pStatus );
	bool RestoreDatabase( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus );
	bool RestoreDatabase( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus, const char* szDatabaseName, const char* szDestinationDir );
	bool CreateAuthorization( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus );
	bool CreateAuthorization( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus, const char* szAuthorizationID, const char* szAuthorizationPassword, const char* szDatabaseName );

public:
	vdbString _sDataSource;		// passed back to caller upon OK
	vdbString _sDescription;	// passed back to caller upon OK
	vdbDatabase* _pDB;			// a derived class of vdbDatabase whose CreateUserTables function will be called
	vdbFilename _sBackupFile;	// the file to restore.  Selected by the user
	CBitmap _listBitmap;
	CImageList _listImage;
};

#endif __vdbDatabaseRestore_h

