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

#include "StreamSocketTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/StreamSocket.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(StreamSocketTest);

/*
 */

CppUnit::Test *StreamSocketTest::suite()
{
  CCXX_TESTSUITE_BEGIN(StreamSocketTest);
  CCXX_TESTSUITE_TEST(StreamSocketTest, testStreamSocket);
  CCXX_TESTSUITE_TEST(StreamSocketTest, testTimeoutConnect);
  CCXX_TESTSUITE_END();
}

/*
 */

void StreamSocketTest::setUp()
{
}

/*
 */

void StreamSocketTest::tearDown()
{
}

/*
 */

void StreamSocketTest::testStreamSocket()
{
  ByteBuffer buf(512);
  size_t total = 0;
  StreamSocket sock;

  try
  {
    sock.init();
    sock.setTimeout(5000);

    sock.connect("www.oracle.com", 80);

    buf.put((const byte_t *)"GET /\r\n\r\n", 9);
    buf.flip();

    sock.write(buf);

    for(;;)
    {
      buf.clear();
      size_t r = sock.read(buf);

      std::cout << '.' << std::flush;

      if(r == 0)
        break;
      else
        total += r;

      // buf.flip();
      // printf("%*s", buf.getRemaining(), buf.getBase());
      // fflush(stdout);
    }

    sock.close();
    sock.shutdown();
  }
  catch(EOFException& ex)
  {
    sock.close();
    sock.shutdown();
  }
  catch(Exception& ex)
  {
    sock.close();
    sock.shutdown();

    CCXX_TEST_FAIL_EXCEPTION(ex);
  }

  CPPUNIT_ASSERT(total > 0);
}

/*
 */

void StreamSocketTest::testTimeoutConnect()
{
  try
  {
    StreamSocket sock;
    sock.init();
    sock.setTimeout(3000);

    puts("connecting...");
    sock.connect("google.com", 25);
    puts("done");
  }
  catch(TimeoutException& tex)
  {
    // expected
  }
  catch(Exception& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}
