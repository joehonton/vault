//=============================================================================
//
// FILE NAME:       vdbFileDialog.cpp
//
// ESSENCE:         Dialog for displaying and selecting files and directories on
//                  this computer
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    10/24/98
//
// COPYRIGHT:       Copyright © 1998 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbFileDialog.h"
#include "..\\vdbResources\\resource.h"
#include "vdbAssert.h"
#include "VaultResourceInstance.h"
#include <vdbException.h>
#include "vdbUtility.h"
#include "vdbWindowSupport.h"
#include <cursors.h>
#include <vdbSetSort.h>
#include <vdbFileDialogSort.h>
#include "vdbString.h"


//=============================================================================
//  Data transfer into and out of the dialog
//=============================================================================

//-------------------------------------
vdbFileDlgData::vdbFileDlgData()
	: vdbDirectoryDlgData(),
	  _sDescription( "Select file" ),
	  _bMultipleSelect( false ),
	  _hFileImage( 0 )
{
	_sTitle = "Select file";
	_setOfFiles.Add( new vdbFilename );
}


//-------------------------------------
vdbFileDlgData::vdbFileDlgData( HWND hParent, const char* szTitle, const char* szDescription, const char* szDefaultDirectory, const char* szDefaultFile, bool bMultipleSelect )
	: vdbDirectoryDlgData( hParent, szTitle, szDefaultDirectory, false ),
	  _sDescription( szDescription ),
	  _bMultipleSelect( bMultipleSelect ),
	  _hFileImage( 0 )
{
	_setOfFiles.Add( new vdbFilename( szDefaultFile ) );
}


//-------------------------------------
vdbFileDlgData::~vdbFileDlgData()
{
	vdbDirectoryDlgData::Cleanup();
	Cleanup();
}


//-------------------------------------
void vdbFileDlgData::Cleanup()
{
	if ( _hFileImage != NULL )
	{
		// TODO: fix this
		// For some unknown reason this statement causes an exception 0x000005 Access Violation
		// only on Windows 98 and only from the derived class ImportExportSelectFile
		
		// ImageList_Destroy( _hFileImage );

		_hFileImage = 0;
	}
}


//-------------------------------------
void vdbFileDlgData::SetSelectedFilename( const char* szFilename )
{
	_setOfFiles[0] = szFilename;
}


//-------------------------------------
int vdbFileDlgData::GetSelectedFileCount()
{
	return _setOfFiles.Count();
}


//-------------------------------------
const char* vdbFileDlgData::GetSelectedFilename( int which )
{
	if ( _setOfFiles.Count() > which )
		return (const char*) _setOfFiles[which];
	else
		return "";
}


//-------------------------------------
const char* vdbFileDlgData::GetSelectedFilename()
{
	if ( _setOfFiles.Count() > 0 )
		return (const char*) _setOfFiles[0];
	else
		return "";
}


//-------------------------------------
//^ The AddFileFilter function takes a description like "All files (*.*)" and
//  a mask like "*" or "txt" and adds them to the filter list.
//
void vdbFileDlgData::AddFileFilter( const char* szDescription, const char* szMask, bool bSelected )
{
	_filterDescriptions.Add( new vdbString( szDescription ) );
	_filterMasks.Add( new vdbString( szMask ) );
	_filterSelection.Add( new bool(bSelected) );
}


//-------------------------------------
//^ The GetSelectedFilter function returns the filter selected by the user.
//  Something like "*" or "txt"
const char* vdbFileDlgData::GetSelectedFilter() const
{
	for ( int i = 0; i < _filterSelection.Count(); i++ )
	{
		if ( _filterSelection[i] == true )
			return (const char*) _filterMasks[i];
	}
	
	return "";
}


//=============================================================================
//  Public function for showing the dialog
//=============================================================================

