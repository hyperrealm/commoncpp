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

#include "BufferedStreamTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/BufferedStream.h++"
#include "commonc++/File.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(BufferedStreamTest);

/*
 */

CppUnit::Test *BufferedStreamTest::suite()
{
  CCXX_TESTSUITE_BEGIN(BufferedStreamTest);
  CCXX_TESTSUITE_TEST(BufferedStreamTest, testBufferedStream);
  CCXX_TESTSUITE_END();
}

/*
 */

void BufferedStreamTest::setUp()
{
}

/*
 */

void BufferedStreamTest::tearDown()
{
}

/*
 */

void BufferedStreamTest::testBufferedStream()
{
  File f("./testdata/testfile.txt");

  f.open(IOWrite);

  BufferedCharStream strm(f);

  strm << "Hello, world!\n" << false << ' ' << 5 << ", " << 3.14159625
       << std::endl;

  for(int i = 0; i < 135; i++)
    strm << "Hello, world. This is line #" << i << std::endl;

  f.close();

  // TODO
}
