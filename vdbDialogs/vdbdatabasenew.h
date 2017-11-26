//=============================================================================
//
// CLASS:           vdbDatabaseNew
//
// ESSENCE:         Create a new ODBC data source and SQL Server Desktop Engine
//                  devices and tables
//
// FILE NAME:       vdbDatabaseNew.h
//
// INITIAL DATE:    3/6/00
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __vdbDatabaseNew_h
#define __vdbDatabaseNew_h

#include "vdbDLLDialog.h"
#include "..\\vdbResources\\resource.h"

#ifndef __vdbDLLBitmapButton_h
	#include "vdbDLLBitmapButton.h"
#endif

#ifndef __vdbString_h
	#include <vdbString.h>
#endif

class vdbStatusWindow;
class vdbMicrosoftSQLServer;
class vdbDatabase;


class vdbDatabaseNew : public vdbDLLDialog
{
public:
	vdbDatabaseNew( vdbDatabase* pDB, CWnd* pParent = NULL );
	virtual ~vdbDatabaseNew();

	//{{AFX_DATA(vdbDatabaseNew)
	enum { IDD = IDD_DATABASE_NEW };
	CEdit	m_ctlDataDirectory;
	CEdit	m_ctlLogSize;
	CEdit	m_ctlAuthorizationPassword;
	CEdit	m_ctlAuthorizationID;
	CEdit	m_ctlDataSize;
	CEdit	m_ctlDatabaseName;
	CEdit	m_ctlDescription;
	CEdit	m_ctlDataSourceName;
	vdbDLLBitmapButton	m_ctlSelectDirectory;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(vdbDatabaseNew)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

private:
	//{{AFX_MSG(vdbDatabaseNew)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeDataSourceName();
	afx_msg void OnSelectDirectory();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	// creation and initialization of database
	bool Execute();
	bool CreateDataSource( vdbStatusWindow* pStatus );
	bool DeleteDataSource( vdbStatusWindow* pStatus );
	vdbMicrosoftSQLServer* ConnectAsAdministrator( vdbStatusWindow* pStatus );
	bool CreateDatabase( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus );
	bool CreateAuthorization( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus );
	bool InitializeDatabase( vdbStatusWindow* pStatus );

public:
	vdbString _sDataSource;		// passed back to caller upon OK
	vdbString _sDescription;	// passed back to caller upon OK
	vdbDatabase* _pDB;			// a derived class of vdbDatabase whose CreateUserTables function will be called
};

#endif __vdbDatabaseNew_h

