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

#include "commonc++/StopWatch.h++"
#include "commonc++/System.h++"

#ifdef CCXX_OS_WINDOWS
#include "commonc++/Windows.h++"
#endif

#ifdef CCXX_OS_POSIX
#include <unistd.h>
#include <sys/times.h>
#endif

namespace ccxx {

/*
 */

#ifdef CCXX_OS_POSIX

static long __msPerTick = 1000 / ::sysconf(_SC_CLK_TCK);

#endif

/*
 */

StopWatch::StopWatch()
  : _userTime(0),
    _kernelTime(0),
    _realTime(0),
    _userElapsed(0),
    _kernelElapsed(0),
    _realElapsed(0),
    _running(false)
{
}

/*
 */

StopWatch::~StopWatch()
{
}

/*
 */

void StopWatch::start()
{
  if(! _running)
  {
#ifdef CCXX_OS_WINDOWS

    FILETIME cTime, eTime, kTime, uTime;

    ::GetProcessTimes(::GetCurrentProcess(), &cTime, &eTime, &kTime, &uTime);

    _userTime = Windows::decodeFileTime(uTime);
    _kernelTime = Windows::decodeFileTime(kTime);
    _realTime = System::currentTimeMillis();

#else

    struct tms buf;
    ::times(&buf);

    _userTime = buf.tms_utime * __msPerTick;
    _kernelTime = buf.tms_stime * __msPerTick;

    struct timespec ts;
    ::clock_gettime(CLOCK_REALTIME, &ts);

    _realTime = (ts.tv_sec * 1000) + (ts.tv_nsec / 1E6);

#endif

    _running = true;
  }
}

/*
 */

void StopWatch::stop()
{
  if(_running)
  {

#ifdef CCXX_OS_WINDOWS

    FILETIME cTime, eTime, kTime, uTime;

    ::GetProcessTimes(::GetCurrentProcess(), &cTime, &eTime, &kTime, &uTime);

    _userElapsed += (Windows::decodeFileTime(uTime) - _userTime);
    _kernelElapsed += (Windows::decodeFileTime(kTime) - _kernelTime);
    _realElapsed += (System::currentTimeMillis() - _realTime);

#else

    struct tms buf;
    ::times(&buf);

    _userElapsed += (buf.tms_utime * __msPerTick) - _userTime;
    _kernelElapsed += (buf.tms_stime * __msPerTick) - _kernelTime;

    struct timespec ts;
    ::clock_gettime(CLOCK_REALTIME, &ts);

    time_ms_t now = (ts.tv_sec * 1000) + (ts.tv_nsec / 1E6);
    _realElapsed += (now - _realTime);

#endif

    _userTime = _kernelTime = _realTime = 0;

    _running = false;
  }
}

/*
 */

void StopWatch::reset()
{
  _running = false;

  _userTime = _kernelTime = _realTime = 0;
  _userElapsed = _kernelElapsed = _realElapsed = 0;
}


} // namespace ccxx
