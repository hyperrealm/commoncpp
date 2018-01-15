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

#include "CPUStatsTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/CPUStats.h++"
#include "commonc++/Thread.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(CPUStatsTest);

/*
 */

CppUnit::Test *CPUStatsTest::suite()
{
  CCXX_TESTSUITE_BEGIN(CPUStatsTest);
  CCXX_TESTSUITE_TEST(CPUStatsTest, testCPUStats);
  CCXX_TESTSUITE_END();
}

/*
 */

void CPUStatsTest::setUp()
{
}

/*
 */

void CPUStatsTest::tearDown()
{
}

/*
 */

void CPUStatsTest::testCPUStats()
{
  CPUStats stats;

  for(int i = 0; i < 10; ++i)
  {
    Thread::sleep(1000);
    CPPUNIT_ASSERT(stats.collect());

    for(uint_t n = 0; n < stats.getCPUCount(); ++n)
    {
      printf("CPU # %d: ", n);
      float sys = stats.getSystem(n);
      float user = stats.getUser(n);
      float nice = stats.getNice(n);
      float idle = stats.getIdle(n);

      printf("  System: %.2f, User: %.2f, Nice: %.2f, Idle: %.2f\n",
             sys, user, nice, idle);
    }
    printf("OVERALL:   System: %.2f, User: %.2f, Nice: %.2f, Idle: %.2f\n",
           stats.getSystem(), stats.getUser(), stats.getNice(),
           stats.getIdle());

    putchar('\n');
  }

}
