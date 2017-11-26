//=============================================================================
//
// CLASS:           ReportBuilderPageHolder
//
// ESSENCE:         The property sheet for the Report Builder Wizard
//
// FILE NAME:       ReportBuilderPageHolder.cpp
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
// AUTHOR:          Joe Honton
//
//=============================================================================

#ifndef __ReportBuilderPageHolder_h
#define __ReportBuilderPageHolder_h

#ifndef __ReportDefinition_h
	#include "ReportDefinition.h"
#endif

class ReportBuilderPage1;
class ReportBuilderPage2;
class ReportBuilderPage3;
class ReportBuilderPage4;
class ReportBuilderPage5;
class ReportBuilderPage6;
class ReportBuilderPage7;
class ReportBuilderPage8;
class ReportBuilderPage9;
class ReportBuilderPage10;
class vdbServerInfo;


class ReportBuilderPageHolder : public CPropertySheet
{
public:
	ReportBuilderPageHolder( CWnd* pParentWnd, ReportDefinition* pDef );
	virtual ~ReportBuilderPageHolder();

private:
	//{{AFX_VIRTUAL(ReportBuilderPageHolder)
	protected:
	//}}AFX_VIRTUAL

private:
	//{{AFX_MSG(ReportBuilderPageHolder)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int DetermineNextPage( int currentPage );
	int DetermineBackPage( int currentPage );

private:
	ReportBuilderPage1* _pPage1;
	ReportBuilderPage2* _pPage2;
	ReportBuilderPage3* _pPage3;
	ReportBuilderPage4* _pPage4;
	ReportBuilderPage5* _pPage5;
	ReportBuilderPage6* _pPage6;
	ReportBuilderPage7* _pPage7;
	ReportBuilderPage8* _pPage8;
	ReportBuilderPage9* _pPage9;
	ReportBuilderPage10* _pPage10;

public:
	inline void ReuseFile()		{ _bReuseFile = true; }

public:
	ReportDefinition* _pDef;

public:
	bool _bReuseFile;				// set to true to modify existing report definition
	bool _bValid;					// Is everything OK
	bool _bUseSelectionCriteria;	// should we ask for filter criteria
	bool _bDefineAnotherSelection;	// should we loop back to define another filter criteria
	bool _bUseSortCriteria;			// should we ask for filter criteria
	bool _bDefineAnotherSort;		// should we loop back to define another sort criteria
	vdbServerInfo* _pServerInfo;	// ODBC to Vault type mappings
};


#endif // __ReportBuilderPageHolder_h
