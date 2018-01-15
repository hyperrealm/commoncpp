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

#include "SystemLogTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/SystemLog.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(SystemLogTest);

/*
 */

CppUnit::Test *SystemLogTest::suite()
{
  CCXX_TESTSUITE_BEGIN(SystemLogTest);
  CCXX_TESTSUITE_TEST(SystemLogTest, testSystemLog);
  CCXX_TESTSUITE_END();
}

/*
 */

void SystemLogTest::setUp()
{
}

/*
 */

void SystemLogTest::tearDown()
{
}

/*
 */

void SystemLogTest::testSystemLog()
{
  SystemLog log;

  log.open();
  log.logMessage(SystemLog::LogInfo, "(INFO) This is a test: %s %d", "foo", 5);

  log.logMessage(SystemLog::LogWarning, "(WARN) This is a test");
  log.logMessage(SystemLog::LogError, "(ERROR) This is a test");

  log.close();

  std::cout << "Three messages should have been logged. Are they in the"
    " system log? (y/n) ";
  char c;
  std::cin >> c;
  CPPUNIT_ASSERT(tolower(c) == 'y');
}
