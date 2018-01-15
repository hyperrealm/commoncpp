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

#include "BitSetTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/BitSet.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(BitSetTest);

/*
 */

CppUnit::Test *BitSetTest::suite()
{
  CCXX_TESTSUITE_BEGIN(BitSetTest);
  CCXX_TESTSUITE_TEST(BitSetTest, testBitSet);
  CCXX_TESTSUITE_TEST(BitSetTest, testBitShift);
  CCXX_TESTSUITE_TEST(BitSetTest, testLogicalOps);
  CCXX_TESTSUITE_TEST(BitSetTest, testFindNext);
  CCXX_TESTSUITE_END();
}

/*
 */

void BitSetTest::setUp()
{
}

/*
 */

void BitSetTest::tearDown()
{
}

/*
 */

void BitSetTest::testBitSet()
{
  BitSet set(65);
  uint_t i;

  for(i = 0; i < set.getSize(); i += 2)
    set.set(i);

  String s = set.toString();

  CPPUNIT_ASSERT_EQUAL(String("10101010101010101010101010101010101010101010101010101010101010101"), s);

  set.setRange(30,40);
  set.setRange(63, 80);
  s = set.toString();

  CPPUNIT_ASSERT_EQUAL(String("10101010101010101010101010101011111111111010101010101010101010111"), s);

  set.clearRange(5,8);
  s = set.toString();

  CPPUNIT_ASSERT_EQUAL(String("10101000001010101010101010101011111111111010101010101010101010111"), s);

  set.flip();
  s = set.toString();

  CPPUNIT_ASSERT_EQUAL(String("01010111110101010101010101010100000000000101010101010101010101000"), s);

  CPPUNIT_ASSERT_EQUAL(true, set.isClear(2));

  CPPUNIT_ASSERT_EQUAL(true, set.isAnySet());
  set.clearAll();
  CPPUNIT_ASSERT_EQUAL(false, set.isAnySet());

  for(i = 0; i < set.getSize(); i+= 2)
    set[i] = 1;

  s = set.toString();
  CPPUNIT_ASSERT_EQUAL(String("10101010101010101010101010101010101010101010101010101010101010101"), s);

  uint32_t blk = set.getWord(2);
  CPPUNIT_ASSERT_EQUAL(0x0001U, blk);

  set.setAll();

  blk = set.getWord(2);
  CPPUNIT_ASSERT_EQUAL(0x0001U, blk);
}

/*
 */

void BitSetTest::testBitShift()
{
  BitSet set(65), orig(65);
  String s;
  uint32_t firstWord, lastWord;

  for(int i = 0; i < 65; i += 3)
    set.set(i);

  firstWord = set.getWord(0);
  lastWord = set.getWord(2);

  orig = set;
  CPPUNIT_ASSERT_EQUAL(set, orig);

  // test left shift

  set <<= 4;

  firstWord = set.getWord(0);
  lastWord = set.getWord(2);

  CPPUNIT_ASSERT_EQUAL(1U, lastWord);

  s = set.toString();
  CPPUNIT_ASSERT_EQUAL(String("00001001001001001001001001001001001001001001001001001001001001001"), s);

  set <<= 30;

  firstWord = set.getWord(0);
  lastWord = set.getWord(2);

  CPPUNIT_ASSERT_EQUAL(1U, lastWord);

  s = set.toString();
  CPPUNIT_ASSERT_EQUAL(String("00000000000000000000000000000000001001001001001001001001001001001"), s);

  set = orig;
  set <<= 34;

  firstWord = set.getWord(0);
  lastWord = set.getWord(2);

  CPPUNIT_ASSERT_EQUAL(1U, lastWord);

  // test right shift

  set = orig;
  set >>= 4;

  firstWord = set.getWord(0);
  lastWord = set.getWord(2);

  CPPUNIT_ASSERT_EQUAL(0U, lastWord);

  s = set.toString();
  CPPUNIT_ASSERT_EQUAL(String("00100100100100100100100100100100100100100100100100100100100100000"), s);

  set >>= 30;

  firstWord = set.getWord(0);
  lastWord = set.getWord(2);

  CPPUNIT_ASSERT_EQUAL(0U, lastWord);

  s = set.toString();
  CPPUNIT_ASSERT_EQUAL(String("00100100100100100100100100100100000000000000000000000000000000000"), s);

  set = orig;
  set >>= 34;

  firstWord = set.getWord(0);
  lastWord = set.getWord(2);

  CPPUNIT_ASSERT_EQUAL(0U, lastWord);

  CPPUNIT_ASSERT_EQUAL(String("00100100100100100100100100100100000000000000000000000000000000000"), s);
}

/*
 */

void BitSetTest::testLogicalOps()
{
  BitSet set(65), orig(65);
  String s;

  for(int i = 0; i < 65; i += 3)
    set.set(i);

  orig = set;

  // test NOT

  BitSet not_set = ~set;

  s = not_set.toString();
  CPPUNIT_ASSERT_EQUAL(String("01101101101101101101101101101101101101101101101101101101101101101"), s);

  // test AND

  set |= not_set;

  s = set.toString();
  CPPUNIT_ASSERT_EQUAL(String("11111111111111111111111111111111111111111111111111111111111111111"), s);

  // test OR

  set = orig;
  set &= not_set;

  s = set.toString();
  CPPUNIT_ASSERT_EQUAL(String("00000000000000000000000000000000000000000000000000000000000000000"), s);

  // test XOR

  set = orig;
  set ^= not_set;

  s = set.toString();
  CPPUNIT_ASSERT_EQUAL(String("11111111111111111111111111111111111111111111111111111111111111111"), s);
}

/*
 */

void BitSetTest::testFindNext()
{
  BitSet set(65);
  uint_t set_bits[] = { 5, 31, 64 };

  for(size_t i = 0; i < CCXX_LENGTHOF(set_bits); ++i)
    set.set(set_bits[i]);

  int pos = 0;
  uint_t n = 0;
  while((pos = set.nextSetBit(pos)) >= 0)
  {
    CPPUNIT_ASSERT(n < CCXX_LENGTHOF(set_bits));
    CPPUNIT_ASSERT_EQUAL(set_bits[n], static_cast<uint_t>(pos));
    ++n;
    ++pos;
  }

  pos = 0;
  while((pos = set.nextClearBit(pos)) >= 0)
  {
    CPPUNIT_ASSERT_EQUAL(true, set.isClear(pos));
    ++pos;
  }
}
