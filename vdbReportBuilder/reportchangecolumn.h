//=============================================================================
//
// CLASS:           ReportChangeColumn
//
// OVERVIEW:        Changes the display name and width of a column
//
// FILE NAME:       ReportChangeColumn.cpp
//
// INITIAL DATE:    11/22/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#include "vdbDLLDialog.h"
#include "..\\..\\Vault\\vdbResources\\resource.h"


//-------------------------------------
class ReportChangeColumn : public vdbDLLDialog
{
public:
	ReportChangeColumn(CWnd* pParent = NULL);

	//{{AFX_DATA(ReportChangeColumn)
	enum { IDD = IDD_REPORT_CHANGE_COLUMN };
	CEdit	m_ctlDisplayName;
	CEdit	m_ctlDisplayWidth;
	CEdit	m_ctlFormatString;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(ReportChangeColumn)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	// Generated message map functions
	//{{AFX_MSG(ReportChangeColumn)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeDisplayName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CString _sDisplayName;
	int _displayWidth;
	CString _sFormatString;
};
