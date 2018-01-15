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

#include "commonc++/Thread.h++"
#include "commonc++/System.h++"
#include "commonc++/Log.h++"

#ifdef CCXX_OS_WINDOWS
#include <Winbase.h>
//#define _WIN32_WINNT 0x0400
#endif

#ifdef CCXX_OS_POSIX
#include "commonc++/POSIX.h++"

#include <sched.h>
#include <unistd.h>
#include <cstdlib>
#include <time.h>
#endif

#include <cerrno>

namespace ccxx {

/*
 */

Thread::Thread(bool detached /* = false */, size_t stackSize /* = 0 */)
  : _cancelled(false),
    _detached(detached),
    _stackSize(stackSize),
#ifdef CCXX_OS_WINDOWS
    _cleanupDone(false),
#endif
    _runnable(NULL)
{
  _init();
}

/*
 */

Thread::Thread(Runnable *runnable, bool detached /* = false */,
               size_t stackSize /* = 0 */)
  : _cancelled(false),
    _detached(detached),
    _stackSize(stackSize),
#ifdef CCXX_OS_WINDOWS
    _cleanupDone(false),
#endif
    _runnable(runnable)
{
  _init();
}

/*
 * Private constructor to create Thread object for pre-existing thread.
 */

Thread::Thread(ThreadHandle id)
  : _cancelled(false),
    _detached(true),
    _thread(id),
    _runnable(NULL)
{
  _init();
}

/*
 */

void Thread::_init()
{
#ifdef CCXX_OS_WINDOWS

  _cancelEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
  _thread = NULL;

#else

  ::pthread_attr_init(&_attrs);

#endif
}

/*
 */

Thread::~Thread()
{
#ifdef CCXX_OS_WINDOWS

  if(_thread != NULL)
    ::CloseHandle(_thread);

  ::CloseHandle(_cancelEvent);

#endif
}

/*
 */

#ifdef CCXX_OS_WINDOWS
DWORD WINAPI Thread::_startupDispatcher(LPVOID arg)
#else
void *Thread::_startupDispatcher(void *arg)
#endif
{
  Thread *thread = reinterpret_cast<Thread *>(arg);
  thread->_run();

#ifdef CCXX_OS_WINDOWS
  return(0);
#else
  return(NULL);
#endif
}

/*
 */

void Thread::start()
{
  int32_t x = _runFlag.testAndSet(1, 0);
  if(x == 1)
    return; // thread is already started

  _cancelled = false;

#ifdef CCXX_OS_WINDOWS

  ::ResetEvent(_cancelEvent);

  _thread = ::CreateThread(NULL, static_cast<DWORD>(_stackSize),
                           &Thread::_startupDispatcher, this, 0, NULL);

#else

  if(_stackSize != 0)
    ::pthread_attr_setstacksize(&_attrs, _stackSize);

  if(_detached)
    ::pthread_attr_setdetachstate(&_attrs, PTHREAD_CREATE_DETACHED);

  ::pthread_create(&_thread, &_attrs, &Thread::_startupDispatcher, this);

#endif
}

/*
 */

void Thread::stop()
{
  _cancelled = true;

#ifdef CCXX_OS_WINDOWS

  ::SetEvent(_cancelEvent);

#else

//  ::pthread_cancel(_thread); // not reliable for C++ & pthreads

#endif
}

/*
 */

bool Thread::join()
{
  if(_detached)
    return(false);

  if(! isRunning())
    return(true);

#ifdef CCXX_OS_WINDOWS

  DWORD r = 0;

  for(;;)
  {
    r = ::WaitForSingleObjectEx(_thread, INFINITE, TRUE);
    if(r != WAIT_IO_COMPLETION)
      break;
  }

  return(r == WAIT_OBJECT_0);

#else

  void *rval;

  return(::pthread_join(_thread, &rval) == 0);

#endif
}

/*
 */

void Thread::yield()
{
#ifdef CCXX_OS_WINDOWS

  (void)::SleepEx(0, TRUE);

#else

  ::sched_yield();

#endif
}

/*
 */

void Thread::setPriority(Priority priority)
{
#ifdef CCXX_OS_WINDOWS

  int pri = 0;

  switch(priority)
  {
    case PrioLowest:
      pri = THREAD_PRIORITY_LOWEST;
      break;

    case PrioLow:
      pri = THREAD_PRIORITY_BELOW_NORMAL;
      break;

    case PrioNormal:
      pri = THREAD_PRIORITY_NORMAL;
      break;

    case PrioHigh:
      pri = THREAD_PRIORITY_ABOVE_NORMAL;
      break;

    case PrioHighest:
      pri = THREAD_PRIORITY_HIGHEST;
      break;
  }

  ::SetThreadPriority(_thread, pri);

#else

  struct sched_param schedp;
  int policy, min, max;

  ::pthread_attr_getschedpolicy(&_attrs, &policy);

  min = ::sched_get_priority_min(policy);
  max = ::sched_get_priority_max(policy);

  int range = max - min;

  schedp.sched_priority = min + ((range / 5) * priority);

  ::pthread_attr_setschedparam(&_attrs, &schedp);

#endif
}

/*
 */

Priority Thread::getPriority() const
{
#ifdef CCXX_OS_WINDOWS

  int pri = ::GetThreadPriority(_thread);

  switch(pri)
  {
    case THREAD_PRIORITY_LOWEST:
      return(PrioLowest);

    case THREAD_PRIORITY_BELOW_NORMAL:
      return(PrioLow);

    case THREAD_PRIORITY_NORMAL:
      return(PrioNormal);

    case THREAD_PRIORITY_ABOVE_NORMAL:
      return(PrioHigh);

    case THREAD_PRIORITY_HIGHEST:
      return(PrioHighest);

    default:
      return(PrioNormal);
  }

#else

  struct sched_param schedp;

  ::pthread_attr_getschedparam(&_attrs, &schedp);

  int policy, min, max;

  ::pthread_attr_getschedpolicy(&_attrs, &policy);

  min = ::sched_get_priority_min(policy);
  max = ::sched_get_priority_max(policy);

  int range = max - min;
  int step = range / 5;
  int lo = min + step;

  if(schedp.sched_priority < lo)
    return(PrioLowest);

  lo += step;
  if(schedp.sched_priority < lo)
    return(PrioLow);

  lo += step;
  if(schedp.sched_priority < lo)
    return(PrioNormal);

  lo += step;
  if(schedp.sched_priority < lo)
    return(PrioHigh);

  return(PrioHighest);

#endif
}

/*
 */

void Thread::run()
{
  // no-op
}

/*
 */

void Thread::_run()
{
#ifdef CCXX_OS_WINDOWS

  Windows::setCurrentThread(this);

  try
  {
    if(_runnable)
      _runnable->run();
    else
      run();
  }
  catch(...)
  {
    Log_warning("Thread exited with unhandled exception.");
    System::printStackTrace();
  }

  if(! _cleanupDone)
  {
    try
    {
      cleanup();
    }
    catch(...)
    {
      Log_warning("Thread cleanup raised unhandled exception.");
      System::printStackTrace();
    }

    _cleanupTLS();
  }

  _runFlag.set(0);

#else

  POSIX::setCurrentThread(this);

  // not reliable for C++ & pthreads
//  int last_state, last_type;
//  ::pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &last_type);
//  ::pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &last_state);

  pthread_cleanup_push(&Thread::_cleanupDispatcher, this);

  try
  {
    if(_runnable)
      _runnable->run();
    else
      run();
  }
  catch(...)
  {
    Log_warning("Thread exited with unhandled exception.");
    System::printStackTrace();
  }

  pthread_cleanup_pop(1);
  return;

#endif

}

/*
 */

bool Thread::isRunning() const
{
  return(_runFlag.get() == 1);
}

/*
 */

bool Thread::testCancel()
{
#ifdef CCXX_OS_WINDOWS

  if(::WaitForSingleObjectEx(_cancelEvent, 0, TRUE) == WAIT_OBJECT_0)
    return(true);

  return(false);

#else

//  ::pthread_testcancel(); // not reliable for C++ & pthreads
  return(_cancelled);

#endif
}

/*
 */

void Thread::cleanup()
{
  // no-op by default
}

/*
 */

#ifdef CCXX_OS_POSIX

void Thread::_cleanupDispatcher(void *arg)
{
  Thread *thread = reinterpret_cast<Thread *>(arg);

  try
  {
    thread->cleanup();
  }
  catch(...)
  {
    Log_warning("Thread cleanup raised unhandled exception.");
    System::printStackTrace();
  }

  thread->_runFlag.set(0);
}

#endif

/*
 */

void Thread::exit()
{
#ifdef CCXX_OS_WINDOWS

  Thread* self = Thread::currentThread();

  if(self)
  {
    try
    {
      self->cleanup();
    }
    catch(...)
    {
      Log_warning("Thread cleanup raised unhandled exception.");
      System::printStackTrace();
    }
  }

  _cleanupTLS();

  self->_cleanupDone = true;

  ::ExitThread(0);

#else

  ::pthread_exit(NULL);

#endif
}

/*
 */

bool Thread::trySleep(timespan_ms_t msec)
{

#ifdef CCXX_OS_WINDOWS

  int r = ::WaitForSingleObjectEx(_cancelEvent, static_cast<DWORD>(msec),
                                  TRUE);
  return(r == WAIT_TIMEOUT);

#else

  time_ms_t endTime = 0;

  for(;;)
  {
    if(_cancelled)
      return(false);

    time_ms_t now = System::currentTimeMillis();
    if(endTime == 0)
      endTime = now + msec;

    int32_t delta = static_cast<int32_t>(endTime - now);

    if((delta <= 0) || (delta > static_cast<int32_t>(msec)))
      break;
    else if(delta > 1000)
      delta = 1000;

    struct timespec treq;
    div_t dv = ::div(delta, 1000);

    treq.tv_sec = dv.quot;
    treq.tv_nsec = dv.rem * 1000000;

    ::nanosleep(&treq, NULL);
  }

  return(true);

#endif
}

/*
 */

ccxx::ThreadID Thread::currentThreadID()
{
#ifdef CCXX_OS_WINDOWS

  return(static_cast<ThreadID>(::GetCurrentThreadId()));

#else

  return(static_cast<ThreadID>(::pthread_self()));

#endif
}

/*
 */

void Thread::sleep(timespan_ms_t msec)
{

#ifdef CCXX_OS_WINDOWS

  ::SleepEx(static_cast<DWORD>(msec), TRUE);

#else

  struct timespec treq, trem;
  div_t dv = ::div(msec, 1000);

  treq.tv_sec = dv.quot;
  treq.tv_nsec = dv.rem * 1000000;

  for(;;)
  {
    int r = ::nanosleep(&treq, &trem);
    if((r == 0) || ((r < 0) && (errno != EINTR)))
      break;

    treq = trem;
  }

#endif
}

/*
 */

Thread *Thread::currentThread()
{
  Thread *thread = NULL;

#ifdef CCXX_OS_WINDOWS
  thread = reinterpret_cast<Thread *>(Windows::getCurrentThread());
#else
  thread = reinterpret_cast<Thread *>(POSIX::getCurrentThread());
#endif

  return(thread);
}

/*
 */

#ifdef CCXX_OS_WINDOWS

void Thread::_cleanupTLS()
{
  // Delete any TLS objects held by this thread (Windows doesn't do this for
  // us automatically).

  Windows::cleanupTLS();
}

#endif


} // namespace ccxx
