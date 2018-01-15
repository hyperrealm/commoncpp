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

#ifndef __ccxx_Logger_hxx
#define __ccxx_Logger_hxx

#include <commonc++/Common.h++>
#include <commonc++/Buffer.h++>
#include <commonc++/LogFormat.h++>
#include <commonc++/String.h++>

#include <cstdarg>

namespace ccxx {

/**
 * An abstract base class for threadsafe loggers.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Logger
{
 public:

  /**
   * Construct a new Logger with the given log message format.
   *
   * @param format The log message format.
   */
  Logger(const String& format);

  /** Destructor. */
  virtual ~Logger();

  /**
   * Set the log message format.
   *
   * @param format The log message format.
   */
  void setFormat(const String& format);

  /**
   * %Log a message.
   *
   * @param level The log level (severity).
   * @param file The source filename.
   * @param line The source file line number.
   * @param message The log message.
   * @param ... Optional message arguments.
   */
  void log(LogLevel level, const char* file, int line, const char* message,
           ...);

  /**
   * %Log a message.
   *
   * @param level The log level (severity).
   * @param file The source filename.
   * @param line The source file line number.
   * @param message The log message.
   * @param args Optional message arguments.
   */
  void vlog(LogLevel level, const char* file, int line, const char* message,
            va_list args);

  /**
   * Enable a specific log level.
   *
   * @param level The level to enable.
   */
  void enableLogLevel(LogLevel level);

  /**
   * Disable a specific log level.
   *
   * @param level The level to disable.
   */
  void disableLogLevel(LogLevel level);

  /** Test if a specific log level is enabled. */
  bool isLogLevelEnabled(LogLevel level) const;

  /** Get the LogFormat used by this Logger. */
  inline LogFormat &getLogFormat()
  { return(_format); }

  /**
   * The log buffer size. Indicates the maximum length of a log message,
   * in characters. Messages which exceed this length will be truncated.
   */
  static const size_t LOG_BUFFER_SIZE;

 protected:

  /**
   * Write a formatted log message to the logging endpoint.
   *
   * @param buffer The buffer containing the log message.
   * @return <b>true</b> if the message was written successfully,
   * <b>false</b> otherwise.
   */
  virtual bool write(CharBuffer& buffer) = 0;

 private:

  CharBuffer _buffer;
  LogFormat _format;
  uint_t _levelMask;

  CCXX_COPY_DECLS(Logger);
};

} // namespace ccxx

#endif // __ccxx_Logger_hxx
