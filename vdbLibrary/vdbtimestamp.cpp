//=============================================================================
//
// CLASS:           vdbTimeStamp
//
// ESSENCE:         timestamp datatype encapsulation
//
// FILE NAME:       vdbTimeStamp.cpp
//
// INITIAL DATE:    1/5/96 
//
// DEVELOPER:       Joe Honton
//
// SPECIAL NOTE:	mktime fails on dates greater than 12/31/2000
//
// COPYRIGHT:		Copyright © 1996 by Joe Honton
//
//=============================================================================
 
#include "vdbTimeStamp.h"
#include <stdlib.h>
#include "vdbUtility.h"
#ifndef UNICODE
	#include <fstream>
#endif

//warning C4996: 'localtime': This function or variable may be unsafe. Consider using localtime_s instead.
//warning C4996: 'strncpy': This function or variable may be unsafe. Consider using strncpy_s instead.
#pragma warning( disable : 4996 )


//=============================================================================
// Constructors
//=============================================================================

//--------------------------------
vdbTimeStamp::vdbTimeStamp()
{
	SetNullValue();
}


//--------------------------------
vdbTimeStamp::vdbTimeStamp( const tm& tmTime )
{
	_ts.year = tmTime.tm_year;
	_ts.year < 70 ? _ts.year += 2000 : _ts.year += 1900;
	_ts.month = tmTime.tm_mon + 1;
	_ts.day = tmTime.tm_mday;
	_ts.hour = tmTime.tm_hour;
	_ts.minute = tmTime.tm_min;
	_ts.second = tmTime.tm_sec;
	_ts.fraction = 0;
}


//--------------------------------
vdbTimeStamp::vdbTimeStamp( const DBTIMESTAMP& dbTimeStamp )
{
	_ts.year = dbTimeStamp.year;
	_ts.month = dbTimeStamp.month;
	_ts.day = dbTimeStamp.day;
	_ts.hour = dbTimeStamp.hour;
	_ts.minute = dbTimeStamp.minute;
	_ts.second = dbTimeStamp.second;
	_ts.fraction = dbTimeStamp.fraction;
}


//--------------------------------
vdbTimeStamp::vdbTimeStamp( const DBTIME& dbTime )
{
	SetDateNullValue();
	_ts.hour = dbTime.hour;
	_ts.minute = dbTime.minute;
	_ts.second = dbTime.second;
	_ts.fraction = 0;
}


//--------------------------------
vdbTimeStamp::vdbTimeStamp( const DBDATE& dbDate )
{
	_ts.year = dbDate.year;
	_ts.month = dbDate.month;
	_ts.day = dbDate.day;
	SetTimeNullValue();
}


//-------------------------------------
//^ The vdbTimeStamp(SYSTEMTIME) constructor
//
vdbTimeStamp::vdbTimeStamp( const SYSTEMTIME& st )
{
	_ts.year = st.wYear;
	_ts.month = st.wMonth;
	_ts.day = st.wDay;
	_ts.hour = st.wHour;
	_ts.minute = st.wMinute;
	_ts.second = st.wSecond;
	_ts.fraction = st.wMilliseconds;
}


//-------------------------------------------
//^ The DBTIMESTAMP conversion operator implicitly
//  or explicitly converts the object to a constant
//  reference to a DBTIMESTAMP.
//
vdbTimeStamp::operator const DBTIMESTAMP& ()
{
	return _ts;
}


//-------------------------------------
//^ The vdbTimeStamp(FILETIME) constructor
//> ft is a 64-bit value representing the number of 100-nanosecond intervals
//  since January 1, 1601. 
//
vdbTimeStamp::vdbTimeStamp( const FILETIME& ft )
{
	SYSTEMTIME st;
	FileTimeToSystemTime( &ft, &st );

	#ifndef _WIN32_WCE
		SYSTEMTIME adjusted;
		SystemTimeToTzSpecificLocalTime( NULL, &st, &adjusted );

		_ts.year = adjusted.wYear;
		_ts.month = adjusted.wMonth;
		_ts.day = adjusted.wDay;
		_ts.hour = adjusted.wHour;
		_ts.minute = adjusted.wMinute;
		_ts.second = adjusted.wSecond;
		_ts.fraction = adjusted.wMilliseconds;
	#endif // _WIN32_WCE
}


#ifndef _WIN32_WCE
//--------------------------------
vdbTimeStamp::vdbTimeStamp( const time_t& ttTime )
{
	tm* tmTime = localtime( &ttTime );
	_ts.year = tmTime->tm_year;
	_ts.year < 70 ? _ts.year += 2000 : _ts.year += 1900;
	_ts.month = tmTime->tm_mon + 1;
	_ts.day = tmTime->tm_mday;
	_ts.hour = tmTime->tm_hour;
	_ts.minute = tmTime->tm_min;
	_ts.second = tmTime->tm_sec;
	_ts.fraction = 0;
}
#endif // _WIN32_WCE


//=============================================================================
// Comparison operators
//=============================================================================

