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

#include "DatagramSocketTest.h++"
#include "Pinwheel.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/DatagramSocket.h++"
#include "commonc++/System.h++"
#include "commonc++/Thread.h++"
#include "commonc++/Runnable.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(DatagramSocketTest);

/*
 */

CppUnit::Test *DatagramSocketTest::suite()
{
  CCXX_TESTSUITE_BEGIN(DatagramSocketTest);
  CCXX_TESTSUITE_TEST(DatagramSocketTest, testDatagramSocket);
  CCXX_TESTSUITE_END();
}

/*
 */

void DatagramSocketTest::setUp()
{
}

/*
 */

void DatagramSocketTest::tearDown()
{
}

/*
 */

void DatagramSocketTest::testDatagramSocket()
{
  _counter = 0;

  RunnableDelegate<DatagramSocketTest> r1(this,
                                          &DatagramSocketTest::_recvThread);
  RunnableDelegate<DatagramSocketTest> r2(this,
                                          &DatagramSocketTest::_sendThread);

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

void DatagramSocketTest::_recvThread()
{
  DatagramSocket sock(40505);
  ByteBuffer buf(8);

  try
  {
    sock.init();
    sock.setTimeout(2000);

    sock.connect("localhost", 40504);

    for(int i = 0; i < 5; i++)
    {
      buf.clear();

      size_t sz = sock.receive(buf);
      std::cout << "got dgram" << std::endl;

      if(sz == 8)
      {
        bool match = true;

        for(int j = 0; j < 8; j++)
        {
          if(buf[j] != i)
          {
            match = false;
            break;
          }
        }

        if(match)
          _counter++;
      }
    }
  }
  catch(Exception& ex)
  {
    std::cout << "recvThread: " << ex << std::endl;
  }
}

/*
 */

void DatagramSocketTest::_sendThread()
{
  DatagramSocket sock(40504);
  ByteBuffer buf(8);
  Pinwheel pinwheel;

  try
  {
    sock.init();
    sock.setTimeout(5000);

    sock.connect("localhost", 40505);

    for(int i = 0; i < 5; i++)
    {
      buf.clear();

      for(int j = 0; j < 8; j++)
      {
        buf[j] = i;
        buf.bump(1);
      }

      buf.flip();

      pinwheel.spin();

      /* size_t sz = */ sock.send(buf);
      // std::cout << "sent dgram" << std::endl;

      Thread::sleep(750);
    }

    Thread::sleep(2000);
  }
  catch(Exception& ex)
  {
    std::cout << "sendThread: " << ex << std::endl;
  }
}
