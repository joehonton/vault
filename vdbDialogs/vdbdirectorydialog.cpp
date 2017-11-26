//=============================================================================
//
// FILE NAME:       vdbDirectoryDialog.cpp
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

#include "vdbWindows.h"
#include "vdbDirectoryDialog.h"
#include "..\\vdbResources\\resource.h"
#include "vdbAssert.h"
#include "VaultResourceInstance.h"
#include <vdbException.h>
#include "vdbUtility.h"
#include "vdbWindowSupport.h"
#include <cursors.h>
#include <vdbSetSort.h>
#include <vdbFileDialogSort.h>
#include <pphFormatBytes.h>
#include <vdbSet.h>

//warning C4996: 'sprintf': This function or variable may be unsafe. Consider using sprintf_s instead.
#pragma warning( disable : 4996 )


//=============================================================================
//  Data transfer into and out of the dialog
//=============================================================================

//-------------------------------------
vdbDirectoryDlgData::vdbDirectoryDlgData()
	: _hParent( NULL ),
	  _sTitle( "Select directory" ),
	  _sDirectory(),
	  _bEnableCreateDirectory( false ),
	  _hImage( 0 ),
	  _hCreateBitmap( 0 ),
	  _hCreateDisabledBitmap( 0 ),
	  _hAcceptBitmap( 0 ),
	  _hRejectBitmap( 0 )
	{}


//-------------------------------------
vdbDirectoryDlgData::vdbDirectoryDlgData( HWND hParent, const char* szTitle, const char* szDefaultDirectory, bool bEnableCreateDirectory )
	: _hParent( hParent ),
	  _sTitle( szTitle ),
	  _sDirectory( szDefaultDirectory ),
	  _bEnableCreateDirectory( bEnableCreateDirectory ),
	  _hImage( 0 ),
	  _hCreateBitmap( 0 ),
	  _hCreateDisabledBitmap( 0 ),
	  _hAcceptBitmap( 0 ),
	  _hRejectBitmap( 0 )
	{}


//-------------------------------------
vdbDirectoryDlgData::~vdbDirectoryDlgData()
{
	Cleanup();
}


//-------------------------------------
void vdbDirectoryDlgData::Cleanup()
{
	if ( _hImage != NULL )
		{ ImageList_Destroy( _hImage ); _hImage = 0; }
	
	if ( _hCreateBitmap != NULL )
		{ DeleteObject( _hCreateBitmap ); _hCreateBitmap = 0; }
	
	if ( _hCreateDisabledBitmap != NULL )
		{ DeleteObject( _hCreateDisabledBitmap ); _hCreateDisabledBitmap = 0; }

	if ( _hAcceptBitmap != NULL )
		{ DeleteObject( _hAcceptBitmap ); _hAcceptBitmap = 0; }

	if ( _hRejectBitmap != NULL )
		{ DeleteObject( _hRejectBitmap ); _hRejectBitmap = 0; }
}


//=============================================================================
//  Public function for showing the dialog
//=============================================================================

//---------------------------------------------
//^ The vdbDirectoryDialog shows a dialog for selecting the drive and directory
//> data is a reference to a vdbDirectoryDlgData instance.  These values will be used
//  to initialize the dialog box.  The selected directory is also returned 
//  through this object.
//< returns true if the user selected a directory, or false if Cancel was pressed.
//
bool vdbDirectoryDialog( vdbDirectoryDlgData& data )
{
	assert ( &data != 0 );
	int rc = DialogBoxParam( VaultResourceInstance::Handle(), MAKEINTRESOURCE(IDD_DIRECTORY_DIALOG), data._hParent, (DLGPROC) DirectoryDlgProc, (LPARAM) &data );
	VaultResourceInstance::UnloadDLL();
	if ( rc == IDOK )
		return true;
	else
		return false;
}


//=============================================================================
// Dialog box procedure
//=============================================================================