//------------------------------------------
//^^ const vdbTimeStamp operator==
bool vdbTimeStamp::operator== ( const vdbTimeStamp& rhs ) const
{
	// nothing to do?
	if ( this == &rhs )
		return true;

	if ( _ts.year != rhs._ts.year )
		return false;
	if ( _ts.month != rhs._ts.month )
		return false;
	if ( _ts.day != rhs._ts.day )
		return false;
	if ( _ts.hour != rhs._ts.hour )
		return false;
	if ( _ts.minute != rhs._ts.minute )
		return false;
	if ( _ts.second != rhs._ts.second )
		return false;
	if ( _ts.fraction != rhs._ts.fraction )
		return false;

	return true;
}


//------------------------------------------
//^^ const vdbTimeStamp operator==
bool vdbTimeStamp::operator!= ( const vdbTimeStamp& rhs ) const
{
	// nothing to do?
	if ( this == &rhs )
		return false;

	if ( _ts.year != rhs._ts.year )
		return true;
	if ( _ts.month != rhs._ts.month )
		return true;
	if ( _ts.day != rhs._ts.day )
		return true;
	if ( _ts.hour != rhs._ts.hour )
		return true;
	if ( _ts.minute != rhs._ts.minute )
		return true;
	if ( _ts.second != rhs._ts.second )
		return true;
	if ( _ts.fraction != rhs._ts.fraction )
		return true;

	return false;
}


#ifndef _WIN32_WCE
//------------------------------------------
//^^ const vdbTimeStamp operator>
bool vdbTimeStamp::operator> ( const vdbTimeStamp& rhs ) const
{
	// nothing to do?
	if ( this == &rhs )
		return false;

	if ( difftime( rhs.GetTimeT(), GetTimeT() ) < 0 )
		return true;
	else
		return false;

}


//------------------------------------------
//^^ const vdbTimeStamp operator<
bool vdbTimeStamp::operator< ( const vdbTimeStamp& rhs ) const
{
	// nothing to do?
	if ( this == &rhs )
		return false;

	if ( difftime( rhs.GetTimeT(), GetTimeT() ) > 0 )
		return true;
	else
		return false;

}
#endif // _WIN32_WCE


//=============================================================================
// Modification functions
//=============================================================================

#ifndef _WIN32_WCE
//-------------------------------------------
//^ The Now function set's the object's values
//  to the current date and time.
//
void vdbTimeStamp::Now()
{
	time_t ttTime;
	time( &ttTime );
	tm* tmTime = localtime( &ttTime );
	_ts.year = tmTime->tm_year;
	_ts.year < 70 ? _ts.year += 2000 : _ts.year += 1900;
	_ts.month = tmTime->tm_mon + 1;
	_ts.day = tmTime->tm_mday;
	_ts.hour = tmTime->tm_hour;
	_ts.minute = tmTime->tm_min;
	_ts.second = tmTime->tm_sec;
	_ts.fraction = 0;
}
#endif // _WIN32_WCE


//-------------------------------------------
//^ The SetTimeStamp function replaces the object's
//  timestamp with the given value.
//
bool vdbTimeStamp::SetTimeStamp( const DBTIMESTAMP& dbTimeStamp )
{
	_ts.year = dbTimeStamp.year;
	_ts.month = dbTimeStamp.month;
	_ts.day = dbTimeStamp.day;
	_ts.hour = dbTimeStamp.hour;
	_ts.minute = dbTimeStamp.minute;
	_ts.second = dbTimeStamp.second;
	_ts.fraction = dbTimeStamp.fraction;

	if ( IsValid() )
		return true;
	else
	{
		SetNullValue();
		return false;
	}
}


//-------------------------------------------
//^ The SetTime function merges the time with the
//  object's timestamp.
//
bool vdbTimeStamp::SetTime( const DBTIME& dbTime )
{
	_ts.hour = dbTime.hour;
	_ts.minute = dbTime.minute;
	_ts.second = dbTime.second;

	if ( IsTimeValid() )
		return true;
	else
	{
		SetTimeNullValue();
		return false;
	}
}


//-------------------------------------------
//^ The SetDate function merges the date with the
//  object's timestamp.
//
bool vdbTimeStamp::SetDate( const DBDATE& dbDate )
{
	_ts.year = dbDate.year;
	_ts.month = dbDate.month;
	_ts.day = dbDate.day;

	if ( IsDateValid() )
		return true;
	else
	{
		SetDateNullValue();
		return false;
	}
}


