/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2012  Mark A Lindner

   This file is part of commonc++.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in theh ope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   ---------------------------------------------------------------------------
 */

#include "DateTimeFormatTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/DateTime.h++"
#include "commonc++/DateTimeFormat.h++"
#include "commonc++/String.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(DateTimeFormatTest);

/*
 */

CppUnit::Test *DateTimeFormatTest::suite()
{
  CCXX_TESTSUITE_BEGIN(DateTimeFormatTest);
  CCXX_TESTSUITE_TEST(DateTimeFormatTest, testFormat);
  CCXX_TESTSUITE_TEST(DateTimeFormatTest, testParse);
  CCXX_TESTSUITE_END();
}

/*
 */

void DateTimeFormatTest::setUp()
{
}

/*
 */

void DateTimeFormatTest::tearDown()
{
}

/*
 */

void DateTimeFormatTest::testFormat()
{
  DateTime dt(6, DateTime::APR, 1975, 15, 30, 12);
  String s;

  DateTimeFormat fmt1("%$$w, %d %$$m %Y, %h:%0M:%0S %P");

  s = fmt1.format(dt);
  CPPUNIT_ASSERT_EQUAL(String("Sunday, 6 April 1975, 3:30:12 PM"), s);

  DateTimeFormat fmt2("%0d/%0m/%y %H:%0M:%0S %%");

  s = fmt2.format(dt);
  CPPUNIT_ASSERT_EQUAL(String("06/04/75 15:30:12 %"), s);

  dt.setHour(6);

  DateTimeFormat fmt3("\n\t%_H:%0M:%0S.%0s %0d %0D %$w %$m");

  s = fmt3.format(dt);
  CPPUNIT_ASSERT_EQUAL(String("\n\t 6:30:12.000 06 096 Sun Apr"), s);
}

/*
 */

void DateTimeFormatTest::testParse()
{
  // test 1

  DateTimeFormat fmt1("%$$w, %d %$$m %Y, %h:%0M:%0S %P");

  String s1 = "Sunday, 6 April 1975, 3:30:12 PM";

  DateTime dt1;

  try
  {
    fmt1.parse(dt1, s1);

    String s2 = fmt1.format(dt1);

    CPPUNIT_ASSERT(s1 == s2);
  }
  catch(ParseException &ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }

  // test 2

  DateTimeFormat fmt2("", "", "%h:%0M %p");

  String s3 = "12:34 am";

  Time dt2;

  try
  {
    fmt2.parse(dt2, s3);

    String s4 = fmt2.format(dt2);

    CPPUNIT_ASSERT(s3 == s4);
  }
  catch(ParseException &ex2)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex2);
  }
}
