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

#include "commonc++/ConditionVar.h++"
#include "commonc++/Mutex.h++"
#include "commonc++/System.h++"

#ifdef CCXX_OS_POSIX
#include "commonc++/POSIX.h++"
#endif

namespace ccxx {

const uint_t ConditionVar::FOREVER = 0;

/*
 */

ConditionVar::ConditionVar()
#ifdef CCXX_OS_WINDOWS
  : _waitersCount(0),
    _broadcast(0)
#endif
{
#ifdef CCXX_OS_WINDOWS

  _sem = ::CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL);
  ::InitializeCriticalSection(&_waitersLock);
  _waitersDone = ::CreateEvent(NULL, FALSE, FALSE, NULL);

#else

  ::pthread_cond_init(&_cond, NULL);

#endif
}

/*
 */

ConditionVar::~ConditionVar()
{
#ifdef CCXX_OS_WINDOWS

  ::CloseHandle(_waitersDone);
  ::DeleteCriticalSection(&_waitersLock);
  ::CloseHandle(_sem);

#else

  while(::pthread_cond_destroy(&_cond) == EBUSY)
  {
    ::pthread_cond_broadcast(&_cond);
    ::sched_yield();
  }

#endif
}

/*
 */

bool ConditionVar::wait(Mutex& mutex, uint_t msec /* = FOREVER */)
{
#ifdef CCXX_OS_WINDOWS

  // avoid race conditions

  ::EnterCriticalSection(&_waitersLock);
  _waitersCount++;
  ::LeaveCriticalSection(&_waitersLock);

  // atomically release the mutex and wait on the semaphore
  // until notify() or notifyAll() is called by another thread

  DWORD r = ::SignalObjectAndWait(mutex._mutex, _sem,
                                  (msec == FOREVER ? INFINITE : msec),
                                  FALSE);

  // require lock to avoid race conditions
  ::EnterCriticalSection(&_waitersLock);
  _waitersCount--; // we're no longer waiting

  // if the wait timed out, just leave

  if(r == WAIT_TIMEOUT)
  {
    if(_waitersCount == 0)
      ::SetEvent(_waitersDone);

    ::LeaveCriticalSection(&_waitersLock);

    return(false);
  }

  // see if we're the last waiter after notifyAll()
  bool last = (_broadcast && (_waitersCount == 0));

  ::LeaveCriticalSection(&_waitersLock);

  // if we were last, then let all other threads proceed
  if(last)
  {
    // atomically signal _waitersDone event and wait until we can acquire
    // the mutex; this is done to ensure fairness

    ::SignalObjectAndWait(_waitersDone, mutex._mutex, INFINITE, FALSE);
  }
  else
    // reacquire the mutex
    for(;;)
    {
      if(::WaitForSingleObjectEx(mutex._mutex, INFINITE, TRUE)
         != WAIT_IO_COMPLETION)
        break;
    }

  return(true);

#else

  if(msec != FOREVER)
  {
    struct timespec tspec;
    POSIX::timespecForDelta(msec, tspec);

    // assume errno == ETIMEDOUT on failure
    return(::pthread_cond_timedwait(&_cond, &mutex._mutex, &tspec) == 0);
  }
  else
  {
    ::pthread_cond_wait(&_cond, &mutex._mutex);

    return(true);
  }

#endif
}

/*
 */

void ConditionVar::notify()
{
#ifdef CCXX_OS_WINDOWS

  ::EnterCriticalSection(&_waitersLock);
  bool haveWaiters = (_waitersCount > 0);
  ::LeaveCriticalSection(&_waitersLock);

  // if there aren't any waiters, this is a no-op
  if(haveWaiters)
    ::ReleaseSemaphore(_sem, 1, 0);

#else

  ::pthread_cond_signal(&_cond);

#endif
}

/*
 */

void ConditionVar::notifyAll()
{
#ifdef CCXX_OS_WINDOWS

  // this is needed to ensure that _waitersCount and _broadcast are
  // consistent relative to each other

  ::EnterCriticalSection(&_waitersLock);
  bool haveWaiters = false;

  if(_waitersCount > 0)
  {
    _broadcast = true;
    haveWaiters = true;
  }

  if(haveWaiters)
  {
    // wake up all the waiters atomically

    ::ReleaseSemaphore(_sem, _waitersCount, NULL);
    ::LeaveCriticalSection(&_waitersLock);

    // wait for all awakened threads to acquire the counting semaphore

    for(;;)
    {
      if(::WaitForSingleObjectEx(_waitersDone, INFINITE, TRUE)
         != WAIT_IO_COMPLETION)
         break;
    }

    // this assignment is OK, even without _waitersLock held,
    // because no other waiter threads can wake up to access it

    _broadcast = false;
  }
  else
    ::LeaveCriticalSection(&_waitersLock);

#else

  ::pthread_cond_broadcast(&_cond);

#endif
}


} // namespace ccxx
