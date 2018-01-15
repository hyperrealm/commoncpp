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

#include "NumericTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Numeric.h++"

using namespace ccxx;

#include <iostream>

CPPUNIT_TEST_SUITE_REGISTRATION(NumericTest);

/*
 */

CppUnit::Test *NumericTest::suite()
{
  CCXX_TESTSUITE_BEGIN(NumericTest);
  CCXX_TESTSUITE_TEST(NumericTest, testNumericCast);
  CCXX_TESTSUITE_END();
}

/*
 */

void NumericTest::setUp()
{
}

/*
 */

void NumericTest::tearDown()
{
}

/*
 */

void NumericTest::testNumericCast()
{
  int32_t x = 65538;

  uint16_t y = ccxx::numeric_clipping_cast<int32_t, uint16_t>(x);

  CPPUNIT_ASSERT_EQUAL(uint16_t(65535), y);

  try
  {
    uint16_t z = ccxx::numeric_cast<int32_t, uint16_t>(x);
    CPPUNIT_FAIL("No OutOfBoundsException thrown.");
  }
  catch(OutOfBoundsException &ex)
  {
    // expected
  }
}
