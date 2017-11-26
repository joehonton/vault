//=============================================================================
//
// CLASS:           vdbTimedef
//
// OVERVIEW:		Typedefs for date and time
//
// FILE NAME:       vdbTimedef.h
//
// INITIAL DATE:    1/8/98
//
// COPYRIGHT:		Copyright © 1998 by Joe Honton
//
//=============================================================================

#ifndef __vdbTimedef_h
#define __vdbTimedef_h

#ifndef __oledb_h__

	typedef struct tagDBDATE
	{
		short		   year;
		unsigned short month;
		unsigned short day;
	} DBDATE;

	typedef struct tagDBTIME
	{
		unsigned short hour;
		unsigned short minute;
		unsigned short second;
	} DBTIME;

	typedef struct tagDBTIMESTAMP
	{
		short		   year;
		unsigned short month;
		unsigned short day;
		unsigned short hour;
		unsigned short minute;
		unsigned short second;
		unsigned long  fraction;
	} DBTIMESTAMP;
#endif // #ifndef __oledb_h__


#endif // __vdbTimedef_h
