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

#include "DirectoryWatcherTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(DirectoryWatcherTest);

/*
 */

CppUnit::Test *DirectoryWatcherTest::suite()
{
  CCXX_TESTSUITE_BEGIN(DirectoryWatcherTest);
  CCXX_TESTSUITE_TEST(DirectoryWatcherTest, testDirectoryChange);
  CCXX_TESTSUITE_END();
}

/*
 */

void DirectoryWatcherTest::setUp()
{
}

/*
 */

void DirectoryWatcherTest::tearDown()
{
}

/*
 */

void DirectoryWatcherTest::testDirectoryChange()
{
  const char *dir;
#ifdef CCXX_OS_WINDOWS
  dir = "C:\\notify";
#else
  dir = "/tmp/notify";
#endif
  TestWatcher notifier(dir);
  try
  {
    notifier.init();
  }
  catch(const SystemException& sysex)
  {
    std::cerr << sysex << std::endl;
  }
  notifier.start();
  Thread::sleep(5000);
  notifier.stop();
  notifier.join();
}

/*
 */

TestWatcher::TestWatcher(const String& directory) throw(SystemException)
  : DirectoryWatcher(directory)
{
}

/*
 */

void TestWatcher::fileCreated(const String& path)
{
  std::cout << "file created: " << path << std::endl;
}

void TestWatcher::fileDeleted(const String& path)
{
  std::cout << "file deleted: " << path << std::endl;
}

void TestWatcher::fileModified(const String &path)
{
  std::cout << "file modified: " << path << std::endl;
}

void TestWatcher::fileRenamed(const String &oldPath, const String &newPath)
{
  std::cout << "file renamed: " << oldPath << " -> " << newPath << std::endl;
}
