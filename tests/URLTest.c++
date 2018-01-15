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

#include "URLTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/URL.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(URLTest);

/*
 */

static const char *__inputs[] =
  {
  "http://www.google.com/",
  "http://www.foo.com:80/blah/baz",
  "ftp://10.0.11.5/",
  "blah",
  "http://foo:0/a/b/c%20c?def=ghi",
  "http://foo/a?bc=de%4C&gh=100+5",
  "file:///c/foo/bar/baz",
  "file://localhost/d=e/f+g/h-i/",
  "http://www.foo.com/~bob/blah.so",
  "foo:///abc/../def/ghi/././../jkl/.",
  NULL
  };

static const char *__schemes[] =
  {
  "http",
  "http",
  "ftp",
  NULL,
  "http",
  "http",
  "file",
  "file",
  "http",
  "foo",
  NULL
  };

static const char *__hosts[] =
  {
  "www.google.com",
  "www.foo.com",
  "10.0.11.5",
  NULL,
  "foo",
  "foo",
  NULL,
  "localhost",
  "www.foo.com",
  NULL,
  NULL
  };

static const uint16_t __ports[] =
  {
  0,
  80,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
  };

static const char *__paths[] =
  {
  "/",
  "/blah/baz",
  "/",
  NULL,
  "/a/b/c c",
  "/a",
  "/c/foo/bar/baz",
  "/d=e/f g/h-i/",
  "/~bob/blah.so",
  "/def/jkl",
  NULL
  };

static const char *__queries[] =
  {
  NULL,
  NULL,
  NULL,
  NULL,
  "def=ghi",
  "bc=de%4C&gh=100+5",
  NULL,
  NULL,
  NULL,
  NULL
  };

/*
 */

/*
 */

CppUnit::Test *URLTest::suite()
{
  CCXX_TESTSUITE_BEGIN(URLTest);
  CCXX_TESTSUITE_TEST(URLTest, testURL);
  CCXX_TESTSUITE_END();
}

/*
 */

void URLTest::setUp()
{
}

/*
 */

void URLTest::tearDown()
{
}

/*
 */

void URLTest::testURL()
{
  URL aurl("http://www.corp.google.com/~mlindner/nightingale/libtest.so");
  std::cout << "is url valid? " << aurl.isValid() << std::endl;

  const char **s = __schemes, **h = __hosts, **f = __paths, **q = __queries;
  int i = 0;

  for(const char **p = __inputs; *p; p++, s++, h++, f++, q++, i++)
  {
    URL url(*p);
    //std::cout << "re-encoded: " << url.toString() << std::endl;

    std::cout << "valid?:     " << url.isValid() << std::endl;
    std::cout << "original:   " << *p << std::endl;
    std::cout << "scheme:     "<< url.getScheme() << std::endl;
    std::cout << "host:       " << url.getHost() << std::endl;
    std::cout << "canon path: " << url.getCanonicalPath() << std::endl;
    std::cout << "re-encoded: " << url.toString() << std::endl;

    CPPUNIT_ASSERT_EQUAL(String(*s), url.getScheme());

    CPPUNIT_ASSERT_EQUAL(String(*h), url.getHost());
    CPPUNIT_ASSERT_EQUAL(String(*f), url.getCanonicalPath());
    CPPUNIT_ASSERT_EQUAL(String(*q), url.getQuery());
    CPPUNIT_ASSERT_EQUAL(__ports[i], url.getPort());
  }

  URL x1("audio:///./../q/./..");
  std::cout << "x1: " << x1.getCanonicalPath() << std::endl;
}