//--------------------------------
// Procedure for Directory dialog box
BOOL CALLBACK DirectoryDlgProc( HWND _hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static DirectoryDlgClass* pDlgClass = 0;

	switch ( message )
	{
		case WM_INITDIALOG:
			if ( !(pDlgClass = new DirectoryDlgClass) )
				return EndDialog( _hwnd, IDABORT );
			else
				return pDlgClass->OnInitDialog( _hwnd, (vdbDirectoryDlgData*) lParam );
		
		case WM_COMMAND:
		{	
			switch ( LOWORD( wParam ) )
			{
				case IDOK:
					pDlgClass->OnOK();
					break;

				case IDCANCEL:
					pDlgClass->OnCancel();
					break;

				case IDC_VDB_CREATE_DIRECTORY:
					pDlgClass->OnCreateDirectory();
					break;

				case IDC_VDB_ACCEPT:
					pDlgClass->OnAccept();
					break;

				case IDC_VDB_REJECT:
					pDlgClass->OnReject();
					break;

				default:
					break;
			}
	
			return TRUE;
		}
		
		case WM_NOTIFY:
		{
			NM_TREEVIEW* pnmtv = (NM_TREEVIEW*) lParam;

			switch( wParam )
			{
				case IDC_VDB_DIRECTORY_TREE:
					if ( pnmtv->hdr.code == TVN_SELCHANGED )
						pDlgClass->OnSelchangedDirectory( pnmtv );
					else if ( pnmtv->hdr.code == NM_DBLCLK )
						pDlgClass->OnDblClickDirectory();
					break;

				case IDC_VDB_DRIVES:
					if ( pnmtv->hdr.code == TVN_SELCHANGED )
						pDlgClass->OnSelchangedDrives();
					break;

				default:
					break;
			}
			return TRUE;
		}

		case WM_DESTROY:
			delete pDlgClass;
			pDlgClass = 0;
			return TRUE;

		case VDB_REFRESH_DRIVES:
			pDlgClass->OnRefreshDrives();
			return TRUE;
					
		default:
    		return FALSE;
	}
}


//=============================================================================
// Class members
//=============================================================================


//---------------------------------------
void DirectoryDlgClass::SetSelectedDirectory( const char* sz )
{
	SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_DIRECTORY, WM_SETTEXT, 0, (LPARAM) sz );
	_pData->SetDirectory( sz );
}


//---------------------------------------
const char* DirectoryDlgClass::GetSelectedDirectory()
{
	int len = SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_DIRECTORY, WM_GETTEXTLENGTH, 0, 0 );
	char* psz = new char[len+1];
	SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_DIRECTORY, WM_GETTEXT, len+1, (LPARAM) (LPCTSTR) psz );
	_pData->SetDirectory( psz );
	delete[] psz; psz = 0;

	return _pData->GetDirectory();
}


//---------------------------------------
BOOL DirectoryDlgClass::OnInitDialog( HWND hwnd, vdbDirectoryDlgData* pData )
{
	assert( pData != 0 );
	_pData = pData;
	_hwnd = hwnd;
	_bIsInitializing = true;

	ShowHourglass();
	vdbCenterWindow( _hwnd );
	SetWindowText( _hwnd, pData->_sTitle );

	// if no directory was specified, just use the current directory
	if ( _pData->_sDirectory.IsNull() )
	{
		char szDir[120];
		GetCurrentDirectory( 120, szDir );
		_pData->_sDirectory = szDir;
	}

	// set the directory tree control bitmaps
	HWND hwndTree = GetDlgItem( _hwnd, IDC_VDB_DIRECTORY_TREE );
	_pData->_hImage = ImageList_LoadImage( VaultResourceInstance::Handle(), MAKEINTRESOURCE(IDB_VDB_DIRECTORY_ICONS), 16, 6, CLR_NONE, IMAGE_BITMAP, LR_DEFAULTCOLOR );
	SendMessage( hwndTree, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM) _pData->_hImage );

	// set the drive tree control bitmaps
	HWND hwndDrives = GetDlgItem( _hwnd, IDC_VDB_DRIVES );
	SendMessage( hwndDrives, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM) _pData->_hImage );

	// Add all drives to the tree as root nodes
	OnRefreshDrives();

	// Clear the dir tree and add the default directory
	SendMessage( hwndTree, TVM_DELETEITEM, 0, (LPARAM) (HTREEITEM) TVI_ROOT );
	HTREEITEM hDirItem = AddDefaultDirectories( hwndTree, _pData->_sDirectory, TVI_ROOT, true );

	if ( _pData->_bEnableCreateDirectory == true )
		ShowWindow( GetDlgItem( _hwnd, IDC_VDB_CREATE_DIRECTORY ), SW_SHOW );
	else
		ShowWindow( GetDlgItem( _hwnd, IDC_VDB_CREATE_DIRECTORY ), SW_HIDE );

	// load the button bitmaps	
	_pData->_hCreateBitmap = (HBITMAP) LoadImage( VaultResourceInstance::Handle(), MAKEINTRESOURCE( IDB_VDB_NEW ), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS );
	_pData->_hCreateDisabledBitmap = (HBITMAP) LoadImage( VaultResourceInstance::Handle(), MAKEINTRESOURCE( IDB_VDB_NEW_DISABLED ), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS );
	_pData->_hAcceptBitmap = (HBITMAP) LoadImage( VaultResourceInstance::Handle(), MAKEINTRESOURCE( IDB_VDB_ACCEPT ), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS );
	_pData->_hRejectBitmap = (HBITMAP) LoadImage( VaultResourceInstance::Handle(), MAKEINTRESOURCE( IDB_VDB_REJECT ), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS );

	// hide the create directory edit until requested
	ShowWindow( GetDlgItem( _hwnd, IDC_VDB_NEW_DIRECTORY_NAME ), SW_HIDE );
	ShowWindow( GetDlgItem( _hwnd, IDC_VDB_ACCEPT ), SW_HIDE );
	ShowWindow( GetDlgItem( _hwnd, IDC_VDB_REJECT ), SW_HIDE );

	SendMessage( GetDlgItem( _hwnd, IDC_VDB_CREATE_DIRECTORY ), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) _pData->_hCreateBitmap );
	SendMessage( GetDlgItem( _hwnd, IDC_VDB_ACCEPT ), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) _pData->_hAcceptBitmap );
	SendMessage( GetDlgItem( _hwnd, IDC_VDB_REJECT ), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) _pData->_hRejectBitmap );

	_bIsInitializing = false;

	// select the current directory in the tree
