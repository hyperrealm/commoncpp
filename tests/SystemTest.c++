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

#include "SystemTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/File.h++"
#include "commonc++/System.h++"
#include "commonc++/Thread.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(SystemTest);

/*
 */

CppUnit::Test *SystemTest::suite()
{
  CCXX_TESTSUITE_BEGIN(SystemTest);
  CCXX_TESTSUITE_TEST(SystemTest, testSystemInfo);
  CCXX_TESTSUITE_TEST(SystemTest, testEnvVars);
  CCXX_TESTSUITE_TEST(SystemTest, testTime);
  CCXX_TESTSUITE_END();
}

/*
 */

void SystemTest::setUp()
{
}

/*
 */

void SystemTest::tearDown()
{
}

/*
 */

void SystemTest::testSystemInfo()
{
  std::cout << '\n';

  std::cout << "POSIX?             " << (System::isPOSIX() ? "yes" : "no")
            << '\n';

  std::cout << "MacOS X?           " << (System::isMacOSX() ? "yes" : "no")
            << '\n';

  std::cout << "Windows?           " << (System::isWindows() ? "yes" : "no")
            << '\n';

//  std::cout << "thread ID:         " << std::hex << System::getThreadID()
//            << std::dec << '\n';
  std::cout << "current time:      " << System::currentTime() << '\n';
  std::cout << "current time (ms): " << System::currentTimeMillis() << '\n';

  std::cout << "current user:      " << System::getUserName() << '\n';
  std::cout << "OS name:           " << System::getOSName() << '\n';
  std::cout << "OS version:        " << System::getOSVersion() << '\n';
  std::cout << "architecture:      " << System::getArchitecture() << '\n';
  std::cout << "machine name:      " << System::getHostName() << '\n';

  std::cout << "home dir:          " << System::getHomeDir() << '\n';
  std::cout << "temp dir:          " << System::getTempDir() << '\n';

  std::cout << "page size:         " << System::getPageSize() << '\n';

  System::printStackTrace();

  std::cout << "\nDoes this look correct? (y/n) ";
  char c;
  std::cin >> c;

  CPPUNIT_ASSERT(tolower(c) == 'y');
}

/*
 */

void SystemTest::testEnvVars()
{
  bool ok = System::setEnvVariable("CCXX_TEST", "Hello, world!");
  CPPUNIT_ASSERT_EQUAL(true, ok);

  String var = System::getEnvVariable("CCXX_TEST");
  CPPUNIT_ASSERT_EQUAL(String("Hello, world!"), var);

  ok = System::delEnvVariable("CCXX_TEST");
  CPPUNIT_ASSERT_EQUAL(true, ok);

  var = System::getEnvVariable("CCXX_TEST");
  CPPUNIT_ASSERT_EQUAL(true, var.isNull());
}

/*
 */

void SystemTest::testTime()
{
  time_ms_t tv1 = System::currentTimeMillis();

  Thread::sleep(1500); // 1.5 sec

  time_ms_t tv2 = System::currentTimeMillis();

  int diff = (int)(tv2 - tv1) - 1500;

  CPPUNIT_ASSERT((diff > -50) && (diff < 50)); // within 50 ms.
}
