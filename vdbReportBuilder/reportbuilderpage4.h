//=============================================================================
//
// CLASS:           ReportBuilderPage4
//
// ESSENCE:         Property page for choosing which column to use as a filter
//
// FILE NAME:       ReportBuilderPage4.cpp
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#ifndef __ReportBuilderPage4_h
#define __ReportBuilderPage4_h

#include "..\\..\\Vault\\vdbResources\\resource.h"

#ifndef __vdbString_h
	#include <vdbString_h>
#endif

class ReportBuilderPageHolder;
class ReportOIDColumn;


class ReportBuilderPage4 : public CPropertyPage
{
public:
	ReportBuilderPage4( ReportBuilderPageHolder* pParent );
	~ReportBuilderPage4();

private:
	//{{AFX_DATA(ReportBuilderPage4)
	enum { IDD = IDD_REPORT_BUILDER4 };
	CListBox	m_ctlOperator;
	CListBox	m_ctlForeignList;
	CEdit	m_ctlComparisonValue;
	CListBox	m_ctlColumns;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(ReportBuilderPage4)
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(ReportBuilderPage4)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeColumns();
	afx_msg void OnSelchangeOperator();
	afx_msg void OnChangeComparisonValue();
	afx_msg void OnSelchangeForeignList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual bool MoveDataToHolder( bool bValidate );

private:
	void GetAllColumns();
	void FillOperatorList();
	void FillForeignList( ReportOIDColumn& rColumn );
	void FillForeignList( vdbString& sForeignTable, vdbString& sDisplayColumn );

		
private:
	ReportBuilderPageHolder* _pParent;
	vdbString _sColumnName;		// the column selected by the user
	vdbString _sColumnType;		// the Vault type of the selected column
	vdbString _sOperator;		// the comparison operator
	vdbString _sValue;			// the comparison value
};


#endif __ReportBuilderPage4_h