//	SendDlgItemMessage( _hwnd, IDC_VDB_DIRECTORY_TREE, TVM_SELECTITEM, TVGN_CARET, (LPARAM) hDirItem );

	// show the currently selected directory in the edit control
	SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_DIRECTORY, WM_SETTEXT, 0, (LPARAM) (const char*) _pData->_sDirectory );

	ShowPointer();
	return TRUE;
}


//---------------------------------------
void DirectoryDlgClass::OnOK()
{
	// get the selected directory from the static control
	int len = SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_DIRECTORY, WM_GETTEXTLENGTH, 0, 0 );
	char* psz = new char[len+1];
	SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_DIRECTORY, WM_GETTEXT, len+1, (LPARAM) (LPCTSTR) psz );
	_pData->SetDirectory( psz );
	delete[] psz; psz = 0;

	EndDialog( _hwnd, IDOK );
}


//---------------------------------------
void DirectoryDlgClass::OnCancel()
{
	_pData->SetDirectory( "" );
	EndDialog( _hwnd, IDCANCEL );
}


//---------------------------------------
void DirectoryDlgClass::OnCreateDirectory()
{
	HWND hCreate = GetDlgItem( _hwnd, IDC_VDB_CREATE_DIRECTORY );
	SendMessage( hCreate, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) _pData->_hCreateDisabledBitmap );
	EnableWindow( hCreate, FALSE );

	HWND hwndName = GetDlgItem( _hwnd, IDC_VDB_NEW_DIRECTORY_NAME );
	ShowWindow( hwndName, SW_SHOW );

	ShowWindow( GetDlgItem( _hwnd, IDC_VDB_ACCEPT ), SW_SHOW );
	ShowWindow( GetDlgItem( _hwnd, IDC_VDB_REJECT ), SW_SHOW );

	SetWindowText( hwndName, "New Directory" );
	SendMessage( hwndName, EM_SETSEL, 0, -1 );
	SetFocus( hwndName );
}


//---------------------------------------
void DirectoryDlgClass::OnAccept()
{
	HWND hwndName = GetDlgItem( _hwnd, IDC_VDB_NEW_DIRECTORY_NAME );
	ShowWindow( hwndName, SW_HIDE );

	ShowWindow( GetDlgItem( _hwnd, IDC_VDB_ACCEPT ), SW_HIDE );
	ShowWindow( GetDlgItem( _hwnd, IDC_VDB_REJECT ), SW_HIDE );

	HWND hCreate = GetDlgItem( _hwnd, IDC_VDB_CREATE_DIRECTORY );
	SendMessage( hCreate, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) _pData->_hCreateBitmap );
	EnableWindow( hCreate, TRUE );

	// use the currently selected directory as a starting point
	int len = SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_DIRECTORY, WM_GETTEXTLENGTH, 0, 0 );
	char* psz = new char[len+1];
	SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_DIRECTORY, WM_GETTEXT, len+1, (LPARAM) (LPCTSTR) psz );
	vdbFilename sNewDir( psz );
	delete[] psz; psz = 0;

	// append the new name to make the full filename
	len = SendMessage( hwndName, WM_GETTEXTLENGTH, 0, 0 );
	char* psz2 = new char[len+1];
	SendMessage( hwndName, WM_GETTEXT, len+1, (LPARAM) (LPCTSTR) psz2 );
	vdbFilename sName( psz2 );
	delete[] psz2; psz2 = 0;

	sName.ReplaceInvalidCharacters();
	sNewDir.Append( sName );
	
	// if the file exists, return to edit mode
	if ( sNewDir.Exists() )
		return;
	else
	{
		// try to create
		if ( sNewDir.CreateDir() == true )
		{
			// add to the tree
			HWND hwndTree = GetDlgItem( _hwnd, IDC_VDB_DIRECTORY_TREE );
			HTREEITEM hSelection = TreeView_GetSelection( hwndTree );
			HTREEITEM hNewItem = InsertDirectory( hwndTree, sName, hSelection, false );
			
			// select the new directory
			SendMessage( hwndTree, TVM_EXPAND, TVE_EXPAND, (LPARAM) (HTREEITEM) hSelection );
			if ( hNewItem != NULL )
				TreeView_SelectItem( hwndTree, hNewItem );
			SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_DIRECTORY, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) sNewDir );
		}
	
		// if the creation failed
		else
			return;
	}
}


