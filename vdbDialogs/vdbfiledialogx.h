//=============================================================================
//
// FILE NAME:       vdbFileDialog.h
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

#ifndef __vdbFileDialog_h
#define __vdbFileDialog_h

#ifndef __vdbString_h
	#include "vdbString.h"
#endif

#ifndef __vdbFilename_h
	#include "vdbFilename.h"
#endif

#ifndef __vdbSet_h
	#include <vdbSet.h>
#endif


class vdbFileDlgDataX;

//=============================================================================
//  Public function for showing the dialog
//=============================================================================
extern "C" bool vdbFileDialogX( vdbFileDlgDataX& data );



//=============================================================================
//  Data transfer into and out of the dialog
//=============================================================================
class vdbFileDlgDataX
{
public:
	vdbFileDlgDataX();
	vdbFileDlgDataX( vdbString title, vdbFilename& defaultFilename, vdbString filters );

public:
	void SetTitle( const vdbString& s )				{ sTitle = s; }
	void SetFilename( const vdbFilename& f )		{ sFilename = f; }
	void SetFilters( const vdbString& s )			{ sFilters = s; }
	void SetFlags( long f )							{ flags = flags | f; }
	void SetDefaultExtension( const vdbString& s )	{ sDefaultExtension = s; }
	const vdbFilename& GetFilename() const			{ return sFilename; }
	vdbFilename Directory() const;
	void GetFileList( vdbSet<vdbFilename>& setOfFiles ) const;

private:
	vdbString sTitle;
	vdbFilename sFilename;
	vdbString sFilters;		// eg "Map Files (*.map)|*.map|All Files (*.*)|*.*||"
	long flags;
	vdbString sDefaultExtension;
	
	friend bool vdbFileDialogX( vdbFileDlgDataX& data );
};



#endif // __vdbFileDialog_h
