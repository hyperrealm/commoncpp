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

#include "SocketSelectorTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/SocketSelector.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(SocketSelectorTest);

/*
 */

CppUnit::Test *SocketSelectorTest::suite()
{
  CCXX_TESTSUITE_BEGIN(SocketSelectorTest);
  CCXX_TESTSUITE_TEST(SocketSelectorTest, testSocketSelector);
  CCXX_TESTSUITE_END();
}

/*
 */

void SocketSelectorTest::setUp()
{
}

/*
 */

void SocketSelectorTest::tearDown()
{
}

/*
 */

void SocketSelectorTest::testSocketSelector()
{
  try
  {
    ServerSocket ssock(40405);
    ssock.init();

    ssock.listen();

    TestSelector tmux;
    tmux.init(&ssock);
    tmux.start();

    std::cout << "Selector started. Press return to stop." << std::endl;

    getchar();

    tmux.stop();
    tmux.join();

    std::cout << "Selector joined." << std::endl;
  }
  catch(Exception& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}

/*
 */

TestConnection::TestConnection(int id)
  : _id(id),
    _rcount(0)
{
}

/*
 */

TestConnection::~TestConnection() throw()
{
}

/*
 */

TestSelector::TestSelector()
  : SocketSelector(64, 10000),
    _counter(0)
{
}

/*
 */

TestSelector::~TestSelector() throw()
{
}

/*
 */

Connection *TestSelector::connectionReady(const SocketAddress& address)
{
  int id = ++_counter;
  std::cout << "connection #" << id << " accepted from: "
            << address.toString() << std::endl;

  return(new TestConnection(id));
}

/*
 */

void TestSelector::dataReceived(Connection *conn)
{
  TestConnection *tconn = static_cast<TestConnection *>(conn);

  std::cout << "data received on conn #" << tconn->getID() << std::endl;

  conn->writeLine("Received some text from client.");

  if(tconn->bumpCount() == 2)
  {
    std::cout << "2 recvs; closing" << std::endl;
    conn->close();
  }
}

/*
 */

void TestSelector::connectionTimedOut(Connection *conn)
{
  TestConnection *tconn = static_cast<TestConnection *>(conn);

  std::cout << "conn #" << tconn->getID() << " timed out" << std::endl;
  delete conn;
}

/*
 */

void TestSelector::connectionClosed(Connection *conn)
{
  TestConnection *tconn = static_cast<TestConnection *>(conn);

  std::cout << "conn #" << tconn->getID() << " closed" << std::endl;
  delete conn;
}