//---------------------------------------
void DirectoryDlgClass::OnReject()
{
	HWND hwndName = GetDlgItem( _hwnd, IDC_VDB_NEW_DIRECTORY_NAME );
	ShowWindow( hwndName, SW_HIDE );

	ShowWindow( GetDlgItem( _hwnd, IDC_VDB_ACCEPT ), SW_HIDE );
	ShowWindow( GetDlgItem( _hwnd, IDC_VDB_REJECT ), SW_HIDE );

	HWND hCreate = GetDlgItem( _hwnd, IDC_VDB_CREATE_DIRECTORY );
	SendMessage( hCreate, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) _pData->_hCreateBitmap );
	EnableWindow( hCreate, TRUE );
}


//---------------------------------------
//^ The OnSelchangedDrives function is triggered when the user changes the
//  selection in the Drive listbox
void DirectoryDlgClass::OnSelchangedDrives()
{
	HTREEITEM hItem = (HTREEITEM) SendDlgItemMessage( _hwnd, IDC_VDB_DRIVES, TVM_GETNEXTITEM, TVGN_CARET, 0 );
	if ( hItem == NULL )
		return;

	ShowHourglass();
	char szDrive[3];
	TV_ITEM tvi;
	tvi.mask = TVIF_TEXT;
	tvi.hItem = hItem;
	tvi.pszText = szDrive;
	tvi.cchTextMax = sizeof(szDrive);
	SendDlgItemMessage( _hwnd, IDC_VDB_DRIVES, TVM_GETITEM, 0, (LPARAM) &tvi );

	// change the selected directory
	char szDriveFormatted[4];
	sprintf( szDriveFormatted, "%c:\\", szDrive[1] );
	SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_DIRECTORY, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) szDriveFormatted );

	// delete everything from the list of directories, add the root and then the children
	HWND hwndTree = GetDlgItem( _hwnd, IDC_VDB_DIRECTORY_TREE );
	SendMessage( hwndTree, TVM_SELECTITEM, TVGN_CARET, 0 );
	SendMessage( hwndTree, TVM_DELETEITEM, 0, (LPARAM) (HTREEITEM) TVI_ROOT );
	HTREEITEM hRoot = InsertDirectory( hwndTree, szDriveFormatted, TVI_ROOT, true );
	AddAllChildren( hwndTree, vdbFilename(szDriveFormatted), hRoot );
	SendMessage( hwndTree, TVM_EXPAND, TVE_EXPAND, (LPARAM) (HTREEITEM) hRoot );

	// select the drive root if this change really came from the user
	if ( _bIsInitializing == false )
		TreeView_SelectItem( hwndTree, TreeView_GetRoot( hwndTree ) );

	ShowPointer();
}


//---------------------------------------
//^ The OnSelchangedDirectory function is triggered when the user clicks an
//  item in the directory tree
void DirectoryDlgClass::OnSelchangedDirectory( NM_TREEVIEW* pnmtv )
{
	if ( _bIsInitializing == true )
		return;

	// get the selected directory
	HWND hwndTree = GetDlgItem( _hwnd, IDC_VDB_DIRECTORY_TREE );
	HTREEITEM hItem = pnmtv->itemNew.hItem;
	
	// recursively get the directory
	vdbFilename sDirectory;	
	if ( hItem != NULL )
		GetFullDirectoryName( hwndTree, hItem, sDirectory );

	// refresh the current directory control
	SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_DIRECTORY, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) sDirectory );
	_pData->SetDirectory( sDirectory );
}


