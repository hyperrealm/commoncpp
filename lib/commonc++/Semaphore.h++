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

#ifndef __ccxx_Semaphore_hxx
#define __ccxx_Semaphore_hxx

#include <commonc++/Common.h++>
#include <commonc++/Lock.h++>
#include <commonc++/Permissions.h++>
#include <commonc++/String.h++>
#include <commonc++/SystemException.h++>

#ifdef CCXX_OS_POSIX
#include <fcntl.h>
#include <semaphore.h>
#endif

namespace ccxx {

/**
 * A counting semaphore -- a synchronization primitive that allows
 * multiple processes to coordinate access to a shared resource. A
 * Semaphore has an initial value, which represents the quantity of
 * some shared resource.  When a process acquires the semaphore,
 * this value is decremented, and when it releases the semaphore,
 * the value is incremented.
 *
 * A Semaphore must be initialized before it can be used, via a call
 * to the init() method.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Semaphore : public Lock
{
 public:

  /**
   * Construct a new Semaphore with the given name, initial value,
   * and permissions.
   *
   * @param name The name of the semaphore. On POSIX systems, the name
   * must consist of at most 14 alphanumeric characters and may not
   * contain slashes.
   * @param value The initial value of the semaphore. Must be at least 1.
   * @param perm The permissions with which to create the semaphore, if it
   * does not yet exist.
   */
  Semaphore(const String& name, uint_t value = 1,
            const Permissions& perm = Permissions::USER_READ_WRITE);

  /**
   * Destructor. Destroys the underlying semaphore object, if no other
   * processes have a reference to it.
   */
  ~Semaphore();

  /**
   * Initialize the semaphore. If the underlying semaphore object
   * did not yet exist, it is created.
   *
   * @throw SystemException If the operation fails.
   */
  void init();

  /**
   * Wait on the semaphore. If the semaphore's value is greater than 0,
   * it's value is decremented and the method returns. Otherwise, the method
   * blocks until the value becomes greater than 0.
   *
   * @return <b>true</b> if the semaphore was acquired, <b>false</b> if an
   * error occurred.
   */
  bool wait();

  /** Equivalent to wait(). */
  inline void lock()
  { wait(); }

  /**
   * Try to wait on the semaphore, returning immediately if it couldn't be
   * acquired.
   *
   * @return <b>true</b> if the semaphore was acquired, <b>false</b>
   * otherwise.
   */
  bool tryWait();

  /**
   * Signal the semaphore. Increments the semaphore's value by 1. The
   * semaphore must have previously been acquired.
   *
   * @return <b>true</b> if the semaphore was released, <b>false</b>
   * otherwise.
   */
  bool signal();

  /** Equivalent to signal(). */
  inline void unlock()
  { signal(); }

  /** Get the current value of the semaphore. */
  int getValue() const;

  /** Get the name of the semaphore. */
  inline String getName() const
  { return(_name); }

 private:

  String _name;
  uint_t _value;
  Permissions _perm;
  bool _initialized;

#ifdef CCXX_OS_WINDOWS
  HANDLE _sem;
#else
  sem_t* _sem;
#endif
};

} // namespace ccxx

#endif // __ccxx_Semaphore_hxx