//---------------------------------------------
//^ The vdbFileDialog shows a dialog for selecting one or more files
//> data is a reference to a vdbFileDlgData instance.  These values will be used
//  to initialize the dialog box.  The selected files, directory, and drive is
//  also returned through this object.
//< returns true if the user pressed OK or false if Cancel was pressed.
//
bool vdbFileDialog( vdbFileDlgData& data )
{
	assert ( &data != 0 );
	int rc = DialogBoxParam( VaultResourceInstance::Handle(), MAKEINTRESOURCE(IDD_FILE_DIALOG), data._hParent, (DLGPROC) FileDlgProc, (LPARAM) &data );
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
// Procedure for File dialog box
BOOL CALLBACK FileDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static FileDlgClass* pDlgClass = 0;

	switch ( message )
	{
		case WM_INITDIALOG:
			if ( !(pDlgClass = new FileDlgClass) )
				return EndDialog( hwnd, IDABORT );
			else
				return pDlgClass->OnInitDialog( hwnd, (vdbFileDlgData*) lParam );
		
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

				case IDC_VDB_FILE_TYPES:
					pDlgClass->OnSelchangedFilter();
					break;

				default:
					break;
			}
	
			return TRUE;
		}
		
		case WM_NOTIFY:
		{
			NM_TREEVIEW* pnmtv = (NM_TREEVIEW*) lParam;
			NMHDR* pnmh = (NMHDR*) lParam;

			switch( wParam )
			{
				case IDC_VDB_FILES:
					if ( pnmh->code == NM_CLICK )
						pDlgClass->OnSelchangedFile();
					else if ( pnmh->code == NM_DBLCLK )
						pDlgClass->OnDblClickFile();
					break;

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
void FileDlgClass::SetDescription( const char* sz )
{
	SendDlgItemMessage( _hwnd, IDC_VDB_DESCRIPTION, WM_SETTEXT, 0, (LPARAM) sz );
}


//---------------------------------------
void FileDlgClass::SetSelectedFilename( const char* szFilename )
{
	_pData->_setOfFiles.ClearAll();
	_pData->_setOfFiles.Add( new vdbFilename( szFilename ) );
	SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_FILE, WM_SETTEXT, 0, (LPARAM) szFilename );
}


//---------------------------------------
void FileDlgClass::AddFileFilter( const char* szDescription, const char* szMask, bool bSelected )
{
	_pData->_filterDescriptions.Add( new vdbString( szDescription ) );
	_pData->_filterMasks.Add( new vdbString( szMask ) );
	_pData->_filterSelection.Add( new bool(bSelected) );
	
	int index = SendDlgItemMessage( _hwnd, IDC_VDB_FILE_TYPES, LB_ADDSTRING, 0, (LPARAM) szDescription );
	if ( bSelected == true )
		SendDlgItemMessage( _hwnd, IDC_VDB_FILE_TYPES, LB_SETSEL, TRUE, index );
}


//---------------------------------------
void FileDlgClass::RemoveFileFilters()
{
	_pData->_filterDescriptions.ClearAll();
	_pData->_filterMasks.ClearAll();
	_pData->_filterSelection.ClearAll();

	int count =  SendDlgItemMessage( _hwnd, IDC_VDB_FILE_TYPES, LB_GETCOUNT, 0, 0 );
	for ( int i = count-1; i >= 0; i-- )
		SendDlgItemMessage( _hwnd, IDC_VDB_FILE_TYPES, LB_DELETESTRING, i, 0 );
}


//---------------------------------------
BOOL FileDlgClass::OnInitDialog( HWND hwnd, vdbFileDlgData* pData )
{
	assert( pData != 0 );
	_pData = pData;
	_bIsInitializing = true;

	if ( DirectoryDlgClass::OnInitDialog( hwnd, (vdbDirectoryDlgData*) pData ) == FALSE )
		return FALSE;

	// set the filters
	HWND hwndFilter = GetDlgItem( hwnd, IDC_VDB_FILE_TYPES );
	if ( _pData->_filterDescriptions.Count() == 0 )
	{
		_pData->_filterDescriptions.Add( new vdbString("All files (*.*)") );
		_pData->_filterMasks.Add( new vdbString("*") );
		_pData->_filterSelection.Add( new bool(true) );
		SendMessage( hwndFilter, LB_ADDSTRING, 0, (LPARAM) "All files (*.*)" );
		SendMessage( hwndFilter, LB_SETSEL, TRUE, 0 );
	}
	else
	{
		for ( int i = 0; i < _pData->_filterDescriptions.Count(); i++ )
		{
			SendMessage( hwndFilter, LB_ADDSTRING, 0, (LPARAM) (const char*) _pData->_filterDescriptions[i] );
			SendMessage( hwndFilter, LB_SETSEL, _pData->_filterSelection[i], i );
		}
	}

	// make the file list single select if requested
	if ( _pData->_bMultipleSelect == false )
	{
		HWND hwndFiles = GetDlgItem( hwnd, IDC_VDB_FILES );
		long styles = GetWindowLong( hwndFiles, GWL_STYLE );
		styles |= LVS_SINGLESEL;
		SetWindowLong( hwndFiles, GWL_STYLE, styles );
	}

	// set the file list control bitmaps
	_pData->_hFileImage = ImageList_LoadImage( VaultResourceInstance::Handle(), MAKEINTRESOURCE(IDB_VDB_FILE_ICONS), 16, 2, CLR_NONE, IMAGE_BITMAP, LR_DEFAULTCOLOR );
	SendDlgItemMessage( hwnd, IDC_VDB_FILES, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM) _pData->_hFileImage );

	// Add one column to the file list
	LV_COLUMN lvc;
	lvc.mask = LVCF_WIDTH;
	lvc.cx = 220;
	SendDlgItemMessage( hwnd, IDC_VDB_FILES, LVM_INSERTCOLUMN, 0, (LPARAM) &lvc );

	// refresh the description
	SendDlgItemMessage( hwnd, IDC_VDB_DESCRIPTION, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) pData->_sDescription );

	// refresh the selected file control
	vdbString s;
	if ( pData->_setOfFiles.Count() == 1 )
		s = pData->_setOfFiles[0];
	else
	{
		for ( int i = 0; i < pData->_setOfFiles.Count(); i++ )
		{
			s += "\"";
			s += pData->_setOfFiles[i];
			s += "\" ";
		}
	}
	SendDlgItemMessage( hwnd, IDC_VDB_SELECTED_FILE, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) s );

	// fill the file list with all files of the selected directory
	_bIsInitializing = false;
	FindAllFiles();

	return TRUE;
}


