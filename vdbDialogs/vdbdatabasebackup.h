//=============================================================================
//
// CLASS:           vdbDatabaseBackup
//
// ESSENCE:         Backup a SQL Server Desktop Engine database
//
// FILE NAME:       vdbDatabaseBackup.h
//
// INITIAL DATE:    7/11/00
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __vdbDatabaseBackup_h
#define __vdbDatabaseBackup_h

#include "vdbDLLDialog.h"
#include "..\\vdbResources\\resource.h"

#ifndef __vdbDLLBitmapButton_h
	#include "vdbDLLBitmapButton.h"
#endif

#ifndef __vdbFilename_h
	#include <vdbFilename.h>
#endif

class vdbStatusWindow;
class vdbMicrosoftSQLServer;
class vdbDatabase;


class vdbDatabaseBackup : public vdbDLLDialog
{
public:
	vdbDatabaseBackup( vdbDatabase* pDB, const char* szDataSource, const char* szBackupFilename, CWnd* pParent = NULL );
	virtual ~vdbDatabaseBackup();

	//{{AFX_DATA(vdbDatabaseBackup)
	enum { IDD = IDD_DATABASE_BACKUP };
	CEdit	m_ctlFilename;
	vdbDLLBitmapButton	m_ctlSelectDirectory;
	CStatic m_ctlDescription;
	CButton m_ctlOK;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(vdbDatabaseBackup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

private:
	//{{AFX_MSG(vdbDatabaseBackup)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelectDirectory();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	// creation and initialization of database
	bool Execute();
	bool Backup( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus );

public:
	vdbDatabase* _pDB;					// a derived class of vdbDatabase
	vdbString _sDataSource;				// the ODBC datasource name
	vdbFilename _sBackupFilename;		// where to put the backup
	vdbString _sDatabaseName;			// the SQL Server database name
	unsigned long _databaseSize;		// number of Mb in the database;	
};

#endif __vdbDatabaseBackup_h

