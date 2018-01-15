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

#include "commonc++/TimeSpec.h++"
#include "commonc++/DateTime.h++"

#include <ctime>

namespace ccxx {

/*
 */

TimeSpec::TimeSpec()
  : _months(12),
    _days(31),
    _dow(7),
    _hours(24),
    _minutes(60)
{
}

/*
 */

TimeSpec::TimeSpec(const DateTime& time)
  : _months(12),
    _days(31),
    _dow(7),
    _hours(24),
    _minutes(60)
{
  setMonth(time.getMonth());
  setDay(time.getDay());
  setHour(time.getHour());
  setMinute(time.getMinute());
  setAllDaysOfWeek();
}

/*
 */

TimeSpec::TimeSpec(uint_t hour, uint_t minute)
  : _months(12),
    _days(31),
    _dow(7),
    _hours(24),
    _minutes(60)
{
  setHour(hour);
  setMinute(minute);
  setAllMonths();
  setAllDays();
  setAllDaysOfWeek();
}

/*
 */

TimeSpec::TimeSpec(uint_t hour, uint_t minute, uint_t month, uint_t day)
 
  : _months(12),
    _days(31),
    _dow(7),
    _hours(24),
    _minutes(60)
{
  setHour(hour);
  setMinute(minute);
  setMonth(month);
  setDay(day);
  setAllDaysOfWeek();
}

/*
 */

TimeSpec::TimeSpec(const String& spec)
  : _months(12),
    _days(31),
    _dow(7),
    _hours(24),
    _minutes(60)
{
  setSpec(spec);
}

/*
 */

TimeSpec::~TimeSpec()
{
}

/*
 */

void TimeSpec::setSpec(const String& spec)
{
  String tok;
  uint_t pos = 0;

  tok = spec.nextToken(pos);
  if(tok.isNull())
    return;

  _parseValues(tok, 0, _minutes);

  tok = spec.nextToken(pos);
  if(tok.isNull())
    return;
  _parseValues(tok, 0, _hours);

  tok = spec.nextToken(pos);
  if(tok.isNull())
    return;
  _parseValues(tok, 1, _days);

  tok = spec.nextToken(pos);
  if(tok.isNull())
    return;
  _parseValues(tok, 1, _months);

  tok = spec.nextToken(pos);
  if(tok.isNull())
    return;
  _parseValues(tok, 1, _dow);
}

/*
 */

TimeSpec& TimeSpec::setMonth(uint_t month)
{
  _set(_months, 1, month, true);
  return(*this);
}

/*
 */

TimeSpec& TimeSpec::setMonths(uint_t month1, uint_t month2)
{
  _setRange(_months, 1, month1, month2, true);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::clearMonth(uint_t month)
{
  _set(_months, 1, month, false);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::clearMonths(uint_t month1, uint_t month2)
{
  _setRange(_months, 1, month1, month2, false);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::setAllMonths()
{
  _months.setAll();

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::clearAllMonths()
{
  _months.clearAll();

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::setDay(uint_t day)
{
  _set(_days, 1, day, true);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::setDays(uint_t day1, uint_t day2)
{
  _setRange(_days, 1, day1, day2, true);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::clearDay(uint_t day)
{
  _set(_days, 1, day, false);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::clearDays(uint_t day1, uint_t day2)
{
  _setRange(_days, 1, day1, day2, false);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::setAllDays()
{
  _days.setAll();

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::clearAllDays()
{
  _days.clearAll();

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::setDayOfWeek(uint_t dow)
{
  if(dow == 0)
    dow = 7;

  _set(_dow, 1, dow, true);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::setDaysOfWeek(uint_t dow1, uint_t dow2)
{
  if(dow1 == 0)
    dow1 = 7;

  if(dow2 == 0)
    dow2 = 7;

  _setRange(_dow, 1, dow1, dow2, true);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::clearDayOfWeek(uint_t dow)
{
  if(dow == 0)
    dow = 7;

  _set(_dow, 1, dow, false);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::clearDaysOfWeek(uint_t dow1, uint_t dow2)
{
  if(dow1 == 0)
    dow1 = 7;

  if(dow2 == 0)
    dow2 = 7;

  _setRange(_dow, 1, dow1, dow2, false);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::setAllDaysOfWeek()
{
  _dow.setAll();

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::clearAllDaysOfWeek()
{
  _dow.clearAll();

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::setHour(uint_t hour)
{
  _set(_hours, 0, hour, true);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::setHours(uint_t hour1, uint_t hour2)
{
  _setRange(_hours, 0, hour1, hour2, true);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::clearHour(uint_t hour)
{
  _set(_hours, 0, hour, false);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::clearHours(uint_t hour1, uint_t hour2)
{
  _setRange(_hours, 0, hour1, hour2, false);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::setAllHours()
{
  _hours.setAll();

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::clearAllHours()
{
  _hours.clearAll();

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::setMinute(uint_t minute)
{
  _set(_minutes, 0, minute, true);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::setMinutes(uint_t minute1, uint_t minute2)
{
  _setRange(_hours, 0, minute1, minute2, true);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::clearMinute(uint_t minute)
{
  _set(_minutes, 0, minute, false);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::clearMinutes(uint_t minute1, uint_t minute2)
{
  _setRange(_minutes, 0, minute1, minute2, false);

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::setAllMinutes()
{
  _minutes.setAll();

  return(*this);
}

/*
 */

TimeSpec& TimeSpec::clearAllMinutes()
{
  _minutes.clearAll();

  return(*this);
}

/*
 */

bool TimeSpec::matches(time_s_t time) const
{
  DateTime dt(time);

  return(matches(dt.getMonth(), dt.getDay(), dt.getWeekday(),
                 dt.getHour(), dt.getMinute()));
}

/*
 */

bool TimeSpec::matches(time_ms_t time) const
{
  DateTime dt(time);

  return(matches(dt.getMonth(), dt.getDay(), dt.getWeekday(),
                 dt.getHour(), dt.getMinute()));
}

/*
 */

bool TimeSpec::matches(uint_t month, uint_t day, uint_t weekday, uint_t hour,
                       uint_t minute) const
{
  int wd = weekday;
  if(wd == 7)
    wd = 0;

  return(_months.isSet(month - 1)
         && _days.isSet(day - 1)
         && _dow.isSet(wd)
         && _hours.isSet(hour)
         && _minutes.isSet(minute));
}

/*
 */

String TimeSpec::toString() const
{
  String s = "";

  _format(_minutes, 0, s);
  s << ' ';

  _format(_hours, 0, s);
  s << ' ';

  _format(_days, 1, s);
  s << ' ';

  _format(_months, 1, s);
  s << ' ';

  _format(_dow, 1, s);

  return(s);
}

/*
 */

void TimeSpec::_set(BitSet& set, uint_t base, uint_t pos, bool on)
{
  size_t sz = set.getSize();

  if((pos >= base) && (pos < (sz + pos)))
    set.set(pos - base, on);
}

/*
 */

void TimeSpec::_setRange(BitSet& set, uint_t base, uint_t min, uint_t max,
                         bool on)
{
  size_t sz = set.getSize();

  if((min >= base) && (min < (sz + base)) && (max >= min)
     && (max <= (sz + base)))
  {
    set.setRange(min - base, max - base);
  }
}

/*
 */

void TimeSpec::_parseValues(String str, uint_t base, BitSet& set)
{
  size_t sz = set.getSize();

  if(str == "*")
    set.setAll();
  else
  {
    for(uint_t pos = 0;;)
    {
      String tok = str.nextToken(pos, ",");
      if(tok.isNull())
        break;

      uint_t first, last;

      uint_t x = tok.indexOf('-');
      if(x > 0)
      {
        String s1 = tok(0, x);
        String s2 = tok(++x);

        first = s1.toInt();
        last = s2.toInt();
      }
      else
        first = last = tok.toInt();

      if((first < base) || (first >= (sz + base)) || (last >= (sz + base))
         || (last < first))
        continue;

      set.setRange(first - base, last - base, true);
    }
  }
}

/*
 */

void TimeSpec::_format(const BitSet& set, uint_t base, String& s) const
{
  if(set.isAllSet())
  {
    s << '*';
    return;
  }

  int lastOnBit = -1, firstOnBit = 0;
  uint_t cur = 0, val = set.getWord(0), bit = 0;
  bool first = true;
  uint_t ct = set.getSize();

  for(uint_t i = 0; i < ct; i++)
  {
    bool on = (val & 1);

    if(bit < 31)
      val >>= 1, ++bit;
    else
      val = set.getWord(++cur), bit = 0;

    if(on)
    {
      if((lastOnBit != static_cast<int>(i - 1)) || (lastOnBit < 0))
        firstOnBit = i;

      lastOnBit = i;
    }
    else
    {
      if(lastOnBit >= 0)
      {
        if(! first)
          s << ',';

        s << (firstOnBit + base);
        if(lastOnBit != firstOnBit)
          s << '-' << (lastOnBit + base);

        first = false;
        lastOnBit = -1;
      }
    }
  }

  if(lastOnBit >= 0)
  {
    if(! first)
      s << ',';

    s << (firstOnBit + base);
    if(lastOnBit != firstOnBit)
      s << '-' << (lastOnBit + base);
  }
}


} // namespace ccxx
