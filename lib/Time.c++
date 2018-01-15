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

#include "commonc++/Time.h++"

namespace ccxx {

Time::Time()
  : _hour(0),
    _minute(0),
    _second(0),
    _msec(0)
{
}

/*
 */

Time::Time(uint_t hour, uint_t minute, uint_t second /* = 0 */,
           uint_t msec /* = 0 */)
  : _hour(hour),
    _minute(minute),
    _second(second),
    _msec(msec)
{
}

/*
 */

Time::Time(const Time &other)
  : _hour(other._hour),
    _minute(other._minute),
    _second(other._second),
    _msec(other._msec)
{
}

/*
 */

Time::~Time()
{
}

/*
 */

void Time::setHour(uint_t hour)
{
  if(hour < 24)
    _hour = hour;
}

/*
 */

void Time::setHour12(uint_t hour, bool pm /* = false */)
{
  if((hour >= 1) && (hour <= 12))
  {
    if(pm)
      _hour = (hour == 12 ? hour : hour + 12);
    else
      _hour = (hour == 12 ? 0 : hour);
  }
}

/*
 */

void Time::setMinute(uint_t minute)
{
  if(minute < 60)
    _minute = minute;
}

/*
 */

void Time::setSecond(uint_t second)
{
  if(second < 60)
    _second = second;
}

/*
 */

void Time::setMillisecond(uint_t msec)
{
  if(msec < 1000)
    _msec = msec;
}

/*
 */

void Time::setTime(uint_t hour, uint_t minute, uint_t second /* = 0 */,
                   uint_t msec /* = 0 */)
{
  setHour(hour);
  setMinute(minute);
  setSecond(second);
  setMillisecond(msec);
}

/*
 */

bool Time::operator==(const Time &other) const
{
  return((_hour == other._hour) && (_minute == other._minute)
         && (_second == other._second) && (_msec == other._msec));
}

/*
 */

bool Time::operator<(const Time &other) const
{
  if(_hour < other._hour)
    return(true);

  else if(_hour > other._hour)
    return(false);

  else // hours equal
  {
    if(_minute < other._minute)
      return(true);

    else if(_minute > other._minute)
      return(false);

    else // minutes equal
    {
      if(_second < other._second)
        return(true);

      else if(_second > other._second)
        return(false);

      else // seconds equal
        return(_msec < other._msec);
    }
  }
}

/*
 */

bool Time::operator<=(const Time &other) const
{
  if(_hour < other._hour)
    return(true);

  else if(_hour > other._hour)
    return(false);

  else // hours equal
  {
    if(_minute < other._minute)
      return(true);

    else if(_minute > other._minute)
      return(false);

    else // minutes equal
    {
      if(_second < other._second)
        return(true);

      else if(_second > other._second)
        return(false);

      else // seconds equal
        return(_msec <= other._msec);
    }
  }
}

/*
 */

Time& Time::operator=(const Time &other)
{
  if(&other != this)
  {
    _hour = other._hour;
    _minute = other._minute;
    _second = other._second;
    _msec = other._msec;
  }

  return(*this);
}


} // namespace ccxx
