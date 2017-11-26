//=============================================================================
//
// CLASS:           ReportBuilderPage10
//
// ESSENCE:         Property page for specifying the title and filename
//
// FILE NAME:       ReportBuilderPage10.cpp
//
// INITIAL DATE:    10/26/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#ifndef __ReportBuilderPage10_h
#define __ReportBuilderPage10_h

#include "..\\..\\Vault\\vdbResources\\resource.h"

#ifndef __vdbFilename_h
	#include <vdbFilename.h>
#endif

class ReportBuilderPageHolder;


class ReportBuilderPage10 : public CPropertyPage
{
public:
	ReportBuilderPage10( ReportBuilderPageHolder* pParent );
	~ReportBuilderPage10();

private:
	//{{AFX_DATA(ReportBuilderPage10)
	enum { IDD = IDD_REPORT_BUILDER10 };
	CEdit	m_ctlTitle;
	CButton	m_ctlPreview;
	CEdit	m_ctlFilename;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(ReportBuilderPage10)
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardBack();
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(ReportBuilderPage10)
	virtual BOOL OnInitDialog();
	afx_msg void OnPreview();
	afx_msg void OnChangeTitle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual bool MoveDataToHolder( bool bValidate );
	void WriteDefinitionFile();

private:
	ReportBuilderPageHolder* _pParent;
	vdbFilename _sReportDefinitionDirectory;
};


#endif __ReportBuilderPage10_h

