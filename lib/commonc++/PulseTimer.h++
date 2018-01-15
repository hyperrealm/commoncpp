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

#ifndef __ccxx_PulseTimer_hxx
#define __ccxx_PulseTimer_hxx

#include <commonc++/Common.h++>
#include <commonc++/Thread.h++>

namespace ccxx {

/**
 * A timer that fires at regular intervals, with a resolution of 1
 * second.  This is an abstract class; subclasses should implement the
 * <b>pulse</b>() method, which is called each time the timer fires.
 * A PulseTimer is intended to run in its own thread.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API PulseTimer : public Thread
{
 public:

  /**
   * Construct a new PulseTimer with the given interval and initial delay.
   *
   * @param interval The interval, in seconds, at which the timer will fire.
   * Must be &gt; 0.
   * @param initialDelay An initial delay, in seconds, before the timer
   * should begin firing.
   */
  PulseTimer(timespan_s_t interval = 1, timespan_s_t initialDelay = 0);

  /** Destructor. */
  ~PulseTimer();

  /** Get the pluse count, i.e., the number of times the timer has fired. */
  inline uint32_t getPulseCount() const
  { return(_count); }

  /**
   * Get the time at which the timer last fired.
   *
   * @return The time of the last pulse, where 0 indicates that the timer
   * has not yet fired for the first time.
   */
  inline time_ms_t getLastPulseTime() const
  { return((_count > 0) ? _last : 0); }

 protected:

  void run();
  void cleanup();

  /**
   * This method is called each time the timer fires.
   *
   * @param now The current system time.
   */
  virtual void pulse(const time_ms_t& now) = 0;

 private:

  timespan_ms_t _interval;
  timespan_ms_t _delay;
  time_ms_t _last;
  uint32_t _count;
};

} // namespace ccxx

#endif // __ccxx_PulseTimer_hxx
