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

#ifndef __ccxx_Time_hxx
#define __ccxx_Time_hxx

#include <commonc++/Common.h++>

#include <ctime>

namespace ccxx {

/**
 * A representation of a clock time.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Time
{
 public:

  /** Construct a new Time initialized to 0:00:00:000 */
  Time();

  /** Construct a new Time with the given components. */
  Time(uint_t hour, uint_t minute, uint_t second = 0, uint_t msec = 0);

  /** Copy constructor. */
  Time(const Time& other);

  /** Destructor. */
  virtual ~Time();

  /** Set the hour component of the time. */
  void setHour(uint_t hour);

  /** Set the hour component of the time (12-hour clock). */
  void setHour12(uint_t hour, bool pm = false);

  /** Set the minute component of the time. */
  void setMinute(uint_t minute);

  /** Set the second component of the time. */
  void setSecond(uint_t second);

  /** Set the millisecond component of the time. */
  void setMillisecond(uint_t msec);

  /** Set the hour, minute, second, and millisecond components of the time. */
  void setTime(uint_t hour, uint_t minute, uint_t second = 0, uint_t msec = 0);

  /** Get the hour component of the time. */
  inline uint_t getHour() const
  { return(_hour); }

  /** Get the hour component of the time (12-hour clock). */
  inline uint_t getHour12() const
  { return(_hour == 0 ? 12 : _hour % 12); }

  /** Test if the hour is post meridiem. */
  inline bool isPM() const
  { return(_hour >= 12); }

  /** Test if the hour is ante meridiem. */
  inline bool isAM() const
  { return(_hour < 12); }

  /** Get the minute component of the time. */
  inline uint_t getMinute() const
  { return(_minute); }

  /** Get the second component of the time. */
  inline uint_t getSecond() const
  { return(_second); }

  /** Get the millisecond component of the time. */
  inline uint_t getMillisecond() const
  { return(_msec); }

  bool operator==(const Time& other) const;
  bool operator<(const Time& other) const;
  bool operator<=(const Time& other) const;

  inline bool operator!=(const Time& other) const
  { return(! operator==(other)); }

  inline bool operator>(const Time& other) const
  { return(! operator<=(other)); }

  inline bool operator>=(const Time& other) const
  { return(! operator<(other)); }

  Time& operator=(const Time& other);

 protected:

  /** @cond INTERNAL */
  uint_t _hour;
  uint_t _minute;
  uint_t _second;
  uint_t _msec;
  /** @endcond */
};

} // namespace ccxx

#endif // __ccxx_Time_hxx
