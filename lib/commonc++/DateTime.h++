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

#ifndef __ccxx_DateTime_hxx
#define __ccxx_DateTime_hxx

#include <commonc++/Common.h++>
#include <commonc++/Date.h++>
#include <commonc++/String.h++>
#include <commonc++/Time.h++>
#include <commonc++/TimeSpan.h++>

namespace ccxx {

/** A representation of a calendar date and clock time.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API DateTime : public Date, public Time
{
 public:

  /** Construct a DateTime for the given system time. */
  DateTime(time_s_t time);

  /** Construct a DateTime for the given time (or the current time). */
  DateTime(time_ms_t time = 0);

  /** Construct a DateTime with the given components. */
  DateTime(uint_t day, uint_t month, uint_t year, uint_t hour = 0,
           uint_t minute = 0, uint_t second = 0, uint_t msec = 0);

  /** Construct a DateTime from the given Date and Time. */
  DateTime(const Date& date, const Time& time);

  /** Construct a DateTime for the given Date. */
  DateTime(const Date& date);

  /** Construct a DateTime for the given Time. */
  DateTime(const Time& time);

  /** Copy constructor. */
  DateTime(const DateTime& other);

  /** Destructor. */
  virtual ~DateTime();

  /**
   * Normalize the DateTime. This method converts invalid dates into
   * valid dates. For example, a date of "February 31, 2000" is
   * normalized to "March 2, 2000".
   */
  DateTime& normalize();

  /** Cast operator. */
  operator time_s_t() const;

  /** Cast operator. */
  operator time_ms_t() const;

  bool operator==(const DateTime& other) const;
  bool operator<(const DateTime& other) const;
  bool operator<=(const DateTime& other) const;

  inline bool operator!=(const DateTime& other) const
  { return(! operator==(other)); }

  inline bool operator>(const DateTime& other) const
  { return(! operator<=(other)); }

  inline bool operator>=(const DateTime& other) const
  { return(! operator<(other)); }

  DateTime& operator=(time_s_t other);
  DateTime& operator=(time_ms_t other);
  DateTime& operator=(const DateTime& other);

  /** Set the hour, minute, second, and millisecond components of the time. */
  inline void setTime(uint_t hour, uint_t minute, uint_t second = 0,
                      uint_t msec = 0)
  { Time::setTime(hour, minute, second, msec); }

  /** Set the day, month, and year components of the date. */
  inline void setDate(uint_t day, uint_t month, uint_t year)
  { Date::setDate(day, month, year); }

  /** Set the date portion of the DateTime. */
  DateTime& setDate(const Date& other);

  /** Set the time portion of the DateTime. */
  DateTime& setTime(const Time& other);

  /** Add a TimeSpan to this DateTime. */
  DateTime& operator+=(const TimeSpan& span);

  /** Subtract a TimeSpan from this DateTime. */
  DateTime& operator-=(const TimeSpan& span);

  /** Test if Daylight Saving Time is in effect for the current local time. */
  static bool isDSTActive();

  /** Get the offset, in seconds, between local time and GMT. */
  static int getGMTOffset();

  /** Get the name of the local time zone. */
  static String getTimeZone();
};

inline const TimeSpan operator-(const DateTime& d1, const DateTime& d2)
{
  time_ms_t t1 = d1;
  time_ms_t t2 = d2;
  return TimeSpan(t1 >= t2 ? t1 - t2 : t2 - t1);
}

inline const DateTime operator-(const DateTime& d, const TimeSpan& ts)
{
  DateTime tmp(d);
  tmp -= ts;
  return tmp;
}

inline const DateTime operator+(const DateTime& d, const TimeSpan& ts)
{
  DateTime tmp(d);
  tmp += ts;
  return tmp;
}

} // namespace ccxx

#endif // __ccxx_DateTime_hxx