//-------------------------------------
//^ The DialogToData function transfers the file selections from the dialog 
//  controls to the data structure
void FileDlgClass::DialogToData()
{
	// get the selected directory from the static control
	int len = SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_DIRECTORY, WM_GETTEXTLENGTH, 0, 0 );
	char* psz = new char[len+1];
	SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_DIRECTORY, WM_GETTEXT, len+1, (LPARAM) (LPCTSTR) psz );
	_pData->SetDirectory( psz );
	delete[] psz; psz = 0;

	// get the selected files from the text control
	_pData->_setOfFiles.ClearAll();
	len = SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_FILE, WM_GETTEXTLENGTH, 0, 0 );
	if ( len <= 0 )
		return;
	char* pszText = new char[len+1];
	SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_FILE, WM_GETTEXT, len+1, (LPARAM) pszText );
	
	// if the first character is not a double quote, then we assume this is a single file
	if ( pszText[0] != '"' )
	{
		_pData->_setOfFiles.Add( new vdbFilename( pszText ) );
		delete[] pszText; pszText = 0;
		return;
	}

	// otherwise the user has selected multiple files, each surrounded by double quotes and separated by spaces
	vdbString sText( pszText );
	delete[] pszText; pszText = 0;

	int interFilenameSpace = 0;
	while ( interFilenameSpace < len )
	{
		// find the opening and closing double quote
		int openingQuote = sText.FindNext( '"', interFilenameSpace );
		if ( openingQuote == -1 )
			break;
		int closingQuote = sText.FindNext( '"', openingQuote + 1 );
		if ( closingQuote == -1 )
			break;
		interFilenameSpace = closingQuote + 1;
		
		int filenameLength = (closingQuote - openingQuote) - 1;
		if ( filenameLength > 0 )
			_pData->_setOfFiles.Add( new vdbFilename( sText.Middle( openingQuote+1, filenameLength) ) );
	}
}


//---------------------------------------
void FileDlgClass::OnOK()
{
	DialogToData();

	EndDialog( _hwnd, IDOK );
}


//---------------------------------------
void FileDlgClass::OnCancel()
{
	_pData->SetDirectory( "" );
	_pData->_setOfFiles.ClearAll();
	EndDialog( _hwnd, IDCANCEL );
}


