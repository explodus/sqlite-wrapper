///	@file		include/sqlite_wrapper/detail/time.hpp
///	@date		15.3.2010 8:57
///	file base	time
///	file ext	hpp
///
/// @author		T. Schroeder (explodus@gmx.de)
///
/// This software is released under the BSD Licence
///	
///	Using		:
///		This code may be used in compiled form in any way you desire. This
///		file may be redistributed unmodified by any means PROVIDING it is
///		not sold for profit without the authors written consent, and
///		providing that this notice and the authors name is included.
///		This file is provided 'as is' with no expressed or implied warranty.
///		The author accepts no liability if it causes any damage to your 
///		computer.
///	Expect Bugs		:
///		Please let me know of any bugs/mods/improvements.
///		and I will try to fix/incorporate them into this file.
///		Enjoy!
///
///	@brief 

#ifndef DB_TIME_HPP_INCLUDED
#define DB_TIME_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>

namespace db { namespace detail {

	/// @todo need documentation
	///
	/// <BR>qualifier
	/// <BR>access    public 
	/// 
	/// @return       db::string
	/// @param        _tm as const tm *
	///
	/// @date         16.3.2010 21:26
	/// 
	inline string to_sql_string(const tm* _tm) 
	{
		ostringstream ss;
		if (!_tm)
			return ss.str();
		ss << std::setw(2) << std::setfill(DB_TEXT('0'))
			<< _tm->tm_mday << DB_TEXT(".")
			<< std::setw(2) << std::setfill(DB_TEXT('0')) 
			<< _tm->tm_mon
			<< DB_TEXT(".") << 1900+_tm->tm_year << DB_TEXT(" ")
			<< _tm->tm_hour << DB_TEXT(":") << _tm->tm_min << DB_TEXT(":")
			<< _tm->tm_sec;
		return ss.str();
	}

} }

#ifndef BOOST_NO_STD_LOCALE

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace db { namespace detail 
{
	/// @todo need documentation
	/// <BR>qualifier
	/// <BR>access    public  
	/// @return    db::string
	/// @param     d as const boost::gregorian::date & 
	///
	/// @date      18:2:2009   11:44
	///
	inline string to_sql_string(const boost::gregorian::date& d) 
	{
		boost::gregorian::date::ymd_type ymd = d.year_month_day();
		ostringstream ss;
		ss << ymd.year << DB_TEXT("-")
			<< std::setw(2) << std::setfill(DB_TEXT('0')) 
			<< ymd.month.as_number() //solves problem with gcc 3.1 hanging
			<< DB_TEXT("-")
			<< std::setw(2) << std::setfill(DB_TEXT('0')) 
			<< ymd.day;
		return ss.str();
	}

	/// @todo need documentation
	///
	/// <BR>qualifier
	/// <BR>access    public 
	/// 
	/// @return       db::string
	/// @param        t as const boost::posix_time::ptime &
	///
	/// @date         16.3.2010 21:20
	/// 
	inline string to_sql_string(const boost::posix_time::ptime& t) 
	{
		tm pt_tm = boost::posix_time::to_tm(t);
		return to_sql_string(&pt_tm);
	}

} }

#else

#include <time.h>

namespace db
{
	typedef LONGLONG time_t_ce;

	namespace time
	{
		const unsigned TIME_FAILURE	= 0xFFFFFFFF;
		/// Ascii buffer size is 26 bytes, (24 chars and CR+LF)
		const unsigned ASC_BUFF_SIZE = 26;  
		const long SEC_IN_HOUR = 3600L;
		const long SECS_IN_MIN = 60L;
		const unsigned DAYSPERWEEK = 7;
		const unsigned YEAR0 = 1900;
		const unsigned EPOCH_YR = 1970;
		const long SECS_DAY = (24L * 60L * 60L);
		const long TIME_MAX = 2147483647L;

		/// @brief     LEAPYEAR
		/// @todo need documentation
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    bool
		/// @param     year as T
		///
		/// @date      20:2:2009   9:54
		///
		template<class T>
		bool LEAPYEAR(T year) {
			return (!((year) % 4) && (((year) % 100) || !((year) % 400)));
		}

		static LONG _localtime;

		// Is the local time in daylight savings time
		//
		static DWORD _isdst;

		// Bias for daylight savings time
		//
		static int _dstBias;

		// Contains the time zone string
		//
		static char tz_name[2][32];

		static struct tm tmbuf;

		// Contains the 1/1/1970 reference date/time
		//
		const SYSTEMTIME st1970 = {1970, 1,	4, 1, 0, 0, 0, 0};

