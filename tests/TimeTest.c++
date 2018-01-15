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

#include "TimeTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Time.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(TimeTest);

/*
 */

CppUnit::Test *TimeTest::suite()
{
  CCXX_TESTSUITE_BEGIN(TimeTest);
  CCXX_TESTSUITE_TEST(TimeTest, testTime);
  CCXX_TESTSUITE_END();
}

/*
 */

void TimeTest::setUp()
{
}

/*
 */

void TimeTest::tearDown()
{
}

/*
 */

void TimeTest::testTime()
{
  Time tm1(4, 30); // 4:30 AM
  Time tm2(18, 18); // 6:18 PM

  CPPUNIT_ASSERT(tm1 < tm2);
  CPPUNIT_ASSERT(tm2 > tm1);

  CPPUNIT_ASSERT_EQUAL(true, tm1.isAM());
  CPPUNIT_ASSERT_EQUAL(true, tm2.isPM());

  Time tm3;
  tm3.setTime(1, 0, 0, 333);
  tm3.setHour12(4, true); // 4pm

  CPPUNIT_ASSERT_EQUAL(16U, tm3.getHour());
  CPPUNIT_ASSERT_EQUAL(4U, tm3.getHour12());
  CPPUNIT_ASSERT_EQUAL(true, tm3.isPM());

  CPPUNIT_ASSERT_EQUAL(333U, tm3.getMillisecond());

  tm3.setTime(4, 30, 0, 0);

  CPPUNIT_ASSERT(tm1 >= tm3);
  CPPUNIT_ASSERT(tm1 <= tm3);

  Time tm4 = tm3;
  CPPUNIT_ASSERT(tm3 == tm4);

  tm4.setHour(99);
  tm4.setMinute(99);
  tm4.setSecond(99);

  CPPUNIT_ASSERT(tm3 == tm4);
}
