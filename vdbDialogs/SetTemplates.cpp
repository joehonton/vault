//=============================================================================
//
// FILE NAME:       SetTemplates.cpp
//
// INITIAL DATE:    08/30/01
//
// COPYRIGHT:       Copyright © 2001 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"

#include <vdbSet.cpp>
#include "vdbString.h"
template class vdbSet<vdbString>;
template class vdbSet<bool>;

#include <vdbSetSort.cpp>
#include "vdbFileDialogSort.h"
template class vdbSet<vdbFileDialogSort>;
template class vdbSetSort<vdbFileDialogSort>;
