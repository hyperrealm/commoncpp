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

#include "MD5DigestTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/MD5Digest.h++"

#include <cstring>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(MD5DigestTest);

/*
 */

static const char *__inputs[] =
  {
  "foo",
  "The MD4, MD5 and SHA-1 algorithms are secure hash functions. They take a string input, and produce a fixed size number - 128 bits for MD4 and MD5; 160 bits for SHA-1. This number is a hash of the input - a small change in the input results in a substantial change in the output. The functions are thought to be secure, in the sense that it would require an enormous amount of computing power to find a string which hashes to a chosen value. In others words, there's no way to decrypt a secure hash. The uses of secure hashes include digital signatures and challenge hash authentication.",
  "Hello, world!",
  "",
  NULL };

// http://pajhome.org.uk/crypt/md5/

static const char *__digests[] = { "acbd18db4cc2f85cedef654fccc4a4d8",
                                   "c530bc8598173467fafa354dce9048bb",
                                   "6cd3556deb0da54bca060b4c39479839",
                                   "d41d8cd98f00b204e9800998ecf8427e",
                                   NULL };

/*
 */

CppUnit::Test *MD5DigestTest::suite()
{
  CCXX_TESTSUITE_BEGIN(MD5DigestTest);
  CCXX_TESTSUITE_TEST(MD5DigestTest, testDigest);
  CCXX_TESTSUITE_END();
}

/*
 */

void MD5DigestTest::setUp()
{
}

/*
 */

void MD5DigestTest::tearDown()
{
}

/*
 */

void MD5DigestTest::testDigest()
{
  MD5Digest digest;

  const char **q = __digests;

  for(const char **p = __inputs; *p; p++, q++)
  {
    byte_t dig[16];

    std::memset(dig, 0, sizeof(dig));

    char dighex[33];
    char *x = dighex;

    digest.reset();

    digest.update((const byte_t *)*p, std::strlen(*p));
    digest.finish(dig);

    for(int i = 0; i < 16; i++)
    {
      sprintf(x, "%02x", dig[i]);
      x += 2;
    }
    *x = 0;

    std::cout << "dighex=" << dighex << std::endl;
    std::cout << "*q=" << *q << std::endl;

    CPPUNIT_ASSERT(! std::strcmp(dighex, *q));
  }
}