//---------------------------------------
//^ The OnDblClickDirectory function is triggered when the user double clicks an
//  item in the directory tree
void DirectoryDlgClass::OnDblClickDirectory()
{
	ShowHourglass();
	// get the selected directory
	HWND hwndTree = GetDlgItem( _hwnd, IDC_VDB_DIRECTORY_TREE );
	HTREEITEM hItem = TreeView_GetSelection( hwndTree );
	
	// recursively get the directory
	vdbFilename sDirectory;	
	GetFullDirectoryName( hwndTree, hItem, sDirectory );

	// delete all children, find them again and add back in
	DeleteAllChildren( hwndTree, hItem );
	AddAllChildren( hwndTree, sDirectory, hItem );

	// refresh the current directory control
	SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_DIRECTORY, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) sDirectory );
	_pData->SetDirectory( sDirectory );
	ShowPointer();
}


//---------------------------------------
//^ The OnRefreshDrives function obtains all drives and displays them in the
//  listbox.
void DirectoryDlgClass::OnRefreshDrives()
{
	HWND hwndDrives = GetDlgItem( _hwnd, IDC_VDB_DRIVES );
	RefreshDrives( hwndDrives );

	// select the default drive
	vdbString s;
	s.Format( "(%c:)  ", _pData->_sDirectory[0] );
	HTREEITEM hDefaultItem = FindTreeItemByString( hwndDrives, s );
	if ( hDefaultItem != NULL )
		SendMessage( hwndDrives, TVM_SELECTITEM, TVGN_CARET, (LPARAM) hDefaultItem );
}


//---------------------------------------
//^ The OnRefreshDrives function obtains all drives and displays them in the
//  listbox.
void DirectoryDlgClass::RefreshDrives( HWND hwnd )
{
	// delete everything first
	SendMessage( hwnd, TVM_DELETEITEM, 0, (LPARAM) (HTREEITEM) TVI_ROOT );

	// Add all drives to the tree as root nodes
    char szDriveStrings[105];
	DWORD lenDriveString = GetLogicalDriveStrings( sizeof(szDriveStrings), szDriveStrings );
	char* p = szDriveStrings;
	 
	// loop through the character array which contains a list of null-terminated strings followed by a final null terminator. 
	while ( *p != 0 )
	{
		char* pDrive = p;
		while ( *p != 0 )
			p++;

		vdbString sFullDescription;
		sFullDescription.Format( "(%c:)  ", pDrive[0] );

		TV_INSERTSTRUCT tvis;
		tvis.hParent = TVI_ROOT;
		tvis.hInsertAfter = TVI_LAST;
		tvis.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;

		UINT driveType = GetDriveType( pDrive );
		if ( driveType == DRIVE_REMOVABLE )
			tvis.item.iImage = 0;
		else if ( driveType == DRIVE_FIXED )
			tvis.item.iImage = 1;
		else if ( driveType == DRIVE_CDROM )
			tvis.item.iImage = 2;
		else if ( driveType == DRIVE_REMOTE )
			tvis.item.iImage = 3;
		else if ( driveType == DRIVE_RAMDISK )
			tvis.item.iImage = 0;
		else
			tvis.item.iImage = 0;
		tvis.item.iSelectedImage = tvis.item.iImage;

		if ( driveType == DRIVE_REMOVABLE )
		{
			if ( pDrive[0] == 'A' || pDrive[0] == 'B' )
				sFullDescription += "Floppy";
			else
				sFullDescription += "Removable";
		}
		else if ( driveType == DRIVE_CDROM )
		{
			sFullDescription += "CD-ROM";
		}
		else if ( driveType == DRIVE_FIXED || driveType == DRIVE_REMOTE )
		{
			if ( driveType == DRIVE_FIXED )
			{
				char szVolumeName[80];
				DWORD volumeSerialNumber = 0; 
				DWORD maximumComponentLength = 0;
				DWORD fileSystemFlags = 0;
				if ( GetVolumeInformation( pDrive, szVolumeName, sizeof(szVolumeName), &volumeSerialNumber, &maximumComponentLength, &fileSystemFlags, NULL, 0 ) == TRUE )
					sFullDescription += szVolumeName;
			}
			else if ( driveType == DRIVE_REMOTE )
			{
				char szDrive[3];
				szDrive[0] = pDrive[0];
				szDrive[1] = ':';
				szDrive[2] = 0x0;

				char szDeviceDesc[80];
				DWORD size = 80;
				if ( WNetGetConnection( szDrive, szDeviceDesc, &size ) == NO_ERROR )
					sFullDescription += szDeviceDesc;
			}

			typedef BOOL (WINAPI *pGDFS)(LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);
			pGDFS pFunction = (pGDFS) GetProcAddress( GetModuleHandle( "kernel32.dll" ), "GetDiskFreeSpaceExA" );
			if ( pFunction != NULL )
			{
				ULARGE_INTEGER freeBytesAvailableToCaller;
				ULARGE_INTEGER totalNumberOfBytes;
				ULARGE_INTEGER totalNumberOfFreeBytes;
				vdbString sFreeDiskSpace;

				// GetDiskFreeSpaceEx( pDrive, &freeBytesAvailableToCaller, &totalNumberOfBytes, &totalNumberOfFreeBytes ) == TRUE ) 		// tested and works on NT/2000
				if ( (*pFunction)( pDrive, &freeBytesAvailableToCaller, &totalNumberOfBytes, &totalNumberOfFreeBytes ) == TRUE )
				{
					pphFormatBytes( sFreeDiskSpace, freeBytesAvailableToCaller.QuadPart );
					sFullDescription += "  (";
					sFullDescription += sFreeDiskSpace;
					sFullDescription += " Available)";
				}
			}
		}
		
		tvis.item.pszText = (char*)(const char*) sFullDescription;
		SendMessage( hwnd, TVM_INSERTITEM, 0, (LPARAM) &tvis );
		p++;
	}
}


