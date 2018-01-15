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

#ifndef __ccxx_StopWatch_hxx
#define __ccxx_StopWatch_hxx

#include <commonc++/Common.h++>

namespace ccxx {

/**
 * A stopwatch-style timer. The timer can be used for code benchmarking and
 * other timing tasks. The timer accumulates time while it is "running"; this
 * accumulated time (in kernel time, user time, and real time) can be
 * queried while the timer is stopped.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API StopWatch
{
 public:

  /** Construct a new StopWatch. */
  StopWatch();

  /** Destructor. */
  virtual ~StopWatch();

  /**
   * Start the timer. Attemping to start an already running timer will
   * have no effect.
   */
  void start();

  /**
   * Stop the timer. Attempting to stop a timer that isn't running will
   * have no effect.
   */
  void stop();

  /**
   * Pause the timer. Attempting to pause a timer that isn't running
   * will have no effect. Equivalent to <b>stop()</b>.
   */
  inline void pause()
  { stop(); }

  /**
   * Resume the timer. Attempting to resume a timer that isn't paused
   * will have no effect. Equivalent to <b>start()</b>.
   */
  inline void resume()
  { start(); }

  /** Reset the timer. All elapsed times are cleared to 0. */
  void reset();

  /**
   * Return the total elapsed kernel time for this timer (up to the
   * point where the timer was last stopped) in milliseconds.
   */
  inline uint64_t elapsedKernelTime() const
  { return(_kernelElapsed); }

  /**
   * Return the total elapsed user time for this timer (up to the
   * point where the timer was last stopped) in milliseconds.
   */
  inline uint64_t elapsedUserTime() const
  { return(_userElapsed); }

  /**
   * Return the total elapsed real time for this timer (up to the
   * point where the timer was last stopped) in milliseconds.
   */
  inline uint64_t elapsedRealTime() const
  { return(_realElapsed); }

  /** Determine if this timer is running. */
  inline bool isRunning() const
  { return(_running); }

 private:

  time_ms_t _userTime;
  time_ms_t _kernelTime;
  time_ms_t _realTime;

  uint64_t _userElapsed;
  uint64_t _kernelElapsed;
  uint64_t _realElapsed;

  bool _running;

  CCXX_COPY_DECLS(StopWatch);
};

} // namespace ccxx

#endif // __ccxx_StopWatch_hxx
