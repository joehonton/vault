//=============================================================================
//
// CLASS:           vdbTimeStamp
//
// ESSENCE:         timestamp datatype encapsulation
//
// FILE NAME:       vdbTimeStamp.h
//
// INITIAL DATE:    1/5/96 
//
// DEVELOPER:       Joe Honton
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================
 
#ifndef __vdbTimeStamp_h
#define __vdbTimeStamp_h
 
#ifndef __vdbWindows_h
	#include "vdbWindows.h"
#endif

#ifndef _INC_TIME
	#include <time.h>
#endif

#ifndef __vdbTimedef_h
	#include "vdbTimedef.h"
#endif

#ifndef UNICODE
	#include <ostream>
	#include <istream>
#endif


class vdbTimeStamp
{
public:
	// constructors
	vdbTimeStamp();
	vdbTimeStamp( const tm& tmTime );
	vdbTimeStamp( const DBTIMESTAMP& dbTimeStamp );
	vdbTimeStamp( const DBTIME& dbTime );
	vdbTimeStamp( const DBDATE& dbDate );
	vdbTimeStamp( const SYSTEMTIME& st );
	vdbTimeStamp( const FILETIME& ft );
	#ifndef _WIN32_WCE
		vdbTimeStamp( const time_t& ttTime );
	#endif // _WIN32_WCE

	// implicit conversion operator
	operator const DBTIMESTAMP&();

	// Comparison
	bool operator== ( const vdbTimeStamp& rhs ) const;
	bool operator!= ( const vdbTimeStamp& rhs ) const;
	#ifndef _WIN32_WCE
		bool operator< ( const vdbTimeStamp& rhs ) const;
		bool operator> ( const vdbTimeStamp& rhs ) const;
	#endif // _WIN32_WCE

	// modification functions
	#ifndef _WIN32_WCE
		void Now();
	#endif // _WIN32_WCE
	bool SetTimeStamp( const DBTIMESTAMP& dbTimeStamp );
	bool SetTime( const DBTIME& dbTime );
	bool SetDate( const DBDATE& dbDate );
	bool SetFromDatabaseString( const TCHAR* szYMD_HMST );
	bool SetHourAndMinute( const TCHAR* szHourAndMinute );
	bool SetYearMonthDay( const TCHAR* szYearMonthDay );
	bool SetYearMonthDay( const TCHAR* szYearMonthDay, const TCHAR* szDateFormat );
	bool SetYear( const TCHAR* szYear );
	bool SetMonth( const TCHAR* szMonth );
	bool SetDay( const TCHAR* szDay );
	bool SetYear( int year );
	bool SetMonth( int month );
	bool SetDay( int day );
	bool SetHour( int hour, const TCHAR* am_pm );
	bool SetMinute( int minute );
	bool SetSecond( int second );
	bool RoundTimeTo15();
	void AutoCorrectEndOfMonth();

	// retrieval functions
	tm GetTm() const;
	DBTIMESTAMP GetTimeStamp() const;
	DBTIME GetTime() const;
	DBDATE GetDate() const;
	int GetYear() const;
	int GetMonth() const;
	int GetDay() const;
	int GetHour() const;
	int GetMinutes() const;
	int GetSeconds() const;
	SYSTEMTIME GetSystemTime() const;
	#ifndef _WIN32_WCE
		time_t GetTimeT() const;
		void Format( TCHAR* szString, size_t size, const TCHAR* szFormatString ) const;
		double Difference( vdbTimeStamp& endingTimeStamp ) const;
		void AddDays( int numDays );
		void AddMinutes( int numMinutes );
	#endif // _WIN32_WCE
	void AddYears( int numYears );

	bool IsValid() const;
	bool IsDateValid() const;
	bool IsTimeValid() const;
	void SetNullValue();
	void SetDateNullValue();
	void SetTimeNullValue();
	bool IsNullValue() const;
	bool IsDateNullValue() const;
	bool IsTimeNullValue() const;

protected:
	DBTIMESTAMP _ts;

#ifndef UNICODE
	friend std::ostream& operator<< (std::ostream& os, const vdbTimeStamp& obj );
	friend std::istream& operator>> (std::istream& is, vdbTimeStamp& obj );
#endif
};


#endif  // __vdbTimeStamp_h
