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

#include "DynamicArrayTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/DynamicArray.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(DynamicArrayTest);

/*
 */

CppUnit::Test *DynamicArrayTest::suite()
{
  CCXX_TESTSUITE_BEGIN(DynamicArrayTest);
  CCXX_TESTSUITE_TEST(DynamicArrayTest, testArray);
  CCXX_TESTSUITE_END();
}

/*
 */

void DynamicArrayTest::setUp()
{
}

/*
 */

void DynamicArrayTest::tearDown()
{
}

/*
 */

void DynamicArrayTest::testArray()
{
  DynamicArray<int> array(10);
  int i;

  CPPUNIT_ASSERT_EQUAL(10U, array.size());

  int c = 0;
  for(DynamicArray<int>::iterator iter = array.begin();
      iter != array.end();
      iter++)
  {
    *iter = c++;
  }

  for(i = 0; i < 10; i++)
    CPPUNIT_ASSERT(array[i] == i);

  bool exc = false;
  try
  {
    int x = array[10];
  }
  catch(OutOfBoundsException)
  {
    exc = true;
  }

  CPPUNIT_ASSERT_EQUAL(true, exc);

  array.resize(5);

  CPPUNIT_ASSERT_EQUAL(5U, array.size());

  for(i = 0; i < 5; i++)
    array[i] = i * 2;

  CPPUNIT_ASSERT_EQUAL(6, array[3]);

  exc = false;
  try
  {
    int x = array[6];

    uint_t y = 4;

    int z = array[y];
  }
  catch(OutOfBoundsException)
  {
    exc = true;
  }

  CPPUNIT_ASSERT_EQUAL(true, exc);

  DynamicArray<int> zeroarray(0);

  CPPUNIT_ASSERT_EQUAL(0U, zeroarray.size());

  bool empty = true;
  for(DynamicArray<int>::iterator iter = zeroarray.begin();
      iter != zeroarray.end();
      ++iter)
  {
    empty = false;
  }

  CPPUNIT_ASSERT_EQUAL(true, empty);
}