//=============================================================================
// Helper functions
//=============================================================================

//---------------------------------------
//^ The GetFullDirectoryName function is called to recursively obtain the parent
//  directory all the way to the drive.
//> hwndTree is the handle to the tree control
//> hItem is the item whose parent is to be looked up
//> sDirectory is the directory returned
//< returns true if sDirectory contains a valid parent, false if we've reached the root
//
void DirectoryDlgClass::GetFullDirectoryName( HWND hwndTree, HTREEITEM hItem, vdbFilename& sDirectory )
{
	// get this level's text
	char szThisLevel[80];
	TV_ITEM	item;
	item.mask = TVIF_TEXT; 
	item.hItem = hItem;
	item.pszText = szThisLevel;
	item.cchTextMax = sizeof(szThisLevel);
	SendMessage( hwndTree, TVM_GETITEM, 0, (LPARAM) (TV_ITEM*) &item );

	// get the parent directory
	HTREEITEM hParent = (HTREEITEM) SendMessage( hwndTree, TVM_GETNEXTITEM, (WPARAM) (UINT) TVGN_PARENT, (LPARAM) (HTREEITEM) hItem );

	// recurse if there is a parent, stop if there isn't
	if ( hParent == NULL )
	{
		sDirectory = szThisLevel;	// root
		return;
	}
	else
		GetFullDirectoryName( hwndTree, hParent, sDirectory );

	// append all sub-directories upon return
	sDirectory.Append( szThisLevel );
}


//---------------------------------------
//^ The AddAllChildren function adds all sub-directories of the given argument to the
//  directory tree.
//> sDirectory is the drive/directory whose children are to be added.
//> hParent is the parent tree item
//
void DirectoryDlgClass::AddAllChildren( HWND hwndTree, vdbFilename& sDirectory, HTREEITEM hParent )
{
	vdbFilename sAllFiles = sDirectory;
	sAllFiles.Append( "*." );

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile( sAllFiles, &fd );
	if ( hFind == INVALID_HANDLE_VALUE )
		return;

	ShowHourglass();
	vdbSet<vdbFileDialogSort> allFiles;

	BOOL bMore = TRUE;
	while( bMore )
	{
		vdbFilename f( sDirectory );
		f.Append( fd.cFileName );

		// add only directories
		if ( f.IsDirectory() )
		{
			if ( strcmp( fd.cFileName, "." ) )
				if ( strcmp( fd.cFileName, ".." ) )
					allFiles.Add( new vdbFileDialogSort( fd.cFileName, true ) );
		}

		bMore = FindNextFile( hFind, &fd );
	}
	FindClose( hFind );	

	// sort by filename with directories first
	vdbSetSort<vdbFileDialogSort> s( allFiles );
	s.SelectionSort();

	int count = allFiles.Count();
	for ( int i = 0; i < count; i++ )
		InsertDirectory( hwndTree, allFiles[ s[i] ]._sFilename, hParent, false );
	
	ShowPointer();
}


//---------------------------------------
//^ The DeleteAllChildren function deletes all child items for the given item
//> sDirectory is the drive/directory whose children are to be added.
//> hParent is the parent tree item
//
void DirectoryDlgClass::DeleteAllChildren( HWND hwndTree, HTREEITEM hItem )
{
	HTREEITEM hNextItem = (HTREEITEM) SendMessage( hwndTree, TVM_GETNEXTITEM, (WPARAM) (UINT) TVGN_CHILD, (LPARAM) (HTREEITEM) hItem );
	while ( hNextItem != NULL )
	{
		SendMessage( hwndTree, TVM_DELETEITEM, 0, (LPARAM) (HTREEITEM) hNextItem );
		hNextItem = (HTREEITEM) SendMessage( hwndTree, TVM_GETNEXTITEM, (WPARAM) (UINT) TVGN_CHILD, (LPARAM) (HTREEITEM) hItem );
	}
}


