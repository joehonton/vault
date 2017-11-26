//=============================================================================
//
// CLASS:           ReportBuilderPage2
//
// ESSENCE:         Property page for choosing which columns to include in the report
//
// FILE NAME:       ReportBuilderPage2.cpp
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#ifndef __ReportBuilderPage2_h
#define __ReportBuilderPage2_h

#include "..\\..\\Vault\\vdbResources\\resource.h"

#ifndef __pphListCtrl_h
	#include <pphListCtrl.h>
#endif

#ifndef __vdbString_h
	#include <vdbString_h>
#endif

class ReportBuilderPageHolder;


class ReportBuilderPage2 : public CPropertyPage
{
public:
	ReportBuilderPage2( ReportBuilderPageHolder* pParent );
	~ReportBuilderPage2();

private:
	//{{AFX_DATA(ReportBuilderPage2)
	enum { IDD = IDD_REPORT_BUILDER2 };
	CButton	m_ctlChange;
	CButton	m_ctlInclude;
	CButton	m_ctlExclude;
	pphListCtrl	m_ctlColumns;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(ReportBuilderPage2)
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(ReportBuilderPage2)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnExclude();
	afx_msg void OnInclude();
	afx_msg void OnDblclkColumns(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedColumns(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual bool MoveDataToHolder( bool bValidate );

private:
	void GetAllColumns();
	void ConvertToOID( const char* szColumnName, vdbString& sVaultType );
		
private:
	ReportBuilderPageHolder* _pParent;
	CBitmap _listBitmap;
	CImageList _listImage;
};


#endif __ReportBuilderPage2_h

