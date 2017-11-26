//=============================================================================
//
// OVERVIEW:		Template instantiation for all vdbSet based template classes
//
// FILE NAME:       rgaSetTemplates.cpp
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

// "warning C4660: template-class specialization is already instantiated"
#pragma warning( disable : 4660 )

#include <vdbSet.cpp>

#include "ReportColumn.h"
template class vdbSet<ReportColumn>;

#include "ReportSelectionCriteria.h"
template class vdbSet<ReportSelectionCriteria>;

#include "ReportSortCriteria.h"
template class vdbSet<ReportSortCriteria>;

#include "ReportFontSpecs.h"
template class vdbSet<ReportFontSpecs>;