//-------------------------------------
//^ The SetFromDatabaseString function parses the given string looking for
//  the format YYYY-MM-DDTHH:MM:SS.TTTZ or YYYY-MM-DD
//^ The fractional seconds are optional
// 
bool vdbTimeStamp::SetFromDatabaseString( const TCHAR* szYMD_HMST )
{
	TCHAR sz[5];

	vdbUtility::SafeCopy( sz, &szYMD_HMST[0], 5 );
	_ts.year = (int) _ttoi( sz );
	vdbUtility::SafeCopy( sz, &szYMD_HMST[5], 3 );
	_ts.month = (int) _ttoi( sz );
	vdbUtility::SafeCopy( sz, &szYMD_HMST[8], 3 );
	_ts.day = (int) _ttoi( sz );

	if ( _tcslen( szYMD_HMST ) > 10 )
	{
		vdbUtility::SafeCopy( sz, &szYMD_HMST[11], 3 );
		_ts.hour = (int) _ttoi( sz );
		vdbUtility::SafeCopy( sz, &szYMD_HMST[14], 3 );
		_ts.minute = (int) _ttoi( sz );
		vdbUtility::SafeCopy( sz, &szYMD_HMST[17], 3 );
		_ts.second = (int) _ttoi( sz );
	}
	else
		SetTimeNullValue();

	if ( _tcslen( szYMD_HMST ) > 20 )
	{
		vdbUtility::SafeCopy( sz, &szYMD_HMST[20], 4 );
		_ts.fraction = (int) _ttoi( sz );
	}
	else
		_ts.fraction = 0;

	if ( IsValid() )
		return true;
	else
	{
		SetNullValue();
		return false;
	}
}


//-------------------------------------
//^ The SetHourAndMinute function merges the hour and minute
//  from the string parameter into the object's timestamp structure.
//> szHourAndMinute is a null-terminated string containing the hour, minute and am/pm indicator
//  in the format HH:MM AM where the separator may be a colon, space or period.
// 
bool vdbTimeStamp::SetHourAndMinute( const TCHAR* szHourAndMinute )
{
	int startOfHour = _tcscspn( szHourAndMinute, _T("0123456789") );
	int separator1 = _tcscspn( szHourAndMinute, _T(": .") );
	int startOfMinute = _tcscspn( &szHourAndMinute[separator1], _T("0123456789") ) + separator1;
	int separator2 = _tcscspn( &szHourAndMinute[startOfMinute], _T(" AaPp") ) + startOfMinute;
	int startOfAMPM = _tcscspn( &szHourAndMinute[separator2], _T("AaPp") ) + separator2;

	TCHAR szHour[3];
	memset( szHour, 0, sizeof(szHour) );
	_tcsncpy( szHour, &szHourAndMinute[startOfHour], ( separator1 - startOfHour > 1 ) ? 2 : 1 );
	szHour[2] = 0;
	_ts.hour = (int) _ttoi( szHour );
	
	TCHAR szMinute[3];
	memset( szMinute, 0, sizeof(szMinute) );
	_tcsncpy( szMinute, &szHourAndMinute[startOfMinute], ( separator2 - startOfMinute > 1 ) ? 2 : 1 );
	szMinute[2] = 0;
	_ts.minute = (int) _ttoi( szMinute );

	if ( _ts.hour != 0 && _ts.hour != 12 )
		if ( szHourAndMinute[startOfAMPM] == 'p' || szHourAndMinute[startOfAMPM] == 'P' )
			_ts.hour += 12;

	if ( IsTimeValid() )
		return true;
	else
	{
		SetTimeNullValue();
		return false;
	}
}


