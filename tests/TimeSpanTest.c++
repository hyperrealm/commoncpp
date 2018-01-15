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

#include "TimeSpanTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/DateTime.h++"
#include "commonc++/String.h++"
#include "commonc++/TimeSpan.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(TimeSpanTest);

/*
 */

CppUnit::Test *TimeSpanTest::suite()
{
  CCXX_TESTSUITE_BEGIN(TimeSpanTest);
  CCXX_TESTSUITE_TEST(TimeSpanTest, testTimeSpan);
  CCXX_TESTSUITE_TEST(TimeSpanTest, testDateMath);
  CCXX_TESTSUITE_TEST(TimeSpanTest, testToString);
  CCXX_TESTSUITE_END();
}

/*
 */

void TimeSpanTest::setUp()
{
}

/*
 */

void TimeSpanTest::tearDown()
{
}

/*
 */

void TimeSpanTest::testTimeSpan()
{
  TimeSpan ts(1, 1, 1, 1);

  CPPUNIT_ASSERT_EQUAL(int64_t(86400 + 3600 + 60 + 1), ts.toSeconds());

  CPPUNIT_ASSERT(ts.toMilliseconds() == ts.toSeconds() * 1000);

  ts.setDays(2);
  ts.setHours(2);
  ts.setMinutes(2);
  ts.setSeconds(2);

  CPPUNIT_ASSERT_EQUAL(int64_t((86400*2) + (3600*2) + (60*2) + (1*2)),
                       ts.toSeconds());

  ts.addDays(-3);
  ts.addHours(-3);
  ts.addMinutes(-3);
  ts.addSeconds(-3);

  CPPUNIT_ASSERT_EQUAL(int64_t(-86400 + -3600 + -60 + -1), ts.toSeconds());

  CPPUNIT_ASSERT_EQUAL(-1, ts.getDays());
  CPPUNIT_ASSERT_EQUAL(-1, ts.getHours());
  CPPUNIT_ASSERT_EQUAL(-1, ts.getMinutes());
  CPPUNIT_ASSERT_EQUAL(-1, ts.getSeconds());

  TimeSpan ts2 = ts;

  CPPUNIT_ASSERT(ts2 == ts);

  ts2.setSpan(1, 1, 1, 1);

  CPPUNIT_ASSERT(ts2 > ts);

  ts2 += ts;

  TimeSpan ts3 = ts + 100;

  CPPUNIT_ASSERT_EQUAL(int64_t(0), ts2.toSeconds());
}

/*
 */

void TimeSpanTest::testDateMath()
{
  DateTime now;
  DateTime d2 = now + TimeSpan(539);
  TimeSpan diff = now - d2;

  CPPUNIT_ASSERT_EQUAL(int64_t(539), diff.toMilliseconds());
}

/*
 */

void TimeSpanTest::testToString()
{
  TimeSpan span1(12345678);
  String s1 = span1.toString();

  CPPUNIT_ASSERT_EQUAL(String("+03:25:45.678"), s1);

  TimeSpan span2(-12345678);
  String s2 = span2.toString();

  CPPUNIT_ASSERT_EQUAL(String("-03:25:45.678"), s2);

  TimeSpan span3(88, 77, 66, 55, 444);
  String s3 = span3.toString();

  CPPUNIT_ASSERT_EQUAL(String("+91d06:06:55.444"), s3);

  TimeSpan span4;
  String s4 = span4.toString();

  CPPUNIT_ASSERT_EQUAL(String("+00:00:00.000"), s4);

  TimeSpan span5(-1001001);
  String s5 = span5.toString();

  CPPUNIT_ASSERT_EQUAL(String("-00:16:41.001"), s5);
}
