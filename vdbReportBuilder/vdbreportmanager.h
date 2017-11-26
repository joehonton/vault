//=============================================================================
//
// CLASS:           vdbReportManager
//
// ESSENCE:         Add/duplicate/modify/delete reports
//
// FILE NAME:       vdbReportManager.h
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#ifndef __vdbReportManager_h
#define __vdbReportManager_h

#include "vdbDLLDialog.h"

#include "..\\..\\Vault\\vdbResources\\resource.h"

#ifndef __vdbMap_h
	#include <vdbMap.h>
#endif

#ifndef __ReportDefinition_h
	#include "ReportDefinition.h"
#endif


//-------------------------------------
class vdbReportManager : public vdbDLLDialog
{
public:
	vdbReportManager();

	//{{AFX_DATA(vdbReportManager)
	enum { IDD = IDD_REPORT_MANAGER };
	CButton	m_ctlNew;
	CListBox	m_ctlList;
	CButton	m_ctlDuplicate;
	CButton	m_ctlModify;
	CButton	m_ctlDelete;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(vdbReportManager)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(vdbReportManager)
	afx_msg void OnNew();
	afx_msg void OnDuplicate();
	afx_msg void OnDelete();
	afx_msg void OnModify();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList();
	afx_msg void OnPreview();
	afx_msg void OnDblclkList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void AddOneFile( const char* szFilename );
	void RefreshButtonStates();

private:
	vdbMap<ReportDefinition> _reportDefinitions;
};


#endif //__vdbReportManager_h
