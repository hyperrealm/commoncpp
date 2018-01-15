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

#include "VariantTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Variant.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(VariantTest);

/*
 */

CppUnit::Test *VariantTest::suite()
{
  CCXX_TESTSUITE_BEGIN(VariantTest);
  CCXX_TESTSUITE_TEST(VariantTest, testVariant);
  CCXX_TESTSUITE_END();
}

/*
 */

void VariantTest::setUp()
{
}

/*
 */

void VariantTest::tearDown()
{
}

/*
 */

void VariantTest::testVariant()
{
  Variant a;
  a["foo"] = 3.14;

  Variant b = a;

  CPPUNIT_ASSERT(b == a);

  Variant v;
  v["one"] = 1;
  v["two"] = 2.020202;
  v["boolean"] = true;
  Variant &more = v["more"];
  more["a"] = "This is a test";
  more["b"] = 'c';
  more["ptr"] = (void *)this;

  Variant &array = more["d"];

  for(int i = 0; i < 10; i++)
    array[i] = i * 1000;

  std::cout << v << std::endl;

  Variant copy = v;

  CPPUNIT_ASSERT(copy == v);
}
