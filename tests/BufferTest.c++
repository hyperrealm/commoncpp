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

#include "BufferTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Buffer.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(BufferTest);

/*
 */

CppUnit::Test *BufferTest::suite()
{
  CCXX_TESTSUITE_BEGIN(BufferTest);
  CCXX_TESTSUITE_TEST(BufferTest, testBuffer);
  CCXX_TESTSUITE_END();
}

/*
 */

void BufferTest::setUp()
{
}

/*
 */

void BufferTest::tearDown()
{
}

/*
 */

void BufferTest::testBuffer()
{
  static byte_t b1[]
    = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  static byte_t b2[]
    = { 0xAA, 0xBB, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  static byte_t b3[]
    = { 0xAA, 0xBB, 0xCC, 0xDD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  static byte_t b4[]
    = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x00, 0x00, 0x00, 0x00, 0x00 };


  ByteBuffer buf(10);
  buf.setLimit(8);

  CPPUNIT_ASSERT_EQUAL(10U, buf.getSize());

  CPPUNIT_ASSERT_EQUAL(true, _compare(buf, b1, 0, 8));

  // test writes

  byte_t p1[] = { 0xAA, 0xBB, 0xCC };

  buf.put(p1, 3);
  CPPUNIT_ASSERT_EQUAL(true, _compare(buf, b2, 3, 8));

  byte_t p2[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };

  CPPUNIT_ASSERT_EQUAL(false, buf.put(p2, 8));
  CPPUNIT_ASSERT_EQUAL(true, _compare(buf, b2, 3, 8));

  buf.put(0xDD);
  CPPUNIT_ASSERT_EQUAL(true, _compare(buf, b3, 4, 8));

  *(buf.getPointer()) = 0xEE;
  buf.bump(1);
  CPPUNIT_ASSERT_EQUAL(true, _compare(buf, b4, 5, 8));

  CPPUNIT_ASSERT_EQUAL(3U, buf.getRemaining());

  buf.skip(1);
  CPPUNIT_ASSERT_EQUAL(true, _compare(buf, b4, 6, 8));

  buf.skip(100);
  CPPUNIT_ASSERT_EQUAL(true, _compare(buf, b4, 8, 8));

  CPPUNIT_ASSERT_EQUAL(0U, buf.getRemaining());

  // test reads

  buf.flip();
  CPPUNIT_ASSERT_EQUAL(true, _compare(buf, b4, 0, 8));

  byte_t v1 = 0;
  CPPUNIT_ASSERT_EQUAL(true, buf.get(&v1));
  CPPUNIT_ASSERT_EQUAL(byte_t(0xAA), v1);

  byte_t v2[] = { 0, 0, 0 };
  CPPUNIT_ASSERT_EQUAL(true, buf.get(v2, sizeof(v2)));

  CPPUNIT_ASSERT_EQUAL(byte_t(0xBB), v2[0]);
  CPPUNIT_ASSERT_EQUAL(byte_t(0xCC), v2[1]);
  CPPUNIT_ASSERT_EQUAL(byte_t(0xDD), v2[2]);

  CPPUNIT_ASSERT_EQUAL(4U, buf.getRemaining());

  CPPUNIT_ASSERT_EQUAL(true, buf.get(v2, sizeof(v2)));
  CPPUNIT_ASSERT_EQUAL(false, buf.get(v2, sizeof(v2)));
}

/*
 */

bool BufferTest::_compare(ByteBuffer& buf, byte_t data[8], size_t pos,
                          size_t limit)
{
  if(pos != buf.getPosition())
    return(false);

  if(limit != buf.getLimit())
    return(false);

  if(memcmp(data, buf.getBase(), 8))
    return(false);

  return(true);
}
