//=============================================================================
//
// CLASS:           ReportColumnAssignment
//
// OVERVIEW:		The ReportColumnAssignment class connects a column to a
//                  report style definition.
//
// FILE NAME:       ReportColumnAssignment.h
//
// INITIAL DATE:    01/15/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#ifndef __ReportColumnAssignment_h
#define __ReportColumnAssignment_h

#ifndef __vdbString_h
	#include <vdbString.h>
#endif

class ostream;
class istream;


//=============================================================================
class ReportColumnAssignment
{
public:
	ReportColumnAssignment();
	~ReportColumnAssignment();
	ReportColumnAssignment( const ReportColumnAssignment& rhs );			// copy constructor
	ReportColumnAssignment& operator=( const ReportColumnAssignment& rhs );	// assignment operator

public:
	vdbString _sColumnName;
	vdbString _sFontSpecDescription;

	friend ostream& operator<< ( ostream& os, const ReportColumnAssignment& obj );
	friend istream& operator>> ( istream& is, ReportColumnAssignment& obj );
};

#endif  // ReportColumnAssignment_h
