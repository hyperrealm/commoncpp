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

#include "ByteBufferDataWriterTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/ByteBufferDataWriter.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(ByteBufferDataWriterTest);

/*
 */

CppUnit::Test *ByteBufferDataWriterTest::suite()
{
  CCXX_TESTSUITE_BEGIN(ByteBufferDataWriterTest);
  CCXX_TESTSUITE_TEST(ByteBufferDataWriterTest, testByteBufferDataWriter);
  CCXX_TESTSUITE_END();
}

/*
 */

void ByteBufferDataWriterTest::setUp()
{
}

/*
 */

void ByteBufferDataWriterTest::tearDown()
{
}

/*
 */

void ByteBufferDataWriterTest::testByteBufferDataWriter()
{
  static const byte_t expected_buf[] =
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
      0x00, 0x00, 0x00, 0x00, 0x41, 0x41, 0x41, 0x41,
      0x00, 0x00, 0x00, 0x00, 0x42, 0x4C, 0x41, 0x48,
      0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  ByteBuffer buf(64);

  ByteBufferDataWriter writer(buf);

  try
  {
    CPPUNIT_ASSERT_EQUAL(true, writer.isTopContext());

    writer.setLimit(64);

    writer.pushContext();

    CPPUNIT_ASSERT_EQUAL(false, writer.isTopContext());

    writer.setLimit(32);

    for(int i = 0; i < 32; ++i)
      writer << byte_t(0xFF);

    writer.setOffset(5);
    writer << byte_t(0x00);
    writer.setOffset(32);

    writer.popContext();

    CPPUNIT_ASSERT_EQUAL(int64_t(32), writer.getOffset());
    CPPUNIT_ASSERT_EQUAL(int64_t(32), writer.getRemaining());
    CPPUNIT_ASSERT_EQUAL(int64_t(64), writer.getLimit());

    writer.pushContext();

    CPPUNIT_ASSERT_EQUAL(int64_t(0), writer.getOffset());
    CPPUNIT_ASSERT_EQUAL(int64_t(32), writer.getRemaining());
    CPPUNIT_ASSERT_EQUAL(int64_t(32), writer.getLimit());

    for(int i = 0; i < 8; ++i)
      writer << byte_t(0xEE);

    CPPUNIT_ASSERT_EQUAL(int64_t(8), writer.getOffset());
    CPPUNIT_ASSERT_EQUAL(int64_t(24), writer.getRemaining());
    CPPUNIT_ASSERT_EQUAL(int64_t(32), writer.getLimit());

    writer.popContext();

    CPPUNIT_ASSERT_EQUAL(int64_t(40), writer.getOffset());
    CPPUNIT_ASSERT_EQUAL(int64_t(24), writer.getRemaining());
    CPPUNIT_ASSERT_EQUAL(int64_t(64), writer.getLimit());

    writer.skip(4);

    writer << DataWriter::SetLength(8) << "AAAA" << DataWriter::SetLength(0)
           << "BLAH" << byte_t(0x11);

    CPPUNIT_ASSERT_EQUAL(int64_t(57), writer.getOffset());
    CPPUNIT_ASSERT_EQUAL(int64_t(7), writer.getRemaining());
    CPPUNIT_ASSERT_EQUAL(int64_t(64), writer.getLimit());

    CPPUNIT_ASSERT_EQUAL(true, writer.isTopContext());

    buf.flip();
    CPPUNIT_ASSERT_EQUAL(uint_t(57), buf.getRemaining());

/*
    for(size_t s = 0; s < buf.getRemaining(); ++s)
    {
      printf("%02X ", buf[s]);
      if(((s + 1) % 8) == 0)
        putchar('\n');
    }
    putchar('\n');
*/

    CPPUNIT_ASSERT_EQUAL(0, std::memcmp(buf.getPointer(), expected_buf,
                                        buf.getRemaining()));
  }
  catch(IOException &ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}
