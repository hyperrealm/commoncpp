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

#include "CStringBuilderTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/CStringBuilder.h++"

#include <iostream>
#include <cstring>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(CStringBuilderTest);

/*
 */

CppUnit::Test *CStringBuilderTest::suite()
{
  CCXX_TESTSUITE_BEGIN(CStringBuilderTest);
  CCXX_TESTSUITE_TEST(CStringBuilderTest, testCStringBuilder);
  CCXX_TESTSUITE_END();
}

/*
 */

void CStringBuilderTest::setUp()
{
}

/*
 */

void CStringBuilderTest::tearDown()
{
}

/*
 */

void CStringBuilderTest::testCStringBuilder()
{
  char cbuf[256];

  CStringBuilder builder(cbuf, sizeof(cbuf));

  builder.append("foo");
  builder.append('b');
  builder.append('a');
  builder.append('r');
  builder.append(345, 10);
  builder.append("       ", 2);
  builder.append(-12345, 3);
  builder.terminate();

  CPPUNIT_ASSERT(! std::strcmp(cbuf, "foobar0000000345  -12345"));

  builder.clear();
  builder.append("2 digits: ");
  builder.append(1, 2, true);
  builder.terminate();

  CPPUNIT_ASSERT(! std::strcmp(cbuf, "2 digits: 01"));
}
