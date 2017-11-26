//=============================================================================
//
// CLASS:           vdbSetInstantiation
//
// ESSENCE:         Set template instantiation for all classes not
//                  needing template specialization.
//
// FILE NAME:       vdbSetInstantiation.cpp
//
// INITIAL DATE:    2/13/95 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1995 by Joe Honton
//
//=============================================================================

// pragma to suppress warning messages:
// "warning C4660: template-class specialization 'vdbSet<class vdbColumn>' is already instantiated"
// "warning C4660: template-class specialization 'vdbSet<class vdbIndex>' is already instantiated"
// "warning C4660: template-class specialization 'vdbSet<class vdbTable>' is already instantiated"
#pragma warning( disable : 4660 )

#include "vdbSet.cpp"			// include the template definitions first


//--------------------------------------------
// template instantiation see "The Design and Evolution of C++", p367.
// this must be before any implicit definition

#include "vdbColumn.h"
template class vdbSet<vdbColumn>;

#include "vdbIndex.h"
template class vdbSet<vdbIndex>;

#include "vdbTable.h"
template class vdbSet<vdbTable>;

#include "vdbTypedef.h"
template class vdbSet<OID>;

#include "vdbSysDevices.h"
template class vdbSet<vdbSysDevices>;

#include "vdbDatabase.h"
template class vdbSet<vdbDatabase>;
