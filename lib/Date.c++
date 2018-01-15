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

#include "commonc++/Date.h++"

#include <ctime>

namespace ccxx {

/*
 */

const uint_t Date::JAN = 1, Date::FEB = 2, Date::MAR = 3,
  Date::APR = 4, Date::MAY = 5, Date::JUN = 6, Date::JUL = 7, Date::AUG = 8,
  Date::SEP = 9, Date::OCT = 10, Date::NOV = 11, Date::DEC = 12;

static const uint_t __daysInMonth[]
  = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static const uint_t __daysInYearAccum[]
  = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

static const uint_t __daysInLeapYearAccum[]
  = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

/*
 */

Date::Date()
  : _year(1),
    _month(JAN),
    _day(1),
    _wday(Invalid),
    _wnum(-1)
{
}

/*
 */

Date::Date(uint_t day, uint_t month, uint_t year)
  : _year(1),
    _month(0),
    _day(0),
    _wday(Invalid),
    _wnum(-1)
{
  setDate(day, month, year);
}

/*
 */

Date::Date(const Date& other)
  : _year(other._year),
    _month(other._month),
    _day(other._day),
    _wday(other._wday),
    _wnum(-1)
{
}

/*
 */

Date::~Date()
{
}

/*
 */

void Date::setMonth(uint_t month)
{
  if((month >= 1) && (month <= 12))
  {
    _month = month;
    _wday = Invalid;
  }
}

/*
 */

void Date::setDay(uint_t day)
{
  if((day >= 1) && (day <= 31))
  {
    _day = day;
    _wday = Invalid;
  }
}

/*
 */

void Date::setYear(uint_t year)
{
  if((year >= 1) && (year <= 9999))
  {
    _year = year;
    _wday = Invalid;
  }
}

/*
 */

void Date::setDate(uint_t day, uint_t month, uint_t year)
{
  setDay(day);
  setMonth(month);
  setYear(year);
}

/*
 */

Date::Weekday Date::getWeekday() const
{
  if(_wday == Invalid)
    _recalculate();

  return(_wday);
}

/*
 */

uint_t Date::getDayOfYear() const
{
  if(_wday == Invalid)
    _recalculate();

  return(_yday);
}

/*
 */

uint_t Date::getWeekOfYear() const
{
  if(_wday == Invalid)
    _recalculate();

  return(_wnum);
}

/*
 */

Date& Date::operator=(const Date &other)
{
  if(&other != this)
  {
    _day = other._day;
    _month = other._month;
    _year = other._year;
    _wday = other._wday;
  }

  return(*this);
}

/*
 */

bool Date::operator==(const Date &other) const
{
  return((_day == other._day) && (_month == other._month)
         && (_year == other._year));
}

/*
 */

bool Date::operator<(const Date &other) const
{
  if(_year < other._year)
    return(true);

  else if(_year > other._year)
    return(false);

  else // years equal
  {
    if(_month < other._month)
      return(true);

    else if(_month > other._month)
      return(false);

    else // months equal
      return(_day < other._day);
  }
}

/*
 */

bool Date::operator<=(const Date &other) const
{
  if(_year < other._year)
    return(true);

  else if(_year > other._year)
    return(false);

  else // years equal
  {
    if(_month < other._month)
      return(true);

    else if(_month > other._month)
      return(false);

    else // months equal
      return(_day <= other._day);
  }
}

/*
 */

bool Date::isLeapYear(uint_t year)
{
  return((((year % 4) == 0) && ((year % 100) != 0)) || ((year % 400) == 0));
}

/*
 */

bool Date::isValid() const
{
  if((_month == 0) || (_year == 0))
    return(false);

  if(isLeapYear(_year) && (_month == FEB))
    return(_day <= 29);
  else
    return(_day <= __daysInMonth[_month - 1]);
}

/*
 */

void Date::_recalculate() const
{
  int a = (14 - _month) / 12;
  int y = _year - a;
  int m = _month + 12 * a - 2;
  int d = (_day + y + y/4 - y/100 + y/400 + (31 * m / 12)) % 7;

  switch(d)
  {
    case 0: _wday = Sunday; break;
    case 1: _wday = Monday; break;
    case 2: _wday = Tuesday; break;
    case 3: _wday = Wednesday; break;
    case 4: _wday = Thursday; break;
    case 5: _wday = Friday; break;
    case 6: _wday = Saturday; break;
  }

  _yday = _day + (isLeapYear(_year) ? __daysInLeapYearAccum[_month - 1]
                  : __daysInYearAccum[_month - 1]);

  a = _day - _wday;
  if(a < 0)
    _wnum = (a > -2 ? 1 : 0);
  else
  {
    int x = a + 6;
    _wnum = (x / 7) + (((x % 7) > 3) ? 1 : 0);
  }
}


} // namespace ccxx