//---------------------------------------
//^ The InsertDirectory function adds a single directory to the tree
//> sDirectory is the drive/directory to add.
//> bIsDrive is true if this is a drive specifier
//
HTREEITEM DirectoryDlgClass::InsertDirectory( HWND hwndTree, const char* szDirectory, HTREEITEM hParent, bool bIsDrive )
{
	assert( hParent != 0 );

	char szText[256];
	vdbUtility::SafeCopy( szText, szDirectory, sizeof(szText) );

	TV_INSERTSTRUCT tvis;
    tvis.hParent = hParent;
    tvis.hInsertAfter = TVI_LAST; 
    tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE; 
	tvis.item.pszText = szText;

	if ( bIsDrive == false )
		tvis.item.iImage = 4;
	else
	{
		UINT driveType = GetDriveType( szText );
		if ( driveType == DRIVE_REMOVABLE )
			tvis.item.iImage = 0;
		else if ( driveType == DRIVE_FIXED )
			tvis.item.iImage = 1;
		else if ( driveType == DRIVE_CDROM )
			tvis.item.iImage = 2;
		else if ( driveType == DRIVE_REMOTE )
			tvis.item.iImage = 3;
		else if ( driveType == DRIVE_RAMDISK )
			tvis.item.iImage = 0;
		else
			tvis.item.iImage = 0;
	}
	tvis.item.iSelectedImage = tvis.item.iImage;

	return (HTREEITEM) SendMessage( hwndTree, TVM_INSERTITEM, 0, (LPARAM) (LPTV_INSERTSTRUCT) &tvis );
}	


//---------------------------------------
//^ The AddDefaultDirectories function is called upon initialization to add all
//  the directories necessary to show the user's default.
//  This is a recursive function
//> hwndTree is the handle to the tree control
//> sDirectory is the drive/directory to add.
//> hParent is the parent tree item
//> bIsDrive is true if this is a drive, not a directory
//< returns the handle to the item added, but not its children.  Thus at the end
//  of the recursive calls, the final handle returned will be the innermost item,
//  excluding its children.
//
HTREEITEM DirectoryDlgClass::AddDefaultDirectories( HWND hwndTree, vdbFilename sDirectory, HTREEITEM hParent, bool bIsDrive )
{
	vdbFilename sThisLevel;

	int firstSlash = sDirectory.Find( '\\' );
	if ( firstSlash != -1 )
		sThisLevel = sDirectory.First( firstSlash );
	else
		sThisLevel = sDirectory;
		
	HTREEITEM hItem = NULL;
	if ( bIsDrive == true )
		hItem = InsertDirectory( hwndTree, sThisLevel, hParent, bIsDrive );
	else
		// if this is a sub-directory, find it
		hItem = FindChild( hwndTree, hParent, sThisLevel );
	
	// if hItem is NULL something went wrong
	if ( hItem == NULL )
		return hParent;
		 
	// now add all children of this level
	vdbFilename sFullDirectory;	
	GetFullDirectoryName( hwndTree, hItem, sFullDirectory );
	AddAllChildren( hwndTree, sFullDirectory, hItem );

	// expand all children
	SendMessage( hwndTree, TVM_EXPAND, TVE_EXPAND, (LPARAM) (HTREEITEM) hItem );

	// select this level
	HTREEITEM hDefaultItem = FindTreeItemByString( hwndTree, sThisLevel, hItem );
	if ( hDefaultItem != NULL )
		SendMessage( hwndTree, TVM_SELECTITEM, TVGN_CARET, (LPARAM) hDefaultItem );

	// if there are more sub-directories, add them
	if ( firstSlash != -1 )
	{
		vdbFilename sNextLevel = sDirectory.Middle( firstSlash + 1 );
		
		// when we've reached the bottom
		if ( sNextLevel.IsNull() )
			return hItem;
		
		// if there are more sub-directories, recurse
		else
			AddDefaultDirectories( hwndTree, sNextLevel, hItem, false );
	}

	return hItem;
}


