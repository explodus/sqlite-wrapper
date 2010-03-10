///	@file		db.h
///	@date		09.03.2010	14:52
///	file base	db
///	file ext	h
///
/// @author		T. Schroeder (explodus@gmx.de)
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
///	@brief interface of the db class	
///        purpose: c++ sqlite handling

#ifndef db_h__
#define db_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( push, 1 )

#include <windows.h>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>

#ifndef BOOST_NO_STD_LOCALE
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/signals.hpp>
namespace sigb = boost::BOOST_SIGNALS_NAMESPACE;
#else
#include <time.h>
#endif // BOOST_NO_STD_LOCALE
#ifdef _MFC_VER
#include <ATLComTime.h>
#endif

#include <boost/format.hpp>
#include <boost/detail/allocator_utilities.hpp>
#include <boost/detail/quick_allocator.hpp> 
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include "sqlite3.h"

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P) { (P) = (P); } 
#endif

#ifndef _T
	#ifdef _UNICODE
		#define _T(x) L ## x
	#else
		#define _T(x) ## x
	#endif // _UNICODE
#endif

#pragma warning( pop )

namespace db
{
#ifdef _UNICODE
  typedef std::wstring string;
  typedef std::wostringstream ostringstream;
  typedef std::wistringstream istringstream;
  typedef std::wostream ostream;
  typedef boost::basic_format<wchar_t > format;
#else
  typedef std::string string;
  typedef std::ostringstream ostringstream;
  typedef std::istringstream istringstream;
  typedef std::ostream ostream;
  typedef boost::basic_format<char > format;
#endif // _UNICODE

  typedef std::pair<string, string> string_pair;
  typedef std::vector<string> string_vec;

  typedef LONGLONG time_t_ce;

  namespace detail
  {
#ifndef BOOST_NO_STD_LOCALE

    /// @qualifier
    /// @access    public  
    /// @return    db::string
    /// @param     d as const boost::gregorian::date & 
    ///
    /// @date      18:2:2009   11:44
    ///
    inline string to_sql_string(const boost::gregorian::date& d) 
    {
      boost::gregorian::date::ymd_type ymd = d.year_month_day();
      ostringstream ss;
      ss << ymd.year << _T("-")
        << std::setw(2) << std::setfill(_T('0')) 
        << ymd.month.as_number() //solves problem with gcc 3.1 hanging
        << _T("-")
        << std::setw(2) << std::setfill(_T('0')) 
        << ymd.day;
      return ss.str();
    }

#else

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
      ///
      /// @qualifier
      /// @access    public  
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
      /// @qualifier
      /// @access    public  
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
      /// @qualifier
      /// @access    public  
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
        { st = NULL; return; }

        // convert hundreds of nanosecs to secs: 1 sec = 1e7 100ns
        // Gives number of seconds since 1/1/1601 (UTC)
        memcpy(&uli, timer, sizeof(uli));
        memcpy(&uli1970, &ft1970, sizeof(uli1970));

        // If we want local time then subtract the number of seconds between
        // UTC and current local time
        if (local) // Calculate 100ns since 1/1/1601 local time
          uli.QuadPart = (((uli.QuadPart - _localtime)*10000000) + uli1970.QuadPart);
        else // Calculate 100ns since 1/1/1601 UTC
          uli.QuadPart = ((uli.QuadPart)*10000000 + uli1970.QuadPart);

        // copy data back into the ft
        memcpy(&ft, &uli, sizeof(uli));

        // convert to a SYSTEMTIME
        if(!FileTimeToSystemTime(&ft, st))
        { st = NULL; return; }

        return;
      }

      /// @brief     SetTz
      ///
      /// @qualifier
      /// @access    public  
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
        // the two dates.  If so then we are in daylight savings otherwise we
        // are not.
        //
        SystemTimeToFileTime(&st, &ft);
        tz.DaylightDate.wYear = st.wYear;
        tz.StandardDate.wYear = st.wYear;
        SystemTimeToFileTime(&tz.DaylightDate, &ftDT);
        SystemTimeToFileTime(&tz.StandardDate, &ftST);

        // -1 First file time is earlier than second file time. 
        //  0 First file time is equal to second file time. 
        //  1 First file time is later than second file time. 
        //
        if ((CompareFileTime(&ft,&ftDT) >= 0) && 
					(CompareFileTime(&ft,&ftST) <= 0) )
          _isdst = TRUE;
        else
          _isdst = FALSE;

        // Set localtime difference in seconds from UTC
        if (_isdst) // Adjust for Daylight Savings Time and convert to seconds
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
      ///
      /// @qualifier
      /// @access    public  
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
      ///
      /// @qualifier
      /// @access    public  
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
        tptr->tm_wday = (day + 4) % 7;               // Day 0 was thursday (4)

        // start the seconds calculation by totaling the hours, min, seconds
        seconds = ((tptr->tm_hour * 60L) + tptr->tm_min) * 60L + tptr->tm_sec;

        // make sure we are not going to overflow
        if ((TIME_MAX - seconds) / SECS_DAY < day) overflow++;

        // calculate the number of seconds for the number of days
        seconds += day * SECS_DAY;

        // Now adjust according to timezone and daylight saving time
        if (((_localtime > 0) && 
					(TIME_MAX - _localtime < static_cast<long>(seconds)))
          || ((_localtime < 0) && (static_cast<long>(seconds) < -_localtime)))
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
      /// @qualifier
      /// @access    public  
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
        if(tloc != NULL)
        {
          if(IsBadWritePtr(tloc, sizeof(time_t_ce)))
            return TIME_FAILURE;
          memcpy(tloc, &secs, sizeof(time_t_ce));
        }

        return secs;
      }

      /// @qualifier
      /// @access    public  
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
        jddYearStart = 1 + (153 * m - 457) / 5 + 365 * y + (y / 4) - (y / 100) + 
          (y / 400) + 1721118.5;

        // Calculate Julian Day for Current Date
        if (st->wMonth >= 3)
        {
          m = st->wMonth;
          y = st->wYear;
        }
        jdd = st->wDay + (153 * m - 457) / 5 + 365 * y + (y / 4) - (y / 100) + 
          (y / 400) + 1721118.5;

        // Subract the year start Julian date from the Current Julian date to get
        // the number of Julian days from the year start
        return (DWORD)(jdd - jddYearStart);
      }

      /// @qualifier
      /// @access    public  
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

      /// @qualifier
      /// @access    public  
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

      /// @qualifier
      /// @access    public  
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

      /// @qualifier
      /// @access    public  
      /// @return    struct tm*
      /// @param     timer as const time_t_ce * 
      ///
      /// @date      18:3:2009   13:40
      ///
      inline struct tm* localtime_ce(const time_t_ce* timer)
      { return localtime_r_ce(timer, &tmbuf); }
    }

    /// @qualifier
    /// @access    public  
    /// @return    db::string
    /// @param     d as const time_t_ce & 
    ///
    /// @date      18:2:2009   11:44
    ///
    inline string to_sql_string(const time_t_ce& d) 
    {
      tm* _tm(0);
      _tm = time::localtime_ce(&d);
      ostringstream ss;
      if (!_tm)
        return ss.str();
      ss << std::setw(2) << std::setfill(_T('0'))
        << _tm->tm_mday << _T(".")
        << std::setw(2) << std::setfill(_T('0')) 
        << _tm->tm_mon
        << _T(".") << _tm->tm_year << _T(" ")
        << _tm->tm_hour << _T(":") << _tm->tm_min << _T(":")
        << _tm->tm_sec;
      return ss.str();
    }

    /// @qualifier
    /// @access    public static  
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
			/// @todo replace UNREFERENCED_PARAMETER
      UNREFERENCED_PARAMETER(tmp);
      tm _tm = {0};
      int err = _stscanf(s.c_str(), _T("%d.%d.%d %d:%d:%d"), 
        &_tm.tm_mday, &_tm.tm_mon, &_tm.tm_year,
        &_tm.tm_hour, &_tm.tm_min, &_tm.tm_sec);
			/// @todo replace UNREFERENCED_PARAMETER
      UNREFERENCED_PARAMETER(err);
      return time::mktime_ce(&_tm);
    }

#endif

    /// @brief     replace_all
    ///
    /// @qualifier
    /// @access    public  
    /// @return    typename S::size_type
    /// @param     s as S & 
    /// @param     from as const S & 
    /// @param     to as const S & 
    ///
    /// @date      20:2:2009   9:53
    ///
    template<typename S>
    inline typename S::size_type replace_all(S& s, const S& from, const S& to)
    {
      typedef typename S::size_type st;
      st cnt(S::npos);

      if(from != to && !from.empty()) {
        st pos1(0), pos2(0);
        const st from_len(from.size()), to_len(to.size());
        cnt = 0;

        while((pos1 = s.find(from, pos2)) != S::npos) {
          s.replace(pos1, from_len, to);
          pos2 = pos1 + to_len;
          ++cnt;
        }
      }

      return cnt;
    }

    /// @brief     erase_all
    ///
    /// @qualifier
    /// @access    public  
    /// @return    typename S::size_type
    /// @param     s as S & 
    /// @param     from as const S & 
    ///
    /// @date      20:2:2009   9:53
    ///
    template<typename S>
    inline typename S::size_type erase_all(S& s, const S& from)
    {
      return replace_all<S>(s, from, _T(""));
    }

    /// @brief     replace_all_copy
    ///
    /// @qualifier
    /// @access    public  
    /// @return    S
    /// @param     s as const S & 
    /// @param     from as const S & 
    /// @param     to as const S & 
    ///
    /// @date      20:2:2009   9:52
    ///
    template<typename S>
    inline S replace_all_copy(const S& s, const S& from, const S& to)
    {
      S ret(s);
      replace_all<S>(ret, from, to);
      return ret;
    }

    /// @brief     erase_all_copy
    ///
    /// @qualifier
    /// @access    public  
    /// @return    S
    /// @param     s as const S & 
    /// @param     from as const S & 
    ///
    /// @date      20:2:2009   9:52
    ///
    template<typename S>
    inline S erase_all_copy(const S& s, const S& from)
    {
      S ret(s);
      replace_all<S>(ret, from, _T(""));
      return ret;
    }

    /// @brief     to_string
    ///
    /// @access    public  
    /// @return    db::string
    /// @param     a as T 
    ///
    /// @date      20:2:2009   9:52
    ///
    template<class T> 
    inline string to_string(const T& a) 
    { ostringstream ost; ost << a; return ost.str(); }   


    /// @brief        to_type
    ///
    /// @return       typename T
    /// @param        a as const string &
    ///
    /// @date         9.3.2010 9:16
    ///
    template<typename T> 
    inline typename T to_type(const string& a) 
    { istringstream ost(a); T ret; ost >> ret; return ret; }   

    /// @brief        to_string
    ///
    /// @return       db::string
    /// @param        a as const double &
    ///
    /// @date         9.3.2010 9:16
    ///
    template<> 
    inline string to_string(const double& a) 
    { 
			ostringstream ost; 
			ost << std::setprecision(16) << a; 
			return ost.str(); 
		}   

    /// @brief        specialise
    ///
    /// @return       db::string
    /// @param        a as const bool &
    ///
    /// @date         9.3.2010 9:16
    ///
    template<> 
    inline string to_string(const bool& a) 
    { return string(a?_T("1"):_T("0")); } 

    /// @brief        specialise
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       db::string
    /// @param        a as const string &
    ///
    /// @date         9.3.2010 9:17
    ///
    template<> 
    inline string to_string(const string& a) 
    { return a; }   

    /// @brief        specialise
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       db::string
    /// @param        a as const CString &
    ///
    /// @date         9.3.2010 9:17
    ///
    template<> 
    inline string to_string(const CString& a) 
    { return string(a); }   

