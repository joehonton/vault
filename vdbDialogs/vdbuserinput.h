//=============================================================================
//
// CLASS:           UserInputDlg
//
// FILE NAME:       vdbUserInput.h
//
// ESSENCE:         Dialog box for generic user input
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    1/02/97
//
// COPYRIGHT:       Copyright © 1997 by Joe Honton
//
//=============================================================================

#ifndef __UserInputDlg_h
#define __UserInputDlg_h

#ifndef __MessageBox_h
	#include "vdbMessageBox.h"
#endif

#ifndef __vdbExport_h
	#include "vdbExport.h"
#endif

#ifndef __vdbString_h
	#include "vdbString.h"
#endif

//=============================================================================
//  Data transfer into and out of the dialog
//=============================================================================
class vdbUserInputDlgData
{
public:
	vdbUserInputDlgData() {}
	vdbUserInputDlgData( const char* szMessage, const char* szUserInput, const char* szTitle, unsigned int buttons = MB_OKCANCEL )
	{
		_szMessage = szMessage;
		_szUserInput = szUserInput;
		_szTitle = szTitle;
		_buttons = buttons;
	}
	vdbUserInputDlgData( const vdbString& szMessage, const vdbString& szUserInput, const vdbString& szTitle, unsigned int buttons = MB_OKCANCEL )
	{
		_szMessage = szMessage;
		_szUserInput = szUserInput;
		_szTitle = szTitle;
		_buttons = buttons;
	}
	inline const char* GetUserInput()		{ return (const char*) _szUserInput; }

public:
	vdbString _szMessage;
	vdbString _szUserInput;
	vdbString _szTitle;
	unsigned int _buttons;
};


//=============================================================================
//  Public function for showing the dialog
//=============================================================================
extern "C" int vdbUserInputDialog( vdbUserInputDlgData& data );


//=============================================================================
//  Class data and member functions for handling window messages
//=============================================================================
class UserInputDlgClass
{
	friend BOOL CALLBACK UserInputDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

private:	 
	// message processing functions
	BOOL OnInitDialog( HWND hwnd, vdbUserInputDlgData* pData );
	void OnOK( HWND hwnd );
	void OnCancel( HWND hwnd );
	void OnSkip( HWND hwnd );

private:
	vdbUserInputDlgData* _pData;
};


#endif // __vdbUserInput_h