//---------------------------------------
//^ The FindChild function finds the child item with the requested text
//> hwndTree is the handle to the tree control
//> hParent is the parent tree item
//> szFindString is the search text
//< returns the item if found or NULL if not found
//
HTREEITEM DirectoryDlgClass::FindChild( HWND hwndTree, HTREEITEM hParent, const char* szFindString )
{
	// get the parent directory
	HTREEITEM hNextItem = (HTREEITEM) SendMessage( hwndTree, TVM_GETNEXTITEM, (WPARAM) (UINT) TVGN_CHILD, (LPARAM) (HTREEITEM) hParent );
		
	while( hNextItem != NULL )
	{
		// get this level's text
		char szThisLevel[80];
		TV_ITEM	item;
		item.mask = TVIF_TEXT; 
		item.hItem = hNextItem;
		item.pszText = szThisLevel;
		item.cchTextMax = sizeof(szThisLevel);
		SendMessage( hwndTree, TVM_GETITEM, 0, (LPARAM) (TV_ITEM*) &item );

		int len = strlen( szFindString );
		if ( !_strnicmp( szThisLevel, szFindString, len) )
			return hNextItem;

		hNextItem = (HTREEITEM) SendMessage( hwndTree, TVM_GETNEXTITEM, (WPARAM) (UINT) TVGN_NEXT, (LPARAM) (HTREEITEM) hNextItem );
	}
	
	return NULL;	
}


//-------------------------------------
//^ The FindTreeItemByString function searches the tree to find the tree item
//  with the specified string value
//> ctlTree is the tree control to search
//> searchKey is the value to search for
//> hStart is the starting point for the search, or NULL to begin at the root
//< returns the handle to the tree item if found, otherwise returns NULL
//
HTREEITEM DirectoryDlgClass::FindTreeItemByString( HWND ctlTree, vdbString& searchKey, HTREEITEM hStart )
{
	HTREEITEM hItem;

	// if there's no user-specified start, get the first item of the entire tree
	if ( hStart == NULL )
		hItem = (HTREEITEM) SendMessage( ctlTree, TVM_GETNEXTITEM, TVGN_ROOT, NULL );
	else
		hItem = hStart;


	while ( hItem != NULL )
	{
		char szText[81];
		TV_ITEM tvi;
		tvi.mask = TVIF_TEXT;
		tvi.hItem = hItem;
		tvi.pszText = szText;
		tvi.cchTextMax = 81;
		SendMessage( ctlTree, TVM_GETITEM, 0, (LPARAM) &tvi );

		int len = min( searchKey.Length(), 81);
		if ( !_strnicmp( szText, searchKey, len ) )
			return hItem;
		hItem = WalkTree( ctlTree, hItem );
	}
	return NULL;
}


//-------------------------------------
//^ The WalkTree function gets the first child of the given item, or if it has
//  no children, gets the items next sibling, or if it has no more siblings,
//  gets its parent's next sibling.
//^ This is a recursive function
//> ctlTree is the tree control to walk
//> hItem is the current position to walk from
//< returns the next item or NULL when there are no more items
//
HTREEITEM DirectoryDlgClass::WalkTree( HWND ctlTree, HTREEITEM hItem )
{
	HTREEITEM hNextItem;

	// get the first child item of this item.
	hNextItem = (HTREEITEM) SendMessage( ctlTree, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM) hItem );
  	if ( hNextItem != NULL )
		return hNextItem;

	// if this item has no children, get this item's next sibling.
	hNextItem = (HTREEITEM) SendMessage( ctlTree, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM) hItem );
  	if ( hNextItem != NULL )
		return hNextItem;

	// if this item has no more sibling's, go back to the parent and get the next sibling of the parent,
	HTREEITEM hParentItem = WalkUpTree( ctlTree, hItem );
	return hParentItem;
}


//-------------------------------------
//^ The WalkUpTree function gets the next sibling of this item's parent
//  (the next aunt), or if it has no more aunt's get's the next great-aunt, and
//  so forth until the end is reached.
//^ This is a recursive function
//> ctlTree is the tree control to walk up
//> hItem is the current position to walk from
//< returns the next item or NULL when there are no more items
//
HTREEITEM DirectoryDlgClass::WalkUpTree( HWND ctlTree, HTREEITEM hItem )
{
	HTREEITEM hNextItem;

	// walk back to the parent
	HTREEITEM hParentItem = (HTREEITEM) SendMessage( ctlTree, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM) hItem );
  	if ( hParentItem == NULL )
		return NULL;
	
	// get the next sibling
	hNextItem = (HTREEITEM) SendMessage( ctlTree, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM) hParentItem );
  	if ( hNextItem != NULL )
		return hNextItem;

	// recurse back up to top
	return WalkUpTree( ctlTree, hParentItem );
}