//-------------------------------------------
//^ The SetYearMonthDay function parses the string and merges the date
//  into the object's timestamp structure.
//> szMonthDayYear is a null-terminated string containing the
//  month, day and year in the format:
//  Jan 20, 01 or Jan 20, 2001 or
//  01/20/01 or 01/20/2001 or
//  01-20-01 or 01-20-2001 or
//  January 20, 01 or January 20, 2001
//> If szYearMonthDay is an invalid date this is set to the default date
// 
bool vdbTimeStamp::SetYearMonthDay( const TCHAR* szYearMonthDay )
{
	// first see if the first three characters is a valid month
	int literalMonth;
	if ( !_tcsnicmp( szYearMonthDay, _T("Jan"), 3 ) )
		literalMonth = 1;
	else if ( !_tcsnicmp( szYearMonthDay, _T("Feb"), 3 ) )
		literalMonth = 2;
	else if ( !_tcsnicmp( szYearMonthDay, _T("Mar"), 3 ) )
		literalMonth = 3;
	else if ( !_tcsnicmp( szYearMonthDay, _T("Apr"), 3 ) )
		literalMonth = 4;
	else if ( !_tcsnicmp( szYearMonthDay, _T("May"), 3 ) )
		literalMonth = 5;
	else if ( !_tcsnicmp( szYearMonthDay, _T("Jun"), 3 ) )
		literalMonth = 6;
	else if ( !_tcsnicmp( szYearMonthDay, _T("Jul"), 3 ) )
		literalMonth = 7;
	else if ( !_tcsnicmp( szYearMonthDay, _T("Aug"), 3 ) )
		literalMonth = 8;
	else if ( !_tcsnicmp( szYearMonthDay, _T("Sep"), 3 ) )
		literalMonth = 9;
	else if ( !_tcsnicmp( szYearMonthDay, _T("Oct"), 3 ) )
		literalMonth = 10;
	else if ( !_tcsnicmp( szYearMonthDay, _T("Nov"), 3 ) )
		literalMonth = 11;
	else if ( !_tcsnicmp( szYearMonthDay, _T("Dec"), 3 ) )
		literalMonth = 12;
	else
		literalMonth = 0;

	int separator1;	// location of the delimiter between the month and day
	
	// if a literal month was found continue by searching for the day
	if ( literalMonth != 0 )
	{
		separator1 = _tcscspn( szYearMonthDay, _T(" 0123456789") );
		_ts.month = literalMonth;
	}
	// otherwise restart from the beginning searching for a numeric month
	else
	{
		int startOfMonth = _tcscspn( szYearMonthDay, _T("0123456789") );
		separator1 = _tcscspn( szYearMonthDay, _T("/- ") );

		TCHAR szMonth[3];
		memset( szMonth, 0, sizeof(szMonth) );
		_tcsncpy( szMonth, &szYearMonthDay[startOfMonth], ( separator1 - startOfMonth > 1 ) ? 2 : 1 );
		szMonth[2] = 0;
		_ts.month = (int) _ttoi( szMonth );
	}

	int startOfDay = _tcscspn( &szYearMonthDay[separator1], _T("0123456789") ) + separator1;
	int separator2 = _tcscspn( &szYearMonthDay[startOfDay], _T("/- ,") ) + startOfDay;
	int startOfYear = _tcscspn( &szYearMonthDay[separator2], _T("0123456789") ) + separator2;

	TCHAR szDay[3];
	memset( szDay, 0, sizeof(szDay) );
	_tcsncpy( szDay, &szYearMonthDay[startOfDay], ( separator2 - startOfDay > 1 ) ? 2 : 1 );
	szDay[2] = 0;
	_ts.day = (int) _ttoi( szDay );

	TCHAR szYear[5];
	memset( szYear, 0, sizeof(szYear) );
	_tcsncpy( szYear, &szYearMonthDay[startOfYear], 4 );
	szYear[5] = 0;
	_ts.year = (int) _ttoi( szYear );
	if ( _ts.year >= 0 && _ts.year <= 99 )
		_ts.year < 70 ? _ts.year += 2000 : _ts.year += 1900;

	AutoCorrectEndOfMonth();

	if ( IsDateValid() )
		return true;
	else
	{
		SetDateNullValue();
		return false;
	}
}


//-------------------------------------------
//^ The SetYearMonthDay function parses the string and merges the date
//  into the object's timestamp structure.
//> szMonthDayYear is a null-terminated string containing the
//  month, day and year in the format:
//  1-Jan-01 or 1-Jan-2001
//> If this routine doesn't work, it's passed along to the other SetYearMonthDay()
//  to try it.
//> szDateFormat contains a strftime format string
//
bool vdbTimeStamp::SetYearMonthDay( const TCHAR* szYearMonthDay, const TCHAR* szDateFormat )
{
	if ( !strcmp( szDateFormat, "%#d-%b-%y" ) ||
		 !strcmp( szDateFormat, "%#d-%b-%Y" ) ||
	     !strcmp( szDateFormat, "%d-%b-%y" ) ||
		 !strcmp( szDateFormat, "%d-%b-%Y" ) )
	{
		// search for the day
		int startOfDay = _tcscspn( szYearMonthDay, _T("0123456789") );
		int separator1 = _tcscspn( szYearMonthDay, _T("/- ") );

		TCHAR szDay[3];
		memset( szDay, 0, sizeof(szDay) );
		_tcsncpy( szDay, &szYearMonthDay[startOfDay], ( separator1 - startOfDay > 1 ) ? 2 : 1 );
		szDay[2] = 0;
		_ts.day = (int) _ttoi( szDay );

		// search for the month
		int startOfMonth = separator1 + 1;
		int literalMonth;
		if ( !_tcsnicmp( &szYearMonthDay[startOfMonth], _T("Jan"), 3 ) )
			literalMonth = 1;
		else if ( !_tcsnicmp( &szYearMonthDay[startOfMonth], _T("Feb"), 3 ) )
			literalMonth = 2;
		else if ( !_tcsnicmp( &szYearMonthDay[startOfMonth], _T("Mar"), 3 ) )
			literalMonth = 3;
		else if ( !_tcsnicmp( &szYearMonthDay[startOfMonth], _T("Apr"), 3 ) )
			literalMonth = 4;
		else if ( !_tcsnicmp( &szYearMonthDay[startOfMonth], _T("May"), 3 ) )
			literalMonth = 5;
		else if ( !_tcsnicmp( &szYearMonthDay[startOfMonth], _T("Jun"), 3 ) )
			literalMonth = 6;
		else if ( !_tcsnicmp( &szYearMonthDay[startOfMonth], _T("Jul"), 3 ) )
			literalMonth = 7;
		else if ( !_tcsnicmp( &szYearMonthDay[startOfMonth], _T("Aug"), 3 ) )
			literalMonth = 8;
		else if ( !_tcsnicmp( &szYearMonthDay[startOfMonth], _T("Sep"), 3 ) )
			literalMonth = 9;
		else if ( !_tcsnicmp( &szYearMonthDay[startOfMonth], _T("Oct"), 3 ) )
			literalMonth = 10;
		else if ( !_tcsnicmp( &szYearMonthDay[startOfMonth], _T("Nov"), 3 ) )
			literalMonth = 11;
		else if ( !_tcsnicmp( &szYearMonthDay[startOfMonth], _T("Dec"), 3 ) )
			literalMonth = 12;
		else
			literalMonth = 0;

		// if literal month not found, try searching for the date the other way around
		if ( literalMonth == 0 )
			return SetYearMonthDay( szYearMonthDay );
			
		// if a literal month was found continue by searching for the year
		else
		{
			_ts.month = literalMonth;

			int separator2 = _tcscspn( &szYearMonthDay[startOfMonth], _T("/- ,") ) + startOfMonth;
			int startOfYear = _tcscspn( &szYearMonthDay[separator2], _T("0123456789") ) + separator2;

			TCHAR szYear[5];
			memset( szYear, 0, sizeof(szYear) );
			_tcsncpy( szYear, &szYearMonthDay[startOfYear], 4 );
			szYear[5] = 0;
			_ts.year = (int) _ttoi( szYear );
			if ( _ts.year >= 0 && _ts.year <= 99 )
				_ts.year < 70 ? _ts.year += 2000 : _ts.year += 1900;

			AutoCorrectEndOfMonth();

			if ( IsDateValid() )
				return true;
			else
			{
				SetDateNullValue();
				return false;
			}
		}

	}

	else // szDateFormat something other than "%#d-%b-%y"
		return SetYearMonthDay( szYearMonthDay );
}


