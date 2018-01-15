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

#ifndef __ccxx_ThreadLocal_hxx
#define __ccxx_ThreadLocal_hxx

#include <commonc++/Common.h++>
#include <commonc++/NullPointerException.h++>
#include <commonc++/System.h++>
#include <commonc++/SystemException.h++>

#ifdef CCXX_OS_WINDOWS
#include <commonc++/Windows.h++>
#endif

#ifdef CCXX_OS_POSIX
#include <pthread.h>
#endif

namespace ccxx {

/**
 * Thread-local storage smart pointer. This template provides a
 * mechanism for creating an object that has a distinct value for
 * each thread that accesses it.  The template parameter T is the
 * type of thread-local object being created. Subclasses should
 * override the <b>initialValue()</b> method to provide an initial
 * value for the object.
 *
 * @author Mark Lindner
 */
template <typename T> class ThreadLocal
{
 public:

  /**
   * Construct a new ThreadLocal object.
   *
   * @throw SystemException If a system-level error occurred.
   */
  ThreadLocal();

  /**
   * Destructor. Note that the destructor only deallocates system
   * resources associated with the thread-local storage slot, not
   * the actual values stored in that slot. Each thread should
   * destroy its instance of the thread-local object when it is no
   * longer needed.
   */
  virtual ~ThreadLocal();

  /**
   * Pointer-dereference operator.
   *
   * @return A reference to the thread-local object.
   * @throw NullPointerException If the value of the object was never
   * initialized for the calling thread.
   */
  T& operator*();

  /**
   * Pointer operator.
   *
   * @return A pointer to the thread-local object.
   * @throw NullPointerException If the value of the object was never
   * initialized for the calling thread.
   */
  T* operator->();

  /**
   * Set the value of the thread-local object for the calling thread.
   *
   * @param value The new value.
   */
  void setValue(T* value);

  /**
   * Get the value of the thread-local object for the calling thread.
   *
   * @return The current (possibly <b>NULL</b>) value.
   */
  T* getValue();

  /**
   * Get the maximum number of thread-local storage slots available
   * on this platform. A return value of -1 indicates that there is
   * no limit.
   */
  static int getSlotCount();

 protected:

  /**
   * Provide the intial value of the thread-local object for the calling
   * thread. The default implementation returns <b>NULL</b>.
   *
   * @return The initial value.
   */
  virtual T* initialValue();

 private:

#ifdef CCXX_OS_WINDOWS
  int _key;
#else
  pthread_key_t _key;
#endif

  static void keyDestructor(void* arg);

  CCXX_COPY_DECLS(ThreadLocal);
};

#include <commonc++/ThreadLocalImpl.h++>

} // namespace ccxx

#endif // __ccxx_ThreadLocal_hxx
