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

#ifndef __ccxx_CriticalSection_hxx
#define __ccxx_CriticalSection_hxx

#include <commonc++/Common.h++>
#include <commonc++/Lock.h++>
#ifndef CCXX_OS_WINDOWS
#include <commonc++/AtomicCounter.h++>
#include <commonc++/ThreadLocalCounter.h++>
#include <commonc++/Random.h++>
#endif

namespace ccxx {

/**
 * A critical section, a synchronization primitive that is
 * typically more efficient than but roughly semantically equivalent
 * to a Mutex. Only one thread may be "within" a CriticalSection at
 * any one time.
 *
 * CriticalSections are recursive, so a thread may re-enter a
 * CriticalSection that it has already entered. However, the
 * thread must leave the CriticalSection the same number of times
 * that it has entered it in order to release it.
 *
 * See also ScopedLock.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API CriticalSection : public Lock
{
 public:

  /** Constructor. */
  CriticalSection();

  /**
   * Destructor. It is the calling code's responsibility to ensure that
   * a CriticalSection is deleted only when no thread is within it.
   */
  ~CriticalSection();

  /** Enter the critical section, blocking if necessary. */
  void enter();

  /** Equivalent to enter(). */
  inline void lock()
  { enter(); }

  /**
   * Try to enter the critical section, returning immediately if it could
   * not be entered.
   *
   * @return <b>true</b> if the critical section was entered, <b>false</b>
   * otherwise.
   */
  bool tryEnter();

  /** Leave the critical section. */
  void leave();

  /** Equivalent to leave(). */
  inline void unlock()
  { leave(); }

 private:

#ifdef CCXX_OS_WINDOWS
  CRITICAL_SECTION _lock;
#else
  AtomicCounter _lock;
  ThreadLocalCounter _counter;
  Random _rand;
#endif
};

} // namespace ccxx

#endif // __ccxx_CriticalSection_hxx
