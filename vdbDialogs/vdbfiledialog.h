//=============================================================================
//
// FILE NAME:       vdbFileDialog.h
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

#ifndef __vdbFileDialog_h
#define __vdbFileDialog_h

#ifndef __vdbDirectoryDialog_h
	#include "vdbDirectoryDialog.h"
#endif

#ifndef __vdbExport_h
	#include "vdbExport.h"
#endif

#ifndef __vdbString_h
	#include "vdbString.h"
#endif

#ifndef __vdbFilename_h
	#include "vdbFilename.h"
#endif

#ifndef __vdbSet_h
	#include "vdbSet.h"
#endif


//=============================================================================
//  Data transfer into and out of the dialog
//=============================================================================
class vdbFileDlgData : public vdbDirectoryDlgData
{
public:
	vdbFileDlgData();
	vdbFileDlgData( HWND hParent, const char* szTitle, const char* szDescription, const char* szDefaultDirectory, const char* szDefaultFile, bool bMultipleSelect );
	~vdbFileDlgData();
	void Cleanup();

public:
	void SetDescription( const char* sz )			{ _sDescription = sz; }
	void SetMultipleSelect( bool b = true )			{ _bMultipleSelect = b; }
	void SetSelectedFilename( const char* szFilename );
	int GetSelectedFileCount();
	const char* GetSelectedFilename( int which );	// multiple select list
	const char* GetSelectedFilename();				// single select list
	void AddFileFilter( const char* szDescription, const char* szMask, bool bSelected );
	const char* GetSelectedFilter() const;

public:
	vdbString _sDescription;
	bool _bMultipleSelect;
	vdbSet<vdbFilename> _setOfFiles;
	vdbSet<vdbString> _filterDescriptions;
	vdbSet<vdbString> _filterMasks;
	vdbSet<bool> _filterSelection;
	HIMAGELIST _hFileImage;
};


//=============================================================================
//  Public function for showing the dialog
//=============================================================================
extern "C" bool vdbFileDialog( vdbFileDlgData& data );



//=============================================================================
//  Class data and member functions for handling window messages
//=============================================================================
class FileDlgClass : public DirectoryDlgClass
{
	friend BOOL CALLBACK FileDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

public:
	void SetDescription( const char* sz );
	void SetSelectedFilename( const char* szFilename );
	void AddFileFilter( const char* szDescription, const char* szMask, bool bSelected );
	void RemoveFileFilters();
	void DialogToData();

protected:	 
	// message processing functions
	BOOL OnInitDialog( HWND hwnd, vdbFileDlgData* pData );
	void OnOK();
	void OnCancel();
	void OnSelchangedFile();
	void OnDblClickFile();
	void OnSelchangedFilter();
	void OnSelchangedDirectory( NM_TREEVIEW* pnmtv );
	void OnDblClickDirectory();
	void OnSelchangedDrives();

private:
	// helper functions
	void FindAllFiles();

private:
	vdbFileDlgData* _pData;
	bool _bIsInitializing;
};

#endif // __vdbFileDialog_h
