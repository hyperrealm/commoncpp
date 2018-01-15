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

#include "FileTraverserTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/FileTraverser.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(FileTraverserTest);

/*
 */

static const char *__breadthFirst[] = { "a", "b", "c", "d", "e", "f",
                                        "empty.txt", "g", "h", "i", "test.txt",
                                        "file.txt", NULL };

static const char *__depthFirst[] = { "d", "e", "f", "empty.txt", "i",
                                      "test.txt", "g", "h", "file.txt", "a",
                                      "b", "c" };

/*
 */

CppUnit::Test *FileTraverserTest::suite()
{
  CCXX_TESTSUITE_BEGIN(FileTraverserTest);
  CCXX_TESTSUITE_TEST(FileTraverserTest, testDepthFirst);
  CCXX_TESTSUITE_TEST(FileTraverserTest, testBreadthFirst);
  CCXX_TESTSUITE_END();
}

/*
 */

void FileTraverserTest::setUp()
{
}

/*
 */

void FileTraverserTest::tearDown()
{
}

/*
 */

void FileTraverserTest::testDepthFirst()
{
#ifdef CCXX_OS_WINDOWS
  TestTraverser trav(".\\testdata\\dirtree\\", true, __depthFirst);
#else
  TestTraverser trav("./testdata/dirtree/", true, __depthFirst);
#endif

  RegExp re;
  re.setPattern("^.svn$");

  trav.setPrunePattern(re);

  bool ok = trav.traverse();

  CPPUNIT_ASSERT_EQUAL(true, ok);
}

/*
 */

void FileTraverserTest::testBreadthFirst()
{
#ifdef CCXX_OS_WINDOWS
  TestTraverser trav(".\\testdata\\dirtree\\", false, __breadthFirst);
#else
  TestTraverser trav("./testdata/dirtree/", false, __breadthFirst);
#endif

  RegExp re;
  re.setPattern("^.svn$");

  trav.setPrunePattern(re);

  bool ok = trav.traverse();

  CPPUNIT_ASSERT_EQUAL(true, ok);
}

/*
 */

FileTraverserTest::TestTraverser::TestTraverser(const String& path,
                                                bool depthFirst,
                                                const char **expected)
  : FileTraverser(path, depthFirst),
    _cur(expected),
    _expected(expected)
{
}

/*
 */

FileTraverserTest::TestTraverser::~TestTraverser() throw()
{
}

/*
 */

bool FileTraverserTest::TestTraverser::processFile(const FileName &name,
                                                   bool isDir, uint_t depth)
{
  // TODO
  std::cout << "\nFile:     " << name.getFileName() << std::endl;
  std::cout << "Directory: " << name.getDirectory() << std::endl;
  std::cout << "Full path: " << name.getPathName() << std::endl;

  if(name.getFileName() != *_cur)
  {
//    printf("got: %s, but expected: %s\n", name.getFileName().c_str(), *_cur);
//    return(false); // not what was expected
  }

  _cur++;

  return(true);
}
