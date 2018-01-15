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

#include "commonc++/Logger.h++"

namespace ccxx {

const size_t Logger::LOG_BUFFER_SIZE = 1024;

/*
 */

Logger::Logger(const String &format)
  : _buffer(LOG_BUFFER_SIZE),
    _format(format),
    _levelMask(LogDebug | LogInfo | LogWarning | LogError)
{
}

/*
 */

Logger::~Logger()
{
}

/*
 */

void Logger::setFormat(const String &format)
{
  _format.setFormat(format);
}

/*
 */

void Logger::log(LogLevel level, const char *file, int line,
                 const char *message, ...)
{
  if(_levelMask & level)
  {
    va_list vp;

    va_start(vp, message);
    vlog(level, file, line, message, vp);
    va_end(vp);
  }
}

/*
 */

void Logger::vlog(LogLevel level, const char *file, int line,
                  const char *message, va_list args)
{
  if(_levelMask & level)
  {
    _buffer.clear();
    _format.format(_buffer, level, file, line, message, args);

    _buffer.flip();
    write(_buffer);
  }
}

/*
 */

void Logger::enableLogLevel(LogLevel level)
{
  _levelMask |= level;
}

/*
 */

void Logger::disableLogLevel(LogLevel level)
{
  _levelMask &= ~level;
}

/*
 */

bool Logger::isLogLevelEnabled(LogLevel level) const
{
  return((_levelMask & level) != 0);
}


} // namespace ccxx