#ifdef _MFC_VER
    ///
    /// @qualifier
    /// @access    public  
    /// @return    db::string
    /// @param     d as const COleDateTime & 
    ///
    /// @date      19:3:2009   10:28
    ///
    inline string to_sql_string(const COleDateTime& d) 
    {
      ostringstream ss;
      ss << std::setw(2) << std::setfill(_T('0'))
        << d.GetDay() << _T(".")
        << std::setw(2) << std::setfill(_T('0')) 
        << d.GetMonth()
        << _T(".") << d.GetYear() << _T(" ")
        << d.GetHour() << _T(":") << d.GetMinute() << _T(":")
        << d.GetSecond();
      return ss.str();
    }

    /// @qualifier
    /// @access    public static  
    /// @return    COleDateTime
    /// @param     s as const string & 
    /// @param     tmp as const COleDateTime * 
    ///
    /// @date      19:3:2009   10:33
    ///
    static COleDateTime from_sql_string(const string& s, 
      const COleDateTime *tmp=0) 
    {
			/// @todo replace UNREFERENCED_PARAMETER
      UNREFERENCED_PARAMETER(tmp);
      COleDateTime ret;
      tm _tm = {0};
      int err = _stscanf(s.c_str(), _T("%d.%d.%d %d:%d:%d"), 
        &_tm.tm_mday, &_tm.tm_mon, &_tm.tm_year,
        &_tm.tm_hour, &_tm.tm_min, &_tm.tm_sec);
			/// @todo replace UNREFERENCED_PARAMETER
      UNREFERENCED_PARAMETER(err);
      ret.SetDateTime(_tm.tm_year, _tm.tm_mon, _tm.tm_mday, 
        _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
      if (ret.GetStatus()==COleDateTime::invalid)
        ret = to_type<double>(s);
      return ret;
    }
#endif // _MFCVER

    /// @brief     front_back_delim
    ///
    /// @qualifier
    /// @access    protected  
    /// @return    void
    /// @param     sText as T & 
    /// @param     delim1 as const T & 
    /// @param     delim2 as const T & 
    ///
    /// @date      20:2:2009   9:08
    ///
    template<class T> 
    inline void front_back_delim(T& sText, const T& delim1, const T& delim2)
    { sText = delim1 + sText + delim2; }

    inline string escape_sql(const string& str)
    { 
      if (str == _T("NULL"))
        return str;

      string ret(detail::replace_all_copy<string>(str, 
        _T("\'NULL\'"), _T("NULL")));
      ret = detail::replace_all_copy<string>(str, _T("'"), _T("''"));
      ret = _T("'") + ret + _T("'");
      return ret;
    }  
  }

  /** 
  \enum  db::param_types
  \brief Parameter Typen 
  */
  enum param_types
  {
    e_null,       //!< \a null type
    e_bool,       //!< \a boolean type
    e_int,        //!< \a integer type
    e_unsigned,   //!< \a unsigned integer type
    e_long,       //!< \a long type
    e_float,      //!< \a float type
    e_double,     //!< \a double type
    e_char,       //!< \a char type
    e_string,     //!< \a std::string type
    e_blob,       //!< \a blob type (big string)
    e_date_time   //!< \a boost::gregorian::date type
  }; 

  inline param_types tp(const bool&)
  { return e_bool; }
  inline param_types tp(const int&)
  { return e_int; }
  inline param_types tp(const unsigned&)
  { return e_unsigned; }
  inline param_types tp(const long&)
  { return e_long; }
  inline param_types tp(const float&)
  { return e_float; }
  inline param_types tp(const double&)
  { return e_double; }
  inline param_types tp(const char&)
  { return e_char; }
  inline param_types tp(const std::string&, bool blob=false)
  { return blob?e_blob:e_string; }
  inline param_types tp(const std::wstring&, bool blob=false)
  { return blob?e_blob:e_string; }
#ifndef BOOST_NO_STD_LOCALE
  inline param_types tp(const boost::gregorian::date &)
  { return e_date_time; } 
#else
  inline param_types tp(const time_t_ce &)
  { return e_date_time; } 
