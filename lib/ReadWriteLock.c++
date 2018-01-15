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

#include "commonc++/ReadWriteLock.h++"
#include "commonc++/System.h++"

#ifdef CCXX_OS_POSIX
#include "commonc++/POSIX.h++"
#endif

namespace ccxx {

/*
 */

ReadWriteLock::ReadWriteLock()
#ifdef CCXX_OS_WINDOWS
  : _readersReading(0),
    _writerWriting(0)
#endif
{
#ifdef CCXX_OS_POSIX

  pthread_rwlock_init(&_lock, NULL);

#endif
}

/*
 */

ReadWriteLock::~ReadWriteLock()
{
#ifdef CCXX_OS_POSIX

  ::pthread_rwlock_destroy(&_lock);

#endif
}

/*
 */

void ReadWriteLock::lockRead()
{
#ifdef CCXX_OS_WINDOWS

  _mutex.lock();
  while(_writerWriting)
    _lockFree.wait(_mutex);

  _readersReading++;
  _mutex.unlock();

#else

  pthread_rwlock_rdlock(&_lock);

#endif
}

/*
 */

bool ReadWriteLock::tryLockRead(timespan_ms_t timeout /* = 0 */)
{
  if(timeout < 0)
    timeout = 0;

#ifdef CCXX_OS_WINDOWS

  if(! _mutex.tryLock(timeout))
    return(false);

  time_ms_t expires = System::currentTimeMillis() + timeout;

  while(_writerWriting)
  {
    uint_t left = 0;
    time_ms_t now = System::currentTimeMillis();
    if(now < expires)
      left = (expires - now);

    if(! _lockFree.wait(_mutex, left))
    {
      _mutex.unlock();
      return(false);
    }
  }

  _readersReading++;
  _mutex.unlock();

  return(true);

#else

  if(timeout == 0)
    return(::pthread_rwlock_tryrdlock(&_lock) == 0);
  else
  {
#ifdef HAVE_PTHREAD_RWLOCK_TIMEDRDLOCK

    struct timespec tspec;
    POSIX::timespecForDelta(timeout, tspec);

    return(::pthread_rwlock_timedrdlock(&_lock, &tspec) == 0);

#else

    return(::pthread_rwlock_tryrdlock(&_lock) == 0);

#endif // HAVE_PTHREAD_RWLOCK_TIMEDRDLOCK
  }

#endif
}

/*
 */

void ReadWriteLock::lockWrite()
{
#ifdef CCXX_OS_WINDOWS

  _mutex.lock();

  while(_writerWriting || _readersReading)
    _lockFree.wait(_mutex);

  _writerWriting++;

  _mutex.unlock();

#else

  ::pthread_rwlock_wrlock(&_lock);

#endif
}

/*
 */

bool ReadWriteLock::tryLockWrite(timespan_ms_t timeout /* = 0 */)
{
  if(timeout < 0)
    timeout = 0;

#ifdef CCXX_OS_WINDOWS

  if(! _mutex.tryLock(timeout))
    return(false);

  time_ms_t expires = System::currentTimeMillis() + timeout;

  while(_writerWriting || _readersReading)
  {
    uint_t left = 0;
    time_ms_t now = System::currentTimeMillis();
    if(now < expires)
      left = (expires - now);

    if(! _lockFree.wait(_mutex, left))
    {
      _mutex.unlock();
      return(false);
    }
  }

  _writerWriting++;

  _mutex.unlock();

  return(true);

#else

  if(timeout == 0)
    return(::pthread_rwlock_trywrlock(&_lock) == 0);
  else
  {
#ifdef HAVE_PTHREAD_RWLOCK_TIMEDWRLOCK

    struct timespec tspec;
    POSIX::timespecForDelta(timeout, tspec);

    return(::pthread_rwlock_timedwrlock(&_lock, &tspec) == 0);

#else

    return(::pthread_rwlock_trywrlock(&_lock) == 0);

#endif // HAVE_PTHREAD_RWLOCK_TIMEDWRLOCK
  }

#endif
}

/*
 */

void ReadWriteLock::unlock()
{
#ifdef CCXX_OS_WINDOWS

  _mutex.lock();

  if(_readersReading > 0) // unlock read lock
  {
    if(--_readersReading == 0)
      _lockFree.notify();
  }
  else if(_writerWriting > 0) // unlock write lock
  {
    _writerWriting = 0;
    _lockFree.notifyAll();
  }

  _mutex.unlock();

#else

  ::pthread_rwlock_unlock(&_lock);

#endif
}

/*
 */

bool ReadWriteLock::supportsTimedLocks()
{
#ifdef HAVE_PTHREAD_RWLOCK_TIMEDRDLOCK

  return(true);

#else

  return(false);

#endif
}


} // namespace ccxx
