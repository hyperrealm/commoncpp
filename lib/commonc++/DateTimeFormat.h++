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

#ifndef __ccxx_DateTimeFormat_hxx
#define __ccxx_DateTimeFormat_hxx

#include <commonc++/Common.h++>
#include <commonc++/DateTime.h++>
#include <commonc++/ParseException.h++>

#include <cstring>

namespace ccxx {

class CStringBuilder; // fwd decl

/**
 * A formatter for dates and times. Available format directives are
 * enumerated below. Unrecognized directives are ignored, and all other text
 * is treated literally.
 *
 * <table border=1 cellpadding=2 cellspacing=0>
 * <tr><th>Directive</th><th>Meaning</th><th>Range</th></tr>
 * <tr><td>%%H</td><td>Hour of 24-hour clock</td><td>0 - 23</td></tr>
 * <tr><td>%%0H</td><td>Hour of 24-hour clock (padded with leading zeroes)</td><td>00 - 23</td></tr>
 * <tr><td>%%_H</td><td>Hour of 24-hour clock (padded with leading spaces)</td><td>&nbsp;0 - 23</td></tr>
 * <tr><td>%%h</td><td>Hour of 12-hour clock</td><td>1 - 12</td></tr>
 * <tr><td>%%0h</td><td>Hour of 12-hour clock (padded with leading zeroes)</td><td>01 - 12</td></tr>
 * <tr><td>%%_h</td><td>Hour of 12-hour clock (padded with leading spaces)</td><td>&nbsp;1 - 12</td></tr>
 * <tr><td>%%M</td><td>Minutes (unpadded)</td><td>0 - 59</td></tr>
 * <tr><td>%%0M</td><td>Minutes (padded with leading zeroes)</td><td>00 - 59</td></tr>
 * <tr><td>%%_M</td><td>Minutes (padded with leading spaces)</td><td>&nbsp;0 - 59</td></tr>
 * <tr><td>%%S</td><td>Seconds (unpadded)</td><td>0 - 60</td></tr>
 * <tr><td>%%0S</td><td>Seconds (padded with leading zeroes)</td><td>00 - 60</td></tr>
 * <tr><td>%%_S</td><td>Seconds (padded with leading spaces)</td><td>&nbsp;0 - 60</td></tr>
 * <tr><td>%%s</td><td>Milliseconds (unpadded)</td><td>0 - 999</td></tr>
 * <tr><td>%%0s</td><td>Milliseconds (padded with leading zeroes)</td><td>000 - 999</td></tr>
 * <tr><td>%%_s</td><td>Milliseconds (padded with leading spaces)</td><td>&nbsp;&nbsp;0 - 999</td></tr>

 * <tr><td>%%d</td><td>Day of month</td><td>1 - 31</td></tr>
 * <tr><td>%%0d</td><td>Day of month (padded with leading zeroes)</td><td>01 - 31</td></tr>
 * <tr><td>%%_d</td><td>Day of month (padded with leading spaces)</td><td>&nbsp;1 - 31</td></tr>

 * <tr><td>%%D</td><td>Day of year</td><td>1 - 366</td></tr>
 * <tr><td>%%0D</td><td>Day of year (padded with leading zeroes)</td><td>001 - 366</td></tr>
 * <tr><td>%%_D</td><td>Day of year (padded with leading spaces)</td><td>&nbsp;&nbsp;1 - 366</td></tr>

 * <tr><td>%%m</td><td>Month number (1 = January)</td><td>1 - 12</td></tr>
 * <tr><td>%%0m</td><td>Month number (1 = January) (padded with leading zeroes)</td><td>01 - 12</td></tr>
 * <tr><td>%%_m</td><td>Month number (1 = January) (padded with leading spaces)</td><td>&nbsp;1 - 12</td></tr>
 * <tr><td>%$m</td><td>Abbreviated month name</td></tr>
 * <tr><td>%$$m</td><td>Full month name</td></tr>
 * <tr><td>%%w</td><td>Weekday number (0 = Sunday)</td><td>0 - 6</td></tr>
 * <tr><td>%$w</td><td>Abbreviated weekday name</td></tr>
 * <tr><td>%$$w</td><td>Full weekday name</td></tr>

 * <tr><td>%%W</td><td>Week number (ISO standard)</td><td>0 - 52</td></tr>
 * <tr><td>%%0W</td><td>Week number (ISO standard) (padded with leading zeroes)</td><td>00 - 52</td></tr>
 * <tr><td>%%_w</td><td>Week number (ISO standard) (padded with leading spaces)</td><td>&nbsp;0 - 52</td></tr>

 * <tr><td>%%y</td><td>2-digit year </td><td>0 - 99</td></tr>
 * <tr><td>%%0y</td><td>2-digit year (padded with leading zeroes)</td><td>00 - 99</td></tr>
 * <tr><td>%%_y</td><td>2-digit year (padded with leading spaces)</td><td>&nbsp;0 - 99</td></tr>
 * <tr><td>%%Y</td><td>4-digit year </td><td>0 - 9999</td></tr>
 * <tr><td>%%0Y</td><td>4-digit year (padded with leading zeroes)</td><td>0000 - 9999</td></tr>
 * <tr><td>%%_Y</td><td>4-digit year (padded with leading spaces)</td><td>&nbsp;&nbsp;&nbsp;0 - 9999</td></tr>

 * <tr><td>%%P</td><td>Meridiem (uppercase) (local form of "AM" or "PM")</td></tr>
 * <tr><td>%%p</td><td>Meridiem (lowercase) (local form of "am" or "pm")</td></tr>
 * <tr><td>%%t</td><td>%Date</td></tr>
 * <tr><td>%%T</td><td>%Time</td></tr>
 * <tr><td>%%Z</td><td>%Time zone name</td></tr>
 * <tr><td>%%</td><td>Percent sign</td></tr>
 * </table>
 *
 * @author Mark Lindner
 */
class COMMONCPP_API DateTimeFormat
{
 public:

