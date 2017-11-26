//=============================================================================
//
// CLASS:           vdbListCtrlColors
//
// ESSENCE:         Set pphListCtrl background and highlist colors
//
// FILE NAME:       vdbListCtrlColors.h
//
// INITIAL DATE:    1/6/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __vdbListCtrlColors_h
#define __vdbListCtrlColors_h

#include "vdbDLLDialog.h"
#include "..\\vdbResources\\resource.h"

#ifndef __pphListCtrl_h
	#include "pphListCtrl.h"
#endif

#ifndef __vdbSet_h
	#include "vdbSet.h"
#endif

#ifndef __vdbString_h
	#include "vdbString.h"
#endif


class vdbListCtrlColors : public vdbDLLDialog
{
public:
	vdbListCtrlColors( const char* szRegistrationKey, CWnd* pParent = NULL);

private:
	//{{AFX_DATA(vdbListCtrlColors)
	enum { IDD = IDD_LIST_CTRL_COLORS };
	pphListCtrl	m_ctlList;
	CComboBox	m_ctlHighlight;
	CComboBox	m_ctlBackground;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(vdbListCtrlColors)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

private:
	//{{AFX_MSG(vdbListCtrlColors)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeBackground();
	afx_msg void OnSelchangeHighlight();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	vdbString _sRegistrationKey;			// something like "SOFTWARE\\ProgrammersHeaven\\Catalog\\1.0\\Colors"
	vdbString _sColumn1Name;				// the column header of the first column
	vdbString _sColumn2Name;				// the column header of the second column
	vdbSet<vdbString> _column1Samples;		// data values to place into the first column
	vdbSet<vdbString> _column2Samples;		// data values to place into the second column
};

#endif  // __vdbListCtrlColors_h

