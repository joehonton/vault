//=============================================================================
//
// CLASS:           ReportBuilderPage6
//
// ESSENCE:         Property page for choosing whether or not to sort records
//
// FILE NAME:       ReportBuilderPage6.cpp
//
// INITIAL DATE:    10/26/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#ifndef __ReportBuilderPage6_h
#define __ReportBuilderPage6_h

#include "..\\..\\Vault\\vdbResources\\resource.h"

class ReportBuilderPageHolder;


class ReportBuilderPage6 : public CPropertyPage
{
public:
	ReportBuilderPage6( ReportBuilderPageHolder* pParent );
	~ReportBuilderPage6();

private:
	//{{AFX_DATA(ReportBuilderPage6)
	enum { IDD = IDD_REPORT_BUILDER6 };
	CButton	m_ctlYes;
	CButton	m_ctlNo;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(ReportBuilderPage6)
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(ReportBuilderPage6)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual bool MoveDataToHolder( bool bValidate );

private:
	ReportBuilderPageHolder* _pParent;
};


#endif __ReportBuilderPage6_h

