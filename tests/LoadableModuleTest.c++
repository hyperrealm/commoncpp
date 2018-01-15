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

#include "LoadableModuleTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/LoadableModule.h++"

#include <cstring>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(LoadableModuleTest);

/*
 */

CppUnit::Test *LoadableModuleTest::suite()
{
  CCXX_TESTSUITE_BEGIN(LoadableModuleTest);
  CCXX_TESTSUITE_TEST(LoadableModuleTest, testPaths);
  CCXX_TESTSUITE_TEST(LoadableModuleTest, testLoadableModule);
  CCXX_TESTSUITE_END();
}

/*
 */

void LoadableModuleTest::setUp()
{
}

/*
 */

void LoadableModuleTest::tearDown()
{
}

/*
 */

void LoadableModuleTest::testPaths()
{
  String stem = "dummy";
  Version ver(9, 0, 6);

  String path = LoadableModule::fileNameForStem(stem);
  //  std::cout << "path: [" << path << "]" << std::endl;

#if defined CCXX_OS_POSIX || defined CCXX_OS_WINDOWS_GNU
  CPPUNIT_ASSERT(path == "libdummy.so");
#else
  CPPUNIT_ASSERT(path == "dummy.dll");
#endif

  String vpath = LoadableModule::fileNameForStem(stem, ver);
  //  std::cout << "vpath: [" << vpath << "]" << std::endl;

#if defined CCXX_OS_POSIX || defined CCXX_OS_WINDOWS_GNU
  CPPUNIT_ASSERT_EQUAL(String("libdummy.so.3.6.0"), vpath);
#else
  CPPUNIT_ASSERT_EQUAL(String("dummy-3.dll"), vpath);
#endif
}

/*
 */

void LoadableModuleTest::testLoadableModule()
{
  String module = "dummy";

  module.prepend(LoadableModule::getPrefix());
  module << '.' << LoadableModule::getExtension();

#ifdef CCXX_OS_WINDOWS
  module.prepend("..\\debug\\");
#else
  module.prepend(".libs/");  // TODO
#endif

  std::cout << "loading: " << module << std::endl;
  LoadableModule dlobj(module);

  try
  {
    dlobj.open();
  }
  catch(IOException& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }

  void *sym = dlobj["dummy_const"];
  CPPUNIT_ASSERT(sym != NULL);

  int *x = (int *)sym;

  CPPUNIT_ASSERT_EQUAL(12345, *x);


  sym = dlobj["dummy_test1"];
  CPPUNIT_ASSERT(sym != NULL);

  int (*fn)(int, int) = (int (*)(int, int))sym;

  int v = fn(123, 321);

  CPPUNIT_ASSERT_EQUAL(444, v);

  sym = dlobj["dummy_test_two"];
  CPPUNIT_ASSERT(sym != NULL);

  const char *(*fn2)(void) = (const char *(*)(void))sym;
  const char *vs = fn2();

  CPPUNIT_ASSERT(vs != NULL);
  CPPUNIT_ASSERT(! std::strcmp(vs, "hello"));

  dlobj.close();
}
