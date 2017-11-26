//=============================================================================
//
// CLASS:           ReportBuilderPage1
//
// ESSENCE:         Property page for choosing which table to use
//
// FILE NAME:       ReportBuilderPage1.cpp
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#ifndef __ReportBuilderPage1_h
#define __ReportBuilderPage1_h

#include "..\\..\\Vault\\vdbResources\\resource.h"

class ReportBuilderPageHolder;


class ReportBuilderPage1 : public CPropertyPage
{
public:
	ReportBuilderPage1( ReportBuilderPageHolder* pParent );
	~ReportBuilderPage1();

private:
	//{{AFX_DATA(ReportBuilderPage1)
	enum { IDD = IDD_REPORT_BUILDER1 };
	CListBox	m_ctlTables;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(ReportBuilderPage1)
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(ReportBuilderPage1)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkTables();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual bool MoveDataToHolder( bool bValidate );

private:
	void GetAllTables();

private:
	ReportBuilderPageHolder* _pParent;
};


#endif __ReportBuilderPage1_h

