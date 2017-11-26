//=============================================================================
//
// CLASS:           ReportBuilderPage8
//
// ESSENCE:         Property page for showing all defined sorts 
//
// FILE NAME:       ReportBuilderPage8.cpp
//
// INITIAL DATE:    10/26/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#ifndef __ReportBuilderPage8_h
#define __ReportBuilderPage8_h

#include "..\\..\\Vault\\vdbResources\\resource.h"

class ReportBuilderPageHolder;


class ReportBuilderPage8 : public CPropertyPage
{
public:
	ReportBuilderPage8( ReportBuilderPageHolder* pParent );
	~ReportBuilderPage8();

private:
	//{{AFX_DATA(ReportBuilderPage8)
	enum { IDD = IDD_REPORT_BUILDER8 };
	CListBox	m_ctlSorts;
	CButton	m_ctlRemove;
	CButton	m_ctlYes;
	CButton	m_ctlNo;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(ReportBuilderPage8)
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(ReportBuilderPage8)
	virtual BOOL OnInitDialog();
	afx_msg void OnRemove();
	afx_msg void OnSelchangeSorts();
	afx_msg void OnNoThankYou();
	afx_msg void OnYesPlease();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual bool MoveDataToHolder( bool bValidate );
	void BuildSortList();

private:
	ReportBuilderPageHolder* _pParent;
};


#endif __ReportBuilderPage8_h

