//=============================================================================
//
// CLASS:           ReportPreview
//
// OVERVIEW:        Window to display a preview of a report
//
// FILE NAME:       ReportPreview.h
//
// INITIAL DATE:    10/26/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#ifndef __ReportPreview_h
#define __ReportPreview_h

#include "vdbDLLDialog.h"
#include "..\\..\\Vault\\vdbResources\\resource.h"

#ifndef __ReportDefinition_h
	#include "ReportDefinition.h"
#endif


//-------------------------------------
class ReportPreview : public vdbDLLDialog
{
public:
	ReportPreview( ReportDefinition* pDef );
	virtual ~ReportPreview();

public:
	void InitializeForm();

public:
	//{{AFX_DATA(ReportPreview)
	enum { IDD = IDD_REPORT_PREVIEW };
	CEdit	m_ctlEdit;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(ReportPreview)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(ReportPreview)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnErrspaceEdit();
	afx_msg void OnMaxtextEdit();
	afx_msg void OnSetfocusEdit();
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	CFont courierFont;
	ReportDefinition* _pDef;
};


#endif  // __ReportPreview_h
