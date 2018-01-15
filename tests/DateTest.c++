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

#include "DateTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Date.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(DateTest);

/*
 */

CppUnit::Test *DateTest::suite()
{
  CCXX_TESTSUITE_BEGIN(DateTest);
  CCXX_TESTSUITE_TEST(DateTest, testDate);
  CCXX_TESTSUITE_END();
}

/*
 */

void DateTest::setUp()
{
}

/*
 */

void DateTest::tearDown()
{
}

/*
 */

void DateTest::testDate()
{
  Date dt1(4, 7, 1776); // Thursday

  CPPUNIT_ASSERT_EQUAL(Date::Thursday, dt1.getWeekday());
  CPPUNIT_ASSERT_EQUAL(186U, dt1.getDayOfYear());
  CPPUNIT_ASSERT_EQUAL(true, Date::isLeapYear(dt1.getYear()));

  Date dt2(29, 2, 1776); // Thursday
  CPPUNIT_ASSERT_EQUAL(29U, dt2.getDay());
  CPPUNIT_ASSERT_EQUAL(Date::FEB, dt2.getMonth());
  CPPUNIT_ASSERT_EQUAL(Date::Thursday, dt2.getWeekday());

  Date dt3;
  dt3.setDate(6, Date::APR, 1975); // Sunday

  CPPUNIT_ASSERT(dt1 > dt2);
  CPPUNIT_ASSERT(dt1 < dt3);

  dt3.setYear(2020); // Monday
  CPPUNIT_ASSERT_EQUAL(Date::Monday, dt3.getWeekday());

  Date dt4 = dt3;
  CPPUNIT_ASSERT(dt4 >= dt3);
  CPPUNIT_ASSERT(dt4 <= dt3);

  dt4.setMonth(99);
  dt4.setDay(99);
  dt4.setYear(999999);

  CPPUNIT_ASSERT(dt4 == dt3);
  CPPUNIT_ASSERT_EQUAL(2020U, dt4.getYear());
  CPPUNIT_ASSERT_EQUAL(Date::APR, dt4.getMonth());
  CPPUNIT_ASSERT_EQUAL(6U, dt4.getDay());
}
