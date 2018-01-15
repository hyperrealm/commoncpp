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

#ifndef __ccxx_ScopedLock_hxx
#define __ccxx_ScopedLock_hxx

#include <commonc++/Common.h++>
#include <commonc++/Lock.h++>

namespace ccxx {

/**
 * A convenience object for lexical scope based
 * synchronization. ScopedLock locks a Lock in its constructor
 * and unlocks it in its destructor.
 *
 * A block of code can be conveniently protected by a ScopedLock by using
 * the <b>synchronized()</b> macro, e.g.:
 *
 * <pre>
 * Mutex mutex;
 *
 * synchronized(mutex)
 * {
 *   // thread-exclusive code here
 * }
 * </pre>
 *
 * @author Mark Lindner
 */
class /* COMMONCPP_API */ ScopedLock
{
 public:

  /**
   * Construct a new Scopedock for the given Lock. The Lock is locked
   * immediately.
   */
  ScopedLock(Lock& lock)
    : _lock(lock),
      _once(true)
  {
    _lock.lock();
  }

  /** Destructor. Unlocks the Lock. */
  ~ScopedLock()
  {
    _lock.unlock();
  }

  /** @cond INTERNAL */
  inline bool testOnce()
  { bool f = _once; _once = false; return(f); }
  /** @endcond */

 private:

  Lock& _lock;
  bool _once;

  CCXX_COPY_DECLS(ScopedLock);
};

} // namespace ccxx

/**
 * @def synchronized(LOCK)
 * Introduce a code block that is protected by the Lock LOCK.
 */
#ifdef synchronized
#undef synchronized
#endif

#if (defined CCXX_OS_WINDOWS) && (defined _MSC_VER) && (_MSC_VER < 1400)

#define synchronized(LOCK)                      \
  for(ccxx::ScopedLock LOCK ## _locker(LOCK);   \
      LOCK ## _locker.testOnce();)

#else

#define synchronized(LOCK)                                      \
  for(ccxx::ScopedLock                                          \
        CCXX_UNIQUE_VARNAME(LOCK, __LINE__, locker)(LOCK);      \
      CCXX_UNIQUE_VARNAME(LOCK, __LINE__, locker).testOnce();)

#endif

#endif // __ccxx_ScopedLock_hxx
