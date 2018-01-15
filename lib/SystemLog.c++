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

#include "commonc++/SystemLog.h++"
#include "commonc++/Process.h++"

#ifdef CCXX_OS_POSIX
#include <syslog.h>
#endif

#ifdef CCXX_OS_WINDOWS
#include "cbits.h"
#endif

#include <cstdio>

namespace ccxx {

SystemLog::SystemLog()
#ifdef CCXX_OS_WINDOWS
  : _log(NULL)
#endif
{
}

/*
 */

SystemLog::~SystemLog()
{
  close();
}

/*
 */

void SystemLog::open()
{
  CString cstr_progname = Process::getProgramName().toUTF8();

#ifdef CCXX_OS_WINDOWS

  _log = ::RegisterEventSource(NULL, cstr_progname.data());

#else

  ::openlog(cstr_progname.data(), LOG_PID, LOG_USER);

#endif
}

/*
 */

void SystemLog::close()
{
#ifdef CCXX_OS_WINDOWS

  if(_log != NULL)
  {
    ::DeregisterEventSource(_log);
    _log = NULL;
  }

#else

  ::closelog();

#endif
}

/*
 */

void SystemLog::logMessage(LogSeverity severity, const char* msg, va_list args)
{
#ifdef CCXX_OS_WINDOWS

  if(! _log)
    return;

    LPCSTR list[1];
    char buf[256];
    list[0] = buf;

    ::vsnprintf(buf, sizeof(buf), msg, args);
    buf[sizeof(buf) - 1] = 0;

    int type;

    switch(severity)
    {
      case LogInfo:
        type = EVENTLOG_INFORMATION_TYPE;
        break;

      case LogWarning:
        type = EVENTLOG_WARNING_TYPE;
        break;

      case LogError:
        type = EVENTLOG_ERROR_TYPE;
        break;
    }

    ::ReportEvent(_log, type, 0, 0, NULL, 1, 0, list, NULL);

#else

  int prio = LogInfo;

  switch(severity)
  {
    case LogInfo:
      prio = LOG_INFO;
      break;

    case LogWarning:
      prio = LOG_WARNING;
      break;

    case LogError:
      prio = LOG_ERR;
      break;
  }

  ::vsyslog(prio, msg, args);

#endif
}

/*
 */

void SystemLog::logMessage(LogSeverity severity, const char* msg, ...)
{
  va_list vp;
  va_start(vp, msg);

  logMessage(severity, msg, vp);

  va_end(vp);
}


} // namespace ccxx
