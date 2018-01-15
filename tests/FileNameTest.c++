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

#include "FileNameTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/FileName.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(FileNameTest);

/*
 */

CppUnit::Test *FileNameTest::suite()
{
  CCXX_TESTSUITE_BEGIN(FileNameTest);
  CCXX_TESTSUITE_TEST(FileNameTest, testFileName);
  CCXX_TESTSUITE_TEST(FileNameTest, testCanonicalize);
  CCXX_TESTSUITE_END();
}

/*
 */

void FileNameTest::setUp()
{
}

/*
 */

void FileNameTest::tearDown()
{
}

/*
 */

void FileNameTest::testFileName()
{
#ifdef CCXX_OS_WINDOWS
  String paths[7] = { "C:\\tmp\\foo\\bar.txt",
                       "..\\..\\..\\..\\.\\.\\..\\\\\\\\",
                       ".",
                       "foobarbaz",
                       ".xyz",
                       "blah.txt",
                       "C:\\Windows\\" };
#else
  String paths[7] = { "/tmp/foo/bar.txt",
                       "../../../../././..////",
                       ".",
                       "foobarbaz",
                       ".xyz",
                       "blah.txt",
                       "/usr" };
#endif

  for(int i = 0; i < 7; i++)
  {
    std::cout << "Raw path: " << paths[i] << std::endl;
    FileName fn = paths[i];

    _dumpFileName(fn);

    fn.pushDirectory("dir1");
#ifdef CCXX_OS_WINDOWS
    fn.pushDirectory("dir2\\..\\blarf\\.\\\\arg\\..");
#else
    fn.pushDirectory("dir2/../blarf/.//arg/..");
#endif

    std::cout << "after push:" << std::endl;
    _dumpFileName(fn);

    fn.canonicalize();
    std::cout << "after canonicalization:" << std::endl;
    _dumpFileName(fn);
  }

#ifdef CCXX_OS_WINDOWS
  FileName f1 = "\\tmp\\foo.txt";
#else
  FileName f1 = "/tmp/foo.txt";
#endif
  FileName f2(f1);

  std::cout << "f2: " << f2.getPathName() << std::endl;


  // TODO
}

/*
 */

void FileNameTest::_dumpFileName(const FileName& fn)
{
  std::cout << "  path name: " << fn.getPathName() << std::endl;
  std::cout << "  volume:    " << fn.getVolume() << std::endl;
  std::cout << "  directory: " << fn.getDirectory() << std::endl;
  std::cout << "  base name: " << fn.getBaseName() << std::endl;
  std::cout << "  extension: " << fn.getExtension() << std::endl;
  std::cout << "  file name: " << fn.getFileName() << std::endl;
}

/*
 */

void FileNameTest::testCanonicalize()
{
#ifdef CCXX_OS_WINDOWS

  FileName fn("c:\\foo\\bar/..\\//baz\\");

  String path = fn.canonicalize().getPathName();

  std::cout << "path: " << path << std::endl;

  CPPUNIT_ASSERT(path == "C:\\foo\\baz\\");

#else

  // TODO

#endif
}
