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

#include "SearchPathTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/SearchPath.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(SearchPathTest);

/*
 */

CppUnit::Test *SearchPathTest::suite()
{
  CCXX_TESTSUITE_BEGIN(SearchPathTest);
  CCXX_TESTSUITE_TEST(SearchPathTest, testSearchPath);
  CCXX_TESTSUITE_END();
}

/*
 */

void SearchPathTest::setUp()
{
}

/*
 */

void SearchPathTest::tearDown()
{
}

/*
 */

void SearchPathTest::testSearchPath()
{
#ifdef CCXX_OS_WINDOWS
  testSearchPathWindows();
#else
  testSearchPathPOSIX();
#endif
}

/*
 */

void SearchPathTest::testSearchPathPOSIX()
{
  SearchPath path("/bin:/usr/bin:/usr/sbin:/sbin");
  path.prepend("/usr/local/bin");
  path.append("/foo/bin");

  CPPUNIT_ASSERT_EQUAL(
    String("/usr/local/bin:/bin:/usr/bin:/usr/sbin:/sbin:/foo/bin"),
    path.getPath());

  FileName fn = "ifconfig";

  CPPUNIT_ASSERT_EQUAL(true, path.find(fn));
}

/*
 */

void SearchPathTest::testSearchPathWindows()
{
  SearchPath path("C:\\;D:\\blah;C:\\Program Files");
  path.prepend("C:\\Windows\\");

  CPPUNIT_ASSERT_EQUAL(
    String("C:\\Windows;C:\\;D:\\blah;C:\\Program Files"),
    path.getPath());

  path.remove("D:\\blah\\\\");
  CPPUNIT_ASSERT_EQUAL(String("C:\\Windows;C:\\;C:\\Program Files"),
                       path.getPath());

  path.append("C:\\Windows\\System32");
  CPPUNIT_ASSERT_EQUAL(
    String("C:\\Windows;C:\\;C:\\Program Files;C:\\Windows\\System32"),
    path.getPath());

  FileName fn = "notepad.exe";

  CPPUNIT_ASSERT_EQUAL(true, path.find(fn));
}
