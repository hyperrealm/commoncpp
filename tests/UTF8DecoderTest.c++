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

#include "UTF8DecoderTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/UTF8Decoder.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(UTF8DecoderTest);

/*
 */

CppUnit::Test *UTF8DecoderTest::suite()
{
  CCXX_TESTSUITE_BEGIN(UTF8DecoderTest);
  CCXX_TESTSUITE_TEST(UTF8DecoderTest, testDecode);
  CCXX_TESTSUITE_END();
}

/*
 */

void UTF8DecoderTest::setUp()
{
}

/*
 */

void UTF8DecoderTest::tearDown()
{
}

/*
 */

void UTF8DecoderTest::testDecode()
{
  // TODO
}
