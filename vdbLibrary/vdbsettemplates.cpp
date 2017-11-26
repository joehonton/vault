//=============================================================================
//
// OVERVIEW:		Template instantiation for all vdbSet based template classes
//
// FILE NAME:       seqSetTemplates.cpp
//
// INITIAL DATE:    07/10/97
//
// COPYRIGHT:		Copyright © 1997 by Joe Honton
//
//=============================================================================

// "warning C4660: template-class specialization is already instantiated"
#pragma warning( disable : 4660 )


#include <vdbSet.cpp>
#include <vdbSetSort.cpp>

#include "vdbFilename.h"
template class vdbSet<vdbFilename>;

#include "vdbString.h"
template class vdbSet<vdbString>;
template class vdbSetSort<vdbString>;

