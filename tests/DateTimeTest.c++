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

#include "DateTimeTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/DateTime.h++"
#include "commonc++/DateTimeFormat.h++"
#include "commonc++/String.h++"
#include "commonc++/System.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(DateTimeTest);

/*
 */

CppUnit::Test *DateTimeTest::suite()
{
  CCXX_TESTSUITE_BEGIN(DateTimeTest);
  CCXX_TESTSUITE_TEST(DateTimeTest, testDateTime);
  CCXX_TESTSUITE_TEST(DateTimeTest, testTimeT);
  CCXX_TESTSUITE_TEST(DateTimeTest, testConversions);
  CCXX_TESTSUITE_END();
}

/*
 */

void DateTimeTest::setUp()
{
}

/*
 */

void DateTimeTest::tearDown()
{
}

/*
 */

void DateTimeTest::testTimeT()
{
  DateTime dt(6, DateTime::APR, 1975, 5, 30);
  time_s_t t = dt;
  CPPUNIT_ASSERT_EQUAL(166019400, t);
}

/*
 */

void DateTimeTest::testDateTime()
{
  DateTime dt1, dt2;

  dt1 = dt2;

  CPPUNIT_ASSERT(dt1 == dt2);
  TimeSpan ts(3, 3, 3, 0);
  dt2 += ts;

  CPPUNIT_ASSERT(dt2 > dt1);

  CPPUNIT_ASSERT(!(dt2 < dt1));

  DateTime dtx(6, DateTime::APR, 1975, 5, 30);

  CPPUNIT_ASSERT(dtx < dt2);
  CPPUNIT_ASSERT(dtx < dt1);

  DateTime dty(31, DateTime::FEB, 2000, 3, 33, 33, 333);

  CPPUNIT_ASSERT_EQUAL(false, dty.isValid());

  dty.normalize();

  CPPUNIT_ASSERT_EQUAL(true, dty.isValid());
}

/*
 */

void DateTimeTest::testConversions()
{
  DateTime now;
  DateTimeFormat fmt("%Y-%d-%m-%H-%M-%S-%s");

  String nowstr = fmt.format(now);

  time_ms_t tnow = now;
  DateTime now2(tnow);

  time_ms_t tnow2 = now2;

  String now2str = fmt.format(now2);

  CPPUNIT_ASSERT_EQUAL(tnow, tnow2);
}
