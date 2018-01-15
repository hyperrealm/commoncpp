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

#include "BTreeTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(BTreeTest);

/*
 */

CppUnit::Test *BTreeTest::suite()
{
  CCXX_TESTSUITE_BEGIN(BTreeTest);
  CCXX_TESTSUITE_TEST(BTreeTest, testBTree);
  CCXX_TESTSUITE_END();
}

/*
 */

void BTreeTest::setUp()
{
}

/*
 */

void BTreeTest::tearDown()
{
}

/*
 */

void BTreeTest::testBTree()
{
  BTree<int, String *, 5> btree(100);
  int s1[] = {  1, 22, 5 };
  int s2[] = { 22,  1, 5 };
  int s3[] = {  5,  1, 22 };
  int s4[] = {  1, 22 };

  String a = "one";
  String b = "five";
  String c = "twenty-two";

  btree.put(5, &b);
  btree.put(22, &c);
  btree.put(1, &a);

  CPPUNIT_ASSERT_EQUAL(true, btree.contains(22));

  CPPUNIT_ASSERT_EQUAL(true, _iterCheck(btree, s1, CCXX_LENGTHOF(s1)));

  String *x;

  x = btree.get(22);
  CPPUNIT_ASSERT(x != NULL);
  if(x)
    CPPUNIT_ASSERT_EQUAL(String("twenty-two"), *x);

  CPPUNIT_ASSERT_EQUAL(true, _iterCheck(btree, s2, CCXX_LENGTHOF(s2)));

  x = btree.get(1);
  CPPUNIT_ASSERT(x != NULL);
  if(x)
    CPPUNIT_ASSERT_EQUAL(String("one"), *x);

  x = btree.get(5);
  CPPUNIT_ASSERT(x != NULL);
  if(x)
    CPPUNIT_ASSERT_EQUAL(String("five"), *x);

  CPPUNIT_ASSERT_EQUAL(true, _iterCheck(btree, s3, CCXX_LENGTHOF(s3)));

  x = btree.get(8);
  CPPUNIT_ASSERT(x == NULL);

  btree.remove(5);
  x = btree.get(5);
  CPPUNIT_ASSERT(x == NULL);

  CPPUNIT_ASSERT_EQUAL(true, _iterCheck(btree, s4, CCXX_LENGTHOF(s4)));
}

/*
 */

bool BTreeTest::_iterCheck(BTree<int, String *, 5>& tree, int *items,
                           size_t len)
{
  std::list<int> keys;

  tree.getKeys(keys);

  if(keys.size() != len)
    return(false);

  int i = 0;
  for(std::list<int>::const_iterator iter = keys.begin();
      iter != keys.end();
      iter++, i++)
  {
    if(*iter != items[i])
      return(false);
  }

  return(true);
}
