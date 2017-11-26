//=============================================================================
//
// CLASS:           ReportBuilderPage3
//
// ESSENCE:         Property page for choosing whether or not to filter records
//
// FILE NAME:       ReportBuilderPage3.cpp
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#ifndef __ReportBuilderPage3_h
#define __ReportBuilderPage3_h

#include "..\\..\\Vault\\vdbResources\\resource.h"

class ReportBuilderPageHolder;


class ReportBuilderPage3 : public CPropertyPage
{
public:
	ReportBuilderPage3( ReportBuilderPageHolder* pParent );
	~ReportBuilderPage3();

private:
	//{{AFX_DATA(ReportBuilderPage3)
	enum { IDD = IDD_REPORT_BUILDER3 };
	CButton	m_ctlYes;
	CButton	m_ctlNo;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(ReportBuilderPage3)
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(ReportBuilderPage3)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual bool MoveDataToHolder( bool bValidate );

private:
	ReportBuilderPageHolder* _pParent;
};


#endif __ReportBuilderPage3_h

