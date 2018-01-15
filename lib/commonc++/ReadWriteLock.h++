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

#ifndef __ccxx_ReadWriteLock_hxx
#define __ccxx_ReadWriteLock_hxx

#include <commonc++/Common.h++>

#ifdef CCXX_OS_WINDOWS
#include <commonc++/ConditionVar.h++>
#include <commonc++/Mutex.h++>
#endif

#ifdef CCXX_OS_POSIX
#include <pthread.h>
#endif

namespace ccxx {

/**
 * A Read/Write lock -- a synchronization primitive that allows multiple
 * threads to coordinate access to a mutable resource. Any number of threads
 * can simultaneously hold the read lock as long as no thread holds the
 * write lock. Conversely, a thread can acquire the write lock as long as
 * no threads are holding the read lock.
 *
 * See also ScopedReadLock and ScopedWriteLock.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API ReadWriteLock
{
 public:

  /** Construct a new ReadWriteLock. */
  ReadWriteLock();

  /** Destructor. */
  ~ReadWriteLock();

  /** Acquire a read lock, blocking until the lock is acquired. */
  void lockRead();

  /**
   * Try to acquire a read lock, returning if the lock could not be
   * acquired within the given amount of time. On platforms that do
   * not support timed read/write locks, the method returns
   * immediately if the lock could not be acquired.
   *
   * @param timeout The timeout, in milliseconds.
   * @return <b>true</b> if the lock was acquired, <b>false</b> otherwise.
   */
  bool tryLockRead(timespan_ms_t timeout = 0);

  /** Acquire a write lock, blocking until the lock is acquired. */
  void lockWrite();

  /**
   * Try to acquire a write lock, returning if the lock could not
   * be acquired within the given amount of time. On platforms that
   * do not support timed read/write locks, the method returns
   * immediately if the lock could not be acquired.
   *
   * @param timeout The timeout, in milliseconds.
   * @return <b>true</b> if the lock was acquired, <b>false</b> otherwise.
   */
  bool tryLockWrite(timespan_ms_t timeout = 0);

  /** Release the lock. */
  void unlock();

  /** Determine if the host system supports timed read/write locks. */
  static bool supportsTimedLocks();

 private:

#ifdef CCXX_OS_WINDOWS
  int _readersReading;
  int _writerWriting;
  Mutex _mutex;
  ConditionVar _lockFree;
#else
  pthread_rwlock_t _lock;
#endif

  CCXX_COPY_DECLS(ReadWriteLock);
};

} // namespace ccxx

#endif // __ccxx_ReadWriteLock_hxx
