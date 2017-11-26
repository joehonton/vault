//=============================================================================
//
// FILE NAME:       vdbColorConstants.cpp
//
// PURPOSE:         global color constants
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    12/15/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef RGB
	typedef unsigned long  COLORREF;
	typedef unsigned long  DWORD;
	typedef unsigned short WORD;
	typedef unsigned char  BYTE;
	#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#endif


// use this color to indicate failure of some sort
COLORREF g_unspecifiedColor = RGB(254,254,254);

// light red to dark red
COLORREF g_veryPalestRed = RGB(255,248,248);
COLORREF g_veryPaleRed = RGB(255,240,240);
COLORREF g_palerRed = RGB(255,232,232);
COLORREF g_paleRed = RGB(255,224,224);
COLORREF g_veryLightRed = RGB(255,208,208);
COLORREF g_lightRed = RGB(255,192,192);
COLORREF g_mediumLightRed = RGB(255,160,160);
COLORREF g_mediumRed = RGB(255,128,128);
COLORREF g_mediumDarkRed = RGB(255,96,96);
COLORREF g_red = RGB(255,0,0);
COLORREF g_darkRed = RGB(224,0,0);
COLORREF g_darkerRed = RGB(192,0,0);
COLORREF g_veryDarkRed = RGB(160,0,0);
COLORREF g_veryDarkestRed = RGB(128,0,0);

// light orange to dark orange
COLORREF g_veryPalestOrange = RGB(255,253,182);
COLORREF g_veryPaleOrange = RGB(255,240,176);
COLORREF g_palerOrange = RGB(255,237,168);
COLORREF g_paleOrange = RGB(255,224,160);
COLORREF g_veryLightOrange = RGB(255,208,144);
COLORREF g_lightOrange = RGB(255,192,128);
COLORREF g_mediumLightOrange = RGB(255,176,96);
COLORREF g_mediumOrange = RGB(255,160,64);
COLORREF g_mediumDarkOrange = RGB(255,144,32);
COLORREF g_orange = RGB(255,128,0);
COLORREF g_darkOrange = RGB(240,128,0 );
COLORREF g_darkerOrange = RGB(224,128,0);
COLORREF g_veryDarkOrange = RGB(192,128,0);
COLORREF g_veryDarkestOrange = RGB(160,96,0);

// light yellow to dark yellow
COLORREF g_veryPalestYellow = RGB(255,255,248);
COLORREF g_veryPaleYellow = RGB(255,255,240);
COLORREF g_palerYellow = RGB(255,255,232);
COLORREF g_paleYellow = RGB(255,255,224);
COLORREF g_veryLightYellow = RGB(255,255,208);
COLORREF g_lightYellow = RGB(255,255,192);
COLORREF g_mediumLightYellow = RGB(255,255,160);
COLORREF g_mediumYellow = RGB(255,255,128);
COLORREF g_mediumDarkYellow = RGB(255,255,96);
COLORREF g_yellow = RGB(255,255,0);
COLORREF g_darkYellow = RGB(224,224,0);
COLORREF g_darkerYellow = RGB(192,192,0);
COLORREF g_veryDarkYellow = RGB(160,160,0);
COLORREF g_veryDarkestYellow = RGB(128,128,0);

// light green to dark green
COLORREF g_veryPalestGreen = RGB(248,255,248);
COLORREF g_veryPaleGreen = RGB(240,255,240);
COLORREF g_palerGreen = RGB(232,255,232);
COLORREF g_paleGreen = RGB(224,255,224);
COLORREF g_veryLightGreen = RGB(208,255,208);
COLORREF g_lightGreen = RGB(192,255,192);
COLORREF g_mediumLightGreen = RGB(160,255,160);
COLORREF g_mediumGreen = RGB(128,255,128);
COLORREF g_mediumDarkGreen = RGB(96,255,96);
COLORREF g_green = RGB(0,255,0);
COLORREF g_darkGreen = RGB(0,224,0);
COLORREF g_darkerGreen = RGB(0,192,0);
COLORREF g_veryDarkGreen = RGB(64,128,64);
COLORREF g_veryDarkestGreen = RGB(32,64,32);
COLORREF g_crescentBloomGreen = RGB(102,153,153);

