//=============================================================================
//
// CLASS:           vdbFormatStyles
//
// OVERVIEW:        Dialog to format the appearance of reports
//
// FILE NAME:       vdbFormatStyles.h
//
// INITIAL DATE:    01/13/00
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __vdbFormatStyles_h
#define __vdbFormatStyles_h

#include "vdbDLLDialog.h"
#include "..\\..\\Vault\\vdbResources\\resource.h"

#ifndef __pphColorListbox_h
	#include <pphColorListbox.h>
#endif

#ifndef __SampleWindow_h
	#include "SampleWindow.h"
#endif

class ReportStyleDefinition;
class cdbEntry;


class vdbFormatStyles : public vdbDLLDialog
{
public:
	vdbFormatStyles( HDC hdc, ReportStyleDefinition* pDef, OID entryID );

private:
	//{{AFX_DATA(vdbFormatStyles)
	enum { IDD = IDD_REPORT_FORMAT_STYLE };
	CButton	m_ctlOk;
	CStatic	m_ctlFontStyleDesc;
	CStatic	m_ctlFontSizeDesc;
	CStatic	m_ctlFontFaceDesc;
	CStatic	m_ctlFontColorDesc;
	CStatic	m_ctlCategoryDesc;
	CListBox	m_ctlFontStyle;
	SampleWindow	m_ctlSample;
	CListBox	m_ctlFontSize;
	CListBox	m_ctlFontFace;
	pphColorListbox	m_ctlFontColor;
	CListBox	m_ctlCategory;
	CButton	m_ctlIncludeTagNumber;
	//}}AFX_DATA

private:
	virtual void DoDataExchange(CDataExchange* pDX);
	//{{AFX_MSG(vdbFormatStyles)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSelchangeFontFace();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchangeFontSize();
	afx_msg void OnSelchangeFontColor();
	afx_msg void OnSelchangeFontStyle();
	afx_msg void OnSelchangeCategory();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void AddFontsToList();
	void AddColorToList( COLORREF color );
	void ShowSample();

private:
	HDC _hdc;							// printer device context for looking up fonts
	ReportStyleDefinition* _pDef;		// the style for this whole dialog session
	OID _entryID;						// the entry to use as a sample
	int _selectedCategory;				// the index into _pDef of the user-selected catagory or -1 if "<All categories>"
	bool _bInhibitSelchange;			// true to prevent selchange messages from being effective

	friend int CALLBACK EnumFontFaces( const LOGFONT *plf, const TEXTMETRIC *ptm, unsigned long FontType, LPARAM lParam );
	friend int CALLBACK EnumFontStyles( const LOGFONT *plf, const TEXTMETRIC *ptm, unsigned long FontType, LPARAM lParam );
};


#endif __vdbFormatStyles_h
