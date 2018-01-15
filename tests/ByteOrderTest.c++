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

#include "ByteOrderTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/ByteOrder.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(ByteOrderTest);

/*
 */

CppUnit::Test *ByteOrderTest::suite()
{
  CCXX_TESTSUITE_BEGIN(ByteOrderTest);
  CCXX_TESTSUITE_TEST(ByteOrderTest, testByteOrder);
  CCXX_TESTSUITE_TEST(ByteOrderTest, testReverseBytes);
  CCXX_TESTSUITE_END();
}

/*
 */

void ByteOrderTest::setUp()
{
}

/*
 */

void ByteOrderTest::tearDown()
{
}

/*
 */

void ByteOrderTest::testByteOrder()
{
  bool bigEndian = ByteOrder::isBigEndian();

  CPPUNIT_ASSERT(bigEndian != ByteOrder::isLittleEndian());

  uint32_t x4 = 0xAABBCCDD;
  uint32_t x4r = 0xDDCCBBAA;
  uint32_t v4;

  v4 = ByteOrder::hostToNetwork(x4);
  CPPUNIT_ASSERT((bigEndian && (v4 == x4)) || (v4 == x4r));

  uint16_t x2 = 0x1122;
  uint16_t x2r = 0x2211;
  uint16_t v2;

  v2 = ByteOrder::hostToNetwork(x2);
  CPPUNIT_ASSERT((bigEndian && (v2 == x2)) || (v2 == x2r));

  uint64_t x8 = UINT64_CONST(0x1122334455667788);
  uint64_t x8r = UINT64_CONST(0x8877665544332211);
  uint64_t v8;

  v8 = ByteOrder::hostToNetwork(x8);

  CPPUNIT_ASSERT((bigEndian && (v8 == x8)) || (v8 == x8r));
}

/*
 */

void ByteOrderTest::testReverseBytes()
{
  uint64_t val = UINT64_CONST(0x1122334455667788);

  ByteOrder::reverseBytes(val);

  CPPUNIT_ASSERT_EQUAL(UINT64_CONST(0x8877665544332211), val);

  byte_t bval = 0xAA;

  ByteOrder::reverseBytes(bval);

  CPPUNIT_ASSERT_EQUAL(byte_t(0xAA), bval);

  uint16_t val16 = 0x1234;

  ByteOrder::reverseBytes(val16);

  CPPUNIT_ASSERT_EQUAL(uint16_t(0x3412), val16);
}
