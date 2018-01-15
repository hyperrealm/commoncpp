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

#include "StopWatchTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/StopWatch.h++"
#include "commonc++/System.h++"
#include "commonc++/Thread.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(StopWatchTest);

/*
 */

CppUnit::Test *StopWatchTest::suite()
{
  CCXX_TESTSUITE_BEGIN(StopWatchTest);
  CCXX_TESTSUITE_TEST(StopWatchTest, testStopWatch);
  CCXX_TESTSUITE_END();
}

/*
 */

void StopWatchTest::setUp()
{
}

/*
 */

void StopWatchTest::tearDown()
{
}

/*
 */

void StopWatchTest::testStopWatch()
{
  StopWatch timer;

  timer.start();

  Thread::sleep(1500);

  timer.pause();

  Thread::sleep(400);

  timer.resume();

  Thread::sleep(300);

  timer.stop();

  uint64_t tm = timer.elapsedRealTime();
//  std::cout << "tm is: " << tm << std::endl;

  uint64_t delta = (tm > 1800 ? tm - 1800 : 1800 - tm);
//  std::cout << "delta is: " << delta << std::endl;
  CPPUNIT_ASSERT(delta <= 100);
}
