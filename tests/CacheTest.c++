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

#include "CacheTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(CacheTest);

/*
 */

CppUnit::Test *CacheTest::suite()
{
  CCXX_TESTSUITE_BEGIN(CacheTest);
  CCXX_TESTSUITE_TEST(CacheTest, testCache);
  CCXX_TESTSUITE_END();
}

/*
 */

void CacheTest::setUp()
{
}

/*
 */

void CacheTest::tearDown()
{
}

/*
 */

TestCache::TestCache(uint_t maxSize)
  : Cache<int, String>(maxSize)
{
}

/*
 */

void CacheTest::testCache()
{
  static const uint_t CACHE_SIZE = 5;

  TestCache cache(CACHE_SIZE);
  cache.put(5, new String("five"));
  cache.put(4, new String("four"));
  cache.put(3, new String("three"));
  cache.put(2, new String("two"));
  cache.put(1, new String("one"));

  String *s;

  CPPUNIT_ASSERT_EQUAL(CACHE_SIZE, cache.getSize());

  s = cache.get(5);
  CPPUNIT_ASSERT_EQUAL(String("five"), *s);

  s = cache.get(5);
  CPPUNIT_ASSERT_EQUAL(String("five"), *s);

  // cache order should now be: five one two three four

  cache.put(6, new String("six"));
  CPPUNIT_ASSERT_EQUAL(CACHE_SIZE, cache.getSize());

  CPPUNIT_ASSERT_EQUAL(String("four"), cache.takeLastRemoved());

  CPPUNIT_ASSERT_EQUAL(false, cache.isEmpty());

  cache.clear();
  CPPUNIT_ASSERT_EQUAL(0U, cache.getSize());
  CPPUNIT_ASSERT_EQUAL(true, cache.isEmpty());
}

/*
 */

void TestCache::onRemove(int key, String *value)
{
  _removedItem = *value;
}

/*
 */

String TestCache::takeLastRemoved() const
{
  String val = _removedItem;
  _removedItem.clear();
  return(val);
}
