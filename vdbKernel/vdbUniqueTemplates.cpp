//=============================================================================
//
// CLASS:           vdbUniqueTemplates.cpp
//
// OVERVIEW:		Template instantiation for all vdbUnique
//
// FILE NAME:       vdbUniqueTemplates.cpp
//
// INITIAL DATE:    01/17/97 
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================

//{{ClassInstantiation

// vdbSet<vdbUnique> instantiation
#include <vdbSet.cpp>
#ifndef __vdbUnique_h
	#include "vdbUnique.h"
#endif
template class vdbSet<vdbUnique>;

// vdbMap<vdbUnique> instantiation
#include <vdbMap.cpp>
#ifndef __vdbUnique_h
	#include "vdbUnique.h"
#endif
template class vdbMap<vdbUnique>;

// vdbQuery<vdbUnique> instantiation
#include <vdbQuery.cpp>
#ifndef __vdbUnique_h
	#include "vdbUnique.h"
#endif
template class vdbQuery<vdbUnique>;

// vdbProperty<vdbUnique> instantiation
#include <vdbProperty.cpp>
#ifndef __vdbUnique_h
	#include "vdbUnique.h"
#endif
template class vdbProperty<vdbUnique>;

// vdbPtr<vdbUnique> instantiation
#include <vdbPtr.cpp>
#ifndef __vdbUnique_h
	#include "vdbUnique.h"
#endif
template class vdbPtr<vdbUnique>;

// vdbLinkSet<vdbUnique> instantiation
#include <vdbLinkSet.cpp>
#ifndef __vdbUnique_h
	#include "vdbUnique.h"
#endif
template class vdbLinkSet<vdbUnique>;

//ClassInstantiation}}
