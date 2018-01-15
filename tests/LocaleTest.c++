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

#include "LocaleTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Locale.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(LocaleTest);

/*
 */

CppUnit::Test *LocaleTest::suite()
{
  CCXX_TESTSUITE_BEGIN(LocaleTest);
  CCXX_TESTSUITE_TEST(LocaleTest, testLocale);
  CCXX_TESTSUITE_END();
}

/*
 */

void LocaleTest::setUp()
{
}

/*
 */

void LocaleTest::tearDown()
{
}

/*
 */

void LocaleTest::testLocale()
{
//  const Locale locale = Locale::getCurrent();
//  Locale locale("en", "US");

  Locale locale = Locale::getCurrent();

  for(int day = 1; day <= 7; ++day)
  {
    std::cout << "Day " << day << ": " << locale.getWeekDayName(day)
              << " (" << locale.getAbbreviatedWeekDayName(day) << ")"
              << std::endl;
  }

  for(int month = 1; month <= 12; ++month)
  {
    std::cout << "Month " << month << ": " << locale.getMonthName(month)
              << " (" << locale.getAbbreviatedMonthName(month) << ")"
              << std::endl;
  }


  std::cout << "AM: " << locale.getAMAffix() << std::endl;
  std::cout << "PM: " << locale.getPMAffix() << std::endl;

  std::cout << "encoding: " << locale.getCurrentEncoding() << std::endl;

  // TODO
}
