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

#include "SHA1DigestTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/SHA1Digest.h++"

#include <cstring>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(SHA1DigestTest);

/*
 */

static const char *__inputs[] =
  {
  "foo",
  "The MD4, SHA1 and SHA-1 algorithms are secure hash functions. They take a string input, and produce a fixed size number - 128 bits for MD4 and SHA1; 160 bits for SHA-1. This number is a hash of the input - a small change in the input results in a substantial change in the output. The functions are thought to be secure, in the sense that it would require an enormous amount of computing power to find a string which hashes to a chosen value. In others words, there's no way to decrypt a secure hash. The uses of secure hashes include digital signatures and challenge hash authentication.",
  "Hello, world!",
  "",
  NULL };

static const char *__digests[] = { "0beec7b5ea3f0fdbc95d0dd47f3c5bc275da8a33",
                                   "f7aa6c1945331a5ef3b78348f6dcc4794249ba5a",
                                   "943a702d06f34599aee1f8da8ef9f7296031d699",
                                   "da39a3ee5e6b4b0d3255bfef95601890afd80709",
                                   NULL };

/*
 */

CppUnit::Test *SHA1DigestTest::suite()
{
  CCXX_TESTSUITE_BEGIN(SHA1DigestTest);
  CCXX_TESTSUITE_TEST(SHA1DigestTest, testDigest);
  CCXX_TESTSUITE_END();
}

/*
 */

void SHA1DigestTest::setUp()
{
}

/*
 */

void SHA1DigestTest::tearDown()
{
}

/*
 */

void SHA1DigestTest::testDigest()
{
  SHA1Digest digest;

  const char **q = __digests;

  for(const char **p = __inputs; *p; p++, q++)
  {
    byte_t dig[20];

    std::memset(dig, 0, sizeof(dig));

    char dighex[41];
    char *x = dighex;

    digest.reset();

    digest.update((const byte_t *)*p, strlen(*p));
    digest.finish(dig);

    for(int i = 0; i < 20; i++)
    {
      sprintf(x, "%02x", dig[i]);
      x += 2;
    }
    *x = 0;

    int match = std::strcmp(dighex, *q);
    CPPUNIT_ASSERT_EQUAL(0, match);
  }
}
