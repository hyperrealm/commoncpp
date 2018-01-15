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

#include "LogTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Log.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(LogTest);

/*
 */

CppUnit::Test *LogTest::suite()
{
  CCXX_TESTSUITE_BEGIN(LogTest);
  CCXX_TESTSUITE_TEST(LogTest, testLog);
  CCXX_TESTSUITE_TEST(LogTest, testLogRotate);
  CCXX_TESTSUITE_TEST(LogTest, testLogMacros);
  CCXX_TESTSUITE_END();
}

/*
 */

void LogTest::setUp()
{
}

/*
 */

void LogTest::tearDown()
{
}

/*
 */

void LogTest::testLog()
{
  Log::setConsoleLogFormat("[%D %T] <%u@%H> %l %F:%L\n\t%@%m%.");

  Log_info("informational message %d %c %s", 123, 'x', "foo");
  Log_warning("warning message");
  Log_error("error message");
  Log_debug("debugging message");

  std::cout << "\nDoes this look correct? (y/n) ";
  char c;
  std::cin >> c;
  CPPUNIT_ASSERT(tolower(c) == 'y');

  Log_assert(2 * 2 == 2 + 2);
}

/*
 */

void LogTest::testLogRotate()
{
  Log::setUseConsoleLog(false);

  FileLogger *logger = new FileLogger("[%D %T] %m", 256, 6);
  Log::setFileLogger(logger);

  Log::setUseFileLog(true);
  Log::setLogFile(".", "logtest");

  for(int i = 0; i < 60000; i++)
  {
    Log_info("This is a test of the file log rotation mechanism.");
  }

  Log::setLogFileRotateCount(4);
  Log::setLogFileMaxSize(128);

  for(int i = 0; i < 60000; i++)
  {
    Log_info("This is a test of the file log rotation mechanism.");
  }

  Log::setUseConsoleLog(true);
  Log::setUseFileLog(false);
}

/*
 */

void LogTest::testLogMacros()
{
}
