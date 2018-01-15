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

#include "RegExpTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/ParseException.h++"
#include "commonc++/RegExp.h++"
#include "commonc++/String.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(RegExpTest);

/*
 */

CppUnit::Test *RegExpTest::suite()
{
  CCXX_TESTSUITE_BEGIN(RegExpTest);
  CCXX_TESTSUITE_TEST(RegExpTest, testRegExp);
  CCXX_TESTSUITE_END();
}

/*
 */

void RegExpTest::setUp()
{
}

/*
 */

void RegExpTest::tearDown()
{
}

/*
 */

void RegExpTest::testRegExp()
{
  RegExp re;

  try
  {
    re.setPattern(" *([A-Za-z0-9]+) +([A-Za-z0-9]+) *");
  }
  catch(ParseException& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }

  RegExpMatch matches[3];
  String str = "  Hello  World1234  ";

  bool b = re.match(str, matches, CCXX_LENGTHOF(matches));
  CPPUNIT_ASSERT_EQUAL(true, b);

  if(b)
  {
    String m0 = str.substring(matches[0].getStartIndex(),
                               matches[0].getEndIndex());

    CPPUNIT_ASSERT_EQUAL(str, m0);

    String m1 = str.substring(matches[1].getStartIndex(),
                               matches[1].getEndIndex());

    CPPUNIT_ASSERT_EQUAL(String("Hello"), m1);

    String m2 = str.substring(matches[2].getStartIndex(),
                               matches[2].getEndIndex());

    CPPUNIT_ASSERT_EQUAL(String("World1234"), m2);
  }
}
