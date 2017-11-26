//=============================================================================
//
// OVERVIEW:		Template instantiation for all vdbMap based template classes
//
// FILE NAME:       rgaMapTemplates.cpp
//
// INITIAL DATE:    10/25/99
//
// COPYRIGHT:       Copyright © 1999 by Joe Honton
//
//=============================================================================

#include <vdbMap.cpp>
#include <vdbSet.cpp>

// vdbMap<ReportDefinition> instantiation
#include "ReportDefinition.h"
template class vdbSet< vdbMapIterator<ReportDefinition> >;
template class vdbMap<ReportDefinition>;
template class vdbMapIterator<ReportDefinition>;

// vdbMap<ReportStyleDefinition> instantiation
#include "ReportStyleDefinition.h"
template class vdbSet< vdbMapIterator<ReportStyleDefinition> >;
template class vdbMap<ReportStyleDefinition>;
template class vdbMapIterator<ReportStyleDefinition>;
