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

#include "StreamPipeTest.h++"
#include "Pinwheel.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/ServerStreamPipe.h++"
#include "commonc++/StreamPipe.h++"
#include "commonc++/System.h++"
#include "commonc++/Thread.h++"
#include "commonc++/Runnable.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(StreamPipeTest);

/*
 */

CppUnit::Test *StreamPipeTest::suite()
{
  CCXX_TESTSUITE_BEGIN(StreamPipeTest);
  CCXX_TESTSUITE_TEST(StreamPipeTest, testStreamPipe);
  CCXX_TESTSUITE_END();
}

/*
 */

void StreamPipeTest::setUp()
{
}

/*
 */

void StreamPipeTest::tearDown()
{
}

/*
 */

void StreamPipeTest::testStreamPipe()
{
  _counter = 0;

  RunnableDelegate<StreamPipeTest> r1(this, &StreamPipeTest::_server);
  RunnableDelegate<StreamPipeTest> r2(this, &StreamPipeTest::_client);

  Thread t1(&r1);
  Thread t2(&r2);

  t1.start();
  t2.start();

  t1.join();
  t2.join();

  CPPUNIT_ASSERT_EQUAL(5, _counter);
}

/*
 */

void StreamPipeTest::_server()
{
  try
  {
    Pinwheel pinwheel;

#ifdef CCXX_OS_POSIX
    ServerStreamPipe spipe("/tmp/testpipe");
#else
    ServerStreamPipe spipe("testpipe");
#endif

    spipe.listen();
    ByteBuffer buf(8);

    for(int i = 0; i < 5; i++)
    {
      StreamPipe pipe;

//      std::cout << "accepting..." << std::endl;
      spipe.accept(pipe);
//      std::cout << "got a connection" << std::endl;
      pinwheel.spin();

      buf.clear();
      bool match = false;
//      std::cout << "reading..." << std::endl;
      size_t sz = pipe.read(buf);

      pipe.close();

//      std::cout << "got " << sz << " bytes" << std::endl;

      if(sz == 8)
      {
        buf.flip();

        for(int j = 0; j < 8; j++)
        {
          if(buf[j] != 'A' + i)
            break;
        }

        match = true;
      }

      if(match)
        _counter++;
    }

    spipe.shutdown();
  }
  catch(Exception& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}

/*
 */

void StreamPipeTest::_client()
{
  ByteBuffer buf(8);

  Thread::sleep(500);

  try
  {
    for(int i = 0; i < 5; i++)
    {
      Thread::sleep(500);

      buf.clear();
      for(int j = 0; j < 8; j++)
        buf[j] = 'A' + i;

      buf.bump(8);

      buf.flip();

      StreamPipe pipe;
#ifdef CCXX_OS_POSIX
      pipe.connect("/tmp/testpipe");
#else
      pipe.connect("testpipe");
#endif

      pipe.write(buf);

      pipe.close();
//      std::cout << '.' << std::flush;
    }
  }
  catch(Exception& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}
