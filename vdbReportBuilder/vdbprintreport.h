//=============================================================================
//
// CLASS:           vdbPrintReport
//
// ESSENCE:         Select a printer and send a report to it
//
// FILE NAME:       vdbPrintReport.cpp
//
// INITIAL DATE:    11/12/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#ifndef __vdbPrintReport_h
#define __vdbPrintReport_h

#include "vdbDLLDialog.h"
#include "..\\..\\Vault\\vdbResources\\resource.h"

#ifndef __vdbPrinterDialog_h
	#include <vdbPrinterDialog.h>
#endif

#ifndef __vdbMap_h
	#include <vdbMap.h>
#endif

#ifndef __ReportDefinition_h
	#include "ReportDefinition.h"
#endif

#ifndef __ReportStyleDefinition_h
	#include "ReportStyleDefinition.h"
#endif


class vdbPrintReport : public vdbDLLDialog, public vdbPrinterDialogClass
{
public:
	vdbPrintReport();
	virtual ~vdbPrintReport();

	//{{AFX_DATA(vdbPrintReport)
	enum { IDD = IDD_VDB_PRINT_REPORT };
	CButton	m_ctlPages;
	CButton	m_ctlAll;
	CEdit	m_ctlFromPage;
	CEdit	m_ctlToPage;
	CButton	m_ctlPrint;
	CListBox	m_ctlListDefs;
	CButton	m_ctlFormatStyle;
	CListBox	m_ctlListStyles;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(vdbPrintReport)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(vdbPrintReport)
	virtual BOOL OnInitDialog();
	afx_msg void OnPrint();
	afx_msg void OnPrinterSetup();
	afx_msg void OnSelchangeListDefs();
	afx_msg void OnVdbPages();
	afx_msg void OnVdbAll();
	afx_msg void OnFormatStyle();
	afx_msg void OnSelchangeListStyles();
	afx_msg void OnDblclkListStyles();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void AddOneReportDefinition( const char* szFilename );
	void AddOneReportStyle( const char* szFilename );
	void SaveOneReportStyle( ReportStyleDefinition* pDef );
	void PrintHelper( HDC hdc, ReportDefinition* pDef, ReportStyleDefinition* pStyle, int fromPage, int toPage );
	void CreateFonts( HDC hdc, ReportStyleDefinition* pRSD );
	void DeleteFonts( HDC hdc );

private:
	vdbMap<ReportDefinition> _reportDefinitions;
	vdbMap<ReportStyleDefinition> _reportStyleDefinitions;
	vdbPrinterDialogData _dlgData;
};


#endif //__vdbPrintReport_h
