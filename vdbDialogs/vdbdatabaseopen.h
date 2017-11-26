//=============================================================================
//
// CLASS:           vdbDatabaseOpen
//
// ESSENCE:         Select a database to open
//
// FILE NAME:       vdbDatabaseOpen.h
//
// INITIAL DATE:    6/14/00
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __vdbDatabaseOpen_h
#define __vdbDatabaseOpen_h

#include "vdbDLLDialog.h"
#include "..\\vdbResources\\resource.h"

#ifndef __pphListCtrl_h
	#include "pphListCtrl.h"
#endif

#ifndef __CommanderControl_h
	#include <CommanderControl.h>
#endif

#ifndef __vdbString_h
	#include <vdbString.h>
#endif

#ifndef __vdbSQL_h
	#include <vdbSQL.h>
#endif

class vdbDatabase;

//-------------------------------------
// global function
RETCODE ListDataSources( char szSources[][SQL_MAX_DSN_LENGTH+1], char szDescriptions[][255], int& numSources );


//-------------------------------------
class vdbDatabaseOpen : public vdbDLLDialog
{
public:
	vdbDatabaseOpen( const char* szDatasource, vdbDatabase* pDB = 0, CWnd* pParent = NULL );
	virtual ~vdbDatabaseOpen();

	//{{AFX_DATA(vdbDatabaseOpen)
	enum { IDD = IDD_DATABASE_OPEN };
	pphListCtrl m_ctlDataSources;
	CommanderControl	m_ctlOK;
	CommanderControl	m_ctlNew;
	CommanderControl	m_ctlCancel;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(vdbDatabaseOpen)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

private:
	//{{AFX_MSG(vdbDatabaseOpen)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedDataSource(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblClkDataSource(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNew();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	vdbDatabase* _pDB;			// if supplied, then used for creating a new database
	vdbString _sDataSource;		// passed in as the default selection, passed back to caller upon OK
	CBitmap _listBitmap;
	CImageList _listImage;
};

// Automatically link to Commander.lib and cbcProxy.lib
#pragma comment(linker,"/defaultlib:Commander.lib")
#pragma comment(linker,"/defaultlib:cbcProxy.lib")

#endif __vdbDatabaseOpen_h

