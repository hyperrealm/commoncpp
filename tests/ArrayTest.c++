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

#include "ArrayTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Array.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(ArrayTest);

/*
 */

CppUnit::Test *ArrayTest::suite()
{
  CCXX_TESTSUITE_BEGIN(ArrayTest);
  CCXX_TESTSUITE_TEST(ArrayTest, testArray);
  CCXX_TESTSUITE_END();
}

/*
 */

void ArrayTest::setUp()
{
}

/*
 */

void ArrayTest::tearDown()
{
}

/*
 */

void ArrayTest::testArray()
{
  Array<int, 10> array;

  CPPUNIT_ASSERT_EQUAL(10U, array.size());

  int c = 0;
  for(Array<int, 10>::iterator iter = array.begin();
      iter != array.end();
      iter++)
  {
    *iter = c++;
  }

  for(int i = 0; i < 10; i++)
    CPPUNIT_ASSERT_EQUAL(i, array[i]);

  bool exc = false;
  try
  {
    int x = array[10];
  }
  catch(OutOfBoundsException)
  {
    exc = true;
  }

  CPPUNIT_ASSERT(exc);
}
