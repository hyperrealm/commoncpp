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

#include "commonc++/Mutex.h++"

#ifdef CCXX_OS_POSIX
#include "commonc++/POSIX.h++"
#endif

namespace ccxx {

/*
 */

Mutex::Mutex(bool recursive /* = false */)
  : _recursive(recursive)
{
#ifdef CCXX_OS_WINDOWS

  if(_recursive)
    _mutex = ::CreateMutex(NULL, FALSE, NULL);
  else
    _mutex = ::CreateEvent(NULL, FALSE, TRUE, NULL);

#else

  pthread_mutexattr_t attr;
  ::pthread_mutexattr_init(&attr);

#if (defined CCXX_OS_MACOSX) || (defined CCXX_OS_ANDROID)
  ::pthread_mutexattr_settype(&attr, (_recursive
                                      ? PTHREAD_MUTEX_RECURSIVE
                                      : PTHREAD_MUTEX_DEFAULT));
#elif defined CCXX_OS_POSIX
  ::pthread_mutexattr_settype(&attr, (_recursive
                                      ? PTHREAD_MUTEX_RECURSIVE_NP
                                      : PTHREAD_MUTEX_TIMED_NP));
#endif

  ::pthread_mutex_init(&_mutex, &attr);

#endif
}

/**
 */

Mutex::~Mutex()
{
#ifdef CCXX_OS_WINDOWS

  if(_mutex != NULL)
    ::CloseHandle(_mutex);

#else

  ::pthread_mutex_destroy(&_mutex);

#endif
}

/**
 */

void Mutex::lock()
{
#ifdef CCXX_OS_WINDOWS

  for(;;)
  {
    if(::WaitForSingleObjectEx(_mutex, INFINITE, TRUE) == WAIT_OBJECT_0)
      break;
  }

#else

  ::pthread_mutex_lock(&_mutex);

#endif
}

/**
 */

bool Mutex::tryLock(timespan_ms_t timeout /* = 0 */)
{
  if(timeout < 0)
    timeout = 0;

#ifdef CCXX_OS_WINDOWS

  return(::WaitForSingleObjectEx(_mutex, timeout, TRUE) == WAIT_OBJECT_0);

#else

  if(timeout == 0)
    return(::pthread_mutex_trylock(&_mutex) == 0);
  else
  {
#ifdef HAVE_PTHREAD_MUTEX_TIMEDLOCK

    struct timespec tspec;
    POSIX::timespecForDelta(timeout, tspec);

    return(::pthread_mutex_timedlock(&_mutex, &tspec) == 0);

#else

    return(::pthread_mutex_trylock(&_mutex) == 0);

#endif // HAVE_PTHREAD_MUTEX_TIMEDLOCK
  }

#endif // CCXX_OS_WINDOWS
}

/**
 */

void Mutex::unlock()
{
#ifdef CCXX_OS_WINDOWS

  if(_recursive)
    ::ReleaseMutex(_mutex);
  else
    ::SetEvent(_mutex);

#else

  ::pthread_mutex_unlock(&_mutex);

#endif
}

/*
 */

bool Mutex::supportsTimedLocks()
{
#ifdef HAVE_PTHREAD_MUTEX_TIMEDLOCK

  return(true);

#else

  return(false);

#endif
}


} // namespace ccxx
