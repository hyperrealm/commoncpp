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

#include "XDREncoderTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/ByteBufferDataWriter.h++"
#include "commonc++/File.h++"
#include "commonc++/MemoryBlock.h++"
#include "commonc++/XDREncoder.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(XDREncoderTest);

/*
 */

CppUnit::Test *XDREncoderTest::suite()
{
  CCXX_TESTSUITE_BEGIN(XDREncoderTest);
  CCXX_TESTSUITE_TEST(XDREncoderTest, testXDREncoder);
  CCXX_TESTSUITE_END();
}

/*
 */

void XDREncoderTest::setUp()
{
}

/*
 */

void XDREncoderTest::tearDown()
{
}

/*
 */

void XDREncoderTest::testXDREncoder()
{
  // encode test

  ByteBuffer buf(128);
  ByteBufferDataWriter writer(buf);
  XDREncoder xdrout(writer);

  byte_t out1[] =
    { 0x00, 0x00, 0x00, 0x63, // char 'c'
      0x00, 0x00, 0x00, 0xFF, // unsigned char 0xFF
      0x00, 0x00, 0x00, 0x01, // bool true
      0x00, 0x00, 0x7F, 0xFF, // INT16_MAX
      0x00, 0x00, 0xFF, 0xFF, // UINT16_MAX
      0x7F, 0xFF, 0xFF, 0xFF, // INT32_MAX
      0xFF, 0xFF, 0xFF, 0xFF, // UINT32_MAX
      0x7F, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, // INT64_MAX
      0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, // UINT64_MAX
      0x00, 0x00, 0x00, 0x0D,
      0x48, 0x65, 0x6C, 0x6C,
      0x6F, 0x2C, 0x20, 0x77,
      0x6F, 0x72, 0x6C, 0x64,
      0x21, 0x00, 0x00, 0x00, // "Hello, world!"
      0x40, 0x49, 0x0E, 0x56, // float 3.1415
      0x40, 0x09, 0x21, 0xCA,
      0xC0, 0x83, 0x12, 0x6F // double 3.1415
    };

  xdrout.encode('c'); // char
  xdrout.encode(0xFF); // unsigned char
  xdrout.encode(true); // bool
  xdrout.encode((int16_t)INT16_MAX); // 16-bit int
  xdrout.encode((uint16_t)UINT16_MAX); // 16-bit unsigned int
  xdrout.encode((int32_t)INT32_MAX); // 32-bit int
  xdrout.encode((uint32_t)UINT32_MAX); // 32-bit unsigned int
  xdrout.encode((int64_t)INT64_MAX); // 64-bit int
  xdrout.encode((uint64_t)UINT64_MAX); // 64-bit unsigned int
  xdrout.encode("Hello, world!"); // string
  xdrout.encode(3.1415f); // float
  xdrout.encode(3.1415); // double

  buf.flip();

  MemoryBlock m1(buf.getBase(), buf.getRemaining());
  MemoryBlock m2(out1, CCXX_LENGTHOF(out1));

  CPPUNIT_ASSERT(m1 == m2);

  // decode test

  // TODO
  /*
  XDREncoder xdrin(&buf, EncoderDecode);

  char c;
  xdrin >> c;
  CPPUNIT_ASSERT(c == 'c');

  unsigned char x;
  xdrin >> x;
  CPPUNIT_ASSERT(x == 0xFF);

  bool b;
  xdrin >> b;
  CPPUNIT_ASSERT(b == true);

  int16_t i16;
  xdrin >> i16;
  CPPUNIT_ASSERT(i16 == INT16_MAX);

  uint16_t iu16;
  xdrin >> iu16;
  CPPUNIT_ASSERT(iu16 == UINT16_MAX);

  int32_t i32;
  xdrin >> i32;
  CPPUNIT_ASSERT(i32 == INT32_MAX);

  uint32_t iu32;
  xdrin >> iu32;
  CPPUNIT_ASSERT(iu32 == UINT32_MAX);

  int64_t i64;
  xdrin >> i64;
  CPPUNIT_ASSERT(i64 == INT64_MAX);

  uint64_t iu64;
  xdrin >> iu64;
  CPPUNIT_ASSERT(iu64 == UINT64_MAX);

  String s;
  xdrin >> s;
  CPPUNIT_ASSERT(s == "Hello, world!");

  float f4;
  xdrin >> f4;
  CPPUNIT_ASSERT(f4 == 3.1415f);

  double f8;
  xdrin >> f8;
  CPPUNIT_ASSERT(f8 == 3.1415);
  */
}