//-------------------------------------------
//^ The SetYear function accepts numbers like 1999 or 2001 as well as
//  numbers like 99 and 01
bool vdbTimeStamp::SetYear( const TCHAR* szYear )
{
	if ( !_tcsncmp( szYear, _T("19"), 2) || !_tcsncmp( szYear, _T("20"), 2) )
		_ts.year = (int) _ttoi( szYear );
	else
	{
		_ts.year = (int) _ttoi( szYear );
		_ts.year < 70 ? _ts.year += 2000 : _ts.year += 1900;
	}

	if ( _ts.year >= 1900 && _ts.year <= 2100 )
		return true;
	else
	{
		SetDateNullValue();
		return false;
	}
}


//-------------------------------------------
//^ The SetMonth function accepts numbers from 1 to 12 or short or long
//  character names like "Jan" or "January"
bool vdbTimeStamp::SetMonth( const TCHAR* szMonth )
{
	_ts.month = (int) _ttoi( szMonth );
	if ( _ts.month >= 1 && _ts.month <= 12 )
		return true;

	if ( !_tcsnicmp( szMonth, _T("Jan"), 3 ) )
		_ts.month = 1;
	else if ( !_tcsnicmp( szMonth, _T("Feb"), 3 ) )
		_ts.month = 2;
	else if ( !_tcsnicmp( szMonth, _T("Mar"), 3 ) )
		_ts.month = 3;
	else if ( !_tcsnicmp( szMonth, _T("Apr"), 3 ) )
		_ts.month = 4;
	else if ( !_tcsnicmp( szMonth, _T("May"), 3 ) )
		_ts.month = 5;
	else if ( !_tcsnicmp( szMonth, _T("Jun"), 3 ) )
		_ts.month = 6;
	else if ( !_tcsnicmp( szMonth, _T("Jul"), 3 ) )
		_ts.month = 7;
	else if ( !_tcsnicmp( szMonth, _T("Aug"), 3 ) )
		_ts.month = 8;
	else if ( !_tcsnicmp( szMonth, _T("Sep"), 3 ) )
		_ts.month = 9;
	else if ( !_tcsnicmp( szMonth, _T("Oct"), 3 ) )
		_ts.month = 10;
	else if ( !_tcsnicmp( szMonth, _T("Nov"), 3 ) )
		_ts.month = 11;
	else if ( !_tcsnicmp( szMonth, _T("Dec"), 3 ) )
		_ts.month = 12;
	else
		_ts.month = 0;

	if ( _ts.month >= 1 && _ts.month <= 12 )
		return true;
	else
	{
		SetDateNullValue();
		return false;
	}
}


//-------------------------------------------
//^ The SetDay function accepts number from 1 to 31
bool vdbTimeStamp::SetDay( const TCHAR* szDay )
{
	_ts.day = (int) _ttoi( szDay );

	AutoCorrectEndOfMonth();

	if ( _ts.day >= 1 && _ts.day <= 31 )
		return true;
	else
	{
		SetDateNullValue();
		return false;
	}
}


//-------------------------------------------
bool vdbTimeStamp::SetYear( int year )
{
	_ts.year = year;

	if ( _ts.year >= 1900 && _ts.year <= 2100 )
		return true;
	else
	{
		SetDateNullValue();
		return false;
	}
}