#endif

	/// @brief Parameter Extras
  enum field_extra
  {
    e_nullExtra          = 0,  //!< \a null extra
    eFixedField         = 2,  //!< \a Feld mit fester Größe (numerischer Typ)
    eVariableField      = 4,  //!< \a variables Feld (textlicher Typ)
    eAutoIncrField      = 8,  //!< \a automatisch erhöhendes Feld
    eUpdatableField     = 16, //!< \a veränderbares Feld
    eRequiredField      = 32, //!< \a benötigtes Feld
    eZeroLengthAllowed  = 64  //!< \a Nulllänge erlaubt
  };

  class split;
  class param;
  class row;
  class query;
  class base;
  namespace expr
  {
    class base;
    class connective;
    class in;
    class and;
    class or;
    class not;
    class not_eq;
    class eq;
    class lt;
    class lt_eq;
    class gt;
    class gt_eq;
    class like;
  }
  class sel;
  class del;
  class ins;
  class upd;
  class field;

  namespace exception
  {
    /// exception basis interface
    class base
    {
    private:
      string _msg; 
    public:
      base(const string& msg)  : _msg(msg) 
      { }
      base(const base& _That)  : _msg(_That._msg) 
      { }
      virtual ~base() {}

      virtual base& operator=(const base& _That)
      {
        _msg = _That._msg;
        return *this;
      }

      const TCHAR* what() { return _msg.c_str(); }

#ifdef _UNICODE
      virtual void report() { std::wcout << _msg << L"\n"; }
#else
      virtual void report() { std::cout << _msg << "\n"; }
#endif // _UNICODE

      friend ostream &operator<<(ostream &os, base &e) {
        os << e._msg;
        return os;
      } 
    };

    /** exception thrown when a record is not found */    
    class not_found : public base {
    public:
      not_found(const string& s=_T("")) : base(_T("NotFound: ")+s){}
    };
    /** exception thrown when database cannot be accessed */
    class db_error : public base {
    public:
      db_error(const string& m=_T("")) : base(_T("Database Error: ")+m){}
    };
    /** exception thrown when SQL statement cannot be executed */
    class sql_error : public base {
    public:
      sql_error(const string& m=_T("")) : base(_T("SQL Error: ")+m){}
    };
    /** exception thrown when Param handling get failed */
    class param_error : public base {
    public:
      param_error(const string& m=_T("")) : base(_T("Param Error: ")+m){}
    };
    /** exception thrown when backend produces internal error */
    class internal_error : public base {
    public:
      internal_error(const string& m=_T("")) : base(_T("Internal Error: ")+m){}
    };
    /** exception thrown when backend cannot allocate memory */
    class memory_error : public base {
    public:
      memory_error(const string& m=_T("")) : base(_T("Allocation failed: ")+m){}
    };
    /** exception thrown when database (disk) is full */
    class insertion_error : public base {
    public:
      insertion_error(const string& m=_T("")) : base(_T("Database full: ")+m){}
    };
    /** exception thrown when none of other exceptions match */
    class unknown_error : public base {
      // handles the rest
    public:
      unknown_error(const string& m=_T("")) : base(_T("Unknown Error: ")+m){}
    };
  }

  /** splitmap basis */
  class split_map : public std::map<string, string> 
  {
  public:
    /** empty splitmap */
    split_map() {}
    /** from map */
    split_map(std::map<string, string>& data) 
      : std::map<string, string>(data) {}

    /** gibt Text zurück mit eingefügtem Abgrenzer */
    string join_fields(string delim) const
    {
      string res;
      for (const_iterator i = begin(); i != end(); i++)
      {
        if (i != begin())
          res += delim;
        res += i->first;
      }
      return res; 
    }

    string join_values(string delim) const
    {
      string res;
      for (const_iterator i = begin(); i != end(); i++)
      {
        if (i != begin())
          res += delim;
        if(i->second.length()!=0)
          res += i->second;
        else
          res += _T("NULL");
      }
      return res; 
    }
  };

  /** fieldtype interface */
  class field 
  {
  public:
    typedef std::vector< std::pair<string, string> > Values;
  protected:
    string _name, _table, _sourcename;
    param_types _type;
    unsigned _extra;
    Values _values;
    long _length;
  public:
    /// @brief     field
    ///
    /// @qualifier : _name(n), _type(t), _extra(e_nullExtra), _length(-1)
    /// @access    public  
    /// @return    
    /// @param     n as const string & 
    /// @param     t as const param_types & 
    ///
    /// @date      20:2:2009   11:03
    ///
    explicit field(const string& n, 
      const param_types& t=e_long)
      : _name(n), _type(t), _extra(e_nullExtra), _length(-1) {}

    /// @brief     field
    ///
    /// @qualifier : _name(n), _type(tp(val)), _extra(e_nullExtra), _length(-1)
    /// @access    public  
    /// @return    
    /// @param     n as const string & 
    /// @param     val as const T & 
    ///
    /// @date      20:2:2009   11:03
    ///
    template<typename T>
    explicit field(const string& n, const T& val)
      : _name(n), _type(tp(val)), _extra(e_nullExtra), _length(-1) 
    {
      _values.push_back(Values::value_type(n, detail::to_string(val))); 
    }

    /// @brief     field
    ///
    /// @qualifier : _name(n_tbl.first), _type(t), _table(n_tbl.second), 
    ///              _values(vals), _extra(e_nullExtra), _length(-1)
    /// @access    public  
    /// @return    
    /// @param     n_tbl as const string_pair & 
    /// @param     t as const param_types & 
    /// @param     vals as const Values & 
    ///
    /// @date      6:8:2009   12:57
    ///
    explicit field(const string_pair& n_tbl,
      const param_types& t=e_long, 
      const Values& vals = Values())
      : _name(n_tbl.first), _type(t), _table(n_tbl.second), _values(vals), 
      _extra(e_nullExtra), _length(-1) {}

    /// @brief     field
    ///
    /// @qualifier : _name(f._name), _type(f._type), _table(f._table), 
    ///              _values(f._values), _extra(f._extra), _length(f._length)
    /// @access    public  
    /// @return    
    /// @param     const field & f
    ///
    /// @date      6:8:2009   10:56
    ///
    field(const field& f)
      : _name(f._name), _type(f._type), _table(f._table), _values(f._values), 
      _extra(f._extra), _length(f._length) {}
    
    /// @brief     field
    ///
    /// @qualifier : _type(e_long), _extra(e_nullExtra), _length(-1)
    /// @access    public  
    /// @return    
    ///
    /// @date      6:8:2009   11:30
    ///
    field() : _type(e_long), _extra(e_nullExtra), _length(-1) {}

    string fullName() const { 
      if (table().length()>0) 
        return  table() + _T(".") + name(); 
      else
        return  name(); }

    ~field() {}

    string      name()        const { return _name; }
    string      sourcename()  const { return _sourcename; }
    param_types type()        const { return _type; }
    string      table()       const { return _table; }
    const long& length()      const { return _length; }

    field&      set_type(param_types type) { _type = type; return *this; }
    void        set_table(const string& table) { _table = table; }
    void        set_sourcename(const string& sourcename) 
    {_sourcename = sourcename;}
    void        set_length(const long& len) { _length = len; }
    bool        has_extra(field_extra extra) const 
    { return ((_extra&extra)==1); }
    void        add_extra(field_extra extra) { 
      if(!(_extra&extra)) 
        _extra |= extra; 
    }
    void        remove_extra(field_extra extra) { 
      if(_extra&extra) 
        _extra &= ~extra; 
    }

    /// @brief     values
    ///
    /// @qualifier
    /// @access    public  
    /// @return    Values&
    ///
    /// @date      20:2:2009   14:26
    ///
    Values& values() { return _values; }

		/// @brief     values
    ///
    /// @qualifier const
    /// @access    public  
    /// @return    const Values&
    ///
    /// @date      20:2:2009   14:26
    ///
    const Values& values() const { return _values; }

    inline expr::in In(const string& set) const;

    inline expr::in In(const sel& sel) const;

    inline expr::like Like(const string& s);

    bool operator==(const field & fd) const {
      return fd.fullName() == fullName();
    }
    bool operator!=(const field & fd) const {
      return ! (*this == fd);
    }
  };
  typedef boost::detail::allocator::partial_std_allocator_wrapper<
    boost::detail::quick_allocator<field > > alloc_field; 

  typedef std::pair<field, field> field_pair;

  /** split basis */
  class split : public std::vector<string> 
  {
  public:
    /** empty split */
    split() {}

    /** from string vector */
    split(std::vector<string> data) 
      : std::vector<string>(data) {}

    /** from string. split to parts using delimeter */
    split(string s, string delim=_T(" "))
    {
      if (s.length()==0)
        return;
      TCHAR *ptr(&*s.begin());
      int len(delim.length());
      std::vector<TCHAR*> pointers;
      pointers.push_back(ptr);
      while((ptr = _tcsstr(ptr, delim.c_str()))) 
      {
        *ptr = _T('\0');
        ptr += len;
        pointers.push_back(ptr);
      }
      for (std::vector<TCHAR*>::iterator i(pointers.begin()), 
        e(pointers.end()); i != e; ++i)
        push_back(string(*i));
    }

    /** Gibt eine Teilmenge von Texten zurück
    \param start Startindex
    \param end Endindex
    Indexe können negativ sein. Aktueller Index is dann berechnet vom Ende von split. */
    split slice(int start, int end) const
    {
      std::vector<string> data;
      if (start < 0)
        start = start+size();
      if (end < 0)
        end = end+size();
      if (start >= static_cast<int>(size()))
        start = size()-1;
      if (end >= static_cast<int>(size()))
        end = size()-1;
      if (start >= end)
        return data;
      for (int i = start; i < end; i++)
        data.push_back(this->operator[](i));
      return data; 
    }

    /** gibt Text zurück mit eingefügtem Abgrenzer */
    string join(string delim) const
    {
      string res;
      for (const_iterator i = begin(); i != end(); i++)
      {
        if (i != begin())
          res += delim;
        res += *i;
      }
      return res; 
    }
    /** Fügt Ausdrücke ans Ende eines anderen split an */
    split & extend(const split & s)
    {
      for (size_t i = 0; i < s.size(); i++)
        push_back(s[i]);
      return *this; 
    }
  }; 

  namespace expr
  {
    /** expression base class */
    class base 
    {
    protected:
      split extraTables; //!< extra tables, to be joined
    public:

			/// @brief     default expression is true
      ///
      /// @qualifier const
      /// @access    virtual public  
      /// @return    db::string
      ///
      /// @date      18:2:2009   11:16
      ///
      virtual string str() const { return _T("True"); }

      /// @brief     operator string
      ///
      /// @qualifier const
      /// @access    virtual public  
      /// @return    
      ///
      ///
      virtual operator string() const
      { return str(); }

      /// @qualifier const
      /// @access    public  
      /// @return    db::split
      ///
      /// @date      18:2:2009   11:16
      ///
      split getExtraTables() const { 
        return extraTables;
      }

      /// @brief     default virtual destructor
      ///
      /// @qualifier
      /// @access    virtual public  
      /// @return    
      ///
      /// @date      18:2:2009   11:17
      ///
      virtual ~base() {}
    }; 

    class raw : public base 
    {
      string expr;
    public:
      /// @brief     usable for special expressions in WHERE terms
      ///
      /// @qualifier : expr(e)
      /// @access    public  
      /// @return    
      /// @param     const string & e
      ///
      /// @date      18:2:2009   11:17
      ///
      explicit raw(const string& e) : expr(e) {}

      virtual string str() const { return expr; }
    };

    class connective : public base 
    {
    private:
      string op;
    protected:
      const base &e1, &e2;

      /// @brief     base class to connect 2 expressions
      ///
      /// @qualifier : op(o), e1(e1_), e2(e2_)
      /// @access    protected  
      /// @return    
      /// @param     string o
      /// @param     const base & e1_
      /// @param     const base & e2_
      ///
      /// @date      18:2:2009   11:20
      ///
      explicit connective(string o, const base & e1_, const base & e2_)
        : op(o), e1(e1_), e2(e2_) { }

    public:        

      /// @brief     str
      ///
      /// @qualifier const
      /// @access    virtual public  
      /// @return    db::string
      ///
      /// @date      20:2:2009   12:55
      ///
      virtual string str() const {
        string res = _T("(") + e1.str() + _T(") ") + op 
          + _T(" (") + e2.str() + _T(")");
        return res;
      }
    };  

    class and : public connective 
    {
    public:
      /// @brief     connects 2 expressions with the AND operator
      ///
      /// @qualifier : connective(_T("and"), e1_, e2_)
      /// @access    public  
      /// @return    
      /// @param     const base & e1_
      /// @param     const base & e2_
      ///
      /// @date      18:2:2009   11:22
      ///
      explicit and(const base & e1_, const base & e2_) : 
      connective(_T("and"), e1_, e2_) {}

      /// @brief     str
      ///
      /// @qualifier const
      /// @access    public  
      /// @return    db::string
      ///
      /// @date      20:2:2009   12:56
      ///
      virtual string str() const {     
        if (e1.str() == _T("True"))
          return e2.str();
        else if (e2.str() == _T("True"))
          return e1.str();
        else
          return connective::str();
      }
    };

    class or : public connective 
    {
    public:
      /// @brief     connects 2 expressions with the OR operator
      ///
      /// @qualifier : connective(_T("or"), e1_, e2_)
      /// @access    public  
      /// @return    
      /// @param     const base & e1_
      /// @param     const base & e2_
      ///
      /// @date      18:2:2009   11:22
      ///
      explicit or(const base & e1_, const base & e2_) 
        : connective(_T("or"), e1_, e2_) {}

      /// @brief     str
      ///
      /// @qualifier const
      /// @access    public  
      /// @return    db::string
      ///
      /// @date      20:2:2009   12:56
      ///
      virtual string str() const {
        if (e1.str() == _T("True"))
          return _T("True");
        else if (e2.str() == _T("True"))
          return _T("True");
        else
          return connective::str();
      }
    };

    class not : public base 
    {
    private:
      const base & exp;
    public:
      /// @brief     negates expression
      ///
      /// @qualifier : exp(_exp)
      /// @access    public  
      /// @return    
      /// @param     const base & _exp
      ///
      /// @date      18:2:2009   11:23
      ///
      explicit not(const base & _exp) : exp(_exp) {}

      /// @brief     str
      ///
      /// @qualifier const
      /// @access    virtual public  
      /// @return    db::string
      ///
      /// @date      20:2:2009   12:57
      ///
      virtual string str() const {    
        return _T("not (")+exp.str()+_T(")");
      }

    };

    /** base class for operators in sql terms */
    class oper : public base 
    {
    protected:
      const field & _field;
      string op;
      string data;
      bool escape;

      ///
      /// @qualifier
      /// @access    protected  
      /// @return    bool
      /// @param     const param_types & type
      ///
      /// @date      18:2:2009   11:26
      ///
      inline bool check_escape(const param_types& type)
      {
        switch(type) 
        {
        case e_string:
        case e_char:
        case e_blob:
        case e_date_time:
          return true;
          break;
        default:
          return false;
        }
      }

      /// @brief     string overload
      ///
      /// @qualifier : _field(fld), op(o), data(d), escape(check_escape(_field.type()))
      /// @access    protected  
      /// @return    
      /// @param     const field & fld
      /// @param     const string & o
      /// @param     const string & d
      ///
      /// @date      18:2:2009   11:28
      ///
      explicit oper(const field & fld, const string& o, const string& d) 
        : _field(fld), op(o), data(d), escape(check_escape(_field.type())) 
      {
        extraTables.push_back(fld.table());
      }

      template<typename T>
      explicit oper(const field & fld, const string& o, const T& d) 
        : _field(fld), op(o), data(_T("0")), 
        escape(check_escape(_field.type())) 
      {
        data = detail::to_string(d);
        extraTables.push_back(fld.table());
      }

      /// @brief     db::field overload
      ///
      /// @qualifier : _field(fld), op(o), data(f2.fullName()), escape(false)
      /// @access    protected  
      /// @return    
      /// @param     const field & fld
      /// @param     const string & o
      /// @param     const field & f2
      ///
      /// @date      18:2:2009   11:30
      ///
      explicit oper(const field & fld, const string& o, const field &f2) 
        : _field(fld), op(o), data(f2.fullName()), escape(false) 
      {
        extraTables.push_back(fld.table());
      }

    public:
      /// @brief     str
      ///
      /// @qualifier const
      /// @access    virtual public  
      /// @return    db::string
      ///
      /// @date      20:2:2009   13:03
      ///
      virtual string str() const {
        string res;
        res += _field.fullName() + _T(" ") + op + _T(" ") + 
          (escape ? detail::escape_sql(data) : data);
        return res;
      }
    };

    /** ==, is equal operator */
    class eq : public oper {
    public:
      /// @brief     ==, is equal operator
      ///
      /// @qualifier : oper<typename T>(fld, _T("="), d)
      /// @access    public  
      /// @return    
      /// @param     const field & fld
      /// @param     const T & d
      ///
      /// @date      20:2:2009   11:52
      ///
      template<typename T>
      explicit eq(const field & fld, const T& d)
        : oper(fld, _T("="), d) 
      {}
    };

    /** <> not equal operator */
    class not_eq : public oper {
    public:
      /// @brief     not_eq
      ///
      /// @qualifier : oper(fld, _T("<>"), d)
      /// @access    public  
      /// @return    
      /// @param     const field & fld
      /// @param     const T & d
      ///
      /// @date      20:2:2009   13:03
      ///
      template<typename T>
      explicit not_eq(const field & fld, const T& d)
        : oper(fld, _T("<>"), d) 
      {}
    };

    /** > greater then operator */
    class gt : public oper {
    public:
      /// @brief     gt
      ///
      /// @qualifier : oper(fld, _T(">"), d)
      /// @access    public  
      /// @return    
      /// @param     const field & fld
      /// @param     const T & d
      ///
      /// @date      20:2:2009   13:02
      ///
      template<typename T>
      explicit gt(const field & fld, const T& d)
        : oper(fld, _T(">"), d) 
      {}
    };

    /** greater then or equal operator */
    class gt_eq : public oper {
    public:
      /// @brief     gt_eq
      ///
      /// @qualifier : oper(fld, _T(">="), d)
      /// @access    public  
      /// @return    
      /// @param     const field & fld
      /// @param     const T & d
      ///
      /// @date      20:2:2009   13:01
      ///
      template<typename T>
      explicit gt_eq(const field & fld, const T& d)
        : oper(fld, _T(">="), d) 
      {}
    };

    /** lower then operator */
    class lt : public oper {
    public:
      /// @brief     lt
      ///
      /// @qualifier : oper(fld, _T("<"), d)
      /// @access    public  
      /// @return    
      /// @param     const field & fld
      /// @param     const T & d
      ///
      /// @date      20:2:2009   13:01
      ///
      template<typename T>
      explicit lt(const field & fld, const T& d)
        : oper(fld, _T("<"), d) 
      {}
    };

    /** lower then or equal operator */
    class lt_eq : public oper {
    public:
      /// @brief     lt_eq
      ///
      /// @qualifier : oper(fld, _T("<="), d)
      /// @access    public  
      /// @return    
      /// @param     const field & fld
      /// @param     const T & d
      ///
      /// @date      20:2:2009   13:01
      ///
      template<typename T>
      explicit lt_eq(const field & fld, const T& d)
        : oper(fld, _T("<="), d) 
      {}
    };

    /** loke operator */
    class like : public oper {
    public:
      /// @brief     like
      ///
      /// @qualifier : oper(fld, _T("LIKE"), d)
      /// @access    public  
      /// @return    
      /// @param     const field & fld
      /// @param     const string & d
      ///
      /// @date      20:2:2009   13:00
      ///
      explicit like(const field & fld, const string& d)
        : oper(fld, _T("LIKE"), d) 
      {}
    };

    /** in Operator */
    class in : public oper {
    public:
      /// @brief     in
      ///
      /// @qualifier : oper(fld, _T("IN"), _T("(")+set+_T(")"))
      /// @access    public  
      /// @return    
      /// @param     const field & fld
      /// @param     const string & set
      ///
      /// @date      20:2:2009   13:01
      ///
      explicit in(const field & fld, const string& set)
        : oper(fld, _T("IN"), _T("(")+set+_T(")")) 
      {}

      /// @brief     in
      ///
      /// @qualifier
      /// @access    public  
      /// @return    
      /// @param     const field & fld
      /// @param     const sel & s
      ///
      /// @date      20:2:2009   13:01
      ///
      explicit inline in(const field & fld, const sel& s);

      /// @brief     str
      ///
      /// @qualifier const
      /// @access    virtual public  
      /// @return    db::string
      ///
      /// @date      20:2:2009   13:01
      ///
      virtual string str() const 
      { return _field.fullName() + _T(" ") + op + _T(" ") + data; }

    };
  }

  /** dynamic parameter class*/
  class param 
#ifndef BOOST_NO_STD_LOCALE
    : sigb::trackable
