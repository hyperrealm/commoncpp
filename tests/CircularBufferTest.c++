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

#include "CircularBufferTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/CircularBuffer.h++"

CPPUNIT_TEST_SUITE_REGISTRATION(CircularBufferTest);

using namespace ccxx;

/*
 */

CppUnit::Test *CircularBufferTest::suite()
{
  CCXX_TESTSUITE_BEGIN(CircularBufferTest);
  CCXX_TESTSUITE_TEST(CircularBufferTest, testCircularBuffer);
  CCXX_TESTSUITE_END();
}

/*
 */

void CircularBufferTest::setUp()
{
}

/*
 */

void CircularBufferTest::tearDown()
{
}

/*
 */

void CircularBufferTest::testCircularBuffer()
{
  // TODO
}
