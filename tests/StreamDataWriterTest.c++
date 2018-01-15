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

#include "StreamDataWriterTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/File.h++"
#include "commonc++/StreamDataWriter.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(StreamDataWriterTest);

/*
 */

CppUnit::Test *StreamDataWriterTest::suite()
{
  CCXX_TESTSUITE_BEGIN(StreamDataWriterTest);
  CCXX_TESTSUITE_TEST(StreamDataWriterTest, testStreamDataWriter);
  CCXX_TESTSUITE_END();
}

/*
 */

void StreamDataWriterTest::setUp()
{
}

/*
 */

void StreamDataWriterTest::tearDown()
{
}

/*
 */

void StreamDataWriterTest::testStreamDataWriter()
{
  byte_t buf[128];
  CCXX_ZERO(buf);

  File file("foo.tmp");

  try
  {
    file.open(IOWrite, FileTruncateElseCreate);
    StreamDataWriter writer(file);

    writer.setLimit(64);

    writer.pushContext();

    writer.setLimit(32);

    for(int i = 0; i < 32; ++i)
      writer << byte_t(0xFF);

    writer.setOffset(5);
    writer << byte_t(0x00);
    writer.setOffset(32);

    writer.popContext();

    std::cout << "popped" << std::endl;
    std::cout << "offset: " << writer.getOffset() << std::endl;
    std::cout << "remain: " << writer.getRemaining() << std::endl;
    std::cout << "limit: " << writer.getLimit() << std::endl;

    writer.pushContext();

    std::cout << "pushed" << std::endl;

    std::cout << "offset: " << writer.getOffset() << std::endl;
    std::cout << "remain: " << writer.getRemaining() << std::endl;
    std::cout << "limit: " << writer.getLimit() << std::endl;

    for(int i = 0; i < 8; ++i)
      writer << byte_t(0xEE);

    std::cout << "offset: " << writer.getOffset() << std::endl;
    std::cout << "remain: " << writer.getRemaining() << std::endl;
    std::cout << "limit: " << writer.getLimit() << std::endl;

    writer.popContext();

    std::cout << "popped" << std::endl;

    std::cout << "offset: " << writer.getOffset() << std::endl;
    std::cout << "remain: " << writer.getRemaining() << std::endl;
    std::cout << "limit: " << writer.getLimit() << std::endl;

    writer.setOffset(40);

    writer.skip(4);

    writer << DataWriter::SetLength(8) << "AAAA" << DataWriter::SetLength(0)
           << "BLAH" << byte_t(0x11);

    writer.pushContext();

    std::cout << "offset: " << writer.getOffset() << std::endl;
    std::cout << "remain: " << writer.getRemaining() << std::endl;
    std::cout << "limit: " << writer.getLimit() << std::endl;

    writer.setOffset(0);
  }
  catch(IOException &ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }

  for(size_t s = 0; s < sizeof(buf); ++s)
  {
    printf("%02X ", buf[s]);
    if(((s + 1) % 16) == 0)
      putchar('\n');
  }

  // TODO
}
