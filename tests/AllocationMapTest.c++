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

#include "AllocationMapTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/AllocationMap.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(AllocationMapTest);

/*
 */

CppUnit::Test *AllocationMapTest::suite()
{
  CCXX_TESTSUITE_BEGIN(AllocationMapTest);
  CCXX_TESTSUITE_TEST(AllocationMapTest, testAllocationMap);
  CCXX_TESTSUITE_END();
}

/*
 */

void AllocationMapTest::setUp()
{
}

/*
 */

void AllocationMapTest::tearDown()
{
}

/*
 */

void AllocationMapTest::testAllocationMap()
{
  AllocationMap map(16);
  int i, index;

  for(i = 0; i < 10; i++)
  {
    index = map.reserve();
    CPPUNIT_ASSERT_EQUAL(i, index);
  }

  for(i = 0; i < 10; i += 2)
  {
    map.release(i);
    CPPUNIT_ASSERT(map.isReserved(i) == false);
  }

  for(i = 0; i < 10; i += 2)
  {
    index = map.reserve();
    CPPUNIT_ASSERT_EQUAL(i, index);
  }

  for(i = 10; i < 16; i++)
  {
    index = map.reserve();
    CPPUNIT_ASSERT_EQUAL(i, index);
  }

  index = map.reserve();
  CPPUNIT_ASSERT_EQUAL(-1, index);
}
