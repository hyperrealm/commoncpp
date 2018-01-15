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

#ifndef __ccxx_ScopedReadWriteLock_hxx
#define __ccxx_ScopedReadWriteLock_hxx

#include <commonc++/Common.h++>
#include <commonc++/ReadWriteLock.h++>

namespace ccxx {

/**
 * A convenience object for lexical scope based
 * synchronization. ScopedReadLock locks a ReadWriteLock for read at
 * construction time and unlocks it when it is destroyed.
 *
 * A block of code can be conveniently protected by a ScopedReadLock by using
 * the <b>synchronized_read()</b> macro, e.g.:
 *
 * <pre>
 * ReadWriteLock rwlock;
 *
 * synchronized_read(rwlock)
 * {
 *   // readers-exclusive code here
 * }
 * </pre>
 *
 * @author Mark Lindner
 */
class /* COMMONCPP_API */ ScopedReadLock
{
 public:

  /**
   * Construct a new ScopedReadLock for the given ReadWriteLock.
   * The lock is locked for read immediately.
   */
  ScopedReadLock(ReadWriteLock& rwlock)
    : _rwlock(rwlock), _once(false)
  {
    _rwlock.lockRead();
  }

  /** Destructor. Unlocks the ReadWriteLock. */
  ~ScopedReadLock()
  {
    _rwlock.unlock();
  }

  /** @cond INTERNAL */
  inline bool testOnce()
  { bool f = _once; _once = false; return(f); }
  /** @endcond */

 private:

  ReadWriteLock& _rwlock;
  bool _once;

  CCXX_COPY_DECLS(ScopedReadLock);
};

/**
 * A convenience object for lexical scope based
 * synchronization. ScopedWriteLock locks a ReadWriteLock for write at
 * construction time and unlocks it when it is destroyed.
 *
 * A block of code can be conveniently protected by a ScopedWriteLock by
 * using the <b>synchronized_write()</b> macro, e.g.:
 *
 * <pre>
 * ReadWriteLock rwlock;
 *
 * synchronized_write(rwlock)
 * {
 *   // writer-exclusive code here
 * }
 * </pre>
 *
 * @author Mark Lindner
 */
class /* COMMONCPP_API */ ScopedWriteLock
{
 public:

  /**
   * Construct a new ScopedWriteLock for the given ReadWriteLock.
   * The lock is locked for write immediately.
   */
  ScopedWriteLock(ReadWriteLock& rwlock)
    : _rwlock(rwlock), _once(true)
  {
    _rwlock.lockWrite();
  }

  /** Destructor. Unlocks the ReadWriteLock. */
  ~ScopedWriteLock()
  {
    _rwlock.unlock();
  }

  /** @cond INTERNAL */
  inline bool testOnce()
  { bool f = _once; _once = false; return(f); }
  /** @endcond */

 private:

  ReadWriteLock& _rwlock;
  bool _once;

  CCXX_COPY_DECLS(ScopedWriteLock);
};

} // namespace ccxx

/**
 * @def synchronized_read(MUTEX)
 * Introduce a code block that is read-locked by the ReadWriteLock LOCK.
 */
#if (defined CCXX_OS_WINDOWS) && (defined _MSC_VER) && (_MSC_VER < 1400)

#define synchronized_read(LOCK)                         \
  for(commoncpp::ScopedReadLock LOCK ## _locker(LOCK);  \
      LOCK ## _locker.testOnce();)

#else

#define synchronized_read(LOCK)                                         \
  for(commoncpp::ScopedReadLock                                         \
        CCXX_OS_UNIQUE_VARNAME(LOCK, __LINE__, locker)(LOCK);           \
      CCXX_OS_UNIQUE_VARNAME(LOCK, __LINE__, locker).testOnce();)

#endif

/**
 * @def synchronized_write(MUTEX)
 * Introduce a code block that is write-locked by the ReadWriteLock LOCK.
 */
#if (defined CCXX_OS_WINDOWS) && (defined _MSC_VER) && (_MSC_VER < 1400)

#define synchronized_write(LOCK)                        \
  for(commoncpp::ScopedWriteLock LOCK ## _locker(LOCK); \
      LOCK ## _locker.testOnce();)

#else

#define synchronized_write(LOCK)                                        \
  for(commoncpp::ScopedWriteLock                                        \
        CCXX_OS_UNIQUE_VARNAME(LOCK, __LINE__, locker)(LOCK);           \
      CCXX_OS_UNIQUE_VARNAME(LOCK, __LINE__, locker).testOnce();)

#endif

#endif // __ccxx_ScopedReadWriteLock_hxx
