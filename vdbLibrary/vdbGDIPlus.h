//=============================================================================
//
// CLASS:           vdbGDIPlus
//
// OVERVIEW:		The vdbGDIPlus class
//
// FILE NAME:       vdbGDIPlus.h
//
// INITIAL DATE:    02/13/2002
//
// COPYRIGHT:		Copyright © 2002 by Joe Honton
//
//=============================================================================

#ifndef __vdbGDIPlus_h
#define __vdbGDIPlus_h

#ifndef __AFXDISP_H__
	#include <afxdisp.h>
#endif

//Nov-1-2009 commented out
//typedef unsigned long * ULONG_PTR;

#include <gdiplus.h>
using namespace Gdiplus;


class vdbGDIPlus
{
public:
	static int GetEncoderClsid( const WCHAR* format, CLSID* pClsid );
	static bool SaveImage( Image* pImage, const char* szFilename );
};

#endif // __vdbGDIPlus_h

