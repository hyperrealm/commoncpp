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

#include "Base64Test.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Base64.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(Base64Test);

static const char *__decoded[] =
  {
  "Man is distinguished, not only by his reason, but by this "
  "singular passion from other animals, which is a lust of the mind, that "
  "by a perseverance of delight in the continued and indefatigable "
  "generation of knowledge, exceeds the short vehemence of any carnal "
  "pleasure.",
  "a",
  "ab",
  "abc",
  "abcd",
  NULL
  };

static const char *__encoded[] =
  {
  "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlz\n"
  "IHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2Yg\n"
  "dGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGlu\n"
  "dWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo\n"
  "ZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=\n",
  "YQ==\n",
  "YWI=\n",
  "YWJj\n",
  "YWJjZA==\n",
  NULL
  };

static const char *__encoded_ws[] =
  {
  "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlz"
  "IHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2Yg"
  "dGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGlu"
  "dWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo"
  "ZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=",
  "YQ==",
  "\n\n    YWI=",
  "YWJj   \n   ",
  "   YWJjZA==",
  NULL
  };


/*
 */

CppUnit::Test *Base64Test::suite()
{
  CCXX_TESTSUITE_BEGIN(Base64Test);
  CCXX_TESTSUITE_TEST(Base64Test, testEncode);
  CCXX_TESTSUITE_TEST(Base64Test, testDecode);
  CCXX_TESTSUITE_END();
}

/*
 */

void Base64Test::setUp()
{
}

/*
 */

void Base64Test::tearDown()
{
}

/*
 */

void Base64Test::testEncode()
{
  const char **q = __encoded;

  for(const char **p = __decoded; *p; p++, q++)
  {
    size_t len = Base64::encode((const byte_t *)*p, strlen(*p), NULL, 0);

    char *buf = new char[len + 1];
    buf[len] = 0;

    size_t r = Base64::encode((const byte_t *)*p, strlen(*p), buf, len);

    CPPUNIT_ASSERT(r > 0);
    int match = strcmp(*q, (char *)buf);
    CPPUNIT_ASSERT_EQUAL(0, match);

    delete[] buf;
  }
}

/*
 */

void Base64Test::testDecode()
{
  const char **q = __decoded;

  for(const char **p = __encoded_ws; *p; p++, q++)
  {
    size_t len = Base64::decode(*p, strlen(*p), NULL, 0);

    char *buf = new char[len + 1];
    buf[len] = 0;

    size_t r = Base64::decode(*p, strlen(*p), (byte_t *)buf, len);
    buf[r] = 0;

    CPPUNIT_ASSERT(r > 0);

    int match = strcmp(*q, (char *)buf);
    CPPUNIT_ASSERT_EQUAL(0, match);

    delete[] buf;
  }
}