#endif // _DEBUG
  {
    string _data;
    param_types _type;
    bool _is_changed;
    unsigned _col; 

    /// @brief     protected constructor
    ///
    /// @qualifier : _type(e_null), _is_changed(false)
    /// @access    public  
    /// @return    
    ///
    /// @date      20:2:2009   14:36
    ///
    param() : _type(e_null), _is_changed(false)
    { _data = _T(""); }
  public:
    /// @brief     param
    ///
    /// @qualifier : _col(col), _type(e_null), _is_changed(false)
    /// @access    public  
    /// @return    
    /// @param     unsigned col
    ///
    /// @date      20:2:2009   14:36
    ///
    param(unsigned col) : _col(col), _type(e_null), _is_changed(false)
    { _data = _T(""); }

    /// @brief     ~param
    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    
    ///
    /// @date      20:2:2009   14:35
    ///
    ~param() 
    {}

    /// @brief     copy constructor
    ///
    /// @qualifier : _col(Param._col), _type(Param._type), _is_changed(false), _data(Param._data)
    /// @access    public  
    /// @return    
    /// @param     const param & Param
    ///
    /// @date      20:2:2009   14:35
    ///
    param(const param& Param) : _col(Param._col), _type(Param._type), 
      _is_changed(false), _data(Param._data) 
    { }               

    ///
    /// @qualifier const 
    /// @access    public  
    /// @return    const param_types&
    ///
    /// @date      20:2:2009   14:35
    ///
    param_types get_type() const { return _type; }

    /// @brief     set_type
    ///
    /// @qualifier 
    /// @access    public  
    /// @return    void
    /// @param     param_types & Type
    ///
    /// @date      20:2:2009   14:35
    ///
    void set_type(param_types Type) { _type = Type; }

    /// @brief     is param changed?
    ///
    /// @qualifier
    /// @access    public  
    /// @return    bool&
    ///
    /// @date      20:2:2009   14:35
    ///
    bool& is_changed() { return _is_changed; }

    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    void
    /// @param     sqlite3_stmt * stm
    ///
    /// @date      20:2:2009   14:34
    ///
    void set(sqlite3_stmt* stm)
    {
      if (!stm)
      {
        set_null();
        return;
      }

      switch(sqlite3_column_type(stm, _col)) 
      {
      case SQLITE_INTEGER:
        set(sqlite3_column_int(stm, _col));
        break;
      case SQLITE_FLOAT:
        set(sqlite3_column_double(stm, _col));
        break;
      case SQLITE_TEXT:
#ifdef _UNICODE
        set(reinterpret_cast<const wchar_t*>(
          sqlite3_column_text16(stm, _col)));
#else
        set(reinterpret_cast<const char*>(
          sqlite3_column_text(stm, _col)));
#endif // _UNICODE
        break;
      case SQLITE_BLOB:
        {
#ifdef _UNICODE
          std::string blob(reinterpret_cast<const char*>(
            sqlite3_column_blob(stm, _col)));
          std::wstring wblob = CA2W(blob.c_str());
          set(wblob);
#else
          set(reinterpret_cast<const char*>(
            sqlite3_column_blob(stm, _col)));
#endif // _UNICODE
        }
        break;
      case SQLITE_NULL:
        set_null();
        break;
      default:
        set_null();
      } 
    }

    /** gesetzt als bool */
    void set(bool dat)
    { 
      _type = e_bool;
      _data = dat ? _T("TRUE") : _T("FALSE");
    } 
    /** gesetzt als int */
    void set(int dat)
    { 
      _type = e_int;
      _data = boost::str(format(_T("%d")) % dat);
    } 
    /** gesetzt als unsigned */
    void set(unsigned dat)
    { 
      _type = e_unsigned;
      _data = boost::str(format(_T("%d")) % dat);
    } 
    /** gesetzt als long */
    void set(long dat)
    { 
      _type = e_long;
      _data = boost::str(format(_T("%ld")) % dat);
    } 
    /** gesetzt als float */
    void set(float dat)
    { 
      _type = e_float;
      _data = boost::str(format(_T("%f")) % dat);
    } 
    /** gesetzt als double */
    void set(double dat)
    { 
      _type = e_double;
      _data = boost::str(format(_T("%f")) % dat);
    } 
    /** gesetzt als char */
    void set(char dat)
    { 
      _type = e_char;
      _data = dat;
    } 
    /** gesetzt als const TCHAR* */
    void set(const TCHAR* dat)
    { 
      _type = e_string;
      _data = dat;
    } 
    /** gesetzt als const string& */
    void set(const string& dat, bool isBlob=false)
    { 
      _type = isBlob?e_blob:e_string;
      _data = dat;
    } 

#ifndef BOOST_NO_STD_LOCALE
    /** gesetzt als boost::gregorian::date */
    void set(const boost::gregorian::date& dat)
    { 
      _type = e_date_time;
      _data = detail::to_sql_string(dat);
    } 
#else
    /** gesetzt als time_t_ce */
    void set(const time_t_ce& dat)
    { 
      _type = e_date_time;
      _data = detail::to_sql_string(dat);
    } 
#endif

    /// @brief     set_null
    ///
    /// @qualifier
    /// @access    public  
    /// @return    void
    ///
    /// @date      20:2:2009   14:39
    ///
    void set_null() { _type=e_null; _data = _T(""); }

    /// @brief     set_with_old_type
    ///
    /// @qualifier
    /// @access    public  
    /// @return    void
    /// @param     const string & data
    ///
    /// @date      20:2:2009   14:40
    ///
    void set_with_old_type(const string& data)
    {
      param_types ty(get_type());
      set(data);
      set_type(ty);
    }

    /// @brief     set_with_old_type
    ///
    /// @qualifier
    /// @access    public  
    /// @return    void
    /// @param     const param & data
    ///
    /// @date      20:2:2009   14:40
    ///
    void set_with_old_type(const param& data)
    {
      param_types ty(get_type());
      set(data.str());
      set_type(ty);
    }

    /// @brief     set_with_old_type
    ///
    /// @qualifier
    /// @access    public  
    /// @return    void
    /// @param     const param * data
    ///
    /// @date      20:2:2009   14:40
    ///
    void set_with_old_type(const param* data)
    {
      if (!data)
        return;
      param_types ty(get_type());
      set(data->str());
      set_type(ty);
    }

    /// @brief     is_null
    ///
    /// @qualifier const
    /// @access    public  
    /// @return    bool
    ///
    /// @date      20:2:2009   14:33
    ///
    bool is_null() const { return _type==e_null ? true : false; }

    ///
    /// @qualifier const
    /// @access    public  
    /// @return    bool
    ///
    /// @date      20:2:2009   14:40
    ///
    bool get_bool() const
    {
      if (_type == e_bool)
        return _data == _T("TRUE") ? true : false;
      else
        return false; 
    } 


    ///
    /// @qualifier const
    /// @access    public  
    /// @return    int
    ///
    /// @date      20:2:2009   14:40
    ///
    int get_int() const
    {
      return detail::to_type<int>(_data);
    }

    ///
    /// @qualifier const
    /// @access    public  
    /// @return    unsigned
    ///
    /// @date      20:2:2009   14:42
    ///
    unsigned get_unsigned() const
    {
      return detail::to_type<unsigned>(_data);
    }

    ///
    /// @qualifier const
    /// @access    public  
    /// @return    long
    ///
    /// @date      11:3:2009   13:56
    ///
    long get_long() const
    {
      return detail::to_type<long>(_data);
    }

    ///
    /// @qualifier const
    /// @access    public  
    /// @return    float
    ///
    /// @date      11:3:2009   13:56
    ///
    float get_float() const
    {
      return detail::to_type<float>(_data);
    }

    ///
    /// @qualifier const
    /// @access    public  
    /// @return    double
    ///
    /// @date      11:3:2009   13:57
    ///
    double get_double() const
    {
      return detail::to_type<double>(_data);
    }

    ///
    /// @qualifier const
    /// @access    public  
    /// @return    char
    ///
    /// @date      11:3:2009   13:57
    ///
    TCHAR get_char() const 
    {
      if (_data.length()==0)
        return _T(' ');
      else
        return _data[0];
    }

    operator string() const
    { return _data; }

    ///
    /// @qualifier const
    /// @access    public  
    /// @return    db::string
    ///
    /// @date      18:2:2009   11:33
    ///
    string& str()
    { return _data; }

    ///
    /// @qualifier const
    /// @access    public  
    /// @return    const string&
    ///
    /// @date      18:2:2009   11:33
    ///
    const string& str() const
    { return _data; }

#ifdef _UNICODE
    const wchar_t* c_str() const
    { return _data.c_str(); }
#else
    const char* c_str() const
    { return _data.c_str(); }
#endif // _UNICODE

#ifndef BOOST_NO_STD_LOCALE
    ///
    /// @qualifier const
    /// @access    public  
    /// @return    boost::gregorian::date
    ///
    /// @date      18:2:2009   11:34
    ///
    boost::gregorian::date get_date_time() const
    {
#ifdef _UNICODE
      std::string data = CW2AEX<256>(_data.c_str());
      return boost::date_time::parse_date<
        boost::gregorian::date>(data);
#else
      return boost::date_time::parse_date<
        boost::gregorian::date>(_data);
#endif // _UNICODE
    }
#else
    ///
    /// @qualifier const
    /// @access    public  
    /// @return    db::time_t_ce
    /// @param     const time_t_ce * tmp
    ///
    /// @date      19:3:2009   10:36
    ///
    time_t_ce get_date_time(const time_t_ce* tmp = 0) const
    { return detail::from_sql_string(_data, tmp); }
#endif

#ifdef _MFC_VER
    ///
    /// @qualifier const
    /// @access    public  
    /// @return    COleDateTime
    /// @param     const COleDateTime * tmp
    ///
    /// @date      19:3:2009   10:36
    ///
    COleDateTime get_date_time(const COleDateTime* tmp = 0) const
    { return detail::from_sql_string(_data, tmp); }
#endif // _MFCVER

    /// @brief     operator=
    ///
    /// @qualifier
    /// @access    public  
    /// @return    const param&
    /// @param     const param & v
    ///
    /// @date      20:2:2009   9:24
    ///
    const param& operator=( const param &v )
    {
      _data = v._data;
      _type = v._type;
      _is_changed = v._is_changed;
      return *this; 
    }

    /// @brief     operator!
    ///
    /// @qualifier const
    /// @access    public  
    /// @return    bool
    ///
    /// @date      20:2:2009   9:27
    ///
    bool operator!() const 
    { return !this->is_null(); }

    /// @brief     operator==
    ///
    /// @qualifier const
    /// @access    public  
    /// @return    bool
    /// @param     const param & v
    ///
    /// @date      20:2:2009   9:24
    ///
    bool operator== (const param& v) const 
    { return (!(*this) && (!v) && _data == v._data); }

#ifndef BOOST_NO_STD_LOCALE
    typedef boost::signal2<void, const string&, bool&> type_signal_external_check; 
    type_signal_external_check signal_external_check;
#endif // BOOST_NO_STD_LOCALE
  };
  typedef boost::detail::allocator::partial_std_allocator_wrapper<
    boost::detail::quick_allocator<param > > alloc_param; 

  namespace detail
  {
    template<> 
    inline string to_string(const db::param& a) 
    { return a.str(); } 
  }

  /** table rows interface */
  class row
  {
  protected:
    typedef std::vector<param, alloc_param > data_type;
    data_type _data;
    unsigned _row;
    query* _query;

  public:
    typedef data_type::value_type value_type;
    typedef data_type::reference reference;
    typedef data_type::const_reference const_reference;
    typedef data_type::size_type size_type;
    typedef data_type::difference_type difference_type;
    typedef data_type::iterator iterator;
    typedef data_type::reverse_iterator reverse_iterator;
    typedef data_type::const_iterator const_iterator;

    /// @brief     row
    ///
    /// @qualifier : _query(query_), _row(row_)
    /// @access    public  
    /// @return    
    /// @param     query * query_
    /// @param     unsigned row_
    ///
    /// @date      20:2:2009   9:24
    ///
    row(query* query_, unsigned row_) : _query(query_), _row(row_)
    {
    }

    /// @brief     row
    ///
    /// @qualifier : _query(r._query), _row(r._row)
    /// @access    public  
    /// @return    
    /// @param     const row & r
    ///
    /// @date      20:2:2009   9:23
    ///
    row(const row& r) : _query(r._query), _row(r._row)
    {
      _data.reserve(r.size());
      std::copy(r.begin(), r.end(), std::back_inserter(_data));
    }

    /// @brief     ~row
    ///
    /// @qualifier
    /// @access    public  
    /// @return    
    ///
    /// @date      20:2:2009   9:23
    ///
    ~row() {}


    /// @brief     get a value by number
    ///
    /// @qualifier const
    /// @access    public  
    /// @return    const param&
    /// @param     size_type field_num
    ///
    /// @date      20:2:2009   9:23
    ///
    const param& value(size_type field_num) const
    { 
      const value_type& pValue(*(begin()+field_num));
      return pValue; 
    }

    /// @brief     get a value by number
    ///
    /// @qualifier
    /// @access    public  
    /// @return    param&
    /// @param     size_type field_num
    ///
    /// @date      20:2:2009   9:23
    ///
    param& value(size_type field_num)
    { 
      value_type& pValue(*(begin()+field_num));
      return pValue; 
    }

    /// @brief     get a value by number
    ///
    /// @qualifier const
    /// @access    public  
    /// @return    db::row::const_reference
    /// @param     size_type field_num
    ///
    /// @date      20:2:2009   9:22
    ///
    const_reference operator[] (size_type field_num) const
    { return *(begin()+field_num); }

    /// @brief     get a value by number
    ///
    /// @qualifier
    /// @access    public  
    /// @return    db::row::reference
    /// @param     size_type field_num
    ///
    /// @date      20:2:2009   9:21
    ///
    reference operator[] (size_type field_num)
    { return *(begin()+field_num); }

    /// @brief     operator[]
    ///
    /// @qualifier
    /// @access    public  
    /// @return    param*
    /// @param     const string &
    ///
    /// @date      20:2:2009   9:21
    ///
    param* operator[] (const string& /*field_title*/)
    { return (NULL); }

    /// @brief     const begin iterator
    ///
    /// @qualifier const
    /// @access    public  
    /// @return    db::row::const_iterator
    ///
    /// @date      20:2:2009   9:21
    ///
    const_iterator begin() const          { return _data.begin(); }

    /// @brief     const end iterator
    ///
    /// @qualifier const
    /// @access    public  
    /// @return    db::row::const_iterator
    ///
    /// @date      20:2:2009   9:21
    ///
    const_iterator end() const            { return _data.end(); }

    /// @brief     begin iterator
    ///
    /// @qualifier
    /// @access    public  
    /// @return    db::row::iterator
    ///
    /// @date      20:2:2009   9:21
    ///
    iterator begin()                      { return _data.begin(); }

    /// @brief     end iterator
    ///
    /// @qualifier
    /// @access    public  
    /// @return    db::row::iterator
    ///
    /// @date      20:2:2009   9:20
    ///
    iterator end()                        { return _data.end(); }

    /// @brief     begin reverse iterator
    ///
    /// @qualifier
    /// @access    public  
    /// @return    db::row::reverse_iterator
    ///
    /// @date      20:2:2009   9:20
    ///
    reverse_iterator rbegin()             { return _data.rbegin(); }

    /// @brief     end reverse iterator
    ///
    /// @qualifier
    /// @access    public  
    /// @return    db::row::reverse_iterator
    ///
    /// @date      20:2:2009   9:20
    ///
    reverse_iterator rend()               { return _data.rend(); }

    /// @brief     container size
    ///
    /// @qualifier const
    /// @access    public  
    /// @return    db::row::size_type
    ///
    /// @date      20:2:2009   9:20
    ///
    size_type size() const                { return _data.size(); }

    /// @brief     returns true if the container is empty
    ///
    /// @qualifier const
    /// @access    public  
    /// @return    bool
    ///
    /// @date      20:2:2009   9:19
    ///
    bool empty() const                    { return _data.empty(); }

    /// @brief     operator==
    ///
    /// @qualifier const 
    /// @access    public  
    /// @return    bool
    /// @param     const row & t
    ///
    /// @date      20:2:2009   9:19
    ///
    bool operator==(const row& t) const { return _data == t._data; }

    /// @brief     push a param to the container
    ///
    /// @qualifier
    /// @access    public  
    /// @return    void
    /// @param     const value_type & v
    ///
    /// @date      20:2:2009   9:18
    ///
    void push_back(const value_type& v)  { _data.push_back(v); }

    /// @brief     fill
    ///
    /// @qualifier
    /// @access    public  
    /// @return    void
    /// @param     sqlite3_stmt * stm
    ///
    /// @date      20:2:2009   9:18
    ///
    inline void fill(sqlite3_stmt* stm);

  };
  typedef boost::detail::allocator::partial_std_allocator_wrapper<
    boost::detail::quick_allocator<row > > alloc_row; 

  /** cursor interface */
  class query
  {
  protected:
    typedef row _value_type;
    typedef std::vector<_value_type, alloc_row > data_type;
    typedef field _fvalue_type;
    typedef std::vector<_fvalue_type, alloc_field > field_type;

    data_type _data;
    field_type _field;
    base& _base;
    sqlite3_stmt* _stm; 

  public:
    typedef _fvalue_type fvalue_type;
    typedef _value_type value_type;
    typedef data_type::reference reference;
    typedef data_type::const_reference const_reference;
    typedef data_type::size_type size_type;
    typedef data_type::difference_type difference_type;
    typedef data_type::iterator iterator;
    typedef data_type::const_iterator const_iterator;
    typedef field_type::iterator fiterator;
    typedef field_type::const_iterator const_fiterator;

    query(base& base_) : _base(base_), _stm(0)
    {

    }

    query(const query& q) : _base(q._base), _stm(q._stm)
    {
      _data.reserve(q._data.size());
      std::copy(q.begin(), q.end(), std::back_inserter(_data));

      _field.reserve(q._field.size());
      std::copy(q.fbegin(), q.fend(), std::back_inserter(_field));
    }

    ~query() {}

    inline void execute(const string& cmd);

    /** get a row by number */
    const row& getRow(size_type row_num) const 
    { 
      const value_type& pValue(*(begin()+row_num));
      return pValue; 
    }
    row& getRow(size_type row_num) 
    { 
      value_type& pValue(*(begin()+row_num));
      return pValue; 
    }

    /** get a row by number */
    const_reference operator[] (size_type row_num) const 
    { return *(begin()+row_num); }
    reference operator[] (size_type row_num) 
    { return *(begin()+row_num); }

    /** get id from field title */
    int getIdOf(const string& sField) const  
    { 
      field_type::const_iterator iTB(_field.begin());
      field_type::const_iterator iTE(_field.end());
      for (iTB;iTB!=iTE;++iTB)
        if (iTB->name()==sField)
          return (int)(iTB-_field.begin());
      return -1;
    }
    string getTitleOf(unsigned iField) const  
    { 
      if (iField<_field.size())
        return _field[iField].name();
      else
        return _T("");
    }
    const field* getFieldInfo(unsigned iField) const  
    { 
      if (iField<_field.size())
        return &_field[iField];
      else
        return NULL;
    }

    /** begin iterator */
    const_iterator begin() const { return _data.begin(); }
    /** end iterator */
    const_iterator end() const   { return _data.end(); }
    /** begin iterator */
    iterator begin()             { return _data.begin(); }
    /** end iterator */
    iterator end()               { return _data.end(); }
    /** begin iterator */
    const_fiterator fbegin() const { return _field.begin(); }
    /** end iterator */
    const_fiterator fend() const   { return _field.end(); }
    /** begin iterator */
    fiterator fbegin()             { return _field.begin(); }
    /** end iterator */
    fiterator fend()               { return _field.end(); }
    /** container size */
    size_type size() const         { return _data.size(); }
    /** is container empty? */
    bool empty() const             { return _data.empty(); }
    bool f_empty() const           { return _field.empty(); }

    bool operator==(const query& t) const 
    { return  _data == t._data; }

    /** push a row to the row container */
    void push_back(const value_type& v)  { _data.push_back(v); }
    /** push a field title to the field container */
    void push_back(const fvalue_type& v)  { _field.push_back(v); }
  };

  typedef boost::shared_ptr<query> query_ptr;

  /** helper class, generates SELECT SQL statements */
  class sel 
  {
  protected:
    bool _distinct;
    int _limit;
    int _offset;
    split _results;
    split _sources;
    split _groupBy;
    split _orderBy;

    string _where; 
    string _having;
    string _delim1;
    string _delim2;
    field_pair _join;

  public:
    /// @brief     sel
    ///
    /// @qualifier : _distinct(false), _limit(0), _offset(0), _where(_T("True")), 
    ///              _delim1(_T("")), _delim2(_T(""))
    /// @access    public  
    /// @return    
    ///
    /// @date      20:2:2009   14:03
    ///
    sel() : _distinct(false), _limit(0), _offset(0), 
      _where(_T("True")), _delim1(_T("")), _delim2(_T(""))
    {}

    /// @brief     sel
    ///
    /// @qualifier : _distinct(false), _limit(0), _offset(0), _where(_T("True")), 
    ///              _delim1(_T("")), _delim2(_T(""))
    /// @access    public  
    /// @return    
    /// @param     const string & tablename
    ///
    /// @date      20:2:2009   14:03
    ///
    sel(const string& tablename) : _distinct(false), _limit(0), _offset(0), 
      _where(_T("True")), _delim1(_T("")), _delim2(_T(""))
    { source(tablename); }

    /// @brief     sel
    ///
    /// @qualifier : _distinct(false), _limit(0), _offset(0), _where(_T("True")), 
    ///              _delim1(delim), _delim2(delim)
    /// @access    public  
    /// @return    
    /// @param     const string & tablename
    /// @param     const string & delim
    ///
    /// @date      20:2:2009   14:03
    ///
    sel(const string& tablename, const string& delim) : _distinct(false), 
      _limit(0), _offset(0), _where(_T("True")), _delim1(delim), 
      _delim2(delim)
    { source(tablename); }

    /// @brief     sel
    ///
    /// @qualifier : _distinct(false), _limit(0), _offset(0), _where(_T("True")), 
    ///              _delim1(delim1), _delim2(delim2)
    /// @access    public  
    /// @return    
    /// @param     const string & tablename
    /// @param     const string & delim1
    /// @param     const string & delim2
    ///
    /// @date      20:2:2009   14:03
    ///
    sel(const string& tablename, const string& delim1, 
      const string& delim2) : _distinct(false), 
      _limit(0), _offset(0), _where(_T("True")), _delim1(delim1), 
      _delim2(delim2)
    { source(tablename); }

    /// @brief     sel
    ///
    /// @qualifier : _distinct(s._distinct), _limit(s._limit), _offset(s._offset), _where(s._where), _having(s._having), _delim1(s._delim1), _delim2(s._delim2), _results(s._results), _sources(s._sources), _groupBy(s._groupBy), _orderBy(s._orderBy)
    /// @access    public  
    /// @return    
    /// @param     const sel & s
    ///
    ///
    sel(const sel& s) : _distinct(s._distinct), _limit(s._limit),
      _offset(s._offset), _where(s._where), _having(s._having),
      _delim1(s._delim1), _delim2(s._delim2), _results(s._results),
      _sources(s._sources), _groupBy(s._groupBy), _orderBy(s._orderBy),
      _join(s._join)
    {
    }

    /// @brief     ~sel
    ///
    /// @qualifier
    /// @access    virtualpublic  
    /// @return    
    ///
    /// @date      20:2:2009   14:03
    ///
    virtual ~sel() {}

    /// @brief     operator=
    ///
    /// @qualifier
    /// @access    public  
    /// @return    sel &
    /// @param     const sel & s
    ///
    ///
    sel & operator=(const sel& s)
    { 
      _distinct = s._distinct;
      _limit = s._limit;
      _offset = s._offset;
      _results = s._results;
      _sources = s._sources;
      _groupBy = s._groupBy;
      _orderBy = s._orderBy;

      _where = s._where; 
      _having = s._having;
      _delim1 = s._delim1;
      _delim2 = s._delim2;
      _join = s._join;

      return *this; 
    }

		/// @brief     ,
    ///
    /// @qualifier
    /// @access    public  
    /// @return    sel &
    /// @param     const string & s
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & operator,(const string& s)
    { return this->result(s); }

    /// @brief     operator%
    ///
    /// @qualifier
    /// @access    public  
    /// @return    sel &
    /// @param     const expr::base & e
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & operator%(const expr::base& e)
    { return this->where(e.str()); }

    /// @brief     operator<
    ///
    /// @qualifier
    /// @access    public  
    /// @return    sel &
    /// @param     const field_pair & f
    ///
    /// @date      6:8:2009   10:58
    ///
    virtual sel & operator<(const field_pair& f)
    { return this->join(f); }

    /// @brief     distinct
    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    sel &
    /// @param     bool d
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & distinct(bool d) 
    { 
      _distinct = d; 
      return *this;
    }

    /// @brief     limit
    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    sel &
    /// @param     int value
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & limit(int value)
    { 
      _limit = value;	
      return *this;
    }

    /// @brief     join
    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    sel &
    /// @param     const field_pair& f
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & join(const field_pair& f)
    { 
      _join = f;	
      return *this;
    }

    /// @brief     offset
    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    sel &
    /// @param     int value
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & offset(int value)
    { 
      _offset = value; 
      return *this;
    }

    /// @brief     result_no_delim
    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    sel &
    /// @param     string r
    /// @param     string alias
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & result_no_delim(string r, string alias=_T(""))
    { 
      if (alias.length())
        r += _T(" AS ") + alias;
      _results.push_back(r); 
      return *this;
    }

    /// @brief     result
    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    sel &
    /// @param     string r
    /// @param     string alias
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & result(string r, string alias=_T(""))
    { 
      if (alias.length())
        r += _T(" AS ") + alias;
      if (r!=_T("*")) 
        detail::front_back_delim(r, _delim1, _delim2);
      _results.push_back(r); 
      return *this;
    }

    /// @brief     result_max
    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    sel &
    /// @param     string r
    /// @param     string alias
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & result_max(string r, string alias=_T(""))
    { 
      r = _T("MAX(")+r;
      r += _T(")");
      if (r!=_T("*")) 
        detail::front_back_delim(r, _delim1, _delim2);
      if (alias.length())
      {
        detail::front_back_delim(alias, _delim1, _delim2);
        r += _T(" AS ") + alias;
      }
      _results.push_back(r); 
      return *this;
    }

    /// @brief     clear
    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    sel &
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & clear()
    {
      _results.clear();
      _sources.clear();
      return *this;
    }

    /// @brief     source
    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    sel &
    /// @param     string s
    /// @param     string alias
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & source(string s, string alias=_T(""))
    {
      detail::front_back_delim(s, _delim1, _delim2);
      if (alias.length())
      {
        detail::front_back_delim(alias, _delim1, _delim2);
        s += _T(" AS ") + alias;
      }
      _sources.push_back(s);
      return *this;
    }

    /// @brief     where
    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    sel &
    /// @param     const expr::base & w
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & where(const expr::base & w)
    { 
      _where = expr::and(expr::raw(_where), w).str();	
      return *this;
    }

    /// @brief     where
    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    sel &
    /// @param     string w
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & where(string w)
    { 
      _where = expr::and(expr::raw(_where), expr::raw(w)).str();
      return *this;
    }

    /// @brief     group_by
    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    sel &
    /// @param     string gb
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & group_by(string gb)
    { 
      detail::front_back_delim(gb, _delim1, _delim2);
      _groupBy.push_back(gb);	
      return *this;
    }

    /// @brief     having
    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    sel &
    /// @param     const expr::base & h
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & having(const expr::base & h)
    { 
      _having = h.str(); 
      return *this;
    }

    /// @brief     having
    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    sel &
    /// @param     string h
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & having(string h)
    { 
      _having = h;
      return *this;
    }

    /// @brief     order_by
    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    sel &
    /// @param     string ob
    /// @param     bool ascending
    ///
    /// @date      20:2:2009   14:00
    ///
    virtual sel & order_by(string ob, bool ascending=true)
    { 
      string value = ob;
      detail::front_back_delim(value, _delim1, _delim2);
      if (!ascending)
        value += _T(" DESC");
      _orderBy.push_back(value); 
      return *this;
    }

    /// @brief     operator string
    ///
    /// @qualifier const
    /// @access    virtual public  
    /// @return    
    ///
    /// @date      20:2:2009   14:00
    ///
    virtual operator string() const
    {
      ostringstream res;
      res << _T("SELECT ");
      if (_distinct)
        res << _T("DISTINCT ");
      res << _results.join(_T(","));
      res << _T(" FROM ");
      res << _sources.join(_T(","));
      
      if (_join.first.table().length()!=0 && 
          _join.second.table().length()!=0 && 
          _sources.size()==1)
        res << _T(" INNER JOIN ") << _join.first.table() << _T(" ON ") << 
          _join.first.fullName() << _T(" = ") << _join.second.fullName() << 
          _T(" ");
      
      if (_where != _T("True"))
        res << _T(" WHERE ") << _where;
      if (_groupBy.size() > 0)
        res << _T(" GROUP BY ") << _groupBy.join(_T(","));
      if (_having.length())
        res << _T(" HAVING ") << _having;
      if (_orderBy.size() > 0)
        res << _T(" ORDER BY ") << _orderBy.join(_T(","));
      if (_limit) 
        res << _T(" LIMIT ") << string(detail::to_string(_limit));
      if (_offset) 
        res << _T(" OFFSET ") << string(detail::to_string(_offset));

      string ret(res.str());
      detail::replace_all(ret, _delim1+_delim1, _delim1);
      if (_delim1!=_delim2)
        detail::replace_all(ret, _delim2+_delim2, _delim2);
      return ret;
    }

    /// @brief     str
    ///
    /// @qualifier const
    /// @access    virtual public  
    /// @return    db::string
    ///
    /// @date      20:2:2009   14:12
    ///
    virtual string str() const { return this->operator string(); }
  }; 

  /** helper class, generates DELETE SQL statements */
  class del : public sel
  {
  public:
    del(const string& tablename) : sel(tablename) { }

    /// @brief     operator string
    ///
    /// @qualifier const
    /// @access    virtual public  
    /// @return    
    ///
    /// @date      20:2:2009   14:12
    ///
    virtual operator string() const
    {
      string res = _T("DELETE FROM ");
      res += _sources.join(_T(","));
      if (_where != _T("True"))
        res += _T(" WHERE ") + _where;
      if (_groupBy.size() > 0)
        res += _T(" GROUP BY ") + _groupBy.join(_T(","));
      if (_having.length())
        res += _T(" HAVING ") + _having;
      if (_orderBy.size() > 0)
        res += _T(" ORDER BY ") + _orderBy.join(_T(","));
      if (_limit) 
        res += _T(" LIMIT ") + string(detail::to_string(_limit));
      if (_offset) 
        res += _T(" OFFSET ") + string(detail::to_string(_offset));

      detail::replace_all(res, _delim1+_delim1, _delim1);
      if (_delim1!=_delim2)
        detail::replace_all(res, _delim2+_delim2, _delim2);
      return res;
    }

    /// @brief     ~del
    ///
    /// @qualifier
    /// @access    virtual public  
    /// @return    
    ///
    /// @date      20:2:2009   14:12
    ///
    virtual ~del() {}
  };

  /** helper class, generates INSERT SQL statements */
  class ins 
  {
  protected:
    split_map _values;
    string    _source;
    string    _delim1;
    string    _delim2;

  public:
    /// @brief     ins
    ///
    /// @qualifier : _source(tab)
    /// @access    public  
    /// @return    
    /// @param     const string & tab
    ///
    /// @date      20:2:2009   9:07
    ///
    ins(const string& tab) : _source(tab) 
    {}

    /// @brief     ins
    ///
    /// @qualifier : _source(tab), _delim1(delim), _delim2(delim)
    /// @access    public  
    /// @return    
    /// @param     const string & tab
    /// @param     const string & delim
    ///
    /// @date      20:2:2009   9:07
    ///
    ins(const string& tab, const string& delim) : _source(tab), 
      _delim1(delim), _delim2(delim) 
    {}

    /// @brief     ins
    ///
    /// @qualifier : _source(tab), _delim1(delim1), _delim2(delim2)
    /// @access    public  
    /// @return    
    /// @param     const string & tab
    /// @param     const string & delim1
    /// @param     const string & delim2
    ///
    /// @date      20:2:2009   9:07
    ///
    ins(const string& tab, const string& delim1, const string& delim2) : 
      _source(tab), _delim1(delim1), _delim2(delim2) 
    {}
    
    /// @brief     ~ins
    ///
    /// @qualifier
    /// @access    public  
    /// @return    
    ///
    /// @date      20:2:2009   9:07
    ///
    ~ins() {}
    
    /// @brief     clear_values
    ///
    /// @qualifier
    /// @access    public  
    /// @return    ins &
    ///
    /// @date      20:2:2009   9:07
    ///
    ins & clear_values()
    {
      _values.clear();
      return *this;
    }

    /// @brief     operator%
    ///
    /// @qualifier
    /// @access    public  
    /// @return    ins &
    /// @param     const db::field & f
    ///
    /// @date      20:2:2009   9:07
    ///
    ins & operator%(const db::field& f)
    { 
      if (f.values().size()==1)
        return values(f.name(), f.values().begin()->second, f.type());
      else
        return *this;
    }

    /// @brief     values
    ///
    /// @qualifier
    /// @access    public  
    /// @return    ins &
    /// @param     string t
    /// @param     param * v
    ///
    /// @date      20:2:2009   9:07
    ///
    ins & values(string t, param* v)
    {
      string sV(v->str());
      if (sV.length()==0)
        sV = _T("NULL");
      else
        switch(v->get_type()) 
      {
        case e_string:
        case e_date_time:
          sV = _T("'") + sV;
          sV += _T("'");
          break;
      }
      detail::front_back_delim(t, _delim1, _delim2);
      _values.insert(std::pair<string, string>(t, sV));
      return *this;
    }

    /// @brief     values
    ///
    /// @qualifier
    /// @access    public  
    /// @return    ins &
    /// @param     string t
    /// @param     string v
    /// @param     param_types p
    ///
    /// @date      20:2:2009   9:07
    ///
    ins & values(string t, string v, param_types p=e_int)
    {
      switch(p) 
      {
      case e_char:
      case e_blob:
      case e_string:
      case e_date_time:
        detail::erase_all<string>(v, _T("'"));
        detail::erase_all<string>(v, _T("`"));
        detail::erase_all<string>(v, _T("´"));
        v = _T("'") + v;
        v += _T("'");
        break;
      }
      detail::front_back_delim(t, _delim1, _delim2);
      _values.insert(std::pair<string, string>(t, v));
      return *this;
    }

    /// @brief     values
    ///
    /// @qualifier
    /// @access    public  
    /// @return    ins &
    /// @param     const string & t
    /// @param     const T & v
    ///
    /// @date      20:2:2009   11:07
    ///
    template<typename T>
    ins & values(const string& t, const T& v)
    { return values(t, detail::to_string(v), tp(v)); }

    /// @brief     operator string
    ///
    /// @qualifier const
    /// @access    public  
    /// @return    
    ///
    /// @date      20:2:2009   11:04
    ///
    operator string() const
    { return str(); }

    /// @brief     str
    ///
    /// @qualifier const
    /// @access    public  
    /// @return    db::string
    ///
    /// @date      20:2:2009   11:05
    ///
    string str() const 
    {       
      string res = _T("INSERT INTO ");
      res += _source;

      if (_values.size() > 0)
      {
        res += _T(" (");
        res += _values.join_fields(_T(","));
        res += _T(") VALUES (");
        res += _values.join_values(_T(","));
        res += _T(")");
      }
      return res;
    }
  }; 

  /** hilft beim anlegen von UPDATE-SQL Ausdrücken. */
  class upd 
  {
    string _delim1;
    string _delim2;
    string table;
    string _where;
    split fields;
    split values;
  public:
    /// @brief     ~upd
    ///
    /// @qualifier
    /// @access    public  
    /// @return    
    ///
    /// @date      20:2:2009   11:14
    ///
    ~upd() 
    { }

    /// @brief     upd
    ///
    /// @qualifier : table(t), _where(_T("True"))
    /// @access    public  
    /// @return    
    /// @param     const string & t
    ///
    /// @date      20:2:2009   11:15
    ///
    upd(const string& t) : table(t), _where(_T("True")) 
    { }

    /// @brief     upd
    ///
    /// @qualifier : table(t), _where(_T("True")), _delim1(d), _delim2(d)
    /// @access    public  
    /// @return    
    /// @param     const string & t
    /// @param     const string & d
    ///
    /// @date      20:2:2009   11:15
    ///
    upd(const string& t, const string& d) : table(t), 
      _where(_T("True")), _delim1(d), _delim2(d) 
    { }

    /// @brief     upd
    ///
    /// @qualifier : table(t), _where(_T("True")), _delim1(d1), _delim2(d2)
    /// @access    public  
    /// @return    
    /// @param     const string & t
    /// @param     const string & d1
    /// @param     const string & d2
    ///
    /// @date      20:2:2009   11:15
    ///
    upd(const string& t, const string& d1, const string& d2) : table(t), 
      _where(_T("True")), _delim1(d1), _delim2(d2) 
    { }

    /// @brief     where
    ///
    /// @qualifier
    /// @access    public  
    /// @return    upd&
    /// @param     const expr::base & e
    ///
    /// @date      20:2:2009   11:14
    ///
    upd& where(const expr::base& e)
    {
      _where = expr::and(expr::raw(_where), e).str();
      return *this;
    }

    /// @brief     operator%
    ///
    /// @qualifier
    /// @access    public  
    /// @return    upd &
    /// @param     const expr::base & e
    ///
    /// @date      20:2:2009   11:14
    ///
    upd & operator%(const expr::base& e)
    { return where(e); }

    /// @brief     operator%
    ///
    /// @qualifier
    /// @access    public  
    /// @return    upd &
    /// @param     const db::field & f
    ///
    /// @date      20:2:2009   11:14
    ///
    upd & operator%(const db::field& f)
    { 
      if (f.values().size()==1)
        return set(f, f.values().begin()->second);
      else
        return *this;
    }

    /// @brief     set
    ///
    /// @qualifier
    /// @access    public  
    /// @return    upd&
    /// @param     const field & f
    /// @param     const string & val
    ///
    /// @date      20:2:2009   11:14
    ///
    upd& set(const field& f, const string& val)
    {
      fields.push_back(f.name());
      if (f.type()==e_string||f.type()==e_char||
          f.type()==e_blob||f.type()==e_date_time)
        values.push_back(detail::escape_sql(val));
      else
        values.push_back(val);
      return *this;
    }

    /// @brief     set
    ///
    /// @qualifier
    /// @access    public  
    /// @return    upd&
    /// @param     const field & f
    /// @param     const T & val
    ///
    /// @date      20:2:2009   11:14
    ///
    template<typename T>
    upd& set(const field& f, const T& val)
    { return set(f, detail::to_string(val)); }

    /// @brief     operator string
    ///
    /// @qualifier const
    /// @access    public  
    /// @return    
    ///
    /// @date      20:2:2009   11:15
    ///
    operator string() const
    { return str(); }

    /// @brief     str
    ///
    /// @qualifier const
    /// @access    public  
    /// @return    db::string
    ///
    /// @date      20:2:2009   11:16
    ///
    string str() const 
    {
      string q = _T("UPDATE ") + table + _T(" SET ");
      split sets;
      for (size_t i = 0; i < fields.size(); i++)
        sets.push_back(fields[i] + _T("=") + values[i]);
      q += sets.join(_T(","));
      if (_where.length())
        q += _T(" WHERE ") + _where;
      return q;
    }
  };

  /** datenbank interface */
  class base
  {
    sqlite3* _db;

    class progress_handler
    {
      /// @brief     xProgressCallback
      ///
      /// @qualifier
      /// @access    private static  
      /// @return    int
      /// @param     void * db_
      ///
      /// @date      20:2:2009   14:13
      ///
      static int xProgressCallback(void* db_)
      {
        if (!db_)
          return 1;
        base* base_(reinterpret_cast<base*> (db_));

        if (!base_)
          return 1;

        //if (!base_->Sig_Progress().connected())
        //  return 1;

        //if (base_->Sig_Progress()(0,0))
        //  return 0;
        //else

        return 0;
      } 

      sqlite3* _db;
    public:
      progress_handler(sqlite3* db_, base* base_) : _db(db_)
      {
        sqlite3_progress_handler(_db, 4, 
          progress_handler::xProgressCallback, base_);
      }
    	~progress_handler()
      {
        sqlite3_progress_handler(_db, 0, NULL, NULL); 
      }
    };
    
    typedef std::map<string, query> type_last_queries; 
    type_last_queries _last_queries; 

  public:
    /// @brief     base
    ///
    /// @qualifier : _db(NULL)
    /// @access    public  
    /// @return    
    ///
    /// @date      20:2:2009   14:14
    ///
    base() : _db(NULL)
    {}

    /// @brief     ~base
    ///
    /// @qualifier
    /// @access    public  
    /// @return    
    ///
    /// @date      20:2:2009   14:14
    ///
    ~base()
    {
      if (_db) 
        sqlite3_close(_db);
    }

    enum feature
    {
      e_transact,
      e_blob
    };

    /// @brief     throw_error
    ///
    /// @qualifier
    /// @access    public  
    /// @return    void
    /// @param     int status
    ///
    /// @date      20:2:2009   14:13
    ///
    inline void throw_error(int status) 
    {
      string err(boost::str(
        format(_T("%d=status code : %s")) % 
          status % sqlite3_errmsg(_db)));
      switch(status) 
      {
      case SQLITE_ERROR: throw exception::sql_error(err);
      case SQLITE_INTERNAL: throw exception::internal_error(err);
      case SQLITE_NOMEM: throw exception::memory_error(err);
      case SQLITE_FULL: throw exception::insertion_error(err);
      default: throw exception::unknown_error(_T("compile failed: ") + err);
      }
    } 

    /// @brief     open the specified db
    ///
    /// @qualifier
    /// @access    public  
    /// @return    void
    /// @param     const std::string & file
    ///
    /// @date      20:2:2009   14:14
    ///
    void connect(const std::string& file)
    {
      if (_db) 
        sqlite3_close(_db);
      int rc = sqlite3_open(file.c_str(), &_db); 
      if (rc != SQLITE_OK)
        throw_error(rc);
      rc = sqlite3_errcode(_db);
      if (rc != SQLITE_OK)
        throw_error(rc);
    }

    /// @brief     begins SQL transaction
    ///
    /// @qualifier 
    /// @access    public  
    /// @return    void
    ///
    /// @date      20:2:2009   14:14
    ///
    void begin() 
    {
      int rc(sqlite3_exec(_db, "BEGIN;", NULL, NULL, NULL)); 
      if (rc != SQLITE_OK)
        throw_error(rc);
    }

    /// @brief     commits SQL transaction
    ///
    /// @qualifier 
    /// @access    public  
    /// @return    void
    ///
    /// @date      20:2:2009   14:15
    ///
    void commit() 
    {
      int rc(sqlite3_exec(_db, "COMMIT;", NULL, NULL, NULL)); 
      if (rc != SQLITE_OK)
        throw_error(rc);
    }

    /// @brief     cancels active SQL transaction
    ///
    /// @qualifier 
    /// @access    public  
    /// @return    void
    ///
    /// @date      20:2:2009   14:15
    ///
    void rollback() 
    {
      int rc(sqlite3_exec(_db, "ROLLBACK;", NULL, NULL, NULL)); 
      if (rc != SQLITE_OK)
        throw_error(rc);
    }

    /// @brief     execute SQL statement
    ///
    /// @qualifier
    /// @access    public  
    /// @return    query&
    /// @param     const string & cmd
    ///
    /// @date      20:2:2009   14:15
    ///
    inline query& execute(const string& cmd);

    /// @brief     execute
    ///
    /// @qualifier
    /// @access    public  
    /// @return    db::query_ptr
    /// @param     const string & cmd
    ///
    /// @date      13:7:2009   13:14
    ///
    inline query_ptr execute_ptr(const string& cmd);

    /// @brief     get_db_ptr
    ///
    /// @qualifier
    /// @access    public  
    /// @return    sqlite3*
    ///
    /// @date      20:2:2009   14:16
    ///
    sqlite3* get_db_ptr() {return _db;}

#ifndef BOOST_NO_STD_LOCALE
    typedef boost::signal2<bool, unsigned, unsigned> type_signal_progress; 

    /// @brief     Sig_Progress
    ///
    /// @qualifier
    /// @access    public  
    /// @return    type_signal_progress&
    ///
    /// @date      20:2:2009   14:16
    ///
    type_signal_progress& Sig_Progress() { return sig_progress; }

#endif // BOOST_NO_STD_LOCALE
  protected:
#ifndef BOOST_NO_STD_LOCALE
    type_signal_progress sig_progress;
#endif // BOOST_NO_STD_LOCALE
  };
  typedef boost::shared_ptr<base> base_ptr;

