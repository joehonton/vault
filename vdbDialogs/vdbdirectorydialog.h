//=============================================================================
//
// FILE NAME:       vdbDirectoryDialog.h
//
// ESSENCE:         Dialog for displaying the drive and directories of this
//                  computer
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    9/10/97
//
// COPYRIGHT:       Copyright © 1997 by Joe Honton
//
//=============================================================================

#ifndef __vdbDirectoryDialog_h
#define __vdbDirectoryDialog_h

#ifndef __vdbExport_h
	#include "vdbExport.h"
#endif

#ifndef __vdbString_h
	#include "vdbString.h"
#endif

#ifndef __vdbFilename_h
	#include "vdbFilename.h"
#endif

#define VDB_REFRESH_DRIVES		WM_USER + 1

//=============================================================================
//  Data transfer into and out of the dialog
//=============================================================================
class vdbDirectoryDlgData
{
public:
	vdbDirectoryDlgData();
	vdbDirectoryDlgData( HWND hParent, const char* szTitle, const char* szDefaultDirectory, bool bEnableCreateDirectory );
	~vdbDirectoryDlgData();
	void Cleanup();

public:
	void SetTitle( const char* sz )					{ _sTitle = sz; }
	void SetDirectory( const char* f )				{ _sDirectory = f; }
	const char* GetDirectory() const				{ return _sDirectory; }

public:
	HWND _hParent;
	vdbString _sTitle;
	vdbFilename _sDirectory;
	bool _bEnableCreateDirectory;
	HIMAGELIST _hImage;					// directory and drive images
	HBITMAP _hCreateBitmap;				// Create new directory bitmap
	HBITMAP _hCreateDisabledBitmap;		// Create new directory bitmap (disabled)
	HBITMAP _hAcceptBitmap;				// Accept the new directory name
	HBITMAP _hRejectBitmap;				// Reject the new directory name
};


//=============================================================================
//  Public function for showing the dialog
//=============================================================================
extern "C" bool vdbDirectoryDialog( vdbDirectoryDlgData& data );


//=============================================================================
//  Class data and member functions for handling window messages
//=============================================================================
class DirectoryDlgClass
{
	friend BOOL CALLBACK DirectoryDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

public:
	const char* GetSelectedDirectory();
	void SetSelectedDirectory( const char* sz );

protected:	 
	// message processing functions
	BOOL OnInitDialog( HWND hwnd, vdbDirectoryDlgData* pData );
	void OnOK();
	void OnCancel();
	void OnCreateDirectory();
	void OnSelchangedDrives();
	void OnSelchangedDirectory( NM_TREEVIEW* pnmtv );
	void OnDblClickDirectory();
	void OnRefreshDrives();
	void OnAccept();
	void OnReject();

public:
	static void RefreshDrives( HWND hwnd );

protected:
	// helper functions
	HTREEITEM InsertDirectory( HWND hwndTree, const char* sDirectory, HTREEITEM hParent, bool bIsDrive );
	void AddAllChildren( HWND hwndTree, vdbFilename& sDirectory, HTREEITEM hParent );
	void DeleteAllChildren( HWND hwndTree, HTREEITEM hItem );
	void GetFullDirectoryName( HWND hwndTree, HTREEITEM hItem, vdbFilename& sDirectory );
	HTREEITEM AddDefaultDirectories( HWND hwndTree, vdbFilename sDirectory, HTREEITEM hParent, bool bIsDrive );
	HTREEITEM FindChild( HWND hwndTree, HTREEITEM hParent, const char* szFindString );
	HTREEITEM FindTreeItemByString( HWND ctlTree, vdbString& searchKey, HTREEITEM hStart = NULL  );
	HTREEITEM WalkTree( HWND ctlTree, HTREEITEM hItem );
	HTREEITEM WalkUpTree( HWND ctlTree, HTREEITEM hItem );

protected:
	HWND _hwnd;

private:
	vdbDirectoryDlgData* _pData;
	bool _bIsInitializing;
};


#endif // __vdbDirectoryDialog_h