//-------------------------------------------
bool vdbTimeStamp::SetMonth( int month )
{
	_ts.month = month;

	if ( _ts.month >= 1 && _ts.month <= 12 )
		return true;
	else
	{
		SetDateNullValue();
		return false;
	}
}


//-------------------------------------------
bool vdbTimeStamp::SetDay( int day )
{
	_ts.day = day;

	AutoCorrectEndOfMonth();

	if ( _ts.day >= 1 && _ts.day <= 31 )
		return true;
	else
	{
		SetDateNullValue();
		return false;
	}
}


//-------------------------------------------
bool vdbTimeStamp::SetHour( int hour, const TCHAR* am_pm )
{
	if ( am_pm[0] == 'p' || am_pm[0] == 'P' )
		hour += 12;

	_ts.hour = hour;

	if ( _ts.hour >= 0 && _ts.hour <= 23 )
		return true;
	else
	{
		SetTimeNullValue();
		return false;
	}
}


//-------------------------------------------
bool vdbTimeStamp::SetMinute( int minute )
{
	_ts.minute = minute;

	if ( _ts.minute >= 0 && _ts.minute <= 59 )
		return true;
	else
	{
		SetTimeNullValue();
		return false;
	}
}


//-------------------------------------------
bool vdbTimeStamp::SetSecond( int second )
{
	_ts.second = second;

	if ( _ts.second >= 0 && _ts.second <= 59 )
		return true;
	else
	{
		SetTimeNullValue();
		return false;
	}
}


//-------------------------------------------
//^ The RoundTimeTo15 function rounds the minutes
//  to the nearest quarter hour.
//
bool vdbTimeStamp::RoundTimeTo15()
{
	int minute = _ts.minute;
	if ( minute >= 0 && minute <= 8 )
		_ts.minute = 0;
	else if	( minute > 8 && minute <= 23 )
		_ts.minute = 15;
	else if	( minute > 23 && minute <= 38 )
		_ts.minute = 30;
	else if	( minute > 38 && minute <= 53 )
		_ts.minute = 45;
	else if	( minute > 53 && minute <= 59 )
	{
		_ts.minute = 0;
		_ts.hour++;
	}

	_ts.second = 0;
	_ts.fraction = 0;

	if ( IsTimeValid() )
		return true;
	else
	{
		SetTimeNullValue();
		return false;
	}
}


//-------------------------------------------
//^ The AutoCorrectEndOfMonth function automatically adjusts invalid dates
//  for months with less than 31 days
void vdbTimeStamp::AutoCorrectEndOfMonth()
{
	// quick check
	if ( _ts.day <= 30 && _ts.month != 2 )
		return;

	// February
	if ( _ts.month == 2 )
	{
		if ( _ts.day <= 28 )
			return;

		bool bLeapYear = false;
		if ( _ts.year % 4  == 0 )
		{
			bLeapYear = true;
			if ( _ts.year % 100 == 0 )
			{
				bLeapYear = false;
				if ( _ts.year % 400 == 0 )
					bLeapYear = true;
			}
		}

		if ( bLeapYear == true && _ts.day == 29 )
			return;

		_ts.month = 3;
		if ( bLeapYear == true )
			_ts.day -= 29;
		else
			_ts.day -= 28;
	}

	// April, June, September, November
	else if ( _ts.month == 4 || _ts.month == 6 || _ts.month == 9 || _ts.month == 11 )
	{
		if ( _ts.day == 31 )
		{
			_ts.month += 1;
			_ts.day = 1;
		}
	}
}


//=============================================================================
// Retrieval functions
//=============================================================================

//-------------------------------------------
//^ The GetTm function retrieves the object's
//  date and time and places it in a "tm" struct.
//
tm vdbTimeStamp::GetTm() const
{
	tm tmTime;
	
	tmTime.tm_year = _ts.year - 1900;
	tmTime.tm_mon = _ts.month - 1;
	tmTime.tm_mday = _ts.day;
	tmTime.tm_hour = _ts.hour;
	tmTime.tm_min = _ts.minute;
	tmTime.tm_sec = _ts.second;
	tmTime.tm_wday = 0;
	tmTime.tm_yday = 0;
	tmTime.tm_isdst = -1; 
	return tmTime;
}


//-------------------------------------------
//^ The GetTimeStamp function retrieves the object's
//  date and time.
//
DBTIMESTAMP vdbTimeStamp::GetTimeStamp() const
{
	return _ts;
}


//-------------------------------------------
//^ The GetTime function retrieves the object's
//  date and time.
//
DBTIME vdbTimeStamp::GetTime() const
{
	DBTIME dbTime;
	dbTime.hour = _ts.hour;
	dbTime.minute = _ts.minute;
	dbTime.second = _ts.second;
	return dbTime;
}


//-------------------------------------------
//^ The GetDate function retrieves the object's
//  date and time.
//
DBDATE vdbTimeStamp::GetDate() const
{
	DBDATE dbDate;
	dbDate.year = _ts.year;
	dbDate.month = _ts.month;
	dbDate.day = _ts.day;
	return dbDate;
}


