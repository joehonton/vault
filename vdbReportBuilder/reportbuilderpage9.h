//=============================================================================
//
// CLASS:           ReportBuilderPage9
//
// ESSENCE:         Property page for specifying the header footer and margin texts
//
// FILE NAME:       ReportBuilderPage9.cpp
//
// INITIAL DATE:    4/28/00
//
// COPYRIGHT:       Copyright © 2000 Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#ifndef __ReportBuilderPage9_h
#define __ReportBuilderPage9_h

#include "..\\..\\Vault\\vdbResources\\resource.h"

class ReportBuilderPageHolder;


class ReportBuilderPage9 : public CPropertyPage
{
public:
	ReportBuilderPage9( ReportBuilderPageHolder* pParent );
	~ReportBuilderPage9();

private:
	//{{AFX_DATA(ReportBuilderPage9)
	enum { IDD = IDD_REPORT_BUILDER9 };
	CEdit	m_ctlHeader;
	CButton	m_ctlHeaderCommands;
	CEdit	m_ctlFooter;
	CButton	m_ctlFooterCommands;
	CEdit	m_ctlLeftMargin;
	CButton	m_ctlLeftMarginCommands;
	CEdit	m_ctlRightMargin;
	CButton	m_ctlRightMarginCommands;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(ReportBuilderPage9)
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(ReportBuilderPage9)
	virtual BOOL OnInitDialog();
	afx_msg void OnHeaderCommands();
	afx_msg void OnFooterCommands();
	afx_msg void OnLeftMarginCommands();
	afx_msg void OnRightMarginCommands();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual bool MoveDataToHolder( bool bValidate );

private:
	ReportBuilderPageHolder* _pParent;
};


#endif __ReportBuilderPage9_h

