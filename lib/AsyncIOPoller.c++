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

#include "commonc++/AsyncIOPoller.h++"

#include <set>

#ifdef HAVE_AIO_H
#include <aio.h>
#endif

namespace ccxx {

/*
 */

class AsyncIOPoller::AsyncIOTaskList : public std::set<AsyncIOTask *>
{
};

/*
 */

AsyncIOPoller::AsyncIOPoller()
  :
#ifdef HAVE_AIO_H
    _cblist(NULL),
#endif
    _cbcount(0),
    _dirty(true)
{
  _tasks = new AsyncIOTaskList();
}

/*
 */

AsyncIOPoller::~AsyncIOPoller()
{
  delete _tasks;
#ifdef HAVE_AIO_H
  delete [] _cblist;
#endif
}

/*
 */

void AsyncIOPoller::addTask(AsyncIOTask* task)
{
  _dirty = (_tasks->insert(task).second);
}

/*
 */

void AsyncIOPoller::removeTask(AsyncIOTask* task)
{
  AsyncIOTaskList::iterator iter = _tasks->find(task);
  if(iter != _tasks->end())
  {
    _tasks->erase(iter);
    _dirty = true;
  }
}

/*
 */

void AsyncIOPoller::removeAllTasks()
{
  if(! _tasks->empty())
  {
    _tasks->clear();
    _dirty = true;
  }
}

/*
 */

uint_t AsyncIOPoller::getTaskCount() const
{
  return(_tasks->size());
}

/*
 */

bool AsyncIOPoller::poll(timespan_ms_t timeout /* = 0 */)
{
#ifdef CCXX_OS_ANDROID

  return(false);

#else // CCXX_OS_ANDROID

  if(_dirty)
  {
    delete[] _cblist;
    _cblist = NULL;
    _cbcount = 0;

    if(_tasks->empty())
      return(false);

#ifdef CCXX_OS_WINDOWS
    _cblist = new HANDLE[_tasks->size()];
#else
    _cblist = new aiocb *[_tasks->size()];
#endif

    for(AsyncIOTaskList::iterator iter = _tasks->begin();
        iter != _tasks->end();
        ++iter)
    {
      AsyncIOTask *task = *iter;

#ifdef CCXX_OS_WINDOWS
      _cblist[_cbcount] = task->getFileHandle();
#else
      _cblist[_cbcount] = task->getControlBlock();
#endif
      ++_cbcount;
    }

    _dirty = false;
  }

#ifdef CCXX_OS_WINDOWS

  DWORD r = ::WaitForMultipleObjects(static_cast<DWORD>(_cbcount), _cblist,
                                     FALSE, static_cast<DWORD>(timeout < 0
                                                               ? INFINITE
                                                               : timeout));

  return((r >= WAIT_OBJECT_0) && (r < (WAIT_OBJECT_0 + _cbcount)));

#else

  struct timespec ts, *tsp = NULL;

  if(timeout >= 0)
  {
    ts.tv_sec = timeout / 1000;
    ts.tv_nsec = (timeout % 1000) * 1000000;
    tsp = &ts;
  }

SUSPEND:
  int r = ::aio_suspend(_cblist, _cbcount, tsp);

  if((r == EINTR) && (timeout < 0))
    goto SUSPEND;

  return(r == 0);

#endif

#endif // CCXX_OS_ANDROID
}


} // namespace ccxx
