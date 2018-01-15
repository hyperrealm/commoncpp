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

#include "commonc++/TimeSpan.h++"

namespace ccxx {

/*
 */

const int TimeSpan::msInDay = 86400000;
const int TimeSpan::msInHour = 3600000;
const int TimeSpan::msInMinute = 60000;
const int TimeSpan::msInSecond = 1000;

/*
 */

TimeSpan::TimeSpan(int64_t span /* = INT64_CONST(0) */)
{
  _computeFields(span);
}

/*
 */

TimeSpan::TimeSpan(int days, int hours, int minutes, int seconds,
                   int msec /* = 0 */)
{
  _computeFields(days, hours, minutes, seconds, msec);
}

/*
 */

TimeSpan::TimeSpan(const TimeSpan& other)
  : _days(other._days),
    _hours(other._hours),
    _minutes(other._minutes),
    _seconds(other._seconds),
    _msec(other._msec),
    _span(other._span)
{
}

/*
 */

TimeSpan::~TimeSpan()
{
}

/*
 */

void TimeSpan::setSpan(int days, int hours, int minutes, int seconds,
                       int msec /* = 0 */)
{
  _computeFields(days, hours, minutes, seconds, msec);
}

/*
 */

void TimeSpan::setSpan(int64_t span)
{
  _computeFields(span);
}

/*
 */

void TimeSpan::setDays(int days)
{
  _computeFields(days, _hours, _minutes, _seconds, _msec);
}

/*
 */

void TimeSpan::addDays(int days)
{
  _computeFields(_days + days, _hours, _minutes, _seconds, _msec);
}

/*
 */

void TimeSpan::subtractDays(int days)
{
  _computeFields(_days - days, _hours, _minutes, _seconds, _msec);
}

/*
 */

void TimeSpan::setHours(int hours)
{
  _computeFields(_days, hours, _minutes, _seconds, _msec);
}

/*
 */

void TimeSpan::addHours(int hours)
{
  _computeFields(_days, _hours + hours, _minutes, _seconds, _msec);
}

/*
 */

void TimeSpan::subtractHours(int hours)
{
  _computeFields(_days, _hours - hours, _minutes, _seconds, _msec);
}

/*
 */

void TimeSpan::setMinutes(int minutes)
{
  _computeFields(_days, _hours, minutes, _seconds, _msec);
}

/*
 */

void TimeSpan::addMinutes(int minutes)
{
  _computeFields(_days, _hours, _minutes + minutes, _seconds, _msec);
}

/*
 */

void TimeSpan::subtractMinutes(int minutes)
{
  _computeFields(_days, _hours, _minutes - minutes, _seconds, _msec);
}

/*
 */

void TimeSpan::setSeconds(int seconds)
{
  _computeFields(_days, _hours, _minutes, seconds, _msec);
}

/*
 */

void TimeSpan::addSeconds(int seconds)
{
  _computeFields(_days, _hours, _minutes, _seconds + seconds, _msec);
}

/*
 */

void TimeSpan::subtractSeconds(int seconds)
{
  _computeFields(_days, _hours, _minutes, _seconds - seconds, _msec);
}

/*
 */

void TimeSpan::setMilliseconds(int msec)
{
  _computeFields(_days, _hours, _minutes, _seconds, msec);
}

/*
 */

void TimeSpan::addMilliseconds(int msec)
{
  _computeFields(_days, _hours, _minutes, _seconds, _msec + msec);
}

/*
 */

void TimeSpan::subtractMilliseconds(int msec)
{
  _computeFields(_days, _hours, _minutes, _seconds, _msec - msec);
}

/*
 */

TimeSpan& TimeSpan::operator=(const TimeSpan& other)
{
  _days = other._days;
  _hours = other._hours;
  _minutes = other._minutes;
  _seconds = other._seconds;
  _msec = other._msec;
  _span = other._span;

  return(*this);
}

/*
 */

TimeSpan& TimeSpan::operator+=(const TimeSpan& other)
{
  _computeFields(_span + other._span);

  return(*this);
}

/*
 */

TimeSpan& TimeSpan::operator+=(int seconds)
{
  _computeFields(_span + (static_cast<int64_t>(seconds) * 1000));

  return(*this);
}

/*
 */

TimeSpan& TimeSpan::operator-=(const TimeSpan& other)
{
  _computeFields(_span - other._span);

  return(*this);
}

/*
 */

TimeSpan& TimeSpan::operator-=(int seconds)
{
  _computeFields(_span - (static_cast<int64_t>(seconds) * 1000));

  return(*this);
}

/*
 */

TimeSpan TimeSpan::operator-() const
{
  return(TimeSpan(- _span));
}

/*
 */

String TimeSpan::toString() const
{
  String s;

  if(_span < 0)
    s << '-';
  else
    s << '+';

  int days = (_days > 0 ? _days : -_days);
  int hours = (_hours > 0 ? _hours : -_hours);
  int minutes = (_minutes > 0 ? _minutes : -_minutes);
  int seconds = (_seconds > 0 ? _seconds : -_seconds);
  int msec = (_msec > 0 ? _msec : -_msec);

  if(days > 0)
    s << days << 'd';

  if(hours < 10)
    s << '0';

  s << hours << ':';

  if(minutes  < 10)
    s << '0';

  s << minutes << ':';

  if(seconds  < 10)
    s << '0';

  s << seconds << '.';

  if(msec < 100)
    s << '0';

  if(msec < 10)
    s << '0';

  s << msec;

  return(s);
}

/*
 */

void TimeSpan::_computeFields(int days, int hours, int minutes, int seconds,
                              int msec)
{
  _computeFields((static_cast<int64_t>(days) * msInDay)
                 + (static_cast<int64_t>(hours) * msInHour)
                 + (static_cast<int64_t>(minutes) * msInMinute)
                 + (static_cast<int64_t>(seconds) * msInSecond)
                 + static_cast<int64_t>(msec));
}

/*
 */

void TimeSpan::_computeFields(int64_t span)
{
  _span = span;
  int64_t tmp = _span;

  _days = (tmp / msInDay);
  tmp %= msInDay;

  _hours = (tmp / msInHour);
  tmp %= msInHour;

  _minutes = (tmp / msInMinute);
  tmp %= msInMinute;

  _seconds = (tmp / msInSecond);
  tmp %= msInSecond;

  _msec = tmp;
}

/*
 */

const TimeSpan operator+(const TimeSpan& s1, const TimeSpan& s2)
{
  return(TimeSpan(s1.getSpan() + s2.getSpan()));
}

/*
 */

const TimeSpan operator+(const TimeSpan& s1, int64_t s2)
{
  return(TimeSpan(s1.getSpan() + s2));
}

/*
 */

const TimeSpan operator+(int64_t s1, const TimeSpan& s2)
{
  return(TimeSpan(s1 + s2.getSpan()));
}

/*
 */

const TimeSpan operator-(const TimeSpan& s1, const TimeSpan& s2)
{
  return(TimeSpan(s1.getSpan() - s2.getSpan()));
}

/*
 */

const TimeSpan operator-(const TimeSpan& s1, int64_t s2)
{
  return(TimeSpan(s1.getSpan() - s2));
}

/*
 */

const TimeSpan operator-(int64_t s1, const TimeSpan& s2)
{
  return(TimeSpan(s1 - s2.getSpan()));
}


} // namespace ccxx