//---------------------------------------
//^ The OnSelchangedFile function obtains all selected files in the list and
//  puts their names in the selected files control
void FileDlgClass::OnSelchangedFile()
{
	HWND hwndFiles = GetDlgItem( _hwnd, IDC_VDB_FILES );
	int index = SendMessage( hwndFiles, LVM_GETNEXTITEM, -1, LVNI_SELECTED );
	if ( index == -1 )
		return;

	vdbString s;
	LV_ITEM lvi;
	int count = 0;
	while( index != -1 )
	{
		char szFilename[_MAX_FNAME];
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iItem = index;
		lvi.iSubItem = 0;
		lvi.pszText = szFilename;
		lvi.cchTextMax = sizeof(szFilename);
		SendMessage( hwndFiles, LVM_GETITEM, 0, (LPARAM) &lvi );
		
		// don't do anything for selected directories
		if ( lvi.lParam == false )	// true == directory; false == file
		{
			s += "\"";
			s += lvi.pszText;
			s += "\" ";
			count++;
		}
		index = SendMessage( hwndFiles, LVM_GETNEXTITEM, index, LVNI_SELECTED );
	}
	if ( count == 1 )
		s = lvi.pszText;

	if ( count > 0 )
		SendDlgItemMessage( _hwnd, IDC_VDB_SELECTED_FILE, WM_SETTEXT, 0, (LPARAM) (const char*) s );
}


//---------------------------------------
//^ The OnDblClickFile function is triggered when the user double clicks an
//  item in the file list.  If it is a directory, change directories.
//
void FileDlgClass::OnDblClickFile()
{
	HWND hwndFiles = GetDlgItem( _hwnd, IDC_VDB_FILES );

	// get the selected item from the file control
	int index = SendMessage( hwndFiles, LVM_GETNEXTITEM, -1, LVNI_SELECTED );
	if ( index == -1 )
		return;

	vdbString s;
	LV_ITEM lvi;
	char szDblClickFilename[80];
	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.iItem = index;
	lvi.iSubItem = 0;
	lvi.pszText = szDblClickFilename;
	lvi.cchTextMax = sizeof(szDblClickFilename);
	SendMessage( hwndFiles, LVM_GETITEM, 0, (LPARAM) &lvi );

	// don't do anything if the double clicked item is a file
	if ( lvi.lParam == false )	// true == directory; false == file
		return;

	// start by getting the selected directory in the directory tree
	HWND hwndTree = GetDlgItem( _hwnd, IDC_VDB_DIRECTORY_TREE );
	HTREEITEM hSelectedParentDirectory = TreeView_GetSelection( hwndTree );

	// if it doesn't have children yet, get them
	HTREEITEM hFirstChild = TreeView_GetChild( hwndTree, hSelectedParentDirectory );
	if ( hFirstChild == NULL )
	{
		vdbFilename sDirectory;	
		GetFullDirectoryName( hwndTree, hSelectedParentDirectory, sDirectory );
		AddAllChildren( hwndTree, sDirectory, hSelectedParentDirectory );
		hFirstChild = TreeView_GetChild( hwndTree, hSelectedParentDirectory );
		if ( hFirstChild == NULL )
			return;
	}

	// find the child in the tree which corresponds to the selected directory in the file list
	HTREEITEM hNextChild = hFirstChild;
	char szThisChild[80];
	while ( hNextChild != NULL )
	{
		// get this child's text
		TV_ITEM	item;
		item.mask = TVIF_TEXT; 
		item.hItem = hNextChild;
		item.pszText = szThisChild;
		item.cchTextMax = sizeof(szThisChild);
		SendMessage( hwndTree, TVM_GETITEM, 0, (LPARAM) (TV_ITEM*) &item );

		// have we found the tree item corresponding to the double-clicked item
		if ( !strcmp( szThisChild, szDblClickFilename ) )
		{
			// select the matching item and do the normal directory selection process
			TreeView_SelectItem( hwndTree, hNextChild );
			return;
		}

		// keep searching
		hNextChild = TreeView_GetNextSibling( hwndTree, hNextChild );	
	}	
}


