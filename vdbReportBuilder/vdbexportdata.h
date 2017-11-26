//=============================================================================
//
// CLASS:           vdbExportData
//
// ESSENCE:         Export data via a report definition
//
// FILE NAME:       vdbExportData.h
//
// INITIAL DATE:    11/12/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#ifndef __vdbExportData_h
#define __vdbExportData_h

#include "vdbDLLDialog.h"
#include "..\\..\\Vault\\vdbResources\\resource.h"

#ifndef __vdbDLLBitmapButton_h
	#include "vdbDLLBitmapButton.h"
#endif

#ifndef __vdbMap_h
	#include <vdbMap.h>
#endif

#ifndef __ReportDefinition_h
	#include "ReportDefinition.h"
#endif


//-------------------------------------
class vdbExportData : public vdbDLLDialog
{
public:
	vdbExportData();

	//{{AFX_DATA(vdbExportData)
	enum { IDD = IDD_EXPORT_DATA };
	CButton	m_ctlExport;
	CEdit	m_ctlExportFilename;
	CEdit	m_ctlExportDirectory;
	CListBox	m_ctlList;
	vdbDLLBitmapButton	m_ctlDirectory;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(vdbExportData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(vdbExportData)
	virtual BOOL OnInitDialog();
	afx_msg void OnExport();
	afx_msg void OnSelchangeList();
	afx_msg void OnDirectory();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void AddOneFile( const char* szFilename );

private:
	vdbMap<ReportDefinition> _reportDefinitions;
};


#endif //__vdbExportData_h
