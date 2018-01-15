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

#include "NetworkTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Network.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(NetworkTest);

/*
 */

CppUnit::Test *NetworkTest::suite()
{
  CCXX_TESTSUITE_BEGIN(NetworkTest);
  CCXX_TESTSUITE_TEST(NetworkTest, testNetwork);
  CCXX_TESTSUITE_TEST(NetworkTest, testNetworkInterface);
  CCXX_TESTSUITE_END();
}

/*
 */

void NetworkTest::setUp()
{
}

/*
 */

void NetworkTest::tearDown()
{
}

/*
 */

void NetworkTest::testNetwork()
{
  NetProtocol proto = ProtoAny;

  try
  {
    uint16_t port = Network::getPortForService("telnet", proto);

    CPPUNIT_ASSERT_EQUAL(uint16_t(23), port);

    // The /etc/services file on MAC OS X has both a tcp and a udp entry for
    // every single service, so this test isn't valid on that platform.
#ifndef CCXX_OS_MACOSX
    CPPUNIT_ASSERT_EQUAL(ProtoTCP, proto);
#endif

    proto = ProtoTCP;
    String name = Network::getServiceForPort(443, proto);

    CPPUNIT_ASSERT_EQUAL(String("https"), name);
    CPPUNIT_ASSERT_EQUAL(ProtoTCP, proto);
  }
  catch(SystemException& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}

/*
 */

void NetworkTest::testNetworkInterface()
{
  std::vector<NetworkInterface> list;

  std::cout << "\nEnumerating network interfaces:\n";

  try
  {
    Network::getInterfaces(list);

    for(std::vector<NetworkInterface>::const_iterator iter = list.begin();
        iter != list.end();
        iter++)
    {
      NetworkInterface ifx = *iter;

      std::cout << ifx.getName() << std::endl;
      std::cout << "  addr:  " << ifx.getAddress().toIPString()
                << std::endl;
      std::cout << "  bcast: " << ifx.getBroadcastAddress().toIPString()
                << std::endl;
      std::cout << "  mask:  " << ifx.getNetMask().toIPString()
                << std::endl;
      std::cout << "  MAC:   " << ifx.getHardwareAddress().toString()
                << std::endl;

      std::cout << "  ";

      if(ifx.isUp())
        std::cout << "UP ";
      if(ifx.isBroadcastSupported())
        std::cout << "BROADCAST ";
      if(ifx.isMulticastSupported())
        std::cout << "MULTICAST ";
      if(ifx.isLoopback())
        std::cout << "LOOPBACK ";
      if(ifx.isPPP())
        std::cout << "PPP ";
      if(ifx.isRunning())
        std::cout << "RUNNING ";

      std::cout << std::endl;

      std::cout << "  MTU = " << ifx.getMTU() << std::endl;
      std::cout << "  Metric = " << ifx.getMetric() << std::endl;
    }
  }
  catch(SystemException& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }

  std::cout << "\nDoes this look correct? (y/n) ";
  char c;
  std::cin >> c;
  CPPUNIT_ASSERT(tolower(c) == 'y');
}