//-------------------------------------------
int vdbTimeStamp::GetYear() const
{
	return _ts.year;
}


//-------------------------------------------
int vdbTimeStamp::GetMonth() const
{
	return _ts.month;
}


//-------------------------------------------
int vdbTimeStamp::GetDay() const
{
	return _ts.day;
}


//-------------------------------------------
int vdbTimeStamp::GetHour() const
{
	return _ts.hour;
}


//-------------------------------------------
int vdbTimeStamp::GetMinutes() const
{
	return _ts.minute;
}


//-------------------------------------------
int vdbTimeStamp::GetSeconds() const
{
	return _ts.second;
}


//-------------------------------------------
//^ The GetSystemTime function retrieves the object's
//  date and time and converts it to a "SYSTEMTIME" value.
SYSTEMTIME vdbTimeStamp::GetSystemTime() const
{
	SYSTEMTIME st;

	st.wYear = _ts.year;
	st.wMonth = _ts.month;
	st.wDay = _ts.day;
	st.wHour = _ts.hour;
	st.wMinute = _ts.minute;
	st.wSecond = _ts.second;
	st.wMilliseconds = 0;

	return st;
}


#ifndef _WIN32_WCE
//-------------------------------------------
//^ The GetTimeT function retrieves the object's
//  date and time and converts it to a "time_t" value.
//
time_t vdbTimeStamp::GetTimeT() const
{
	// convert from DBTIMESTAMP to tm
	tm tmTime = GetTm();
	
	// convert from tm to time_t
	return mktime( &tmTime );
}


//-------------------------------------------
//^ The Format function retrieves the object's
//  date or time formatted according to the strftime format codes.
//> szString is the null-terminated string to be filled.
//> size is the maximum length of the string.
//> szFormatString is the string with formatting codes.
//  %a  Abbreviated weekday name
//  %A  Full weekday name
//  %b  Abbreviated month name
//  %B  Full month name
//  %c  Date and time representation appropriate for locale
//  %d  Day of month as decimal number (01 - 31)
//  %H  Hour in 24-hour format (00 - 23)
//  %I  Hour in 12-hour format (01 - 12)
//  %j  Day of year as decimal number (001 - 366)
//  %m  Month as decimal number (01 - 12)
//  %M  Minute as decimal number (00 - 59)
//  %p  Current locale’s A.M./P.M. indicator for 12-hour clock
//  %S  Second as decimal number (00 - 59)
//  %U  Week of year as decimal number, with Sunday as first day of week (00 - 53)
//  %w  Weekday as decimal number (0 - 6; Sunday is 0)
//  %W  Week of year as decimal number, with Monday as first day of week (00 - 53)
//  %x  Date representation for current locale
//  %X  Time representation for current locale
//  %y  Year without century, as decimal number (00 - 99)
//  %Y  Year with century, as decimal number
//  %z, %Z Time-zone name or abbreviation; no characters if time zone is unknown
//
void vdbTimeStamp::Format( TCHAR* szString, size_t size, const TCHAR* szFormatString ) const
{
	// if the date has not been set, just display a blank
	if ( IsNullValue() )
	{
		szString[0] = 0x0;
		return;
	}
		 
	// convert from DBTIMESTAMP to tm
	tm tmTime = GetTm();
	tmTime.tm_isdst = -1;

	// call mktime to ensure that the day of week is correct and that the date and time values are normalized
	time_t ttTime = mktime( &tmTime );
	if ( ttTime != -1 )
		// convert back to tm
		tmTime = *(localtime( &ttTime ));

	int count = _tcsftime( szString, size - 1, szFormatString, &tmTime );

	// ensure a NULL terminator
	szString[count] = 0x0;
}


//-------------------------------------------
//^ The Difference function calculates the difference between
//  the object's timestamp and the timestamp passed as the argument.
//> endingTime is the ending timestamp.
//> The return value is the number of seconds difference between the
//  two values.  The return value is positive if the endingTime is greater
//  than the object's time; otherwise the return value is negative.
//
double vdbTimeStamp::Difference( vdbTimeStamp& endingTimeStamp ) const
{
	return difftime( endingTimeStamp.GetTimeT(), GetTimeT() );
}


//-------------------------------------------
//^ The AddDays function adds the specified number of days to the current
//  value.  Use a negative number to subtract days.
//> numDays is the number of days to add.
//
void vdbTimeStamp::AddDays( int numDays )
{
	// convert from DBTIMESTAMP to tm
	tm tmTime = GetTm();

	// add the specified number of days to the tm structure
	tmTime.tm_mday += numDays;

	// The mktime function converts the supplied time structure (possibly incomplete)
	// pointed to by timeptr into a fully defined structure with normalized values
	// and then converts it to a time_t calendar time value.
	// The original values of the tm_wday and tm_yday components of the timeptr
	// structure are ignored, and the original values of the other components are not
	// restricted to their normal ranges.
	time_t ttTime = mktime( &tmTime );
	if ( ttTime == -1 )
		return;

	// now convert it back
	vdbTimeStamp t2( ttTime );
	*this = t2;
}


