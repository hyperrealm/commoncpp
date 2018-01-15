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

#include "commonc++/CStringBuilder.h++"
#include "commonc++/DateTimeFormat.h++"
#include "commonc++/DeleteFunctor.h++"
#include "commonc++/Locale.h++"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <vector>

namespace ccxx {

class DateTimeFormat::TokenList : public std::vector<Token *>
{
};

/*
 */

String DateTimeFormat::_monthNames[12];
String DateTimeFormat::_monthNamesLong[12];
String DateTimeFormat::_weekdayNames[7];
String DateTimeFormat::_weekdayNamesLong[7];
String DateTimeFormat::_meridiemUpper[2];
String DateTimeFormat::_meridiemLower[2];

#if (! defined CCXX_OS_WINDOWS) && (! defined HAVE_LOCALTIME_R)

static struct tm * localtime_r(const time_t *timep, struct tm *result)
{
  System::lockGlobalMutex();
  struct tm *tms = localtime(&t);
  std::memcpy(result, tms, sizeof(struct tm));
  System::unlockGlobalMutex();
  return(result);
}

#endif

/*
 */

bool DateTimeFormat::_cacheStrings()
{
  struct tm tms = { 0, 0, 0, 1, 1, 100, -1, -1, -1 };
  char buf[40];
  int i;

  for(i = 0; i < 12; ++i)
  {
    tms.tm_mon = i;
    ::strftime(buf, sizeof(buf) - 1, "%B", &tms);
    _monthNamesLong[i] = buf;

    ::strftime(buf, sizeof(buf) - 1, "%b", &tms);
    _monthNames[i] = buf;
  }

  tms.tm_mday = 2;
  tms.tm_mon = 0;
  tms.tm_sec = 1;

  for(i = 0; i < 7; ++i)
  {
    mktime(&tms);

    ::strftime(buf, sizeof(buf) - 1, "%A", &tms);
    _weekdayNamesLong[i] = buf;

    ::strftime(buf, sizeof(buf) - 1, "%a", &tms);
    _weekdayNames[i] = buf;

    tms.tm_mday++;
  }

  for(int i = 0; i < 2; ++i)
  {
    ::strftime(buf, sizeof(buf) - 1, "%p", &tms);
    _meridiemLower[i] = buf;
    _meridiemLower[i].toLowerCase();

    _meridiemUpper[i] = buf;
    _meridiemUpper[i].toUpperCase();

    tms.tm_hour += 12;
  }

  return(true);
}

/*
 */

bool DateTimeFormat::_stringsCached = DateTimeFormat::_cacheStrings();

/*
 */

DateTimeFormat::DateTimeFormat(String format /* = "%t %T" */,
                               String dateFormat /* = "%t" */,
                               String timeFormat /* = "%T" */)
  : _dateFormat(new TokenList()),
    _timeFormat(new TokenList()),
    _dateTimeFormat(new TokenList())
{
  _parseFormat(format, _dateTimeFormat);
  _parseFormat(dateFormat, _dateFormat);
  _parseFormat(timeFormat, _timeFormat);
}

/*
 */

DateTimeFormat::~DateTimeFormat()
{
  std::for_each(_dateFormat->begin(), _dateFormat->end(), DeleteFunctor());
  delete _dateFormat;

  std::for_each(_timeFormat->begin(), _timeFormat->end(), DeleteFunctor());
  delete _timeFormat;

  std::for_each(_dateTimeFormat->begin(), _dateTimeFormat->end(),
                DeleteFunctor());
  delete _dateTimeFormat;
}

/*
 */

String DateTimeFormat::format(const Date& value) const
{
  return(_format(_dateFormat, DateTime(value)));
}

/*
 */

size_t DateTimeFormat::format(const Date& value, char *buf, size_t bufsz) const
{
  CStringBuilder builder(buf, bufsz);
  return(_format(_dateFormat, DateTime(value), builder));
}

/*
 */

String DateTimeFormat::format(const Time& value) const
{
  return(_format(_timeFormat, DateTime(value)));
}

/*
 */

size_t DateTimeFormat::format(const Time& value, char *buf, size_t bufsz) const
{
  CStringBuilder builder(buf, bufsz);
  return(_format(_dateFormat, DateTime(value), builder));
}

/*
 */

String DateTimeFormat::format(const DateTime& value) const
{
  return(_format(_dateTimeFormat, value));
}

/*
 */

size_t DateTimeFormat::format(const DateTime& value, char* buf, size_t bufsz)
  const
{
  CStringBuilder builder(buf, bufsz);
  return(_format(_dateTimeFormat, value, builder));
}

/*
 */

void DateTimeFormat::parse(DateTime& value, const String& text) const
{
  CString cstr_text = text.toUTF8();
  _parse(_dateTimeFormat, value, cstr_text.data());
}

/*
 */

void DateTimeFormat::parse(Date& value, const String& text) const
{
  DateTime tmp(value);
  CString cstr_text = text.toUTF8();
  _parse(_dateFormat, tmp, cstr_text.data());
  value = tmp;
}

/*
 */

void DateTimeFormat::parse(Time& value, const String& text) const
{
  DateTime tmp(value);
  CString cstr_text = text.toUTF8();
  _parse(_timeFormat, tmp, cstr_text.data());
  value = tmp;
}

/*
 */

void DateTimeFormat::_parse(const TokenList* tokens, DateTime& value,
                            const char* buf) const
{
  value = static_cast<time_s_t>(0);
  uint_t pos = 0;
  const char *p = buf;
  bool pm = false;

  for(TokenList::const_iterator iter = tokens->begin();
      (iter != tokens->end());
      ++iter)
  {
    const Token *tok = *iter;
    TokenFormat fmt = tok->getFormat();

    switch(tok->getType())
    {
      case TOK_LITERAL:
      {
        const char *q = tok->getText();

        while(*p && *q)
        {
          if(*p != *q)
            throw ParseException(String::null, 1, pos);

          ++p;
          ++q;
          ++pos;
        }
        if(*q)
          throw ParseException(String::null, 1, pos);

        break;
      }

      case TOK_HOURS:
        value.setHour(_parseInt(p, pos, 0, 23));
        break;

      case TOK_HOURS12:
        value.setHour12(_parseInt(p, pos, 1, 12), pm);
        break;

      case TOK_MINUTES:
        value.setMinute(_parseInt(p, pos, 0, 59));
        break;

      case TOK_SECONDS:
        value.setSecond(_parseInt(p, pos, 0, 60));
        break;

      case TOK_MSEC:
        value.setMillisecond(_parseInt(p, pos, 0, 999));
        break;

      case TOK_AMPM_LOWER:
      {
        pm = (_parseString(p, pos, _meridiemLower, 2) == 1);
        int hr = value.getHour();

        if(pm)
        {
          if(hr < 12)
            value.setHour(hr + 12);
        }
        else
        {
          if(hr == 12)
            hr = 0;
        }
        break;
      }

      case TOK_AMPM_UPPER:
      {
        pm = (_parseString(p, pos, _meridiemUpper, 2) == 1);
        int hr = value.getHour();

        if(pm)
        {
          if(hr < 12)
            value.setHour(hr + 12);
        }
        else
        {
          if(hr == 12)
            hr = 0;
        }
        break;
      }

      case TOK_MONTH:
      {
        switch(fmt)
        {
          case FMT_NUMERIC:
          case FMT_NUMERIC_0:
          case FMT_NUMERIC_SPC:
            value.setMonth(_parseInt(p, pos, 1, 12));
            break;

          case FMT_STRING:
            value.setMonth(_parseString(p, pos, _monthNames, 12) + 1);
            break;

          case FMT_STRING_LONG:
            value.setMonth(_parseString(p, pos, _monthNamesLong, 12) + 1);
            break;

          default:
            break;
        }
        break;
      }

      case TOK_DAY:
        value.setDay(_parseInt(p, pos, 1, 31));
        break;

      case TOK_DAYNUM:
        _parseInt(p, pos, 0, 366);
        // ignore
        break;

      case TOK_YEAR:
      {
        int y = _parseInt(p, pos, 0, 99);
        if(y >= 69)
          y += 1900;
        else
          y += 2000;

        value.setYear(y);
        break;
      }

      case TOK_YEAR_4:
        value.setYear(_parseInt(p, pos, 0, 9999));
        break;

      case TOK_WEEKDAY:
      {
        switch(fmt)
        {
          case FMT_NUMERIC:
          case FMT_NUMERIC_0:
          case FMT_NUMERIC_SPC:
            _parseInt(p, pos, 0, 7); // ignore
            break;

          case FMT_STRING:
            _parseString(p, pos, _weekdayNames, 7); // ignore
            break;

          case FMT_STRING_LONG:
            _parseString(p, pos, _weekdayNamesLong, 7); // ignore
            break;

          default:
            break;
        }
        break;
      }

      case TOK_WEEKNUM:
        _parseInt(p, pos, 0, 52);
        // ignore
        break;

      case TOK_TZ:
        _skip(p, pos, 3);
        break;

      case TOK_DATE:
        // cannot implement portably due to lack of strptime() on Windows
        throw ParseException("directive not supported", 1, pos);
        break;

      case TOK_TIME:
        // cannot implement portably due to lack of strptime() on Windows
        throw ParseException("directive not supported", 1, pos);
        break;

      default:
        // do nothing
        break;
    }
  }
}

/*
 */

int DateTimeFormat::_parseInt(const char*& p, uint_t& pos, int min, int max)
  const
{
  char *ep;

  int val = std::strtol(p, &ep, 10);

  if((val < min) || (val > max))
    throw ParseException("value out of range", 1, pos);

  pos += (ep - p);
  p = ep;

  return(val);
}

/*
 */

int DateTimeFormat::_parseString(const char*& p, uint_t& pos,
                                 const String* list, size_t count) const
{
  for(size_t i = 0; i < count; ++i)
  {
    CString cstr = list[i].toUTF8();
    const char *c = cstr.data();

    const char *s = p;
    size_t len, l;

    l = len = list[i].length();

    for(; *s && *c && l; ++s, ++c, --l)
    {
      if(*s != *c)
        break;
    }

    if(l == 0)
    {
      p += len;
      pos += len;

      return(i);
    }
  }

  throw ParseException(String::null, 1, pos);
}

/*
 */

void DateTimeFormat::_skip(const char*& p, uint_t& pos, uint_t count) const
{
  for(uint_t i = 0; i < count; ++i, ++p, ++pos)
  {
    if(*p == 0)
      throw ParseException(String::null, 1, pos);
  }
}

/*
 */

void DateTimeFormat::_parseFormat(const String& format, TokenList* tokens)
{
  bool inToken = false;
  const char *text = NULL;
  size_t textLen = 0;
  TokenFormat tokFmt = FMT_INVALID;

  CString cstr_format = format.toUTF8();
  const char *fmt = cstr_format.data();
  for(const char *p = fmt; *p; ++p)
  {
    if(! inToken)
    {
      if(*p == '%')
      {
        if(text)
        {
          tokens->push_back(new Token(TOK_LITERAL, FMT_STRING, text, textLen));
          text = NULL;
          textLen = 0;
        }

        inToken = true;
        tokFmt = FMT_INVALID;
      }
      else
      {
        if(! text)
          text = p;
        ++textLen;
      }
    }
    else
    {
      if(*p == '$')
      {
        if(tokFmt == FMT_STRING)
          tokFmt = FMT_STRING_LONG;
        else if(tokFmt == FMT_INVALID)
          tokFmt = FMT_STRING;
      }
      else if(*p == '_' && tokFmt == FMT_INVALID)
      {
        tokFmt = FMT_NUMERIC_SPC;
      }
      else if(*p == '0' && tokFmt == FMT_INVALID)
      {
        tokFmt = FMT_NUMERIC_0;
      }
      else
      {
        TokenCode code = TOK_INVALID;

        switch(*p)
        {
          case 'h': // hours, 12-hr clock
            code = TOK_HOURS12;
            break;

          case 'H': // hours (0-23)
            code = TOK_HOURS;
            break;

          case 'M': // minutes (0-59)
            code = TOK_MINUTES;
            break;

          case 'S': // seconds (0-60)
            code = TOK_SECONDS;
            break;

          case 's': // milliseconds (0-999)
            code = TOK_MSEC;
            break;

          case 'm': // month (0-11)
            code = TOK_MONTH;
            break;

          case 'd': // day (0-31)
            code = TOK_DAY;
            break;

          case 'y': // year (0-99)
            code = TOK_YEAR;
            break;

          case 'Y': // year with century (0-9999)
            code = TOK_YEAR_4;
            break;

          case 'p': // am/pm
            code = TOK_AMPM_LOWER;
            break;

          case 'P': // AM/PM
            code = TOK_AMPM_UPPER;
            break;

          case 'w': // weekday (0-6)
            code = TOK_WEEKDAY;
            break;

          case 'W': // week of year
            code = TOK_WEEKNUM;
            break;

          case 'D': // day of year
            code = TOK_DAYNUM;
            break;

          case 'Z': // timezone
            code = TOK_TZ;
            break;

          case 'T': // time
            code = TOK_TIME;
            break;

          case 't': // date
            code = TOK_DATE;
            break;

          case '%': // literal '%'
            text = p;
            textLen = 1;
            break;

          default:
            // ignore token
            break;
        }

        inToken = false;
        if(code != TOK_INVALID)
        {
          if(tokFmt == FMT_INVALID)
            tokFmt = FMT_NUMERIC;

          tokens->push_back(new Token(code, tokFmt));
        }
      }
    }
  }

  if(text)
    tokens->push_back(new Token(TOK_LITERAL, FMT_STRING, text, textLen));
}

/*
 */

void DateTimeFormat::_formatNumeric(String& s, uint_t val, uint_t width,
                                    TokenFormat format) const
{
  char buf[256];

  switch(format)
  {
    case FMT_NUMERIC_0:
      snprintf(buf, sizeof(buf), "%0*d", width, val);
      s += buf;
      break;

    case FMT_NUMERIC_SPC:
      snprintf(buf, sizeof(buf), "%*d", width, val);
      s += buf;
      break;

    case FMT_NUMERIC:
    default:
      s += val;
      break;
  }
}

/*
 */

size_t DateTimeFormat::_format(const TokenList* tokens, const DateTime& value,
                               CStringBuilder& builder) const
{
  #ifndef CCXX_OS_ANDROID
  uint_t val = 0;
  const Locale locale = Locale::getCurrent();

  for(TokenList::const_iterator iter = tokens->begin();
      (iter != tokens->end()) && builder.hasRemaining();
      ++iter)
  {
    const Token *tok = *iter;
    TokenFormat fmt = tok->getFormat();

    switch(tok->getType())
    {
      case TOK_LITERAL:
        builder.append(tok->getText());
        break;

      case TOK_HOURS:
        builder.append(value.getHour(), 2, (fmt == FMT_NUMERIC_0 ? '0' : NUL));
        break;

      case TOK_HOURS12:
        builder.append(value.getHour12(), 2,
                       (fmt == FMT_NUMERIC_0 ? '0' : NUL));
        break;

      case TOK_MINUTES:
        builder.append(value.getMinute(), 2,
                       (fmt == FMT_NUMERIC_0 ? '0' : NUL));
        break;

      case TOK_SECONDS:
        builder.append(value.getSecond(), 2,
                       (fmt == FMT_NUMERIC_0 ? '0' : NUL));
        break;

      case TOK_MSEC:
        builder.append(value.getMillisecond(), 3,
                       (fmt == FMT_NUMERIC_0 ? '0' : NUL));
        break;

      case TOK_AMPM_LOWER:
      {
        String s = (value.isAM() ? locale.getAMAffix() : locale.getPMAffix());
        builder.append(s.toLowerCase().toUTF8());
        break;
      }

      case TOK_AMPM_UPPER:
      {
        String s = (value.isAM() ? locale.getAMAffix() : locale.getPMAffix());
        builder.append(s.toUpperCase().toUTF8());
        break;
      }

      case TOK_MONTH:
        switch(fmt)
        {
          case FMT_STRING:
            builder.append(locale.getAbbreviatedMonthName(value.getMonth())
                           .toUTF8());
            break;

          case FMT_STRING_LONG:
            builder.append(locale.getMonthName(value.getMonth()).toUTF8());
            break;

          default:
            builder.append(value.getMonth(), 2,
                           (fmt == FMT_NUMERIC_0 ? '0' : NUL));
        }
        break;

      case TOK_DAY:
        builder.append(value.getDay(), 2, (fmt == FMT_NUMERIC_0 ? '0' : NUL));
        break;

      case TOK_DAYNUM:
        builder.append(value.getDayOfYear(), 3,
                       (fmt == FMT_NUMERIC_0 ? '0' : NUL));
        break;

      case TOK_YEAR:
        builder.append(value.getYear() % 100, 2,
                       (fmt == FMT_NUMERIC_0 ? '0' : NUL));
        break;

      case TOK_YEAR_4:
        builder.append(value.getYear(), 4,
                       (fmt == FMT_NUMERIC_0 ? '0' : NUL));
        break;

      case TOK_WEEKDAY:
        val = static_cast<uint_t>(value.getWeekday()) % 7;
        switch(fmt)
        {
          case FMT_STRING:
            builder.append(locale.getAbbreviatedWeekDayName(val).toUTF8());
            break;

          case FMT_STRING_LONG:
            builder.append(locale.getWeekDayName(val).toUTF8());
            break;

          default:
            builder.append(val, 1, (fmt == FMT_NUMERIC_0 ? '0' : NUL));
        }
        break;

      case TOK_WEEKNUM:
        builder.append(value.getWeekOfYear(), 2,
                       (fmt == FMT_NUMERIC_0 ? '0' : NUL));
        break;

      case TOK_TZ:
      {
        builder.append(DateTime::getTimeZone().toUTF8());
        break;
      }

      case TOK_DATE:
      {
        time_s_t t = value;

#ifdef CCXX_OS_WINDOWS
        // localtime() is threadsafe on Windows.
        time_t tt = static_cast<time_t>(t);
        struct tm *tms = ::localtime(&tt);
#else
        struct tm _tms;
        time_t tt = static_cast<time_t>(t);
        struct tm *tms = localtime_r(&tt, &_tms);
#endif
        size_t r = ::strftime(builder.getPosition(), builder.getRemaining(),
                              "%x", tms);
        builder.bump(r);
        break;
      }

      case TOK_TIME:
      {
        time_s_t t = value;

#ifdef CCXX_OS_WINDOWS
        // localtime() is threadsafe on Windows.
        time_t tt = static_cast<time_t>(t);
        struct tm *tms = ::localtime(&tt);
#else
        struct tm _tms;
        time_t tt = static_cast<time_t>(t);
        struct tm *tms = localtime_r(&tt, &_tms);
#endif
        size_t r = ::strftime(builder.getPosition(), builder.getRemaining(),
                              "%X", tms);
        builder.bump(r);
        break;
      }

      default:
        // do nothing
        break;
    }
  }

  builder.terminate();
  return(builder.length());

  #else
  return 0;
  #endif
}

/*
 */

String DateTimeFormat::_format(const TokenList* tokens, const DateTime& value)
  const
{
  String s;

#ifndef CCXX_OS_ANDROID
  uint_t val = 0;
  char buf[80];
  const Locale locale = Locale::getCurrent();

  for(TokenList::const_iterator iter = tokens->begin();
      iter != tokens->end();
      ++iter)
  {
    const Token *tok = *iter;
    TokenFormat fmt = tok->getFormat();

    switch(tok->getType())
    {
      case TOK_LITERAL:
        s += tok->getText();
        break;

      case TOK_HOURS:
        _formatNumeric(s, value.getHour(), 2, fmt);
        break;

      case TOK_HOURS12:
        _formatNumeric(s, value.getHour12(), 2, fmt);
        break;

      case TOK_MINUTES:
        _formatNumeric(s, value.getMinute(), 2, fmt);
        break;

      case TOK_SECONDS:
        _formatNumeric(s, value.getSecond(), 2, fmt);
        break;

      case TOK_MSEC:
        _formatNumeric(s, value.getMillisecond(), 3, fmt);
        break;

      case TOK_AMPM_LOWER:
      {
        String ampm = (value.isAM() ? locale.getAMAffix()
                        : locale.getPMAffix());
        s += ampm.toLowerCase();
        break;
      }

      case TOK_AMPM_UPPER:
      {
        String ampm = (value.isAM() ? locale.getAMAffix()
                        : locale.getPMAffix());
        s += ampm.toUpperCase();
        break;
      }

      case TOK_MONTH:
        switch(fmt)
        {
          case FMT_STRING:
            s += locale.getAbbreviatedMonthName(value.getMonth());
            break;

          case FMT_STRING_LONG:
            s += locale.getMonthName(value.getMonth());
            break;

          default:
            _formatNumeric(s, value.getMonth(), 2, fmt);
        }
        break;

      case TOK_DAY:
        _formatNumeric(s, value.getDay(), 2, fmt);
        break;

      case TOK_DAYNUM:
        _formatNumeric(s, value.getDayOfYear(), 3, fmt);
        break;

      case TOK_YEAR:
        _formatNumeric(s, value.getYear() % 100, 2, fmt);
        break;

      case TOK_YEAR_4:
        _formatNumeric(s, value.getYear(), 4, fmt);
        break;

      case TOK_WEEKDAY:
        val = static_cast<uint_t>(value.getWeekday()) % 7;
        switch(fmt)
        {
          case FMT_STRING:
            s += locale.getAbbreviatedWeekDayName(val);
            break;

          case FMT_STRING_LONG:
            s += locale.getWeekDayName(val);
            break;

          default:
            _formatNumeric(s, val, 1, fmt);
        }
        break;

      case TOK_WEEKNUM:
        _formatNumeric(s, value.getWeekOfYear(), 2, fmt);
        break;

      case TOK_TZ:
        s += DateTime::getTimeZone();
        break;

      case TOK_DATE:
      {
        time_s_t t = value;

#ifdef CCXX_OS_WINDOWS
        // localtime() is threadsafe on Windows.
        time_t tt = static_cast<time_t>(t);
        struct tm *tms = ::localtime(&tt);
#else
        struct tm _tms;
        time_t tt = static_cast<time_t>(t);
        struct tm *tms = localtime_r(&tt, &_tms);
#endif
        ::strftime(buf, sizeof(buf), "%x", tms);
        s += buf;
        break;
      }

      case TOK_TIME:
      {
        time_s_t t = value;

#ifdef CCXX_OS_WINDOWS
        // localtime() is threadsafe on Windows.
        time_t tt = static_cast<time_t>(t);
        struct tm *tms = ::localtime(&tt);
#else
        struct tm _tms;
        time_t tt = static_cast<time_t>(t);
        struct tm *tms = localtime_r(&tt, &_tms);
#endif
        ::strftime(buf, sizeof(buf), "%X", tms);
        s += buf;
        break;
      }

      default:
        // do nothing
        break;
    }
  }
#endif
  return(s);
}


} // namespace ccxx
