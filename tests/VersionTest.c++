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

#include "VersionTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Version.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(VersionTest);

/*
 */

CppUnit::Test *VersionTest::suite()
{
  CCXX_TESTSUITE_BEGIN(VersionTest);
  CCXX_TESTSUITE_TEST(VersionTest, testVersion);
  CCXX_TESTSUITE_END();
}

/*
 */

void VersionTest::setUp()
{
}

/*
 */

void VersionTest::tearDown()
{
}

/*
 */

void VersionTest::testVersion()
{
  Version ver1("9:0:6");
  Version ver2("3.6.0");

  CPPUNIT_ASSERT_EQUAL(9U, ver1.getCurrent());
  CPPUNIT_ASSERT_EQUAL(0U, ver1.getRevision());
  CPPUNIT_ASSERT_EQUAL(6U, ver1.getAge());

  CPPUNIT_ASSERT(ver1 == ver2);

  String s1 = ver1.toString();
  CPPUNIT_ASSERT_EQUAL(String("3.6.0"), s1);

  String s2 = ver2.toCRAString();
  CPPUNIT_ASSERT_EQUAL(String("9:0:6"), s2);

  Version older(8, 0, 0);
  CPPUNIT_ASSERT_EQUAL(true, ver1.satisfies(older));

  Version too_old(2, 0, 0);
  CPPUNIT_ASSERT_EQUAL(false, ver1.satisfies(too_old));

  CPPUNIT_ASSERT_EQUAL(true, older.satisfiedBy(ver1));

  CPPUNIT_ASSERT_EQUAL(false, too_old.satisfiedBy(ver1));

  CPPUNIT_ASSERT(too_old < older);
  CPPUNIT_ASSERT(ver1 >= older);

  CPPUNIT_ASSERT(ver1 != older);
}
