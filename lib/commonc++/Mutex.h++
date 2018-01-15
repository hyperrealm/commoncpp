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

#ifndef __ccxx_Mutex_hxx
#define __ccxx_Mutex_hxx

#include <commonc++/Common.h++>
#include <commonc++/Lock.h++>

#ifdef CCXX_OS_POSIX
#include <pthread.h>
#endif

namespace ccxx {

/**
 * A mutual-exclusion lock.
 *
 * Only one thread may have a Mutex locked at any one time.
 *
 * A Mutex may be created as <i>recursive</i>; such a Mutex can be re-locked
 * by a thread that already holds the lock on that Mutex. However, the
 * thread must unlock the Mutex the same number of times that it has locked
 * it in order for it to become available for locking by other threads.
 *
 * See also ScopedLock.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Mutex : public Lock
{
  friend class ConditionVar;

 public:

  /**
   * Construct a new Mutex.
   *
   * @param recursive A flag indicating whether the mutex will be
   * recursive. A recursive mutex can be re-entered by a thread that
   * already holds the mutex.
   */
  Mutex(bool recursive = false);

  /**
   * Destructor. Note that destroying a locked mutex can lead to
   * deadlock.
   */
  ~Mutex();

  /** %Lock the mutex, blocking until the lock is acquired. */
  void lock();

  /**
   * Try to lock the mutex, returning if it could not be acquired
   * within the given amount of time. On platforms that do not support
   * timed mutex locks, the method returns immediately if the lock could
   * not be acquired. On some platforms (notably Linux), recursive mutexes
   * do not support timed locks; on these platforms, tryLock() behaves
   * identically to lock() when applied to a recursive mutex.
   *
   * @param timeout The timeout, in milliseconds.
   * @return <b>true</b> if the lock was acquired, <b>false</b> otherwise.
   */
  bool tryLock(timespan_ms_t timeout = 0);

  /** Unlock the mutex. */
  void unlock();

  /** Determine if this mutex is recursive. */
  inline bool isRecursive() const
  { return(_recursive); }

  /** Determine if the host system supports timed mutex locks. */
  static bool supportsTimedLocks();

 protected:

  /** @cond INTERNAL */
#ifdef CCXX_OS_WINDOWS
  HANDLE _mutex;
#else
  pthread_mutex_t _mutex;
#endif
  /** @endcond */

 private:

  bool _recursive;

  CCXX_COPY_DECLS(Mutex);
};

} // namespace ccxx

#endif // __ccxx_Mutex_hxx
