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

#include "LogFormatTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Log.h++"
#include "commonc++/LogFormat.h++"

#include <iostream>

CPPUNIT_TEST_SUITE_REGISTRATION(LogFormatTest);

using namespace ccxx;

/*
 */

CppUnit::Test *LogFormatTest::suite()
{
  CCXX_TESTSUITE_BEGIN(LogFormatTest);
  CCXX_TESTSUITE_TEST(LogFormatTest, testLogFormat);
  CCXX_TESTSUITE_END();
}

/*
 */

void LogFormatTest::setUp()
{
}

/*
 */

void LogFormatTest::tearDown()
{
}

/*
 */

void LogFormatTest::testLogFormat()
{
  Log::setConsoleLogFormat("[time (long): %T, time (short): %t] %m");
  Log_info("time test");

  Log::setConsoleLogFormat("[date (long): %D, date (short): %d] %m");
  Log_info("date test");

  Log::setConsoleLogFormat("[%F:%L] %m");
  Log_info("source file and line test");

  Log::setConsoleLogFormat("[process: %p, thread: %h] %m");
  Log_info("process & thread ID test");

  Log::setConsoleLogFormat("[name: %o, version: %v, arch: %a] %m");
  Log_info("OS & architecture test");

  Log::setConsoleLogFormat("[user: %u, hostname: %H] %m");
  Log_info("user & hostname test");

  Log::setConsoleLogFormat("[%d %T] %F:%L %@%m%.");
  Log_info("autocoloring test");

  Log::setConsoleLogFormat("[app: %A level: %l %% %n] %m");
  Log_info("application name, log level & literal %% and newline test");

  Log::setConsoleLogFormat("[%*bold%- %_underline%- %#inverse%-] %m");
  Log_info("font style test");

  Log::setConsoleLogFormat("[%1black%0 %2red%0 %3green%0 %4yellow%0"
                           " %5blue%0 %6magenta%0 %7cyan%0 %8white%0"
                           " %0default] %m");
  Log_info("font foreground color test");

  Log::setConsoleLogFormat("[%? % %q %X% ] %m");
  Log_info("invalid tokens test");

  std::cout << "\nDoes this look correct? (y/n) ";
  char c;
  std::cin >> c;
  CPPUNIT_ASSERT(tolower(c) == 'y');
}
