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

#ifndef __ccxx_AsyncIOPoller_hxx
#define __ccxx_AsyncIOPoller_hxx

#include <commonc++/AsyncIOTask.h++>

namespace ccxx {

/**
 * A class that polls a set of AsyncIOTask objects.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API AsyncIOPoller
{
 public:

  /** Construct a new AsyncIOPoller. */
  AsyncIOPoller();

  /** Destructor. */
  ~AsyncIOPoller();

  /**
   * Register the specified AsyncIOTask with this poller. The poller does
   * not take ownership of the AsyncIOTask object.
   *
   * @param task The task to register.
   */
  void addTask(AsyncIOTask* task);

  /** Unregister the specified AsyncIOTask from this poller.
   *
   * @param task The task to unregister.
   */
  void removeTask(AsyncIOTask* task);

  /** Unregister all AsyncIOTask objects from this poller. */
  void removeAllTasks();

  /** Get the number of tasks currently registered with this poller. */
  uint_t getTaskCount() const;

  /**
   * Poll the AsyncIOTask objects registered with this poller. The
   * call blocks until at least one of the tasks is completed, or
   * the timeout interval passes, whichever occurs first. There is
   * no userspace latency imposed by this call.
   *
   * @param timeout The timeout, in milliseconds. Negative values are
   * interpreted as an infinite timeout.
   * @return <b>true</b> if at least one of the asynchronous I/O tasks
   * registered with this poller has completed, <b>false</b> otherwise.
   */
  bool poll(timespan_ms_t timeout = 0);

 private:

  class AsyncIOTaskList; // fwd decl
  AsyncIOTaskList *_tasks;
#if defined(CCXX_OS_WINDOWS)
  HANDLE* _cblist;
#elif !defined(CCXX_OS_ANDROID)
  struct aiocb** _cblist;
#endif
  uint_t _cbcount;
  bool _dirty;
};

} // namespace ccxx

#endif // __ccxx_AsyncIOTask_hxx
