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

#include "HexTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Hex.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(HexTest);

/*
 */

CppUnit::Test *HexTest::suite()
{
  CCXX_TESTSUITE_BEGIN(HexTest);
  CCXX_TESTSUITE_TEST(HexTest, testHex);
  CCXX_TESTSUITE_END();
}

/*
 */

void HexTest::setUp()
{
}

/*
 */

void HexTest::tearDown()
{
}

/*
 */

void HexTest::testHex()
{
  byte_t buf[] = { 0xAA, 0xBB, 0xCC, 0xDD,
                   0xEE, 0xFF, 0x00, 0x1A,
                   0xA1, 0x20, 0x35, 0x48 };

  size_t len = Hex::encode(buf, sizeof(buf), NULL, 0);

  CPPUNIT_ASSERT(len > 0);

  char *out = new char[len + 1];

  len = Hex::encode(buf, sizeof(buf), out, len);
  out[len] = NUL;

  CPPUNIT_ASSERT(len > 0);
  CPPUNIT_ASSERT_EQUAL(0, strcmp(out, "AABBCCDDEEFF001AA1203548"));

  len = Hex::encode(buf, sizeof(buf), out, len, true);
  out[len] = NUL;

  CPPUNIT_ASSERT(len > 0);
  CPPUNIT_ASSERT_EQUAL(0, strcmp(out, "aabbccddeeff001aa1203548"));

  delete[] out;
}