//-------------------------------------------
//^ The AddMinutes function adds the specified number of minutes to the current
//  value.  Use a negative number to subtract.
//> numMinutes is the number of minutes to add.
//
void vdbTimeStamp::AddMinutes( int numMinutes )
{
	time_t tt = GetTimeT();		// number of seconds since 1/1/1970

	tt += (numMinutes*60);

	vdbTimeStamp ts( tt );

	*this = ts;
}
#endif // _WIN32_WCE


//-------------------------------------------
//^ The AddYears function adds the specified number of years to the current
//  value.  Use a negative number to subtract.
//> numYears is the number of years to add.
//
void vdbTimeStamp::AddYears( int numYears )
{
	_ts.year += numYears;
}


//-------------------------------------------
//^ The IsValid function returns true if the date can be stored in the database
bool vdbTimeStamp::IsValid() const
{
	if ( IsDateValid() && IsTimeValid() )
		return true;
	else
		return false;
}


//-------------------------------------------
//^ The IsDateValid function returns true if the date can be stored in the database
bool vdbTimeStamp::IsDateValid() const
{
	if ( _ts.year >= 1900 && _ts.year <= 2100 )
		if ( _ts.month >= 1 && _ts.month <= 12 )
			if ( _ts.day >= 1 && _ts.day <= 31 )
				return true;

	return false;
}


//-------------------------------------------
//^ The IsTimeValid function returns true if the date can be stored in the database
bool vdbTimeStamp::IsTimeValid() const
{
	if ( _ts.hour >= 0 && _ts.hour <= 23 )
		if ( _ts.minute >= 0 && _ts.minute <= 59 )
			if ( _ts.second >= 0 && _ts.second <= 59 )
				return true;

	return false;
}


//-------------------------------------------
//^ The SetDateNullValue function is called whenever an invalid date is encountered.
//  It replaces the date with a valid well-known set of values, which IsNullValue
//  understands.  This is used to prevent database problems and to allow user interface
//  functions to get and set all blanks instead of an actual date/time.
void vdbTimeStamp::SetNullValue()
{
	SetDateNullValue();
	SetTimeNullValue();
}


//-------------------------------------------
//^ The SetDateNullValue function is called whenever an invalid date is encountered.
//  It replaces the date with a valid well-known set of values, which IsNullValue
//  understands.  This is used to prevent database problems and to allow user interface
//  functions to get and set all blanks instead of an actual date/time.
void vdbTimeStamp::SetDateNullValue()
{
	_ts.year = 1970;
	_ts.month = 2;
	_ts.day = 1;
}


//-------------------------------------------
//^ The SetTimeNullValue function is called whenever an invalid date is encountered.
//  It replaces the date with a valid well-known set of values, which IsNullValue
//  understands.  This is used to prevent database problems and to allow user interface
//  functions to get and set all blanks instead of an actual date/time.
void vdbTimeStamp::SetTimeNullValue()
{
	_ts.hour = 0;
	_ts.minute = 0;
	_ts.second = 0;
	_ts.fraction = 0;
}


//-------------------------------------------
//< returns true if this is the special NULL value
bool vdbTimeStamp::IsNullValue() const
{
	if ( IsDateNullValue() && IsTimeNullValue() )
		return true;
	else
		return false;
}


//-------------------------------------------
//< returns true if this is the special NULL value
bool vdbTimeStamp::IsDateNullValue() const
{
	if ( _ts.year != 1970 )
		return false;
	if ( _ts.month != 2 )
		return false;
	if ( _ts.day != 1 )
		return false;

	return true;
}


//-------------------------------------------
//< returns true if this is the special NULL value
bool vdbTimeStamp::IsTimeNullValue() const
{
	if ( _ts.hour != 0 )
		return false;
	if ( _ts.minute != 0 )
		return false;
	if ( _ts.second != 0 )
		return false;
	if ( _ts.fraction != 0 )
		return false;

	return true;
}


#ifndef UNICODE
//=============================================================================
// IO Stream functions
//=============================================================================

//-------------------------------------
//^^ The ostream operator
std::ostream& operator<< (std::ostream& os, const vdbTimeStamp& obj )
{
	const char sep = ',';

	os << obj._ts.year << sep;
	os << obj._ts.month << sep;
	os << obj._ts.day << sep;
	os << obj._ts.hour << sep;
	os << obj._ts.minute << sep;
	os << obj._ts.second << sep;
	os << obj._ts.fraction;

	return os;
}


//-------------------------------------
//^^ The istream operator
std::istream& operator>> (std::istream& is, vdbTimeStamp& obj )
{
	char sep;

	if ( is.fail() )
		return is;

	is >> obj._ts.year >> sep;
	is >> obj._ts.month >> sep;
	is >> obj._ts.day >> sep;
	is >> obj._ts.hour >> sep;
	is >> obj._ts.minute >> sep;
	is >> obj._ts.second >> sep;
	is >> obj._ts.fraction;

	return is;
}

#endif // UNICODE
