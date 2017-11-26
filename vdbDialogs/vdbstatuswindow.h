//=============================================================================
//
// CLASS:           vdbStatusWindow
//
// ESSENCE:         A modeless dialog for displaying status to the user during
//                  lengthy operations.
//
// FILE NAME:       vdbStatusWindow.h
//
// INITIAL DATE:    10/1/96 
//
// COPYRIGHT:       Copyright © 1996 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#ifndef __vdbStatusWindow_h
#define __vdbStatusWindow_h

#include "vdbDLLDialog.h"
#include "..\\vdbResources\\resource.h"

#ifndef __vdbString_h
	#include <vdbString.h>
#endif


class vdbStatusWindow : public vdbDLLDialog
{
public:
	vdbStatusWindow( const char* szWindowTitle, CWnd* pParent = NULL );

public:
	void Message( const char* szText );
	void ShowCloseButton( bool show = true );

private:
	//{{AFX_DATA(vdbStatusWindow)
	enum { IDD = IDD_STATUS_WINDOW };
	CButton	m_ctlClose;
	CStatic	m_ctlText;
	//}}AFX_DATA
	//{{AFX_VIRTUAL(vdbStatusWindow)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(vdbStatusWindow)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	vdbString _sWindowTitle;
};


#endif  // __vdbStatusWindow_h