//---------------------------------------
//^ The OnSelchangedFilter function is triggered when the user changes filter selections
void FileDlgClass::OnSelchangedFilter()
{
	int count = SendDlgItemMessage( _hwnd, IDC_VDB_FILE_TYPES, LB_GETCOUNT, 0, 0 );
	for ( int i = 0; i < count; i++ )
	{
		if ( SendDlgItemMessage( _hwnd, IDC_VDB_FILE_TYPES, LB_GETSEL, i, 0 ) == 0 )
			_pData->_filterSelection[i] = false;
		else
			_pData->_filterSelection[i] = true;
	}

	FindAllFiles();
}


//---------------------------------------
//^ The OnSelchangedDirectory function is triggered when the user clicks an
//  item in the directory tree
void FileDlgClass::OnSelchangedDirectory( NM_TREEVIEW* pnmtv )
{
	DirectoryDlgClass::OnSelchangedDirectory( pnmtv );
	FindAllFiles();
}


//---------------------------------------
//^ The OnDblClickDirectory function is triggered when the user double clicks an
//  item in the directory tree
void FileDlgClass::OnDblClickDirectory()
{
	DirectoryDlgClass::OnDblClickDirectory();
}


//---------------------------------------
//^ The OnSelchangedDrives function is trigger when the user changes the
//  selection in the Drive listbox
void FileDlgClass::OnSelchangedDrives()
{
	DirectoryDlgClass::OnSelchangedDrives();
}


//=============================================================================
// Helper functions
//=============================================================================

//-------------------------------------
//^ The FindAllFiles function places all directories and files in one in-memory
//  list where the first character is a 0 or 1 to distinguish it as a directory
//  or filename.  This list is sorted and entries are added to the file control.
//  
void FileDlgClass::FindAllFiles()
{
	if ( _bIsInitializing == true )
		return;

	HWND hwndFiles = GetDlgItem( _hwnd, IDC_VDB_FILES );
	SendMessage( hwndFiles, LVM_DELETEALLITEMS, 0, 0L );

	vdbFilename sSelectedDir = _pData->GetDirectory();
	if ( sSelectedDir.IsNull() )
		return;

	vdbFilename sAllFiles( sSelectedDir );
	sAllFiles.Append( "*.*" );

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile( sAllFiles, &fd );
	if ( hFind == INVALID_HANDLE_VALUE )
		return;

	ShowHourglass();
	vdbSet<vdbFileDialogSort> allFiles;

	BOOL bMore = TRUE;
	while( bMore )
	{
		vdbFilename f( sSelectedDir );
		f.Append( fd.cFileName );

		// always add directories
		if ( f.IsDirectory() )
		{
			if ( strcmp( fd.cFileName, "." ) )
				if ( strcmp( fd.cFileName, ".." ) )
					allFiles.Add( new vdbFileDialogSort( fd.cFileName, true ) );
		}
		// if it is not filtered out, add it to the list
		else
		{
			vdbString sExt = f.Extension();
			for ( int i = 0; i < _pData->_filterSelection.Count(); i++ )
			{
				if ( _pData->_filterSelection[i] == true )
				{
					if ( _pData->_filterMasks[i] == "*" || _pData->_filterMasks[i].CompareNoCase(sExt) )
					{
						allFiles.Add( new vdbFileDialogSort( fd.cFileName, false ) );
						break;
					}
				}
			}
		}

		bMore = FindNextFile( hFind, &fd );
	}
	FindClose( hFind );	

	// sort by filename with directories first
	vdbSetSort<vdbFileDialogSort> s( allFiles );
	s.SelectionSort();

	int which = 0;
	int count = allFiles.Count();
	for ( int i = 0; i < count; i++ )
	{
		int index = s[i];
		char sz[100];
		vdbUtility::SafeCopy( sz, allFiles[index]._sFilename, sizeof(sz) );

		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		lvi.iItem = which;
		lvi.iSubItem = 0;
		lvi.pszText = sz;
		lvi.iImage = ( allFiles[index]._bIsDirectory ? 0 : 1 );
		lvi.lParam = allFiles[index]._bIsDirectory;
		SendMessage( hwndFiles, LVM_INSERTITEM, 0, (LPARAM) &lvi );
		which++;
	}
	
	ShowPointer();
}
