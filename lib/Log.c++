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

#include "commonc++/Log.h++"
#include "commonc++/ScopedLock.h++"

namespace ccxx {

/*
 */

ConsoleLogger *Log::_consoleLog = new ConsoleLogger();

FileLogger *Log::_fileLog = new FileLogger();

CriticalSection Log::_lock;

bool Log::_useConsoleLog = true;

bool Log::_useFileLog = false;

/* temporary hackery */

static bool disable_cleanup = false;

void Log::disableCleanup()
{
  disable_cleanup = true;
}

/*
 */

static void __cleanup_logger_singletons(void)
{
  if(! disable_cleanup)
  {
    Log::setFileLogger(NULL);
    Log::setConsoleLogger(NULL);
  }
}

/*
 */

CCXX_STATIC_INITIALIZER
{
  ::atexit(__cleanup_logger_singletons);
}

/*
 */

void Log::setLogFile(const String& dir, const String& name)
{
  synchronized(_lock)
  {
    if(_fileLog)
      _fileLog->setFile(dir, name);
  }
}

/*
 */

void Log::setLogFileMaxSize(size_t maxLogSize)
{
  synchronized(_lock)
  {
    if(_fileLog)
      _fileLog->setMaxLogSize(maxLogSize);
  }
}

/*
 */

void Log::setLogFileRotateCount(uint_t rotateCount)
{
  synchronized(_lock)
  {
    if(_fileLog)
      _fileLog->setRotateCount(rotateCount);
  }
}

/*
 */

void Log::setFileLogFormat(const String &format)
{
  synchronized(_lock)
  {
    if(_fileLog)
      _fileLog->setFormat(format);
  }
}

/*
 */

void Log::setConsoleLogFormat(const String &format)
{
  synchronized(_lock)
  {
    if(_consoleLog)
      _consoleLog->setFormat(format);
  }
}

/*
 */

void Log::setFileLogger(FileLogger *logger)
{
  synchronized(_lock)
  {
    if(_fileLog)
      delete _fileLog;

    _fileLog = logger;
  }
}

/*
 */

void Log::setConsoleLogger(ConsoleLogger *logger)
{
  synchronized(_lock)
  {
    if(_consoleLog)
      delete _consoleLog;

    _consoleLog = logger;
  }
}

/*
 */

void Log::log(LogLevel level, const char *file, int line, const char *message,
              ...)
{
  synchronized(_lock)
  {
    if(_useConsoleLog && _consoleLog)
    {
      va_list vp;

      va_start(vp, message);
      _consoleLog->vlog(level, file, line, message, vp);
      va_end(vp);
    }

    if(_useFileLog && _fileLog)
    {
      va_list vp;

      va_start(vp, message);
      _fileLog->vlog(level, file, line, message, vp);
      va_end(vp);
    }
  }
}

/*
 */

void Log::vlogFile(LogLevel level, const char *file, int line,
                   const char *message, va_list args)
{
  synchronized(_lock)
  {
    if(_useFileLog && _fileLog)
      _fileLog->vlog(level, file, line, message, args);
  }
}

/*
 */

void Log::vlogConsole(LogLevel level, const char *file, int line,
                      const char *message, va_list args)
{
  synchronized(_lock)
  {
    if(_useConsoleLog && _consoleLog)
      _consoleLog->vlog(level, file, line, message, args);
  }
}

/*
 */

bool Log::assert_(const char *file, int line, const char *expr)
{
  log(LogError, file, line, "Assertion failed: %s", expr);
  ::abort();

  return(true);
}

/*
 */

LogFunctor::LogFunctor(const char *file, int line,
                       LogLevel level /* = Log_debug */,
                       bool nodebug /* = false */)
  : _file(file),
    _line(line),
    _level(level),
    _nodebug(nodebug)
{
}

/*
 */

LogFunctor::~LogFunctor()
{
}

/*
 */

void LogFunctor::operator()(const char *fmt, ...)
{
  if((_level == LogDebug) && _nodebug)
    return;

  ScopedLock guard(Log::_lock);

  if(Log::_useConsoleLog && Log::_consoleLog)
  {
    va_list vp;

    va_start(vp, fmt);
    Log::_consoleLog->vlog(_level, _file, _line, fmt, vp);
    va_end(vp);
  }

  if(Log::_useFileLog && Log::_fileLog)
  {
    va_list vp;

    va_start(vp, fmt);
    Log::_fileLog->vlog(_level, _file, _line, fmt, vp);
    va_end(vp);
  }
}

/*
 */

void LogFunctor::operator()(LogLevel level, const char *fmt, ...)
{
  if((level == LogDebug) && _nodebug)
    return;

  ScopedLock guard(Log::_lock);

  if(Log::_useConsoleLog && Log::_consoleLog)
  {
    va_list vp;

    va_start(vp, fmt);
    Log::_consoleLog->vlog(_level, _file, _line, fmt, vp);
    va_end(vp);
  }

  if(Log::_useFileLog && Log::_fileLog)
  {
    va_list vp;

    va_start(vp, fmt);
    Log::_fileLog->vlog(_level, _file, _line, fmt, vp);
    va_end(vp);
  }
}


} // namespace ccxx
