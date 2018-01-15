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

#include "InetAddressTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/InetAddress.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(InetAddressTest);

/*
 */

CppUnit::Test *InetAddressTest::suite()
{
  CCXX_TESTSUITE_BEGIN(InetAddressTest);
  CCXX_TESTSUITE_TEST(InetAddressTest, testInetAddress);
  CCXX_TESTSUITE_END();
}

/*
 */

void InetAddressTest::setUp()
{
}

/*
 */

void InetAddressTest::tearDown()
{
}

/*
 */

void InetAddressTest::testInetAddress()
{
  InetAddress addr1("1.20.100.255");
  CPPUNIT_ASSERT_EQUAL(true, addr1.isResolved());
  CPPUNIT_ASSERT_EQUAL(0x011464FFU, addr1.getAddress());
  CPPUNIT_ASSERT_EQUAL(String("1.20.100.255"), addr1.toIPString());

  InetAddress addr2("255.255.255.255");
  CPPUNIT_ASSERT_EQUAL(true, addr2.isResolved());
  CPPUNIT_ASSERT_EQUAL(0xFFFFFFFF, addr2.getAddress());
  CPPUNIT_ASSERT_EQUAL(String("255.255.255.255"), addr2.toIPString());

  InetAddress addr3("google.com");

  try
  {
    addr3.resolve();
  }
  catch(const HostNotFoundException &hex)
  {
    CCXX_TEST_FAIL_EXCEPTION(hex);
  }

  CPPUNIT_ASSERT_EQUAL(true, addr3.isResolved());

  CPPUNIT_ASSERT(addr3.toIPString().startsWith("74.125."));

  uint32_t ip = 0x0A010A02; // 10.1.10.2
  addr3.setAddress(ip);
  CPPUNIT_ASSERT_EQUAL(true, addr3.isResolved());
  CPPUNIT_ASSERT_EQUAL(String("10.1.10.2"), addr3.toString());
  CPPUNIT_ASSERT(addr3.isNonroutable());

  InetAddress addr4("152.46.7.80");
  CPPUNIT_ASSERT_EQUAL('B', addr4.getNetworkClass());

  byte_t tuple[] = { 17, 251, 200, 70 };
  InetAddress addr5(tuple);

  try
  {
    addr5.resolve();
  }
  catch(const HostNotFoundException &hex)
  {
    CCXX_TEST_FAIL_EXCEPTION(hex);
  }

  CPPUNIT_ASSERT_EQUAL(true, addr5.isResolved());
  CPPUNIT_ASSERT(addr5 == InetAddress("17.251.200.70"));

  String host;

  try
  {
    host = addr5.getHost();
  }
  catch(const HostNotFoundException &hex)
  {
    CCXX_TEST_FAIL_EXCEPTION(hex);
  }

  CPPUNIT_ASSERT(host.endsWith("apple.com"));
}
