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

#include "TempFileTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/File.h++"
#include "commonc++/TempFile.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(TempFileTest);

/*
 */

CppUnit::Test *TempFileTest::suite()
{
  CCXX_TESTSUITE_BEGIN(TempFileTest);
  CCXX_TESTSUITE_TEST(TempFileTest, testTempFile);
  CCXX_TESTSUITE_END();
}

/*
 */

void TempFileTest::setUp()
{
}

/*
 */

void TempFileTest::tearDown()
{
}

/*
 */

void TempFileTest::testTempFile()
{
  File::remove("./testdata/test.txt");

  TempFile tmp("./testdata/test.txt");

  tmp.open();

  ByteBuffer buf(128);

  strcpy((char*)buf.getBase(), "Hello, world!\n");
  buf.bump(14);
  buf.flip();

  tmp.write(buf);

  CPPUNIT_ASSERT_EQUAL(false, File::exists("./testdata/test.txt"));

  tmp.close();

  CPPUNIT_ASSERT_EQUAL(true, File::exists("./testdata/test.txt"));

  // TODO
}
