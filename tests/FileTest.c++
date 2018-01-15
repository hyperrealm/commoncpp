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

#include "FileTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/File.h++"

#ifdef CCXX_OS_WINDOWS
#include <commonc++/Windows.h++>
#endif

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(FileTest);

/*
 */
CppUnit::Test *FileTest::suite()
{
  CCXX_TESTSUITE_BEGIN(FileTest);
  CCXX_TESTSUITE_TEST(FileTest, testFile);
  CCXX_TESTSUITE_TEST(FileTest, testSymLinks);
  CCXX_TESTSUITE_TEST(FileTest, testDirs);
  CCXX_TESTSUITE_TEST(FileTest, testCopyMoveRenameDel);
  CCXX_TESTSUITE_TEST(FileTest, testFilesystem);
  CCXX_TESTSUITE_TEST(FileTest, testPermissions);
  CCXX_TESTSUITE_TEST(FileTest, testTrimSeparators);
  CCXX_TESTSUITE_END();
}

/*
 */

void FileTest::setUp()
{
}

/*
 */

void FileTest::tearDown()
{
}

/*
 */

void FileTest::testFile()
{

  // TODO
}

/*
 */

void FileTest::testSymLinks()
{
#ifdef CCXX_OS_POSIX

  File::remove("./testdata/symlink");

  bool ok;
  String path;

  ok = File::createLink("..", "./testdata/symlink");
  CPPUNIT_ASSERT_EQUAL(true, ok);

  ok = File::isSymLink("./testdata/symlink");
  CPPUNIT_ASSERT_EQUAL(true, ok);

  path = "./testdata/symlink";

  ok = File::readSymLink(path);
  CPPUNIT_ASSERT_EQUAL(true, ok);

  CPPUNIT_ASSERT_EQUAL(String(".."), path);

#endif
}

/*
 */

void FileTest::testDirs()
{
  bool f;

  f = File::isDirectory("testdata/dirtree/");
  CPPUNIT_ASSERT_EQUAL(true, f);
  f = File::isFile("testdata/dirtree/");
  CPPUNIT_ASSERT_EQUAL(false, f);

  f = File::isFile("testdata/menu.xml");
  CPPUNIT_ASSERT_EQUAL(true, f);
  f = File::isDirectory("testdata/menu.xml");
  CPPUNIT_ASSERT_EQUAL(false, f);

  f = File::makeDirectory("testdata/foo/bar", true);
  CPPUNIT_ASSERT_EQUAL(true, f);

  f = File::isDirectory("testdata/foo/bar");
  CPPUNIT_ASSERT_EQUAL(true, f);

  f = File::removeDirectory("testdata/foo/bar");
  CPPUNIT_ASSERT_EQUAL(true, f);

  f = File::removeDirectory("testdata/foo");
  CPPUNIT_ASSERT_EQUAL(true, f);
}

/*
 */

void FileTest::testCopyMoveRenameDel()
{
  // copy

  File::remove("./testdata/sample_copy.gif");

  CPPUNIT_ASSERT_EQUAL(false, File::exists("./testdata/sample_copy.gif"));

  bool ok = File::copy("./testdata/sample.gif", "./testdata/sample_copy.gif");

  CPPUNIT_ASSERT_EQUAL(true, ok);

  CPPUNIT_ASSERT_EQUAL(true, File::exists("./testdata/sample_copy.gif"));

  int64_t sz = File::getSize("./testdata/sample.gif");

  CPPUNIT_ASSERT_EQUAL(File::getSize("./testdata/sample_copy.gif"), sz);

  // move

  File::remove("./testdata/delete_me.gif");

  ok = File::move("./testdata/sample_copy.gif",
                  "./testdata/delete_me.gif");

  CPPUNIT_ASSERT_EQUAL(true, ok);

  CPPUNIT_ASSERT_EQUAL(false, File::exists("./testdata/sample_copy.gif"));
  CPPUNIT_ASSERT_EQUAL(true, File::exists("./testdata/delete_me.gif"));

  CPPUNIT_ASSERT_EQUAL(sz, File::getSize("./testdata/delete_me.gif"));

  // rename

  File::remove("./testdata/delete_me_newname.gif");

  ok = File::rename("./testdata/delete_me.gif",
                    "./testdata/delete_me_newname.gif");

  CPPUNIT_ASSERT_EQUAL(true, ok);

  CPPUNIT_ASSERT_EQUAL(true, File::exists("./testdata/delete_me_newname.gif"));
  CPPUNIT_ASSERT_EQUAL(false, File::exists("./testdata/delete_me.gif"));

  // delete

  ok = File::remove("./testdata/delete_me_newname.gif");
  CPPUNIT_ASSERT_EQUAL(true, ok);
  CPPUNIT_ASSERT_EQUAL(false,
                       File::exists("./testdata/delete_me_newname.gif"));
}

/*
 */

void FileTest::testFilesystem()
{
  StringVec roots;
  int ct = 0;

  File::getFileSystemRoots(roots, true);
  for(StringVec::const_iterator iter = roots.begin();
      iter != roots.end();
      ++iter)
  {
    const String& path = *iter;
    uint64_t space = File::getDiskFreeSpace(path);

    std::cout << path << " - " << space << " bytes free" << std::endl;
    space /= 1024;
    std::cout << path << " - " << space << " Kb free" << std::endl;
    space /= 1024;
    std::cout << path << " - " << space << " Mb free" << std::endl;
    space /= 1024;
    std::cout << path << " - " << space << " Gb free" << std::endl;
    ++ct;
  }

  CPPUNIT_ASSERT(ct > 0);
}

/*
 */

void FileTest::testPermissions()
{

  // TODO
}

/*
 */

void FileTest::testTrimSeparators()
{
  static const char* in[] = { "/",
                              "///",
                              "/foo",
                              "/foo/bar",
                              "/foo/bar/",
                              "/foo/bar///",
                              NULL };

  static const char* out[] = { "/",
                               "/",
                               "/foo",
                               "/foo/bar",
                               "/foo/bar",
                               "/foo/bar",
                               NULL };

  const char **p, **q;

  for(p = in, q = out; *p; ++p, ++q)
  {
    String inPath(*p);
    String outPath(*q);

    String trimmed = File::removeTrailingSeparators(inPath);
    CPPUNIT_ASSERT_EQUAL(outPath, trimmed);
  }

}
