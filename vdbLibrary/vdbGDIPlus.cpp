//=============================================================================
//
// CLASS:           vdbGDIPlus
//
// OVERVIEW:		The vdbGDIPlus class
//
// FILE NAME:       vdbGDIPlus.cpp
//
// INITIAL DATE:    02/13/2002
//
// COPYRIGHT:		Copyright © 2002 by Joe Honton
//
//=============================================================================

#include <vdbWindows.h>
#include "vdbGDIPlus.h"
#include <vdbAssert.h>
#include <vdbFilename.h>
#include <vdbWideChar.h>


//-------------------------------------
//^ The GetEncoderClsid function gets the CLSID corresponding
//  to the requested image format
//> format may be "image/bmp", "image/jpeg", "image/gif", "image/png", "image/tiff"
//< returns -1 on failure or an interger value on success
//
int vdbGDIPlus::GetEncoderClsid( const WCHAR* format, CLSID* pClsid )
{
	// remember to use "jpeg" not "jpg"
	if ( !wcscmp( format, L"jpg" ) )
		assert( false );
	
	UINT countEncoders = 0;
	UINT size = 0;
	GetImageEncodersSize( &countEncoders, &size );
	if ( size == 0 )
		return -1;

	ImageCodecInfo* pICI = (ImageCodecInfo*) new char[size];;
	if ( pICI == NULL )
		return -1;

	GetImageEncoders( countEncoders, size, pICI );

	for ( UINT i = 0; i < countEncoders; i++)
	{
		if ( wcscmp( pICI[i].MimeType, format ) == 0 )
		{
			*pClsid = pICI[i].Clsid;
			delete[] pICI; pICI = 0;
			return i;
		}    
	}

	delete[] pICI; pICI = 0;
	return -1;
}


//-------------------------------------
//^ The SaveImage function saves the bitmap to a file using the encoding
//  implied by the filename extension.
//> pImage is the bitmap object to save
//> szFilename is the fully qualified filename to save the bitmap to
//  If the directory does not exist it will be created.  The filename extension
//  must be "gif", "bmp", "jpg", "jpeg", "png", or "tif"
//< returns true on success, false on failure
//
bool vdbGDIPlus::SaveImage( Image* pImage, const char* szFilename )
{
	vdbFilename sFilename( szFilename );
	vdbFilename sDir( sFilename.Directory() );
	if ( !sDir.Exists() )
	{
		if ( sDir.CreateDir() == false )
			return false;
	}
	
	vdbString sExtension = sFilename.Extension();
	CLSID clsid;
	if ( sExtension.CompareNoCase( "gif" ) )
		GetEncoderClsid( L"image/gif", &clsid );
	else if ( sExtension.CompareNoCase( "jpeg" ) )
		GetEncoderClsid( L"image/jpeg", &clsid );
	else if ( sExtension.CompareNoCase( "jpg" ) )
		GetEncoderClsid( L"image/jpeg", &clsid );
	else if ( sExtension.CompareNoCase( "bmp" ) )
		GetEncoderClsid( L"image/bmp", &clsid );
	else if ( sExtension.CompareNoCase( "png" ) )
		GetEncoderClsid( L"image/png", &clsid );
	else if ( sExtension.CompareNoCase( "tif" ) )
		GetEncoderClsid( L"image/tiff", &clsid );
	else
		return false;

	vdbWideChar swFilename( sFilename );
	if ( pImage->Save( swFilename.GetUnicode(), &clsid, NULL ) == Ok )
		return true;
	else
		return false;
}