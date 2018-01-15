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

#include "MulticastSocketTest.h++"
#include "Pinwheel.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/MulticastSocket.h++"
#include "commonc++/System.h++"
#include "commonc++/Thread.h++"
#include "commonc++/Runnable.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(MulticastSocketTest);

/*
 */

CppUnit::Test *MulticastSocketTest::suite()
{
  CCXX_TESTSUITE_BEGIN(MulticastSocketTest);
  CCXX_TESTSUITE_TEST(MulticastSocketTest, testMulticastSocket);
  CCXX_TESTSUITE_END();
}

/*
 */

void MulticastSocketTest::setUp()
{
}

/*
 */

void MulticastSocketTest::tearDown()
{
}

/*
 */

void MulticastSocketTest::testMulticastSocket()
{
  _counters[0] = _counters[1] = 0;

  RunnableDelegate<MulticastSocketTest> send(this,
                                             &MulticastSocketTest::_sender);
  RunnableDelegate<MulticastSocketTest> recv1(this,
                                              &MulticastSocketTest::_receiver1);
  RunnableDelegate<MulticastSocketTest> recv2(this,
                                              &MulticastSocketTest::_receiver2);

  Thread t1(&send);
  Thread t2(&recv1);
  Thread t3(&recv2);

  t1.start();
  t2.start();
  t3.start();

  t1.join();
  t2.join();
  t3.join();

  CPPUNIT_ASSERT_EQUAL(5, _counters[0]);
  CPPUNIT_ASSERT_EQUAL(5, _counters[1]);
}

/*
 */

void MulticastSocketTest::_sender()
{
  DatagramSocket sock;
  ByteBuffer buf(8);
  Pinwheel pinwheel;

  Thread::sleep(1000);

  try
  {
    sock.init();
    sock.setTimeout(2000);
    sock.connect("224.1.1.1", 40505);

    for(int i = 0; i < 5; i++)
    {
      buf.clear();

      for(int j = 0; j < 8; j++)
      {
        buf[j] = i;
        buf.bump(1);
      }

      buf.flip();

      sock.send(buf);
      // std::cout << "sent dgram #" << i << std::endl;
      pinwheel.spin();

      Thread::sleep(750);
    }
  }
  catch(Exception& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}

/*
 */

void MulticastSocketTest::_receiver1()
{
  _receiver(0);
}

/*
 */

void MulticastSocketTest::_receiver2()
{
  _receiver(1);
}

/*
 */

void MulticastSocketTest::_receiver(int n)
{
  ByteBuffer buf(8);

  MulticastSocket msock(40505);
//  SocketAddress addr("224.1.1.1");

  try
  {
    msock.setReuseAddress(true);
    msock.init();
    msock.setTimeout(2000);
    msock.join("224.1.1.1");

    for(int i = 0; i < 5; i++)
    {
      buf.clear();

      SocketAddress addr;
      msock.receive(buf, addr);

      // std::cout << "got dgram from: " << addr << std::endl;

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
        _counters[n]++;
    }

    msock.leave("224.1.1.1");
  }
  catch(Exception& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}
