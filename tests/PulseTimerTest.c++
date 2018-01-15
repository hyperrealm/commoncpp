/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2012  Mark A Lindner

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

#include "PulseTimerTest.h++"
#include "Pinwheel.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/PulseTimer.h++"
#include "commonc++/System.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(PulseTimerTest);

/*
 */

CppUnit::Test *PulseTimerTest::suite()
{
  CCXX_TESTSUITE_BEGIN(PulseTimerTest);
  CCXX_TESTSUITE_TEST(PulseTimerTest, testPulseTimer);
  CCXX_TESTSUITE_END();
}

/*
 */

void PulseTimerTest::setUp()
{
}

/*
 */

void PulseTimerTest::tearDown()
{
}

/*
 */

void PulseTimerTest::testPulseTimer()
{
  MyPulseTimer pulse;

  pulse.start();
  time_ms_t now = System::currentTimeMillis();

  Thread::sleep(6500);

  pulse.stop();
  pulse.join();

  int ct = pulse.getCount();

  CPPUNIT_ASSERT((ct >= 6) && (ct <= 8));

  time_ms_t lastDelta = 0;
  for(int i = 0; i < 5; i++)
  {
    now += 1000;
    time_ms_t pt = pulse.getTime(i);
    time_ms_t delta = (pt > now ? pt - now : now - pt);

    std::cout << "expected: " << now << ", actual: " << pt << ", delta: "
              << delta << std::endl;

    // ensure pulse is less than 1 second off from expected time
    CPPUNIT_ASSERT(delta < 1000);

    if(i > 0)
    {
      time_ms_t dd = (delta > lastDelta ? delta - lastDelta
                      : lastDelta - delta);
      // std::cout << "dd: " << dd << std::endl;
      // ensure variation in deltas is very small
      CPPUNIT_ASSERT(dd < 75);
    }

    lastDelta = delta;
  }
}

/*
 */

PulseTimerTest::MyPulseTimer::MyPulseTimer()
  : PulseTimer(1, 1),
    _count(0)
{
}

PulseTimerTest::MyPulseTimer::~MyPulseTimer() throw()
{
}

void PulseTimerTest::MyPulseTimer::pulse(const time_ms_t &now)
{
//  _pinwheel.spin();

  std::cout << "pulsed at " << now << std::endl;
  if(_count < 5)
    _pulseTimes[_count] = now;

  _count++;
}