#ifdef _MFC_VER

  namespace serialize
  {
    class field : public CObject
    {
      string _name;
      int    _type;

    public:
      /// @brief     field
      ///
      /// <BR>qualifier : _type(db::e_null)
      /// <BR>access    public  
      /// @return    
      ///
      ///
      field() : CObject(), _type(db::e_null) {}
      
      /// @brief     field
      ///
      /// <BR>qualifier : _type(type_), _name(name_)
      /// <BR>access    public  
      /// @return    
      /// @param     param_types type_
      /// @param     const string & name_
      ///
      /// @date      14:9:2009   10:00
      ///
      field(param_types type_, const string& name_) : CObject(),
        _type(type_), _name(name_) 
      {}

      /// @brief     field
      ///
      /// <BR>qualifier : CObject(), _type(f._type), _name(f._name)
      /// <BR>access    public  
      /// @return    
      /// @param     const field & f
      ///
      /// @date      28:9:2009   15:17
      ///
      field(const field& f) : CObject(), _type(f._type), _name(f._name) 
      {}

      /// @brief     operator=
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    field&
      /// @param     const field & f
      ///
      /// @date      28:9:2009   15:21
      ///
      field& operator=(const field& f)
      {
        _type = f._type;
        _name = f._name;
        return *this;
      }

      const db::string& name() const { return _name; }
      void name(const db::string& val) { _name = val; }

      param_types Type() const { return static_cast<param_types>(_type); }
      void Type(param_types val) { _type = val; }

      /// @brief     Serialize
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    void
      /// @param     CArchive & ar
      ///
      /// @date      14:9:2009   10:00
      ///
      virtual void Serialize(CArchive& ar)
      {
        CObject::Serialize(ar);

        if( ar.IsStoring() )
        {
          ar.WriteCount(_type);
          ar << CString(_name.c_str());
#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(_type).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(_name.c_str());
          OutputDebugStringW(L"\n");
#endif // _DEBUG
#endif        
        }
        else
        {
          _type = ar.ReadCount();
          CString tmp;
          ar >> tmp;
          _name = tmp;

#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(_type).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(_name.c_str());
          OutputDebugStringW(L"\n");
#endif // _DEBUG
#endif        
        }
      }

      DECLARE_SERIAL(field);
    };

    typedef std::vector<db::serialize::field> t_ser_field;

    class row : public CObject
    {
      string_vec _data;
      
    public:
      /// @brief     row
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    
      ///
      /// @date      14:9:2009   10:15
      ///
      row() : CObject() 
      {}

      /// @brief     row
      ///
      /// <BR>qualifier : CObject(), _data(r._data)
      /// <BR>access    public  
      /// @return    
      /// @param     const row & r
      ///
      /// @date      28:9:2009   15:18
      ///
      row(const row& r) : CObject(), _data(r._data)
      {}

      /// @brief     ~row
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    
      ///
      /// @date      14:9:2009   10:15
      ///
      ~row() {}

      /// @brief     operator=
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    row&
      /// @param     const row & r
      ///
      /// @date      28:9:2009   15:22
      ///
      row& operator=(const row& r)
      {
        _data = r._data;
        return *this;
      }

      db::string_vec& Data() { return _data; }
      const db::string_vec& Data() const { return _data; }

      /// @brief     Serialize
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    void
      /// @param     CArchive & ar
      ///
      /// @date      14:9:2009   10:15
      ///
      virtual void Serialize(CArchive& ar)
      {
        CObject::Serialize(ar);

        if( ar.IsStoring() )
        {
          ar.WriteCount(_data.size());
#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(_data.size()).c_str());
          OutputDebugStringW(L"\n");
#endif
#endif          
          for (string_vec::iterator itb(_data.begin()), 
            ite(_data.end()); itb!=ite; ++itb)
          {
            ar << CString(itb->c_str());
#ifdef _BOOST_TEST
#ifdef _DEBUG
            OutputDebugStringW(itb->c_str());
            OutputDebugStringW(L"\n");
#endif // _DEBUG
#endif          
          }
        }
        else
        {
          DWORD_PTR cnts(ar.ReadCount());
#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(cnts).c_str());
          OutputDebugStringW(L"\n");
#endif
#endif          
          if (cnts>0)
          {
            _data.resize(cnts);
            
            for (DWORD_PTR cnt(0); cnt<cnts; ++cnt)
            {
              CString tmp;
              ar >> tmp;
              _data[cnt] = tmp;
#ifdef _BOOST_TEST
#ifdef _DEBUG
              OutputDebugStringW(tmp);
              OutputDebugStringW(L"\n");
#endif // _DEBUG
#endif            
            }
          }
        }
      }
      
      DECLARE_SERIAL(row);
    };

    typedef std::vector<db::serialize::row> t_ser_row;


    class table : public CObject
    {
      string _name;
      t_ser_field  _fields;
      t_ser_row _rows;

    public:
      /// @brief     table
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    
      ///
      /// @date      14:9:2009   9:46
      ///
      table() : CObject() {}

      /// @brief     table
      ///
      /// <BR>qualifier : CObject(), _name(t._name), _fields(t._fields), _rows(t._rows)
      /// <BR>access    public  
      /// @return    
      /// @param     const table & t
      ///
      /// @date      28:9:2009   15:19
      ///
      table(const table& t) : CObject(), _name(t._name), _fields(t._fields),
        _rows(t._rows)
      {}

      /// @brief     table
      ///
      /// <BR>qualifier : _name(name_)
      /// <BR>access    public  
      /// @return    
      /// @param     const db::string & name_
      ///
      /// @date      14:9:2009   9:46
      ///
      table(const db::string& name_) : CObject(), _name(name_) 
      {}

      /// @brief     operator=
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    table&
      /// @param     const table & r
      ///
      /// @date      28:9:2009   15:23
      ///
      table& operator=(const table& t)
      {
        _name   = t._name; 
        _fields = t._fields;
        _rows   = t._rows;

        return *this;
      }

      const db::string& name() const { return _name; }
      void name(const db::string& val) { _name = val; }

      db::serialize::t_ser_field& fields() { return _fields; }
      const db::serialize::t_ser_field& fields() const { return _fields; }

      db::serialize::t_ser_row& rows() { return _rows; }
      const db::serialize::t_ser_row& rows() const { return _rows; }

      /// @brief     Serialize
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    void
      /// @param     CArchive & ar
      ///
      /// @date      14:9:2009   9:46
      ///
      virtual void Serialize(CArchive& ar)
      {
        CObject::Serialize(ar);

        if( ar.IsStoring() )
        {
          ar.WriteCount(_fields.size());
          ar.WriteCount(_rows.size());
          ar << CString(_name.c_str());

#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(_fields.size()).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(db::detail::to_string(_rows.size()).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(_name.c_str());
          OutputDebugStringW(L"\n");
#endif
#endif
          for (t_ser_field::iterator itb(_fields.begin()), 
            ite(_fields.end()); itb!=ite; ++itb)
            itb->Serialize(ar);

          for (t_ser_row::iterator itb(_rows.begin()), 
            ite(_rows.end()); itb!=ite; ++itb)
            itb->Serialize(ar);
        }
        else
        {
          DWORD_PTR cnt_fields(ar.ReadCount());
          DWORD_PTR cnt_rows(ar.ReadCount());
          
          CString name; ar >> name;
          _name = name;

#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(cnt_fields).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(db::detail::to_string(cnt_rows).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(_name.c_str());
          OutputDebugStringW(L"\n");
#endif
#endif          
          _fields.reserve(cnt_fields);
          for (t_ser_field::size_type cnt(0); cnt<cnt_fields; ++cnt)
          {
            _fields.push_back(db::serialize::field());
            _fields.back().Serialize(ar);
          }

          _rows.reserve(cnt_rows);
          for (t_ser_row::size_type cnt(0); cnt<cnt_rows; ++cnt)
          {
            _rows.push_back(db::serialize::row());
            _rows.back().Serialize(ar);
          }
        }
      }

      typedef std::vector<db::serialize::table> t_ser_table;

      DECLARE_SERIAL(table);
    };

    typedef std::vector<table > t_ser_table;
  }

