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

#include "MemoryStatsTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/MemoryStats.h++"
#include "commonc++/Thread.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(MemoryStatsTest);

/*
 */

CppUnit::Test *MemoryStatsTest::suite()
{
  CCXX_TESTSUITE_BEGIN(MemoryStatsTest);
  CCXX_TESTSUITE_TEST(MemoryStatsTest, testMemoryStats);
  CCXX_TESTSUITE_END();
}

/*
 */

void MemoryStatsTest::setUp()
{
}

/*
 */

void MemoryStatsTest::tearDown()
{
}

/*
 */

void MemoryStatsTest::testMemoryStats()
{
  MemoryStats stats;

  CPPUNIT_ASSERT(stats.collect());

  printf("PHYSICAL: Total: %dK, Free: %dK, Used: %dK\n",
         stats.getPhysicalMemoryTotal() / 1024,
         stats.getPhysicalMemoryFree() / 1024,
         stats.getPhysicalMemoryUsed() / 1024);

  printf("VIRTUAL:  Total: %dK, Free: %dK, Used: %dK\n",
         stats.getVirtualMemoryTotal() / 1024,
         stats.getVirtualMemoryFree() / 1024,
         stats.getVirtualMemoryUsed() / 1024);
}
