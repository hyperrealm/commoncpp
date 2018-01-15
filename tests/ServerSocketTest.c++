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

#include "ServerSocketTest.h++"
#include "Pinwheel.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/ServerSocket.h++"
#include "commonc++/System.h++"
#include "commonc++/Thread.h++"
#include "commonc++/Runnable.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(ServerSocketTest);

/*
 */

CppUnit::Test *ServerSocketTest::suite()
{
  CCXX_TESTSUITE_BEGIN(ServerSocketTest);
  CCXX_TESTSUITE_TEST(ServerSocketTest, testServerSocket);
  CCXX_TESTSUITE_END();
}

/*
 */

void ServerSocketTest::setUp()
{
}

/*
 */

void ServerSocketTest::tearDown()
{
}

/*
 */

void ServerSocketTest::testServerSocket()
{
  _counter = 0;

  RunnableDelegate<ServerSocketTest> r1(this,
                                        &ServerSocketTest::_server);
  RunnableDelegate<ServerSocketTest> r2(this,
                                        &ServerSocketTest::_client);

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

void ServerSocketTest::_server()
{
  try
  {
    ServerSocket ssock(40405);
    ssock.setReuseAddress(true);
    ssock.init();
    ssock.listen();

    ByteBuffer buf(2048);
    Pinwheel pinwheel;

    for(int i = 0; i < 5; i++)
    {
      StreamSocket sock;

      std::cout << "accepting conn #" << i << std::endl;

      ssock.accept(sock);
      std::cout << "connection accepted from: "
                << sock.getRemoteAddress().toString()
                << std::endl;

      pinwheel.spin();

      sock.setTimeout(10000);
      sock.read(buf);

      buf.flip();

      /*
      char *s = new char[buf.getRemaining() + 1 ];
      s[buf.getRemaining()] = 0;
      strcpy(s, (char *)buf.getBase());
      delete[] s;
      */

      bool match = true;

      if(buf.getRemaining() == 8)
      {
        for(int j = 0; j < 8; j++)
        {
          if(buf[j] != (i + 'A'))
          {
            match = false;
            break;
          }
        }
      }
      else
        match = false;

      if(match)
        _counter++;

      sock.shutdown();
    }

    ssock.shutdown();
  }
  catch(Exception& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}

/*
 */

void ServerSocketTest::_client()
{
  try
  {
    ByteBuffer buf(8);

    for(int i = 0; i < 5; i++)
    {
      StreamSocket sock;

      sock.init();
      sock.setTimeout(2000);

      std::cout << "connecting to localhost..." << std::endl;
      sock.connect("localhost", 40405);
      std::cout << "connected" << std::endl;

      buf.clear();

      for(int j = 0; j < 8; j++)
      {
        buf[j] = 'A' + i;
        buf.bump(1);
      }

      buf.flip();

      // std::cout << "sending #" << i << std::endl;
      sock.write(buf);

      sock.shutdown();

      Thread::sleep(750);
    }
  }
  catch(Exception& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}