		const int _ytab[2][12] = 
		{
			{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
			{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
		};

		// Contains the days of the week abreviation
		//
		static const char *aday[] = {
			"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
		};

		// Contains the days of the week full name
		//
		static const char *day[] = {
			"Sunday", "Monday", "Tuesday", "Wednesday",
			"Thursday", "Friday", "Saturday"
		};

		// Contains the months of the year abreviation
		//
		static const char *amonth[] = {
			"Jan", "Feb", "Mar", "Apr", "May", "Jun",
			"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
		};

		// Contains the months of the year full name
		//
		static char *month[] = {
			"January", "February", "March", "April", "May", "June",
			"July", "August", "September", "October", "November", "December"
		};

		/// @brief     Convert 100ns units since 1601 to seconds since 1970
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    LONGLONG
		/// @param     st as SYSTEMTIME*
		///
		/// @date      11:3:2009   10:15
		///
		inline LONGLONG SystemTimeToSecondsSince1970(SYSTEMTIME *st)
		{
			ULARGE_INTEGER	uli;
			FILETIME		ft;
			ULARGE_INTEGER	uli1970;
			FILETIME		ft1970;

			// convert to a FILETIME
			// Gives number of 100-nanosecond intervals since January 1, 1601 (UTC)
			if(!SystemTimeToFileTime(st, &ft))
				return TIME_FAILURE;

			// convert to a FILETIME
			// Gives number of 100-nanosecond intervals since January 1, 1970 (UTC)
			if(!SystemTimeToFileTime(&st1970, &ft1970))
				return TIME_FAILURE;

			// Copy file time structures into ularge integer so we can do
			// the math more easily
			memcpy(&uli, &ft, sizeof(uli));
			memcpy(&uli1970, &ft1970, sizeof(uli1970));

			// Subtract the 1970 number of 100 ns value from the 1601 100 ns value
			// so we can get the number of 100 ns value between 1970 and now
			// then devide be 10,000,000 to get the number of seconds since 1970
			uli.QuadPart = ((uli.QuadPart - uli1970.QuadPart) / 10000000);

			return (LONGLONG)uli.QuadPart;
		}

		/// @brief     Convert seconds since 1970 to a file time 
		///	           in 100ns units since 1601 then to a system time.
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    void
		/// @param     timer as const time_t_ce * 
		/// @param     st as SYSTEMTIME * 
		/// @param     local as BOOLEAN 
		///
		inline void SecondsSince1970ToSystemTime(const time_t_ce * timer, 
			SYSTEMTIME * st, BOOLEAN local)
		{
			ULARGE_INTEGER	uli;
			FILETIME		ft;
			ULARGE_INTEGER	uli1970;
			FILETIME		ft1970;

			// Convert system time to file time
			//
			if(!SystemTimeToFileTime(&st1970, &ft1970))
			{ st = 0; return; }

			// convert hundreds of nanosecs to secs: 1 sec = 1e7 100ns
			// Gives number of seconds since 1/1/1601 (UTC)
			memcpy(&uli, timer, sizeof(uli));
			memcpy(&uli1970, &ft1970, sizeof(uli1970));

			// If we want local time then subtract the number of seconds between
			// UTC and current local time
			if (local) // Calculate 100ns since 1/1/1601 local time
				uli.QuadPart = (((uli.QuadPart - _localtime)*10000000) + 
				uli1970.QuadPart);
			else // Calculate 100ns since 1/1/1601 UTC
				uli.QuadPart = ((uli.QuadPart)*10000000 + uli1970.QuadPart);

			// copy data back into the ft
			memcpy(&ft, &uli, sizeof(uli));

			// convert to a SYSTEMTIME
			if(!FileTimeToSystemTime(&ft, st))
			{ st = 0; return; }

			return;
		}

		/// @brief     SetTz
		/// @todo need documentation
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    void
		/// @param     _st as SYSTEMTIME * 
		///
		/// @date      20:2:2009   9:54
		///
		inline void SetTz(SYSTEMTIME *_st)
		{
			TIME_ZONE_INFORMATION	tz;
			SYSTEMTIME				st = *_st;
			FILETIME				ftDT; // Daylight savings file time
			FILETIME				ftST; // Standard time file time
			FILETIME				ft;   // file time to compare
			int						i;

			GetTimeZoneInformation(&tz);

			// Convert current system time, daylight savings changover time
			// and standard time to file time to see if current time is between
			// the two dates.  If so then we are in daylight savings 
			// otherwise we are not.
			//
			SystemTimeToFileTime(&st, &ft);
			tz.DaylightDate.wYear = st.wYear;
			tz.StandardDate.wYear = st.wYear;
			SystemTimeToFileTime(&tz.DaylightDate, &ftDT);
			SystemTimeToFileTime(&tz.StandardDate, &ftST);

			// -1 First file time is earlier than second file time. 
			//  0 First file time is equal to second file time. 
			//  1 First file time is later than second file time. 
			if ((CompareFileTime(&ft,&ftDT) >= 0) && 
				(CompareFileTime(&ft,&ftST) <= 0) )
				_isdst = TRUE;
			else
				_isdst = FALSE;

			// Set localtime difference in seconds from UTC
			if (_isdst) 
				_localtime = (tz.Bias + tz.DaylightBias) * SECS_IN_MIN;
			else // Convert to seconds
				_localtime = tz.Bias * SECS_IN_MIN;

			_dstBias = tz.DaylightBias;

			// Set the standard and daylight strings
			for (i=0;i<32;i++)
			{
				tz_name[0][i] = (char)tz.StandardName[i];
				tz_name[1][i] = (char)tz.DaylightName[i];
			}
		}

		/// @brief     TmToSystemTime
		/// @todo need documentation
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    void
		/// @param     tmbuffer as struct tm * 
		/// @param     st as SYSTEMTIME * 
		///
		/// @date      20:2:2009   9:54
		///
		inline void TmToSystemTime(struct tm * tmbuffer, SYSTEMTIME *st)
		{
			st->wHour = static_cast<WORD>(tmbuffer->tm_hour);
			st->wDay = static_cast<WORD>(tmbuffer->tm_mday);
			st->wMinute = static_cast<WORD>(tmbuffer->tm_min);
			st->wMonth = static_cast<WORD>(tmbuffer->tm_mon + 1);
			st->wSecond = static_cast<WORD>(tmbuffer->tm_sec);
			st->wDayOfWeek = static_cast<WORD>(tmbuffer->tm_wday);
			st->wYear = static_cast<WORD>(tmbuffer->tm_year + 1900);
			st->wMilliseconds = 0;
		}

		/// @brief     mktime_ce
		/// @todo need documentation
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    time_t_ce
		/// @param     tptr as struct tm * 
		///
		/// @date      20:2:2009   9:54
		///
		inline time_t_ce mktime_ce(struct tm *tptr)
		{
			SYSTEMTIME st;
			TmToSystemTime(tptr, &st);
			SetTz(&st);

			DWORD day = 0;
			DWORD year = 0;
			DWORD seconds = 0;
			DWORD overflow;
			DWORD tm_year;
			DWORD yday, month;

			// see if seconds are < 0
			while(tptr->tm_sec < 0)
			{
				// steal 60 seconds from the minutes
				tptr->tm_sec += 60;
				tptr->tm_min--;
			}
			// roll any seconds > 60 into the minutes
			tptr->tm_min += tptr->tm_sec / 60;
			// then crop them off
			tptr->tm_sec %= 60;

			// see if minutes are < 0
			while(tptr->tm_min < 0)
			{
				// steal 60 minutes from the hours
				tptr->tm_min += 60;
				tptr->tm_hour--;
			}
			// roll any minutes > 60 into the hours
			tptr->tm_hour += tptr->tm_min / 60;
			// then crop them off
			tptr->tm_min %= 60;

			// see if hours are < 0
			while(tptr->tm_hour < 0)
			{
				// steal 24 hours from the days
				tptr->tm_hour += 24;
				day--;
			}
			// keep any "excess" days (tm doesn't have a convenient place for this)
			day += tptr->tm_hour / 24;
			// crop
			tptr->tm_hour %= 24;

			// roll any months > 12 into the years
			tptr->tm_year += tptr->tm_mon / 12;
			// then crop the off
			tptr->tm_mon %= 12;

			// see if months are < 0
			if (tptr->tm_mon < 0) 
			{
				// steal 12 months from the years
				tptr->tm_mon += 12;
				tptr->tm_year--;
			}

			// add number of days into the month to total day
			day += (tptr->tm_mday - 1);

			// if days are < 0 then calculate the number of days
			// checking to see if the month is a leap year month
			while (day < 0) 
			{
				// If months are < 0 then steal 12 months from number of years
				// for the day calculation
				if(--tptr->tm_mon < 0) 
				{
					tptr->tm_year--;
					tptr->tm_mon = 11;
				}
				day += _ytab[LEAPYEAR(YEAR0 + tptr->tm_year)][tptr->tm_mon];
			}
			// if day is greater then the number of days in the month
			// subtract the number of days in the month and adjust the 
			// month
			while (day >= static_cast<DWORD>(
				_ytab[LEAPYEAR(YEAR0 + tptr->tm_year)][tptr->tm_mon])) 
			{
				day -= _ytab[LEAPYEAR(YEAR0 + tptr->tm_year)][tptr->tm_mon];
				if (++(tptr->tm_mon) == 12) 
				{
					tptr->tm_mon = 0;
					tptr->tm_year++;
				}
			}
			tptr->tm_mday = day + 1;
			year = EPOCH_YR;

			// if year is less then 1970 then return error
			if (static_cast<DWORD>(tptr->tm_year) < year - YEAR0) 
				return (time_t_ce) -1;

			seconds = 0;
			day = 0;                      // Means days since day 0 now
			overflow = 0;

			// Assume that when day becomes negative, there will certainly
			// be overflow on seconds.
			// The check for overflow needs not to be done for leapyears
			// divisible by 400.
			// The code only works when year (1970) is not a leapyear.
			tm_year = tptr->tm_year + YEAR0;

			// make sure we are not past the max year for 32-bit number
			if (TIME_MAX / 365 < tm_year - year) overflow++;

			// calculate number of days since EPOCH
			day = (tm_year - year) * 365;

			if (TIME_MAX - day < (tm_year - year) / 4 + 1) overflow++;

			day += (tm_year - year) / 4 + 
				((tm_year % 4) && tm_year % 4 < year % 4);
			day -= (tm_year - year) / 100 + 
				((tm_year % 100) && tm_year % 100 < year % 100);
			day += (tm_year - year) / 400 + 
				((tm_year % 400) && tm_year % 400 < year % 400);

			// setup for calculation of the yday or Julian day since Jan 1
			yday = month = 0;

			// add up the number of days for the preceding months
			while (month < static_cast<DWORD>(tptr->tm_mon))
			{
				yday += _ytab[LEAPYEAR(tm_year)][month];
				month++;
			}
			// add the number of days in the current month
			yday += (tptr->tm_mday - 1);

			// make sure the didn't overflow
			if (day + yday < 0) overflow++;

			day += yday;

			// set the year day in the structure
			tptr->tm_yday = yday;

			// calculate the weekday
			tptr->tm_wday = (day + 4) % 7; // Day 0 was thursday (4)

			// start the seconds calculation by totaling the hours, min, seconds
			seconds = ((tptr->tm_hour * 60L) + tptr->tm_min) * 
				60L + tptr->tm_sec;

			// make sure we are not going to overflow
			if ((TIME_MAX - seconds) / SECS_DAY < day) overflow++;

			// calculate the number of seconds for the number of days
			seconds += day * SECS_DAY;

			// Now adjust according to timezone and daylight saving time
			if (((_localtime > 0) 
				&& (TIME_MAX - _localtime < static_cast<long>(seconds)))
				|| ((_localtime < 0) 
				&& (static_cast<long>(seconds) < -_localtime)))
				overflow++;

			// Adjust for local time zone
			seconds += _localtime;

			// return error if we are going to blow the max values
			if (overflow) return (time_t_ce) -1;

			if ((time_t_ce) seconds != seconds) return (time_t_ce) -1;

			// return the number of seconds since EPOCH
			return (time_t_ce) seconds;
		}

		/// @brief     Get the current time from the system clock. 
		///            Stores that value in timer. If timer is null, 
		///            the value is not stored, but it is still returned 
		///            by the function.
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    time_t_ce
		/// @param     tloc as time_t_ce * 
		///
		///
		inline time_t_ce time_ce(time_t_ce *tloc)
		{
			SYSTEMTIME	st;
			LONGLONG	secs = 0;

			// Get current system time
			GetSystemTime(&st);

			// Set time zone information
			SetTz(&st);

			// convert system time to number of seconds since 1970
			secs = SystemTimeToSecondsSince1970(&st);

			// check for failure
			if(secs == TIME_FAILURE)
				return TIME_FAILURE;

			// If tloc is not NULL, the return value is also 
			// stored in the location to which tloc points
			if(tloc != 0)
			{
				if(IsBadWritePtr(tloc, sizeof(time_t_ce)))
					return TIME_FAILURE;
				memcpy(tloc, &secs, sizeof(time_t_ce));
			}

			return secs;
		}

		/// @todo need documentation
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    DWORD
		/// @param     st as SYSTEMTIME * 
		///
		/// @date      18:3:2009   13:45
		///
		inline DWORD JulianDays(SYSTEMTIME * st)
		{
			int m = 0;
			int y = 0;
			double jdd = 0;
			double jddYearStart = 0;

			// Calculate the Julian day for the beginning of the year
			m = 13;
			y = st->wYear - 1;
			jddYearStart = 1 + (153 * m - 457) / 5 + 365 * y + 
				(y / 4) - (y / 100) + (y / 400) + 1721118.5;

			// Calculate Julian Day for Current Date
			if (st->wMonth >= 3)
			{
				m = st->wMonth;
				y = st->wYear;
			}
			jdd = st->wDay + (153 * m - 457) / 5 + 365 * y + 
				(y / 4) - (y / 100) + (y / 400) + 1721118.5;

			// Subract the year start Julian date from the 
			// Current Julian date to get
			// the number of Julian days from the year start
			return (DWORD)(jdd - jddYearStart);
		}

		/// @todo need documentation
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    void
		/// @param     st as SYSTEMTIME * 
		/// @param     tmbuffer as struct tm * 
		///
		/// @date      18:3:2009   13:43
		///
		inline void SystemTimeToTm(SYSTEMTIME *st, struct tm * tmbuffer)
		{
			tmbuffer->tm_hour = st->wHour;
			tmbuffer->tm_mday = st->wDay;
			tmbuffer->tm_min = st->wMinute;
			tmbuffer->tm_mon = st->wMonth - 1;
			tmbuffer->tm_sec = st->wSecond;
			tmbuffer->tm_wday = st->wDayOfWeek;
			// Julian days, numer of days since Jan 1
			tmbuffer->tm_yday = JulianDays(st);		
			tmbuffer->tm_year = st->wYear - 1900;
			// Is Daylight Savings Time
			tmbuffer->tm_isdst = _isdst;			
		}

		/// @todo need documentation
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    struct tm *
		/// @param     timer as const time_t_ce * 
		/// @param     tmbuf as struct tm * 
		/// @param     local as BOOLEAN 
		///
		/// @date      18:3:2009   13:43
		///
		inline struct tm *gmtime_r_ce(const time_t_ce *timer, 
		struct tm *tmbuf, BOOLEAN local)
		{
			SYSTEMTIME	st;

			SecondsSince1970ToSystemTime(timer, &st, local);
			SetTz(&st);
			if(_isdst) {
				SecondsSince1970ToSystemTime(timer, &st, local);
			}

			// copy SYSTEMTIME data to tm structure
			SystemTimeToTm(&st, tmbuf);

			return tmbuf;
		}

		/// @todo need documentation
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    struct tm *
		/// @param     timer as const time_t_ce * 
		/// @param     tmbuf as struct tm * 
		///
		/// @date      18:3:2009   13:40
		///
		inline struct tm *localtime_r_ce(const time_t_ce *timer, 
		struct tm *tmbuf)
		{
			return gmtime_r_ce(timer, tmbuf, TRUE);
		}

		/// @todo need documentation
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    struct tm*
		/// @param     timer as const time_t_ce * 
		///
		/// @date      18:3:2009   13:40
		///
		inline struct tm* localtime_ce(const time_t_ce* timer)
		{ return localtime_r_ce(timer, &tmbuf); }
		}

	} // namespace time 

	namespace detail 
	{
		/// @todo need documentation
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    db::string
		/// @param     d as const time_t_ce & 
		///
		/// @date      18:2:2009   11:44
		///
		inline string to_sql_string(const time_t_ce& d) 
		{
			tm* _tm(0);
			_tm = time::localtime_ce(&d);
			return to_sql_string(_tm);
		}

		/// @todo need documentation
		/// <BR>qualifier
		/// <BR>access    public static  
		/// @return    db::time_t_ce
		/// @param     s as const string & 
		/// @param     tmp as const time_t_ce * 
		///
		/// @date      19:3:2009   10:32
		///
		inline time_t_ce from_sql_string(
			const string& s
		, const time_t_ce* tmp = 0) 
		{
		tm _tm = {0};
		int err = _stscanf(s.c_str(), DB_TEXT("%d.%d.%d %d:%d:%d"), 
			&_tm.tm_mday, &_tm.tm_mon, &_tm.tm_year,
			&_tm.tm_hour, &_tm.tm_min, &_tm.tm_sec);
		return time::mktime_ce(&_tm);
		}
	}
}

#endif // BOOST_NO_STD_LOCALE

#endif // DB_TIME_HPP_INCLUDED
