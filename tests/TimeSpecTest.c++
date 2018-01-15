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

#include "TimeSpecTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/String.h++"
#include "commonc++/TimeSpec.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(TimeSpecTest);

/*
 */

static const char *__inputs[] =
  {
  "-2,31-33,58-59 4,8,12 * * 1-2,4-5,7",
  "*\t* * *   *",
  NULL
  };

static const char *__outputs[] =
  {
  "31-33,58-59 4,8,12 * * 1-2,4-5,7",
  "* * * * *",
  NULL
  };

/*
 */

CppUnit::Test *TimeSpecTest::suite()
{
  CCXX_TESTSUITE_BEGIN(TimeSpecTest);
  CCXX_TESTSUITE_TEST(TimeSpecTest, testTimeSpec);
  CCXX_TESTSUITE_END();
}

/*
 */

void TimeSpecTest::setUp()
{
}

/*
 */

void TimeSpecTest::tearDown()
{
}

/*
 */

void TimeSpecTest::testTimeSpec()
{
  const char **q = __outputs;
  for(const char **p = __inputs; *p; p++, q++)
  {
    String s = *p;
    TimeSpec spec(s);

    String t = spec.toString();

    CPPUNIT_ASSERT(t == *q);
  }

//  std::cout << "matches now? " << spec.matches(System::currentTime())
//            << std::endl;

}
