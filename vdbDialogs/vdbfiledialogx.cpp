//=============================================================================
//
// FILE NAME:       vdbFileDialogX.cpp
//
// ESSENCE:         File Dialog wrapper class
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    2/2/96
//
// COPYRIGHT:       Copyright © 1996 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbAssert.h"
#include "vdbFileDialogX.h"
#include "vdbUtility.h"
#include "VaultResourceInstance.h"
#include <commdlg.h>
#include <vdbException.h>


//=============================================================================
//  Data transfer into and out of the dialog
//=============================================================================

//-------------------------------------
vdbFileDlgDataX::vdbFileDlgDataX()
	: sTitle( "Open file" ),
	  sFilename(),
	  sFilters(),
	  flags( OFN_ENABLEHOOK )
	{}


//-------------------------------------
vdbFileDlgDataX::vdbFileDlgDataX( vdbString title, vdbFilename& defaultFilename, vdbString filters )
	: sTitle( title ),
	  sFilename( defaultFilename ),
	  sFilters( filters ),
	  flags( OFN_ENABLEHOOK )
	{}


//=============================================================================
//  Public function for showing the dialog
//=============================================================================

//-------------------------------------
//^ The vdbDoNothingHook is required in order to enable the old-style dialog box;
//  that is the non Explorer style file open/file save.
//< always returns false to pass this along to the default handler
//
UINT APIENTRY vdbDoNothingHook( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	return FALSE;
}


//---------------------------------------------
//^ The vdbFileDialogX show a open file dialog
//> data is a reference to a FileDialogData instance.  These values will be used
//  to initialize the dialog box.  The selected filename(s) are also returned 
//  through this object.
//< returns true if the user selected a file, or false if Cancel was pressed.
//
bool vdbFileDialogX( vdbFileDlgDataX& data )
{
	assert ( &data != 0 );
	HINSTANCE hResourceDLL = VaultResourceInstance::Handle();

	OPENFILENAME ofn;
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
    ofn.hInstance = hResourceDLL;
    
    data.sFilters.Replace( '|', '\0' );
    ofn.lpstrFilter = data.sFilters;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;

	char defaultFile[2048];
	defaultFile[0] = 0;
	ofn.lpstrFile = defaultFile;
	ofn.nMaxFile = sizeof(defaultFile);
	
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = data.sFilename.Directory();
	ofn.lpstrTitle = data.sTitle;
    ofn.Flags = data.flags;

	ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = data.sDefaultExtension;
	ofn.lCustData = 0;
	ofn.lpfnHook = vdbDoNothingHook;
	ofn.lpTemplateName = 0;

	int rc = ::GetOpenFileName( &ofn );
	data.sFilename = ofn.lpstrFile;
	VaultResourceInstance::UnloadDLL();

	return (rc == TRUE) ? true : false;
}


//-------------------------------------
//^ The Directory function returns a vdbFilename containing the user selected
//  drive and directory without the filename and without the trailing backslash.
//< returns the directory with drive but without directory or extension.
//
vdbFilename vdbFileDlgDataX::Directory() const
{
	// If the user selected only one file there will be no space between the directory
	// info and the filename.  If the user selected multiple files, there will be a space
	// between the end of the directory and the beginning of the first file, and there
	// will be no trailing backslash at the end of the directory.

	int space = sFilename.Find( ' ' );
	int slash = sFilename.ReverseFind( '\\' );
	int endOfDir = max( space, slash );

	vdbFilename f( sFilename.First( endOfDir ) );
	return f;
}


//-------------------------------------
//^ The GetFileList function parses the user selections to put the set of 
//  user selected files into a vdbSet.
//
void vdbFileDlgDataX::GetFileList( vdbSet<vdbFilename>& setOfFiles ) const
{
	// remove the drive and directory portion
	int space = sFilename.Find( ' ' );
	int slash = sFilename.ReverseFind( '\\' );
	int endOfDir = max( space, slash );

	vdbString allFiles;
	if ( endOfDir == -1 )
		allFiles = sFilename;
	else
		allFiles = sFilename.Middle( endOfDir + 1 );

	// loop through and add to list
	space = allFiles.Find( ' ' );
	while ( space != -1 )
	{
		vdbFilename* pNew = new vdbFilename( allFiles.First( space ) );
		if ( pNew == 0 ) throw vdbMemoryException();
		setOfFiles.Add( pNew );

		allFiles = allFiles.Middle( space + 1 );
		space = allFiles.Find( ' ' );
	}
	
	// finally, add the last one
	if ( allFiles.Length() > 0 )
	{
		vdbFilename* pNew = new vdbFilename( allFiles );
		if ( pNew == 0 ) throw vdbMemoryException();
		setOfFiles.Add( pNew );
	}
}

