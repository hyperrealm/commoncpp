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

#include "commonc++/IntervalTimer.h++"
#include "commonc++/System.h++"

#include <cstring>

namespace ccxx {

/*
 */

IntervalTimer::IntervalTimer(uint_t initialDelay, uint_t interval /* = 0 */)
  : _initialDelay(initialDelay),
    _interval(interval),
    _running(false)
{
}

/*
 */

IntervalTimer::~IntervalTimer()
{
  stop();
}

/*
 */

void IntervalTimer::start()
{
  if(_running)
    return;

#ifdef CCXX_OS_WINDOWS

  _timer = ::CreateWaitableTimer(NULL, FALSE, NULL);
  if(! _timer)
    throw SystemException(System::getErrorString("CreateWaitableTimer"));

  // negative value for relative time
  __int64 dueTime = static_cast<__int64>(_initialDelay) * -10000;
  LARGE_INTEGER liDueTime;
  liDueTime.LowPart = static_cast<DWORD>(dueTime & 0xFFFFFFFF);
  liDueTime.HighPart = static_cast<LONG>(dueTime >> 32);

  if(! ::SetWaitableTimer(_timer, &liDueTime, static_cast<LONG>(_interval),
                          IntervalTimer::_callback,
                          reinterpret_cast<LPVOID>(this), FALSE))
    throw SystemException(System::getErrorString("SetWaitableTimer"));

#else

#if defined(HAVE_TIMER_CREATE) && defined(HAVE_TIMER_SETTIME)

  CCXX_ZERO(_event);
  _event.sigev_notify = SIGEV_THREAD;
  _event.sigev_value.sival_ptr = reinterpret_cast<void *>(this);
  _event.sigev_notify_function = &IntervalTimer::_callback;
  _event.sigev_notify_attributes = NULL;

  int r = ::timer_create(CLOCK_REALTIME, &_event, &_timer);
  if(r != 0)
    throw SystemException(System::getErrorString("timer_create"));

  struct itimerspec timerspec;
  timerspec.it_interval.tv_sec = _interval / 1000;
  timerspec.it_interval.tv_nsec = (_interval % 1000) * 1000000;
  timerspec.it_value.tv_sec = _initialDelay / 1000;
  timerspec.it_value.tv_nsec = (_initialDelay % 1000) * 1000000;

  r = ::timer_settime(_timer, 0, &timerspec, NULL);
  if(r != 0)
    throw SystemException(System::getErrorString("timer_settime"));

#else

#warning "no real-time timer implementation available"

#endif

#endif // ! CCXX_OS_WINDOWS

  _running = true;
}

/*
 */

void IntervalTimer::stop()
{
  if(! _running)
    return;

#ifdef CCXX_OS_WINDOWS

  ::CancelWaitableTimer(_timer);
  ::CloseHandle(_timer);

#else

#ifdef HAVE_TIMER_DELETE

  ::timer_delete(_timer);

#else

#warning "no real-time timer implementation available"

#endif

#endif // ! CCXX_OS_WINDOWS

  _running = false;
}

/*
 */

#ifdef CCXX_OS_WINDOWS

void IntervalTimer::_callback(LPVOID arg, DWORD timerLowValue,
                              DWORD timerHighValue)
{
  IntervalTimer *self = reinterpret_cast<IntervalTimer *>(arg);
  self->fired();
}

#else

void IntervalTimer::_callback(union sigval data)
{
  IntervalTimer *self = reinterpret_cast<IntervalTimer *>(data.sival_ptr);
  self->fired();
}

#endif


} // namespace ccxx
