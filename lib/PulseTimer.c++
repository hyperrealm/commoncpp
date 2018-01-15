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

#include "commonc++/PulseTimer.h++"
#include "commonc++/System.h++"
#include "commonc++/Thread.h++"

namespace ccxx {

/*
 */

PulseTimer::PulseTimer(timespan_s_t interval /* = 1 */,
                       timespan_s_t initialDelay /* = 0 */)
  : _interval((interval >= 1) ? (interval * 1000) : 1000),
    _delay(initialDelay * 1000),
    _last(System::currentTimeMillis()),
    _count(0)
{
}

/*
 */

PulseTimer::~PulseTimer()
{
}

/*
 */

void PulseTimer::run()
{
  bool first = true;
  timespan_s_t delaySec = _delay / 1000;
  timespan_s_t intervalSec = _interval / 1000;
  time_s_t last = 0;

  while(! testCancel())
  {
    time_ms_t now = System::currentTimeMillis();
    uint_t msec = (now % 1000);
    if(msec > 0)
    {
      msec = 1000 - msec;
      if(! trySleep(msec))
        break;

      now = System::currentTimeMillis();
    }

    time_s_t cur = now / 1000;
    if((cur < last) || (static_cast<timespan_s_t>(cur - last)
                        >= (first ? delaySec : intervalSec)))
    {
      pulse(now);
      _last = now;
      last = cur;
      ++_count;
    }

    first = false;
  }
}

/*
 */

void PulseTimer::cleanup()
{
}


} // namespace ccxx
