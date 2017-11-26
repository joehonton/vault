//=============================================================================
//
// CLASS:           vdbDatabaseDelete
//
// ESSENCE:         Drop a SQL Server Desktop Engine database, delete the data and
//                  log devices, and remove the ODBC data source.
//
// FILE NAME:       vdbDatabaseDelete.h
//
// INITIAL DATE:    3/7/00
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __vdbDatabaseDelete_h
#define __vdbDatabaseDelete_h

#include "vdbDLLDialog.h"
#include "..\\vdbResources\\resource.h"

#ifndef __vdbString_h
	#include <vdbString.h>
#endif

class vdbStatusWindow;
class vdbMicrosoftSQLServer;
class vdbDatabase;


class vdbDatabaseDelete : public vdbDLLDialog
{
public:
	vdbDatabaseDelete( vdbDatabase* pDB, const char* szDataSourceName, CWnd* pParent = NULL );
	virtual ~vdbDatabaseDelete();

	//{{AFX_DATA(vdbDatabaseDelete)
	enum { IDD = IDD_DATABASE_DELETE };
	CButton	m_ctlOK;
	CEdit	m_ctlDatabaseName;
	CEdit	m_ctlDataSourceName;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(vdbDatabaseDelete)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

private:
	//{{AFX_MSG(vdbDatabaseDelete)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	// creation and initialization of database
	bool Execute();
	vdbMicrosoftSQLServer* ConnectAsAdministrator( vdbStatusWindow* pStatus );
	bool DropDatabase( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus );
	bool DeleteDataSource( vdbStatusWindow* pStatus );

private:
	vdbString _sDataSource;
	vdbString _sDatabaseName;
	bool _bValid;					// did we find the database name?
	vdbDatabase* _pDB;				// only used to ConnectAsAdministrator.  The real work is done by vdbMicrosoftSQLServer
};

#endif __vdbDatabaseDelete_h

