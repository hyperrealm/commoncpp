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

#include "CharTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Char.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(CharTest);

/*
 */

CppUnit::Test *CharTest::suite()
{
  CCXX_TESTSUITE_BEGIN(CharTest);
  CCXX_TESTSUITE_TEST(CharTest, testChar);
  CCXX_TESTSUITE_END();
}

/*
 */

void CharTest::setUp()
{
}

/*
 */

void CharTest::tearDown()
{
}

/*
 */

void CharTest::testChar()
{
  Char a = 'A';

  CPPUNIT_ASSERT(a == 'A');

  Char b = 'A';

  CPPUNIT_ASSERT(b == a);

  a++;

  CPPUNIT_ASSERT(a == 'B');

  CPPUNIT_ASSERT(a > b);
  CPPUNIT_ASSERT(b <= a);

  CPPUNIT_ASSERT(b != a);

  CPPUNIT_ASSERT('A' == b.toChar16());
}
