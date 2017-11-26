//=============================================================================
//
// CLASS:           ReportBuilderPage7
//
// ESSENCE:         Property page for choosing which column to use as a sort
//
// FILE NAME:       ReportBuilderPage7.cpp
//
// INITIAL DATE:    10/26/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#ifndef __ReportBuilderPage7_h
#define __ReportBuilderPage7_h

#include "..\\..\\Vault\\vdbResources\\resource.h"

#ifndef __vdbString_h
	#include <vdbString_h>
#endif

class ReportBuilderPageHolder;


class ReportBuilderPage7 : public CPropertyPage
{
public:
	ReportBuilderPage7( ReportBuilderPageHolder* pParent );
	~ReportBuilderPage7();

private:
	//{{AFX_DATA(ReportBuilderPage7)
	enum { IDD = IDD_REPORT_BUILDER7 };
	CButton	m_ctlAscending;
	CButton	m_ctlDescending;
	CListBox	m_ctlColumns;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(ReportBuilderPage7)
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(ReportBuilderPage7)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeColumns();
	afx_msg void OnAscending();
	afx_msg void OnDescending();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual bool MoveDataToHolder( bool bValidate );

private:
	void GetAllColumns();
		
private:
	ReportBuilderPageHolder* _pParent;
	vdbString _sColumnName;		// the column selected by the user
	vdbString _sOrder;			// ascending or descending
};


#endif __ReportBuilderPage7_h

