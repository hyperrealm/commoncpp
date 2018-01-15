/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2014  Mark A Lindner

   This file is part of commonc++.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   ---------------------------------------------------------------------------
*/

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#include "commonc++/DateTime.h++"
#include "commonc++/System.h++"

#include <cstring>

namespace ccxx {

#ifdef CCXX_OS_WINDOWS
#define TZ
#endif // CCXX_OS_WINDOWS

/*
 */

static int __getGMTOffset()
{
  time_t now = ::time(NULL);
  struct tm lt, gt;

#ifdef CCXX_OS_WINDOWS

  lt = *::localtime(&now);
  gt = *::gmtime(&now);

#else

  ::tzset();
  ::localtime_r(&now, &lt);
  ::gmtime_r(&now, &gt);

#endif

  time_t ltt = ::mktime(&lt);
  time_t gtt = ::mktime(&gt);

  return(ltt - gtt);
}

static const int __GMTOffset = __getGMTOffset();

/*
 */

DateTime::DateTime(time_s_t time)
{
  operator=(time);
}

/*
 */

DateTime::DateTime(time_ms_t time /* = 0 */)
{
  if(time == 0)
    time = System::currentTimeMillis();

  operator=(time);
}

/*
 */

DateTime::DateTime(uint_t day, uint_t month, uint_t year,
                   uint_t hour /* = 0 */, uint_t minute /* = 0 */,
                   uint_t second /* = 0 */,
                   uint_t msec /* = 0 */)
  : Date(day, month, year),
    Time(hour, minute, second, msec)
{
}

/*
 */

DateTime::DateTime(const Date& date, const Time& time)
  : Date(date),
    Time(time)
{
}

/*
 */

DateTime::DateTime(const Date &date)
  : Date(date)
{
}

/*
 */

DateTime::DateTime(const Time &time)
  : Time(time)
{
}

/*
 */

DateTime::DateTime(const DateTime& other)
  : Date(other),
    Time(other)
{
}

/*
 */

DateTime::~DateTime()
{
}

/*
 */

DateTime::operator time_s_t() const
{
  struct tm ltm;
  ltm.tm_year = _year - 1900;
  ltm.tm_mon = _month - 1;
  ltm.tm_mday = _day;

  switch(_wday)
  {
    case Sunday:
    case Invalid:
      ltm.tm_wday = 0;
      break;

    case Monday:
      ltm.tm_wday = 1;
      break;

    case Tuesday:
      ltm.tm_wday = 2;
      break;

    case Wednesday:
      ltm.tm_wday = 3;
      break;

    case Thursday:
      ltm.tm_wday = 4;
      break;

    case Friday:
      ltm.tm_wday = 5;
      break;

    case Saturday:
      ltm.tm_wday = 6;
      break;
  }

  ltm.tm_hour = _hour;
  ltm.tm_min = _minute;
  ltm.tm_sec = _second;

  ltm.tm_isdst = -1;

  return(static_cast<time_s_t>(::mktime(&ltm)));
}

/*
 */

DateTime::operator time_ms_t() const
{
  time_s_t t = operator time_s_t();

  time_ms_t val = static_cast<time_ms_t>(t);
  val *= 1000;
  val += _msec;

  return(val);
}

/*
 */

bool DateTime::operator==(const DateTime& other) const
{
  return(Date::operator==(other) && Time::operator==(other));
}

/*
 */

bool DateTime::operator<(const DateTime& other) const
{
  if(Date::operator<(other))
    return(true);
  else if(Date::operator>(other))
    return(false);
  else // dates equal
    return(Time::operator<(other));
}

/*
 */

bool DateTime::operator<=(const DateTime& other) const
{
  if(Date::operator<(other))
    return(true);
  else if(Date::operator>(other))
    return(false);
  else // dates equal
    return(Time::operator<=(other));
}

/*
 */

DateTime& DateTime::operator=(time_s_t other)
{
  time_t t = static_cast<time_t>(other);

  if(other == 0)
    ::time(&t);

  struct tm ltm;

#ifdef CCXX_OS_WINDOWS
  System::lockGlobalMutex();
  ltm = *::localtime(&t);
  System::unlockGlobalMutex();
#else
  ::localtime_r(&t, &ltm);
#endif // CCXX_OS_WINDOWS

  _year = ltm.tm_year + 1900;
  _month = ltm.tm_mon + 1;
  _day = ltm.tm_mday;

  switch(ltm.tm_wday)
  {
    case 0:
    case 7:
      _wday = Sunday;
      break;

    case 1:
      _wday = Monday;
      break;

    case 2:
      _wday = Tuesday;
      break;

    case 3:
      _wday = Wednesday;
      break;

    case 4:
      _wday = Thursday;
      break;

    case 5:
      _wday = Friday;
      break;

    case 6:
      _wday = Saturday;
      break;

    default:
      _wday = Invalid;
  }

  _hour = ltm.tm_hour;
  _minute = ltm.tm_min;
  _second = ltm.tm_sec;
  _msec = 0;

  return(*this);
}

/*
 */

DateTime& DateTime::operator=(time_ms_t other)
{
  time_s_t t = static_cast<time_s_t>(other / 1000);
  operator=(t);

  _msec = other % 1000;

  return(*this);
}

/*
 */

DateTime& DateTime::operator=(const DateTime& other)
{
  if(&other != this)
  {
    Date::operator=(other);
    Time::operator=(other);
  }

  return(*this);
}

/*
 */

DateTime& DateTime::setDate(const Date& other)
{
  if(&other != this)
    Date::operator=(other);

  return(*this);
}

/*
 */

DateTime& DateTime::setTime(const Time& other)
{
  if(&other != this)
    Time::operator=(other);

  return(*this);
}

/*
 */

DateTime& DateTime::operator+=(const TimeSpan& span)
{
  time_ms_t val = operator time_ms_t();
  val += span;
  operator=(val);

  return(*this);
}

/*
 */

DateTime& DateTime::operator-=(const TimeSpan& span)
{
  operator+=(- span);

  return(*this);
}


/*
 */

DateTime& DateTime::normalize()
{
  time_ms_t val = *this;
  *this = val;

  return(*this);
}

/*
 */

bool DateTime::isDSTActive()
{
  time_t now = ::time(NULL);
  struct tm ltm;

#ifdef CCXX_OS_WINDOWS
  System::lockGlobalMutex();
  ltm = *::localtime(&now);
  System::unlockGlobalMutex();
#else
  ::localtime_r(&now, &ltm);
#endif // CCXX_OS_WINDOWS

  return(ltm.tm_isdst == 1);
}

/*
 */

int DateTime::getGMTOffset()
{
  return(__GMTOffset);
}

/*
 */

String DateTime::getTimeZone()
{
#ifdef CCXX_OS_WINDOWS

  TIME_ZONE_INFORMATION tzinfo;
  DWORD r = ::GetTimeZoneInformation(&tzinfo);

  switch(r)
  {
    case TIME_ZONE_ID_STANDARD:
    {
      String s = reinterpret_cast<const char16_t *>(tzinfo.StandardName);
      return(s);
    }

    case TIME_ZONE_ID_DAYLIGHT:
    {
      String s = reinterpret_cast<const char16_t *>(tzinfo.DaylightName);
      return(s);
    }

    default:
      return("GMT");
  }

#else

  return(isDSTActive() ? ::tzname[1] : ::tzname[0]);

#endif
}


} // namespace ccxx