// light aqua to dark aqua
COLORREF g_veryPalestAqua = RGB(182,255,253);
COLORREF g_veryPaleAqua = RGB(176,255,240);
COLORREF g_palerAqua = RGB(168,255,237);
COLORREF g_paleAqua = RGB(160,255,224);
COLORREF g_veryLightAqua = RGB(144,255,208);
COLORREF g_lightAqua = RGB(128,255,192);
COLORREF g_mediumLightAqua = RGB(96,255,176);
COLORREF g_mediumAqua = RGB(64,255,160);
COLORREF g_mediumDarkAqua = RGB(32,255,144);
COLORREF g_aqua = RGB(0,255,128);
COLORREF g_darkAqua = RGB(0,240,128);
COLORREF g_darkerAqua = RGB(0,224,128);
COLORREF g_veryDarkAqua = RGB(0,192,128);
COLORREF g_veryDarkestAqua = RGB(0,160,128);

// light blue to dark blue	
COLORREF g_veryPalestBlue = RGB(248,248,255);
COLORREF g_veryPaleBlue = RGB(240,240,255);
COLORREF g_palerBlue = RGB(232,232,255);
COLORREF g_paleBlue = RGB(224,224,255);
COLORREF g_veryLightBlue = RGB(208,208,255);
COLORREF g_lightBlue = RGB(192,192,255);
COLORREF g_mediumLightBlue = RGB(160,160,255);
COLORREF g_mediumBlue = RGB(128,128,255);
COLORREF g_mediumDarkBlue = RGB(96,96,255);
COLORREF g_blue = RGB(0,0,255);
COLORREF g_darkBlue = RGB(0,0,224);
COLORREF g_darkerBlue = RGB(0,0,192);
COLORREF g_veryDarkBlue = RGB(0,0,160);
COLORREF g_veryDarkestBlue = RGB(0,0,128);

// light violet to dark violet
COLORREF g_veryPalestViolet = RGB(255,248,255);
COLORREF g_veryPaleViolet = RGB(255,240,255);
COLORREF g_palerViolet = RGB(255,232,255);
COLORREF g_paleViolet = RGB(255,224,255);
COLORREF g_veryLightViolet = RGB(255,208,255);
COLORREF g_lightViolet = RGB(255,192,255);
COLORREF g_mediumLightViolet = RGB(255,160,255);
COLORREF g_mediumViolet = RGB(255,128,255);
COLORREF g_mediumDarkViolet = RGB(255,96,255);
COLORREF g_violet = RGB(255,0,255);
COLORREF g_darkViolet = RGB(224,0,224);
COLORREF g_darkerViolet = RGB(192,0,192);
COLORREF g_veryDarkViolet = RGB(160,0,160);
COLORREF g_veryDarkestViolet = RGB(128,0,128);

// light purple to dark purple
COLORREF g_veryPalestPurple = RGB(253,182,255);
COLORREF g_veryPalePurple = RGB(240,176,255);
COLORREF g_palerPurple = RGB(237,168,255);
COLORREF g_palePurple = RGB(224,160,255);
COLORREF g_veryLightPurple = RGB(208,144,255);
COLORREF g_lightPurple = RGB(192,128,255);
COLORREF g_mediumLightPurple = RGB(176,96,255);
COLORREF g_mediumPurple = RGB(160,64,255);
COLORREF g_mediumDarkPurple = RGB(144,32,255);
COLORREF g_purple = RGB(128,0,255);
COLORREF g_darkPurple = RGB(128,0,240);
COLORREF g_darkerPurple = RGB(128,0,224);
COLORREF g_veryDarkPurple = RGB(128,0,192);
COLORREF g_veryDarkestPurple = RGB(128,0,160);

// white to gray to black
COLORREF g_white = RGB(255,255,255);
COLORREF g_cream = RGB(255,255,221);
COLORREF g_veryPalestGray = RGB(248,248,248);
COLORREF g_veryPaleGray = RGB(240,240,240);
COLORREF g_palerGray = RGB(232,232,232);
COLORREF g_paleGray = RGB(224,224,224);
COLORREF g_veryLightGray = RGB(208,208,208);
COLORREF g_lightGray = RGB(192,192,192);
COLORREF g_mediumLightGray = RGB(160,160,160);
COLORREF g_mediumGray = RGB(128,128,128);
COLORREF g_mediumDarkGray = RGB(112,112,112);
COLORREF g_gray = RGB(96,96,96);
COLORREF g_partialBlack = RGB(64,64,64);
COLORREF g_black = RGB(0,0,0);
