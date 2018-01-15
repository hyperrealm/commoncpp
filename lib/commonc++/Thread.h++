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

#ifndef __ccxx_Thread_hxx
#define __ccxx_Thread_hxx

#include <commonc++/Common.h++>
#include <commonc++/AtomicCounter.h++>
#include <commonc++/Runnable.h++>
#include <commonc++/ThreadLocal.h++>
#include <commonc++/String.h++>

#ifdef CCXX_OS_POSIX
#include <pthread.h>
#endif

namespace ccxx {

/**
 * A thread of execution. Thread can be used in one of two ways:
 * <ul>
 * <li>It can be subclassed. When started, the new thread will begin
 * executing in its <b>run()</b> method, which must be suitably overridden.
 * <li>An object that implements the Runnable interface can be passed to
 * its constructor. When started, the new thread will begin executing
 * in that Runnable object's <b>run()</b> method.
 * </ul>
 * <p>
 * A thread can be detached or joinable. A joinable thread continues to
 * exist after it has finished executing; its resources must be reclaimed by
 * calling the <b>join()</b> method. A joinable thread can be started and
 * stopped multiple times.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Thread : public Runnable
{
 public:

  /**
   * Construct a new Thread.
   *
   * @param detached A flag indicating whether the thread will be
   * created in a detached state.
   * @param stackSize The stack size for the thread, in bytes. A value of
   * 0 indicates that the default stack size should be used.
   */
  Thread(bool detached = false, size_t stackSize = 0);

  /**
   * Construct a new Thread with a Runnable object.
   *
   * @param runnable The runnable object whose <b>run()</b> method will
   * be executed in this thread.
   * @param detached A flag indicating whether the thread will be
   * created in a detached state.
   * @param stackSize The stack size for the thread, in bytes. A value of
   * 0 indicates that the default stack size should be used.
   */
  Thread(Runnable* runnable, bool detached = false, size_t stackSize = 0);

  /** Destructor. */
  virtual ~Thread();

  /**
   * Start executing the thread. If a Runnable object was supplied in
   * the constructor, its <b>run()</b> method is invoked; otherwise the
   * thread object's <b>run()</b> method is called. If the thread is already
   * running, the call has no effect.
   */
  virtual void start();

  /**
   * Stop execution of the thread. The thread is not forcibly
   * killed; instead, an asynchronous cancellation request is posted
   * to the thread. If the thread is not running, the call has no effect.
   */
  virtual void stop();

  /**
   * Wait for the thread to terminate.
   *
   * @return <b>true</b> if the join was successful, <b>false</b> otherwise.
   * Note that threads which were created in a detached state are not
   * joinable.
   */
  bool join();

  /** Test if the thread is currently running. */
  bool isRunning() const;

  /** Test if the thread is detached. */
  inline bool isDetached() const
  { return(_detached); }

  /** Set the thread priority. */
  void setPriority(Priority priority);

  /** Get the thread priority. */
  Priority getPriority() const;

  /** Set the name of this thread. */
  inline void setName(const String& name)
  { _name = name; }

  /** Get the name of this thread. */
  inline String getName() const
  { return(_name); }

  /**
   * Suspend the calling thread for the given time interval.
   *
   * @param msec The number of milliseconds to sleep.
   */
  static void sleep(timespan_ms_t msec);

  /**
   * Obtain a pointer to the Thread object for the calling
   * thread. If the calling thread is the main thread, or some other thread
   * that was not created via commonc++, a NULL pointer is returned. Never
   * delete the object returned by this method.
   */
  static Thread* currentThread();

  /** Get the calling thread's thread ID. */
  static ThreadID currentThreadID();

 protected:

  /**
   * Main function. Thread execution begins in this function. The
   * default implementation does nothing.
   */
  virtual void run();

  /**
   * Cleanup function. This function will be executed by the thread
   * just prior to termination. The default implementation is a
   * no-op.
   */
  virtual void cleanup();

  /**
   * Suspend the thread for the given time interval, returning
   * early if the thread was cancelled via a call to stop().
   *
   * @param msec The number of milliseconds to sleep.
   * @return <b>true</b> if the full interval elapsed, <b>false</b> if the
   * thread was cancelled during the sleep.
   */
  bool trySleep(timespan_ms_t msec);

  /**
   * Test for asynchronous cancellation requests.
   *
   * @return <b>true</b> If a cancellation request has been posted to
   * the calling thread.
   */
  bool testCancel();

  /** Yield the CPU to (potentially) another thread. */
  void yield();

  /** Exit (terminate) the calling thread. */
  static void exit();

 private:

  Thread(ThreadHandle id);

  AtomicCounter _runFlag;
  bool _cancelled;
  bool _detached;
  size_t _stackSize;
  String _name;
#ifdef CCXX_OS_WINDOWS
  HANDLE _thread;
  HANDLE _cancelEvent;
  bool _cleanupDone;
#else
  pthread_t _thread;
  pthread_attr_t _attrs;
#endif
  Runnable* _runnable;

#ifdef CCXX_OS_WINDOWS
  static DWORD WINAPI _startupDispatcher(LPVOID arg);
  static void _cleanupTLS();
#else
  static void* _startupDispatcher(void* arg);
  static void _cleanupDispatcher(void* arg);
#endif

  void _run();
  void _init();

  CCXX_COPY_DECLS(Thread);
};

} // namespace ccxx

#endif // __ccxx_Thread_hxx