#endif // _MFC_VER

} // namespace db

/** overload from the global && operator */
inline db::expr::and operator&&(const db::expr::base& o1, 
                                const db::expr::base& o2)
{ return db::expr::and(o1,o2); }
/** overload from the global || operator */
inline db::expr::or operator||(const db::expr::base& o1, 
                               const db::expr::base& o2)
{ return db::expr::or(o1,o2); }

/// @brief     overload from the global == operator
///
/// @qualifier
/// @access    public  
/// @return    db::expr::eq
/// @param     const db::field & fld
/// @param     const T & f2
///
/// @date      20:2:2009   14:17
///
template<typename T>
inline db::expr::eq operator==(const db::field& fld, const T& f2)
{ return db::expr::eq(fld, f2); }

/// @brief     overload from the global > operator
///
/// @qualifier
/// @access    public  
/// @return    db::expr::gt
/// @param     const db::field & fld
/// @param     const T & o2
///
/// @date      20:2:2009   14:17
///
template<typename T>
inline db::expr::gt operator>(const db::field& fld, const T& o2)
{ return db::expr::gt(fld, o2); }

/// @brief     overload from the global >= operator
///
/// @qualifier
/// @access    public  
/// @return    db::expr::gt_eq
/// @param     const db::field & fld
/// @param     const T & o2
///
/// @date      20:2:2009   14:17
///
template<typename T>
inline db::expr::gt_eq operator>=(const db::field& fld, const T& o2)
{ return db::expr::gt_eq(fld, o2); }

