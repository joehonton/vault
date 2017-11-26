//=============================================================================
//
// ESSENCE:         A single dummy function to force Visual C++ to create a DLL
//
// FILE NAME:       DummyExport.cpp
//
// INITIAL DATE:    2/28/96
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================


//=============================================================================
//  Dummy function
//=============================================================================

extern "C" int __declspec(dllexport) VaultResourcesVersion()
{
	// version 2 includes vdbFileDialog resources
	// version 3 includes vdbPrintDialog resources
	// version 4 includes vdbReportBuilder resources
	return 4;
}
