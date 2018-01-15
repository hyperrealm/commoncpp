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

#ifndef __ccxx_SystemLog_hxx
#define __ccxx_SystemLog_hxx

#include <commonc++/Common.h++>

#include <cstdarg>

namespace ccxx {

/**
 * The system event log. This class logs events to <b>syslog</b> on
 * POSIX systems and to the Windows event log on Windows systems. This
 * class is implicitly a singleton.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API SystemLog
{
 public:

  /** %System logging severity. */
  enum LogSeverity { LogInfo, LogWarning, LogError };

  /** Construct a new SystemLog. */
  SystemLog();

  /** Destructor. */
  ~SystemLog();

  /** Open the log. */
  void open();

  /** Close the log. */
  void close();

  /**
   * %Log a message.
   *
   * @param severity The message severity.
   * @param msg The message.
   * @param args Optional message arguments.
   */
  void logMessage(LogSeverity severity, const char* msg, va_list args);

  /**
   * %Log a message.
   *
   * @param severity The message severity.
   * @param msg The message.
   * @param ... Optional message arguments.
   */
  void logMessage(LogSeverity severity, const char* msg, ...);

 private:

#ifdef CCXX_OS_WINDOWS
  HANDLE _log;
#endif
};

} // namespace ccxx

#endif // __ccxx_SystemLog_hxx