/// @brief     overload from the global < operator
///
/// @qualifier
/// @access    public  
/// @return    db::expr::lt
/// @param     const db::field & fld
/// @param     const T & o2
///
/// @date      20:2:2009   14:21
///
template<typename T>
inline db::expr::lt operator<(const db::field& fld, const T& o2)
{ return db::expr::lt(fld, o2); }

/// @brief     overload from the global <= operator
///
/// @qualifier
/// @access    public  
/// @return    db::expr::lt_eq
/// @param     const db::field & fld
/// @param     const T & o2
///
/// @date      20:2:2009   14:22
///
template<typename T>
inline db::expr::lt_eq operator<=(const db::field& fld, const T& o2)
{ return db::expr::lt_eq(fld, o2); }

/// @brief     overload from the global != operator
///
/// @qualifier
/// @access    public  
/// @return    db::expr::not_eq
/// @param     const db::field & fld
/// @param     const T & f2
///
/// @date      20:2:2009   14:22
///
template<typename T>
inline db::expr::not_eq operator!=(const db::field& fld, const T& f2)
{ return db::expr::not_eq(fld, f2); }

/// @brief     overload from the global ! operator
///
/// @qualifier
/// @access    public  
/// @return    db::expr::not
/// @param     const db::expr::base & exp
///
/// @date      20:2:2009   14:23
///
inline db::expr::not operator!(const db::expr::base &exp)
{ return db::expr::not(exp); }

