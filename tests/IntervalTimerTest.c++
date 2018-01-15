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

#include "IntervalTimerTest.h++"
#include "Pinwheel.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/IntervalTimer.h++"
#include "commonc++/Thread.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(IntervalTimerTest);

/*
 */

CppUnit::Test *IntervalTimerTest::suite()
{
  CCXX_TESTSUITE_BEGIN(IntervalTimerTest);
  CCXX_TESTSUITE_TEST(IntervalTimerTest, testIntervalTimer);
  CCXX_TESTSUITE_END();
}

/*
 */

void IntervalTimerTest::setUp()
{
}

/*
 */

void IntervalTimerTest::tearDown()
{
}

/*
 */

void IntervalTimerTest::testIntervalTimer()
{
  MyIntervalTimer timer;

  try
  {
    time_ms_t now = System::currentTimeMillis();
    timer.start();
    std::cout << "started! @ " << now << std::endl;

    Thread::sleep(1000 * 5);

    timer.stop();
  }
  catch(const SystemException &ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }

//  CPPUNIT_ASSERT_EQUAL(23, timer.getCount());
  // TODO
}

/*
 */

IntervalTimerTest::MyIntervalTimer::MyIntervalTimer()
  : IntervalTimer(500, 200),
    _count(0)
{
}

IntervalTimerTest::MyIntervalTimer::~MyIntervalTimer() throw()
{
}

void IntervalTimerTest::MyIntervalTimer::fired()
{
  time_ms_t now = System::currentTimeMillis();
  std::cout << "fired!   @ " << now << std::endl;

  _count++;
}
