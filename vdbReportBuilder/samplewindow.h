//=============================================================================
//
// CLASS:           SampleWindow
//
// OVERVIEW:        CStatic-derived class to show a sample in the FormatStyles
//                  dialog
//
// FILE NAME:       SampleWindow.h
//
// INITIAL DATE:    01/16/00
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __SampleWindow_h
#define __SampleWindow_h

#ifndef __vdbTypedef_h
	#include <vdbTypedef.h>
#endif

class ReportStyleDefinition;
class ReportFontSpecs;


class SampleWindow : public CStatic
{
public:
	SampleWindow();
	~SampleWindow();

public:
	void Setup( ReportStyleDefinition* pDef, OID entryID );
	void Refresh();

private:
	//{{AFX_MSG(SampleWindow)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	ReportStyleDefinition* _pDef;
	OID _entryID;
};

#endif // __SampleWindow_h

