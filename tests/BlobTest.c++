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

#include "BlobTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Blob.h++"
#include "commonc++/Hex.h++"
#include "commonc++/String.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(BlobTest);

/*
 */

CppUnit::Test *BlobTest::suite()
{
  CCXX_TESTSUITE_BEGIN(BlobTest);
  CCXX_TESTSUITE_TEST(BlobTest, testBlob);
  CCXX_TESTSUITE_TEST(BlobTest, testExtend);
  CCXX_TESTSUITE_END();
}

/*
 */

void BlobTest::setUp()
{
}

/*
 */

void BlobTest::tearDown()
{
}

/*
 */

void BlobTest::testBlob()
{
  Blob blob;

  CPPUNIT_ASSERT(Hex::encode(blob) == String::nullRep);

  blob << 0x0F << "ABC" << 0x00;

  CPPUNIT_ASSERT_EQUAL(5U, blob.length());
  CPPUNIT_ASSERT_EQUAL(String("0F41424300"), Hex::encode(blob));

  blob.clear();

  CPPUNIT_ASSERT_EQUAL(0U, blob.length());

  CPPUNIT_ASSERT_EQUAL(String::nullRep, Hex::encode(blob));

  blob.setLength(10);

  CPPUNIT_ASSERT_EQUAL(String("00000000000000000000"), Hex::encode(blob));

  for(int i = 0; i < 10; i++) {
    for(int j = 0; j < 10; j++) {
      blob << i;
    }
  }

  CPPUNIT_ASSERT_EQUAL(110U, blob.length());

  blob.setLength(10);

  CPPUNIT_ASSERT_EQUAL(String("00000000000000000000"), Hex::encode(blob));

  Blob blob2 = blob;

  blob2.clear();
  blob2 << 0xFF;

  CPPUNIT_ASSERT_EQUAL(String("FF"), Hex::encode(blob2));
  CPPUNIT_ASSERT(blob2 != blob);

  blob2.setLength(3);
  CPPUNIT_ASSERT_EQUAL(String("FF0000"), Hex::encode(blob2));

  blob2 << 0xAA << 0xBB << 0xCC;

  Blob blob3;
  blob3 << 0xFF << 0x00 << 0x00 << 0xAA << 0xBB << 0xCC;

  CPPUNIT_ASSERT_EQUAL(String("FF0000AABBCC"), Hex::encode(blob3));

  CPPUNIT_ASSERT_EQUAL(0, blob2.compareTo(blob3));
}

/*
 */

void BlobTest::testExtend()
{
  Blob blob;
  blob.setLength(20);

  CPPUNIT_ASSERT_EQUAL(20U, blob.getLength());
  CPPUNIT_ASSERT_EQUAL(String("0000000000000000000000000000000000000000"),
                       Hex::encode(blob));
}