/// @brief     overload from the global << operator
///
/// @qualifier
/// @access    public  
/// @return    db::expr::in
/// @param     const db::field & fld
/// @param     const TCHAR * f2
///
/// @date      20:2:2009   14:23
///
inline db::expr::in operator<<(const db::field& fld, const TCHAR* f2)
{ return db::expr::in(fld,f2); }

/// @brief     overload from the global << operator
///
/// @qualifier
/// @access    public  
/// @return    db::expr::in
/// @param     const db::field & fld
/// @param     const db::string & f2
///
/// @date      20:2:2009   14:23
///
inline db::expr::in operator<<(const db::field& fld, const db::string& f2)
{ return db::expr::in(fld,f2); }

/// @brief     syntactic sugar to expression-api, field.in(sel)
///
/// @qualifier const
/// @access    public  
/// @return    db::expr::in
/// @param     const string & set
///
/// @date      20:2:2009   14:24
///
inline db::expr::in db::field::In(const string& set) const
{ return expr::in(*this, set); }

/// @brief     syntactic sugar to expression-api, field.in(sel)
///
/// @qualifier const
/// @access    public  
/// @return    db::expr::in
/// @param     const sel & sel
///
/// @date      20:2:2009   14:23
///
inline db::expr::in db::field::In(const sel& sel) const
{ return expr::in(*this, sel); }

/// @brief     syntactic sugar to expression-api, field.like(string)
///
/// @qualifier
/// @access    public  
/// @return    db::expr::like
/// @param     const string & s
///
/// @date      20:2:2009   14:24
///
inline db::expr::like db::field::Like(const string& s)
{ return expr::like(*this, s); }

inline db::expr::in::in(const db::field & fld, const db::sel& s) : 
  oper(fld, _T("in"), _T("(") + s.str() + _T(")")) 
{
}

inline void db::row::fill(sqlite3_stmt* stm)
{
  if (!stm)
    return;
  if (!_query)
    return;

  _data.reserve(sqlite3_column_count(stm));
  for (int i(0), sz(sqlite3_column_count(stm)); i < sz; ++i)
  {
    _data.push_back(param(i));
    _data.back().set(stm);
  } 
}

inline db::query& db::base::execute(const string& cmd)
{
  progress_handler progress(_db, this);

  type_last_queries::iterator it(_last_queries.find(cmd));
  if (it==_last_queries.end())
  {
    _last_queries.insert(type_last_queries::value_type(cmd, query(*this)));
    it = _last_queries.find(cmd);
  }

  it->second.execute(cmd);
  return it->second;
}

inline db::query_ptr db::base::execute_ptr(const string& cmd)
{
  progress_handler progress(_db, this);
  db::query_ptr ret(new query(*this));
  if (ret)
    ret->execute(cmd);
  return ret;
}

inline void db::query::execute(const string& cmd)
{
  if (_stm) { sqlite3_finalize(_stm); _stm = 0; }
  sqlite3* _db(_base.get_db_ptr());

  _data.clear();

  int rc;
  while (1) 
  {
#ifdef _UNICODE
    rc = sqlite3_prepare16_v2(_db, 
      reinterpret_cast<const wchar_t*>(cmd.c_str()), 
      cmd.length()*sizeof(wchar_t), 
      &_stm, NULL);
#else
    rc = sqlite3_prepare_v2(_db, 
      reinterpret_cast<const char*>(cmd.c_str()), 
      cmd.length()*sizeof(char), 
      &_stm, NULL);
#endif // _UNICODE

    if (rc != SQLITE_OK || _stm == NULL) 
    {
      switch(rc) 
      {
      case SQLITE_BUSY: 
      case SQLITE_LOCKED: 
        Sleep(250); 
        break;
      default: 
        _base.throw_error(rc);
      }
    }
    else
      break;
  } 

  if (rc != SQLITE_OK || !_stm)
    return;

  unsigned nRowCnt(0);
  while ((rc = sqlite3_step(_stm)) == SQLITE_ROW) 
  {
    if (nRowCnt==0)
    {
      _field.reserve(sqlite3_column_count(_stm)); string tmp;
      for (int i(0), sz(sqlite3_column_count(_stm)); i < sz; ++i)
      {
#ifdef _UNICODE
        tmp = static_cast<const wchar_t*>(
          sqlite3_column_origin_name16(_stm, i));
#else
        tmp = sqlite3_column_origin_name(_stm, i);
#endif // _UNICODE

        _field.push_back(db::field(tmp));

        switch (sqlite3_column_type(_stm, i))
        {
        case SQLITE_NULL:
          _field.back().set_type(db::e_null);
          break;
        case SQLITE_INTEGER:
          _field.back().set_type(db::e_long);
          break;
        case SQLITE_FLOAT:
          _field.back().set_type(db::e_double);
          break;
        case SQLITE_TEXT:
          _field.back().set_type(db::e_string);
          break;
        case SQLITE_BLOB:
          _field.back().set_type(db::e_blob);
          break;
        default:
          _field.back().set_type(db::e_null);
          break;
        }
      }
    }
    
    _data.push_back(row(this, nRowCnt++));
    _data.back().fill(_stm);
  }
  sqlite3_finalize(_stm);
  _stm = NULL; 
}

#endif // db_h__