  /**
   * Construct a new DateTimeFormat.
   *
   * @param format The format for dates with times.
   * @param dateFormat The format for dates.
   * @param timeFormat The format for times.
   */
  DateTimeFormat(String format = "%t %T", String dateFormat = "%t",
                 String timeFormat = "%T");

  /** Destructor. */
  ~DateTimeFormat();

  /** Set the format for dates with times. */
  inline void setFormat(const String& format)
  { _parseFormat(format, _dateTimeFormat); }

  /** Set the format for dates. */
  inline void setDateFormat(const String& dateFormat)
  { _parseFormat(dateFormat, _dateFormat); }

  /** Set the format for times. */
  inline void setTimeFormat(const String& timeFormat)
  { _parseFormat(timeFormat, _timeFormat); }

  /**
   * Format a Date.
   *
   * @param value The Date to format.
   * @return The formatted string representation of the date.
   */
  String format(const Date& value) const;

  /**
   * Format a DateTime.
   *
   * @param value The Date to format.
   * @param buf A raw character buffer to write the formatted value to.
   * @param bufsz The size of the character buffer.
   * @return The number of characters written to the buffer.
   */
  size_t format(const Date& value, char* buf, size_t bufsz) const;

  /**
   * Format a Time.
   *
   * @param value The Time to format.
   * @return The formatted string representation of the time.
   */
  String format(const Time& value) const;

  /**
   * Format a DateTime.
   *
   * @param value The Time to format.
   * @param buf A raw character buffer to write the formatted value to.
   * @param bufsz The size of the character buffer.
   * @return The number of characters written to the buffer.
   */
  size_t format(const Time& value, char* buf, size_t bufsz) const;

  /**
   * Format a DateTime.
   *
   * @param value The DateTime to format.
   * @return The formatted string representation of the date & time.
   */
  String format(const DateTime& value) const;

  /**
   * Format a DateTime.
   *
   * @param value The DateTime to format.
   * @param buf A raw character buffer to write the formatted value to.
   * @param bufsz The size of the character buffer.
   * @return The number of characters written to the buffer.
   */
  size_t format(const DateTime& value, char* buf, size_t bufsz) const;

  /**
   * Parse a Date from a string.
   *
   * @param value The value to parse into.
   * @param text The string to parse.
   * @throw ParseException If a parse error occurs.
   */
  void parse(Date& value, const String& text) const;

  /**
   * Parse a Time from a string.
   *
   * @param value The value to parse into.
   * @param text The string to parse.
   * @throw ParseException If a parse error occurs.
   */
  void parse(Time& value, const String& text) const;

  /**
   * Parse a DateTime from a string.
   *
   * @param value The value to parse into.
   * @param text The string to parse.
   * @throw ParseException If a parse error occurs.
   */
  void parse(DateTime& value, const String& text) const;

 private:

  class TokenList; // fwd decl

  static String _monthNames[12];
  static String _monthNamesLong[12];
  static String _weekdayNames[7];
  static String _weekdayNamesLong[7];
  static String _meridiemUpper[2];
  static String _meridiemLower[2];
  static bool _cacheStrings();
  static bool _stringsCached;

  enum TokenCode { TOK_INVALID = -1, TOK_LITERAL = 0, TOK_HOURS,
                   TOK_HOURS12, TOK_MINUTES, TOK_SECONDS, TOK_MSEC,
                   TOK_AMPM_LOWER, TOK_AMPM_UPPER, TOK_MONTH, TOK_DAY,
                   TOK_DAYNUM, TOK_YEAR, TOK_YEAR_4, TOK_WEEKDAY,
                   TOK_WEEKNUM, TOK_TZ, TOK_DATE, TOK_TIME };

  enum TokenFormat { FMT_INVALID = -1, FMT_NUMERIC, FMT_NUMERIC_0,
                     FMT_NUMERIC_SPC, FMT_STRING, FMT_STRING_LONG };

  class Token
  {
   private:

    TokenCode _token;
    TokenFormat _format;
    char* _text;

   public:

    Token(TokenCode token, TokenFormat format, const char* text = NULL,
          int len = 0)
      : _token(token), _format(format)
    {
      if(text != NULL)
      {
        _text = new char[len + 1];
        std::memcpy(_text, text, len);
        _text[len] = NUL;
      }
      else
        _text = NULL;
    }

    ~Token()
    {
      delete[] _text;
    }

    inline TokenCode getType() const
    { return(_token); }

    inline TokenFormat getFormat() const
    { return(_format); }

    inline const char *getText() const
    { return(_text); }

  };

  void _formatNumeric(String& s, uint_t val, uint_t width,
                      TokenFormat format) const;
  void _parseFormat(const String& format, TokenList* tokens);
  String _format(const TokenList* tokens, const DateTime& value) const;
  size_t _format(const TokenList* tokens, const DateTime& value,
                 CStringBuilder& builder) const;
  void _parse(const TokenList* tokens, DateTime& value, const char* buf) const;

  int _parseInt(const char*& p, uint_t& pos, int min, int max) const;
  int _parseString(const char*& p, uint_t& pos, const String* list,
                   size_t count) const;
  void _skip(const char*& p, uint_t& pos, uint_t count) const;

  TokenList* _dateFormat;
  TokenList* _timeFormat;
  TokenList* _dateTimeFormat;
};

} // namespace ccxx

#endif // __ccxx_DateTimeFormat_hxx
