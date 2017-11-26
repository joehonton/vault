//=============================================================================
//
// CLASS:           vdbPrinterDialog
//
// ESSENCE:         Printer Dialog procedure and class
//
// FILE NAME:       vdbPrinterDialog.cpp
//
// INITIAL DATE:    6/7/99
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1999 by Joe Honton
//
//=============================================================================

#ifndef __vdbPrinterDialog_h
#define __vdbPrinterDialog_h

#include "vdbExport.h"


//=============================================================================
//  Data transfer into and out of the dialog
//=============================================================================
class vdbPrinterDialogData
{
public:
	vdbPrinterDialogData();
	vdbPrinterDialogData( bool enablePrintRange, int fromPage, int toPage );
	~vdbPrinterDialogData();
	
	enum vdbPrintRage { ALL, PAGES }; 

public:
	bool _enablePrintRange;			// false to disable 'from' and 'to'
	vdbPrintRage _printRange;		// ALL or PAGES
	int _fromPage;
	int _toPage;
	HDC _printerDC;					// returned to caller.  Deleted by destructor.
	int _dmOrientation;				// returned for information.  The HDC is created with this orientation.
	char _szPrinterName[81];		// 9-26-2004 changed from old length of CCHDEVICENAME
};


//=============================================================================
//  Public function for showing the dialog
//=============================================================================
extern "C" bool vdbPrinterDialog( vdbPrinterDialogData& data );


//=============================================================================
//  Class data and member functions for handling window messages
//=============================================================================
class vdbPrinterDialogClass
{
	friend BOOL CALLBACK vdbPrinterDialogProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

protected:	 
	// message processing functions
	BOOL OnInitDialog( HWND hwnd, vdbPrinterDialogData* pData );
	void OnOK( HWND hwnd );
	void OnCancel( HWND hwnd );
	void OnSetup( HWND hwnd );
	void DialogToData( HWND hwnd );

private:
	vdbPrinterDialogData* _pData;
};


#endif // __vdbPrinterDialog_h
