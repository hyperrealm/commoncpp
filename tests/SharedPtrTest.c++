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

#include "SharedPtrTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/SharedPtr.h++"
#include "commonc++/String.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(SharedPtrTest);

class Foo {
  public:

  Foo(int x)
    : x(x)
  { }

  virtual ~Foo() { }

  int x;
};

class Bar : public Foo {
  public:

  Bar(int x, int y)
    : Foo(x),
      y(y)
  { }

  int y;
};

class Baz : public Foo {
  public:

  Baz(int x, int z)
    : Foo(x),
      z(z)
  { }

  int z;
};

/*
 */

CppUnit::Test *SharedPtrTest::suite()
{
  CCXX_TESTSUITE_BEGIN(SharedPtrTest);
  CCXX_TESTSUITE_TEST(SharedPtrTest, testSharedPtr);
  CCXX_TESTSUITE_TEST(SharedPtrTest, testSharedPtrCasts);
  CCXX_TESTSUITE_TEST(SharedPtrTest, testNullPtr);
  CCXX_TESTSUITE_END();
}

/*
 */

void SharedPtrTest::setUp()
{
}

/*
 */

void SharedPtrTest::tearDown()
{
}

/*
 */

void SharedPtrTest::testSharedPtr()
{
  String *foo = new String("Hello");

  SharedPtr<String> p1(foo);
  SharedPtr<String> p2;
  CPPUNIT_ASSERT_EQUAL(1, p1.getRefCount());
  CPPUNIT_ASSERT_EQUAL(0, p2.getRefCount());

  p2 = p1;
  CPPUNIT_ASSERT_EQUAL(2, p1.getRefCount());
  CPPUNIT_ASSERT_EQUAL(2, p2.getRefCount());
  CPPUNIT_ASSERT(*p1 == *p2);

  p2 = NULL;
  CPPUNIT_ASSERT_EQUAL(1, p1.getRefCount());
  CPPUNIT_ASSERT_EQUAL(0, p2.getRefCount());
}

void SharedPtrTest::testSharedPtrCasts()
{
  SharedPtr<Bar> bar_ptr(new Bar(1, 2));

  // dynamic cast
  SharedPtr<Foo> foo_ptr = bar_ptr.dynamicCast<Foo>();
  CPPUNIT_ASSERT_EQUAL(1, foo_ptr->x);
  CPPUNIT_ASSERT(foo_ptr->x == bar_ptr->x);
  CPPUNIT_ASSERT_EQUAL(2, foo_ptr.getRefCount());
  CPPUNIT_ASSERT_EQUAL(2, bar_ptr.getRefCount());

  // static cast
  SharedPtr<Foo> foo_ptr_s = bar_ptr.staticCast<Foo>();
  CPPUNIT_ASSERT_EQUAL(1, foo_ptr_s->x);
  CPPUNIT_ASSERT_EQUAL(3, foo_ptr_s.getRefCount());
  CPPUNIT_ASSERT_EQUAL(3, foo_ptr.getRefCount());
  CPPUNIT_ASSERT_EQUAL(3, bar_ptr.getRefCount());

  // invalid dynamic cast
  SharedPtr<Baz> baz_ptr = bar_ptr.dynamicCast<Baz>();
  CPPUNIT_ASSERT_EQUAL(true, baz_ptr.isNull());
  CPPUNIT_ASSERT_EQUAL(3, bar_ptr.getRefCount());
}

void SharedPtrTest::testNullPtr()
{
  SharedPtr<Bar> bar_ptr;
  CPPUNIT_ASSERT_EQUAL(true, bar_ptr.isNull());
  CPPUNIT_ASSERT_EQUAL(0, bar_ptr.getRefCount());

  SharedPtr<Bar> bar_2_ptr = bar_ptr;
  CPPUNIT_ASSERT_EQUAL(true, bar_2_ptr.isNull());
  CPPUNIT_ASSERT_EQUAL(0, bar_2_ptr.getRefCount());
}
