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

#ifndef __vdbDatabaseAttach_h
#define __vdbDatabaseAttach_h

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


class vdbDatabaseAttach : public vdbDLLDialog
{
public:
	vdbDatabaseAttach( vdbDatabase* pDB, CWnd* pParent = NULL );
	virtual ~vdbDatabaseAttach();

	//{{AFX_DATA(vdbDatabaseAttach)
	enum { IDD = IDD_DATABASE_ATTACH };
	CEdit	m_ctlDataFile;
	vdbDLLBitmapButton	m_ctlSelectDataFile;
	CEdit	m_ctlLogFile;
	vdbDLLBitmapButton	m_ctlSelectLogFile;
	CEdit	m_ctlDataSourceName;
	CEdit	m_ctlDescription;
	CEdit	m_ctlDatabaseName;
	CEdit	m_ctlAuthorizationID;
	CEdit	m_ctlAuthorizationPassword;
	CButton	m_ctlOK;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(vdbDatabaseAttach)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

private:
	//{{AFX_MSG(vdbDatabaseAttach)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelectDataFile();
	afx_msg void OnSelectLogFile();
	afx_msg void OnChangeDataFile();
	afx_msg void OnChangeDataSourceName();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void EnableControls( bool b );

private:
	// restore database
	bool Execute();
	bool CreateDataSource( vdbStatusWindow* pStatus );
	bool CreateDataSource( vdbStatusWindow* pStatus, const char* szDataSource, const char* szDescription, const char* szDatabaseName );
	bool DeleteDataSource( vdbStatusWindow* pStatus );
	vdbMicrosoftSQLServer* ConnectAsAdministrator( vdbStatusWindow* pStatus );
	bool AttachDatabase( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus );
	bool AttachDatabase( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus, const char* szDatabaseName );
	bool CreateAuthorization( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus );
	bool CreateAuthorization( vdbMicrosoftSQLServer* pServer, vdbStatusWindow* pStatus, const char* szAuthorizationID, const char* szAuthorizationPassword, const char* szDatabaseName );

public:
	vdbString _sDataSource;		// passed back to caller upon OK
	vdbString _sDescription;	// passed back to caller upon OK
	vdbDatabase* _pDB;			// a derived class of vdbDatabase
	vdbFilename _sDataFile;		// the MDF file to attach. Selected by the user
	vdbFilename _sLogFile;		// the LDF file to attach. Selected by the user
};

#endif __vdbDatabaseAttach_h

