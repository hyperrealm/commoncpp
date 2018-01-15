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

#ifndef __ccxx_LogFormat_hxx
#define __ccxx_LogFormat_hxx

#include <commonc++/Common.h++>
#include <commonc++/Buffer.h++>
#include <commonc++/DateTimeFormat.h++>
#include <commonc++/String.h++>

#include <cstdio>
#include <cstdarg>

#ifdef CCXX_OS_POSIX
#include <unistd.h>
#include <sys/utsname.h>
#endif

#include <vector>

namespace ccxx {

/** Logging levels. */
enum LogLevel {
  /** Debugging messages. */
  LogDebug = 1 << 0,
  /** Informational messages. */
  LogInfo = 1 << 1,
  /** Warning messages. */
  LogWarning = 1 << 2,
  /** Error messages. */
  LogError = 1 << 3
};

/**
 * A log message formatter. A format string specifies the format
 * for log messages; it may contain format directives which allow
 * various bits of information to be inserted into the message, and
 * which affect the appearance of the message itself, such as font
 * and color. The following table describes all of the available
 * format directives.
 *
 * <table cellpadding=3 cellspacing=0 border=1>
 * <tr><th>Directive</th><th align=left>Meaning</th></tr>
 * <tr><td>\%T</td>
 * <td>The current time, in long format.</td>
 * </tr>
 * <tr><td>\%t</td>
 * <td>The current time, in short format.</td>
 * </tr>
 * <tr><td>\%D</td>
 * <td>The current date, in long format.</td>
 * </tr>
 * <tr><td>\%d</td>
 * <td>The current date, in short format.</td>
 * </tr>
 * <tr><td>\%F</td>
 * <td>The filename of the current source file.</td>
 * </tr>
 * <tr><td>\%L</td>
 * <td>The line number in the current source file.</td>
 * </tr>
 * <tr><td>\%m</td>
 * <td>The log message proper.</td>
 * </tr>
 * <tr><td>\%h</td>
 * <td>The ID of the calling thread.</td>
 * </tr>
 * <tr><td>\%p</td>
 * <td>The process ID.</td>
 * </tr>
 * <tr><td>\%o</td>
 * <td>The name of the host operating system.</td>
 * </tr>
 * <tr><td>\%a</td>
 * <td>The architecture of the host system.</td>
 * </tr>
 * <tr><td>\%v</td>
 * <td>The version number of the host operating system.</td>
 * </tr>
 * <tr><td>\%u</td>
 * <td>The username of the current user.</td>
 * </tr>
 * <tr><td>\%H</td>
 * <td>The hostname.</td>
 * </tr>
 * <tr><td>\%A</td>
 * <td>The application name.</td>
 * </tr>
 * <tr><td>\%l</td>
 * <td>The log level; one of the characters D, I, W, E.</td>
 * </tr>
 * <tr><td>\%%</td>
 * <td>A literal \% character.</td>
 * </tr>
 * <tr><td>\%*</td>
 * <td>Begin bold text.</td>
 * </tr>
 * <tr><td>\%_</td>
 * <td>Begin underlined text.</td>
 * </tr>
 * <tr><td>\%#</td>
 * <td>Begin inverse text.</td>
 * </tr>
 * <tr><td>\%-</td>
 * <td>Begin plain text (all other attributes off).</td>
 * </tr>
 * <tr><td>\%0</td>
 * <td>Change text foreground color to default.</td>
 * </tr>
 * <tr><td>\%1</td>
 * <td>Change text foreground color to black.</td>
 * </tr>
 * <tr><td>\%2</td>
 * <td>Change text foreground color to red.</td>
 * </tr>
 * <tr><td>\%3</td>
 * <td>Change text foreground color to green.</td>
 * </tr>
 * <tr><td>\%4</td>
 * <td>Change text foreground color to yellow.</td>
 * </tr>
 * <tr><td>\%5</td>
 * <td>Change text foreground color to blue.</td>
 * </tr>
 * <tr><td>\%6</td>
 * <td>Change text foreground color to magenta.</td>
 * </tr>
 * <tr><td>\%7</td>
 * <td>Change text foreground color to cyan.</td>
 * </tr>
 * <tr><td>\%8</td>
 * <td>Change text foreground color to white.</td>
 * </tr>
 * <tr><td>\%.</td>
 * <td>Turn all text styles off.</td>
 * </tr>
 * <tr><td>\%\@</td>
 * <td>Turn on text autocolor.</td>
 * </tr>
 * <tr><td>\%n</td>
 * <td>A newline character.</td>
 * </tr>
 * </table>
 *
 * @author Mark Lindner
 */
class COMMONCPP_API LogFormat
{
 private:

  class TokenList; // fwd decl

  enum TokenCode { TOK_INVALID = -1, TOK_LITERAL = 0, TOK_TIME_LONG,
                   TOK_TIME_SHORT, TOK_DATE_LONG, TOK_DATE_SHORT, TOK_FILE,
                   TOK_LINE, TOK_MESSAGE, TOK_THREAD, TOK_PID, TOK_OSNAME,
                   TOK_ARCH, TOK_OSVER, TOK_USERNAME, TOK_HOST, TOK_LEVEL,
                   TOK_PERCENT, TOK_BOLD, TOK_UNDERLINE, TOK_INVERSE,
                   TOK_PLAIN, TOK_DEFAULT, TOK_BLACK, TOK_RED, TOK_GREEN,
                   TOK_YELLOW, TOK_BLUE, TOK_MAGENTA, TOK_CYAN, TOK_WHITE,
                   TOK_STYLESOFF, TOK_AUTOCOLOR, TOK_NEWLINE, TOK_APPNAME,
                   TOK_MAX };

  TokenCode lookupToken(char c);
  void clearTokens();

  struct Token
  {
    Token(TokenCode token = TOK_LITERAL, const char* text = NULL, int len = 0);
    ~Token();

    TokenCode _token;
    char* _text;
  };

  String _format;
  TokenList* _tokens;

 public:

  /**
   * Construct a new LogFormat.
   *
   * @param format The log message format.
   */
  LogFormat(String format = "[%D %T] %F:%L %m");

  /** Destructor. */
  virtual ~LogFormat();

  /** Set the log message format. */
  void setFormat(const String& format);

  /**
   * Format a log message and write it to a buffer. This method
   * is not meant to be used directly; it is called transparently by
   * the Log class when one of the logging macros is used.
   *
   * @param buffer The output buffer for the resulting message.
   * @param level The logging (severity) level for the message.
   * @param file The source file of the originating log statement.
   * @param line The source line of the originating log statement.
   * @param message The formatted message.
   * @param args Optional message arguments.
   */
  void format(CharBuffer& buffer, LogLevel level, const char* file,
              int line, const char* message, va_list args);

  /** Set the short format for dates. See DateTimeFormat. */
  void setShortDateFormat(const String& format);

  /** Set the long format for dates. See DateTimeFormat. */
  void setLongDateFormat(const String& format);

  /** Set the short format for times. See DateTimeFormat. */
  void setShortTimeFormat(const String& format);

  /** Set the long format for times. See DateTimeFormat. */
  void setLongTimeFormat(const String& format);

 private:

  static size_t _eolLen;

  DateTimeFormat _shortDateFormat;
  DateTimeFormat _longDateFormat;
  DateTimeFormat _shortTimeFormat;
  DateTimeFormat _longTimeFormat;

  CCXX_COPY_DECLS(LogFormat);
};

} // namespace ccxx

#endif // __ccxx_LogFormat_hxx
