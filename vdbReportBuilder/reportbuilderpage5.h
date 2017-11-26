//=============================================================================
//
// CLASS:           ReportBuilderPage5
//
// ESSENCE:         Property page for showing all defined filters 
//
// FILE NAME:       ReportBuilderPage5.cpp
//
// INITIAL DATE:    10/26/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#ifndef __ReportBuilderPage5_h
#define __ReportBuilderPage5_h

#include "..\\..\\Vault\\vdbResources\\resource.h"

class ReportBuilderPageHolder;


class ReportBuilderPage5 : public CPropertyPage
{
public:
	ReportBuilderPage5( ReportBuilderPageHolder* pParent );
	~ReportBuilderPage5();

private:
	//{{AFX_DATA(ReportBuilderPage5)
	enum { IDD = IDD_REPORT_BUILDER5 };
	CListBox	m_ctlFilters;
	CButton	m_ctlRemove;
	CButton	m_ctlAndOr;
	CButton	m_ctlYes;
	CButton	m_ctlNo;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(ReportBuilderPage5)
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(ReportBuilderPage5)
	virtual BOOL OnInitDialog();
	afx_msg void OnRemove();
	afx_msg void OnSelchangeFilters();
	afx_msg void OnAndOr();
	afx_msg void OnNoThankYou();
	afx_msg void OnYesPlease();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual bool MoveDataToHolder( bool bValidate );
	void BuildFilterList();

private:
	ReportBuilderPageHolder* _pParent;
};


#endif __ReportBuilderPage5_h

