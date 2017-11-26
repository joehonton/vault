//=============================================================================
//
// CLASS:           DataSourceDlg
//
// ESSENCE:         Data sources dialog procedure and class
//
// FILE NAME:       vdbDataSource.h
//
// INITIAL DATE:    9/7/97
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1997 by Joe Honton
//
//=============================================================================

#ifndef __vdbDataSource_h
#define __vdbDataSource_h

#ifndef __vdbExport_h
	#include <vdbExport.h>
#endif

#ifndef __vdbString_h
	#include <vdbString.h>
#endif

#ifndef __vdbSQL_h
	#include <vdbSQL.h>
#endif

class vdbDatabase;


//=============================================================================
//  Data transfer into and out of the dialog
//=============================================================================
class vdbDataSourceDlgData
{
public:
	vdbDataSourceDlgData()
		: _sSelectedDataSource( "" ),
		  _pDB( 0 )
	{
	}

	vdbDataSourceDlgData( const char* szSource, vdbDatabase* pDB = NULL )
		: _sSelectedDataSource( szSource ),
		  _pDB( pDB )
	{
	}

public:
	vdbString _sSelectedDataSource;
	vdbDatabase* _pDB;				// if supplied, then used for creating a new database
};


//=============================================================================
//  Public function for showing the dialog
//=============================================================================
extern "C" bool vdbDataSourceDialog( vdbDataSourceDlgData& data );



//=============================================================================
//  Class data and member functions for handling window messages
//=============================================================================
class DataSourceDlgClass
{
	friend BOOL CALLBACK DataSourceDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

private:	 
	// message processing functions
	BOOL OnInitDialog( HWND hwnd, vdbDataSourceDlgData* pData );
	void OnOK( HWND hwnd );
	void OnCancel( HWND hwnd );
	void OnNewDatabase( HWND hwnd );
	void OnSelchangeSource( HWND hwnd );
	void OnDblClickSource( HWND hwnd );

private:
	vdbDataSourceDlgData* _pData;
};


#endif // __vdbDataSource_h
