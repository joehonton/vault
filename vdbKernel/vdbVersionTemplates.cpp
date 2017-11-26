//=============================================================================
//
// CLASS:           vdbVersionTemplates.cpp
//
// OVERVIEW:		Template instantiation for all vdbVersion
//
// FILE NAME:       vdbVersionTemplates.cpp
//
// INITIAL DATE:    01/17/97 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

// pragma to suppress warning messages:
// "warning C4660: template-class specialization is already instantiated"
#pragma warning( disable : 4660 )

//{{ClassInstantiation

// vdbSet<vdbVersion> instantiation
#include <vdbSet.cpp>
#ifndef __vdbVersion_h
	#include "vdbVersion.h"
#endif
template class vdbSet<vdbVersion>;

// vdbMap<vdbVersion> instantiation
#include <vdbMap.cpp>

#ifndef __vdbVersion_h
	#include "vdbVersion.h"
#endif
template class vdbSet< vdbMapIterator<vdbVersion> >;
template class vdbMap<vdbVersion>;
template class vdbMapIterator<vdbVersion>;

// vdbQuery<vdbVersion> instantiation
#include <vdbQuery.cpp>
#ifndef __vdbVersion_h
	#include "vdbVersion.h"
#endif
template class vdbQuery<vdbVersion>;

// vdbProperty<vdbVersion> instantiation
#include <vdbProperty.cpp>
#ifndef __vdbVersion_h
	#include "vdbVersion.h"
#endif
template class vdbProperty<vdbVersion>;

// vdbPtr<vdbVersion> instantiation
#include <vdbPtr.cpp>
#ifndef __vdbVersion_h
	#include "vdbVersion.h"
#endif
template class vdbPtr<vdbVersion>;

// vdbLinkSet<vdbVersion> instantiation
#include <vdbLinkSet.cpp>
#ifndef __vdbVersion_h
	#include "vdbVersion.h"
#endif
template class vdbLinkSet<vdbVersion>;

//ClassInstantiation}}

