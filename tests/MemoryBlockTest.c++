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

#include "MemoryBlockTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/MemoryBlock.h++"

#include <cstring>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(MemoryBlockTest);

/*
 */

CppUnit::Test *MemoryBlockTest::suite()
{
  CCXX_TESTSUITE_BEGIN(MemoryBlockTest);
  CCXX_TESTSUITE_TEST(MemoryBlockTest, testMemoryBlock);
  CCXX_TESTSUITE_END();
}

/*
 */

void MemoryBlockTest::setUp()
{
}

/*
 */

void MemoryBlockTest::tearDown()
{
}

/*
 */

void MemoryBlockTest::testMemoryBlock()
{
  byte_t buf1[16], buf2[10], buf3[20], buf4[16];
  MemoryBlock mem1(buf1, sizeof(buf1));
  MemoryBlock mem2(buf2, sizeof(buf2));
  MemoryBlock mem3(buf3, sizeof(buf3));
  MemoryBlock mem4(buf4, sizeof(buf4));
  uint_t i;
  bool ok;

  // bounds

  CPPUNIT_ASSERT(mem1.getBase() == buf1);
  CPPUNIT_ASSERT_EQUAL(sizeof(buf1), mem1.getSize());

  // zero

  mem1.zero();
  ok = true;
  for(i = 0; i < mem1.getSize(); i++)
  {
    if(mem1[i] != 0)
      ok = false;
  }
  CPPUNIT_ASSERT_EQUAL(true, ok);

  // operator[]

  for(i = 0; i < mem1.getSize(); i++)
  {
    mem1[i] = i;
    CPPUNIT_ASSERT_EQUAL(byte_t(i), buf1[i]);
  }

  // copy (to smaller)

  ok = mem2.copyFrom(mem1);
  CPPUNIT_ASSERT_EQUAL(true, ok);

  CPPUNIT_ASSERT(mem1 != mem2);

  ok = true;
  for(i = 0; i < mem2.getSize(); i++)
  {
    if(mem2[i] != i)
      ok = false;
  }
  CPPUNIT_ASSERT_EQUAL(true, ok);

  // copy (to bigger)

  mem3.zero();
  ok = mem3.copyFrom(mem1);
  CPPUNIT_ASSERT_EQUAL(true, ok);

  CPPUNIT_ASSERT(mem1 != mem3);

  ok = true;
  for(i = 0; i < mem3.getSize(); i++)
  {
    if(!((i < mem1.getSize() && (mem3[i] == i))
         || (mem3[i] == 0)))
      ok = false;
  }
  CPPUNIT_ASSERT_EQUAL(true, ok);

  // copy (to equal)

  mem4.zero();
  mem4.copyFrom(mem1);

  CPPUNIT_ASSERT(mem4 == mem1);

  // index

  int j = mem4.indexOf(4);
  CPPUNIT_ASSERT_EQUAL(4, j);

  j = mem4.indexOf(99);
  CPPUNIT_ASSERT_EQUAL(-1, j);

  // overlapped copy
  // 0 1 2 3 4 5 6 7 8 9 => 2 3 4 5 4 5 6 7 8 9
  byte_t result[] = { 2, 3, 4, 5, 4, 5, 6, 7, 8, 9 };

  MemoryBlock mem5(&(buf2[2]), 4);
  mem2.moveFrom(mem5);

  int m = std::memcmp(mem2.getBase(), result, sizeof(result));
  CPPUNIT_ASSERT_EQUAL(0, m);
}
