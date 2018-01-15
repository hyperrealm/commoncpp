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

#ifndef __ccxx_IntervalTimer_hxx
#define __ccxx_IntervalTimer_hxx

#include <commonc++/Common.h++>
#include <commonc++/SystemException.h++>

#ifdef CCXX_OS_POSIX
#include <signal.h>
#include <time.h>
#endif

namespace ccxx {

/**
 * A high-precision interval timer that does not require blocking
 * or polling from a dedicated thread. This is an abstract class
 * which should be subclassed to provide an implementation of the
 * fired() method, which is invoked each time the timer fires.
 * <p>
 * <b>NOTE:</b> This class is currently not implemented on Mac OS X.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API IntervalTimer
{
 public:

  /** Destructor. */
  virtual ~IntervalTimer();

  /**
   * Start the timer.
   *
   * @throw SystemException If an error occurs while creating the timer.
   */
  void start();

  /** Stop the timer. */
  void stop();

  /** Get the initial delay, in milliseconds. */
  inline uint_t getInitialDelay() const
  { return(_initialDelay); }

  /**
   * Set the initial delay, in milliseconds. The new value will take
   * effect the next time the timer is started.
   */
  inline void setInitialDelay(uint_t initialDelay)
  { _initialDelay = initialDelay; }

  /** Get the interval, in milliseconds. */
  inline uint_t getInterval() const
  { return(_interval); }

  /**
   * Set the interval, in milliseconds. The new value will take
   * effect the next time the timer is started.
   */
  inline void setInterval(uint_t interval)
  { _interval = interval; }

  /** Determine if the timer is currently running. */
  inline bool isRunning() const
  { return(_running); }

 protected:

  /**
   * Construct a new IntervalTimer with the given initial delay and
   * interval.
   *
   * @param initialDelay The delay, in milliseconds, between the time that
   * the timer is started and the first time it fires.
   * @param interval The interval, in milliseconds, between subsequent
   * firings. If 0, the timer is a "one-shot" timer.
   */
  IntervalTimer(uint_t initialDelay, uint_t interval = 0);

  /** Callback. This method is invoked each time the timer fires. */
  virtual void fired() = 0;

 private:

#ifdef CCXX_OS_WINDOWS
  static void CALLBACK _callback(LPVOID arg, DWORD timerLowValue,
                                 DWORD timerHighValue);
#else
  static void _callback(union sigval data);
#endif

  uint_t _initialDelay;
  uint_t _interval;
  bool _running;

#if defined(CCXX_OS_WINDOWS)
  HANDLE _timer;
#elif ! defined(CCXX_OS_MACOSX)
  timer_t _timer;
  struct sigevent _event;
#endif
};

} // namespace ccxx

#endif // __ccxx_IntervalTimer_hxx
