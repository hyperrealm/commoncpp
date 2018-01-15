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

#include "MD5PasswordTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/MD5Password.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(MD5PasswordTest);

/*
 */

CppUnit::Test *MD5PasswordTest::suite()
{
  CCXX_TESTSUITE_BEGIN(MD5PasswordTest);
  CCXX_TESTSUITE_TEST(MD5PasswordTest, testMD5Password);
  CCXX_TESTSUITE_END();
}

/*
 */

void MD5PasswordTest::setUp()
{
}

/*
 */

void MD5PasswordTest::tearDown()
{
}

/*
 */

void MD5PasswordTest::testMD5Password()
{
  String pw = "foobar123";
  String cipher = MD5Password::encrypt(pw);

  // ciphertext is never the same, so we can't check that...

  String pw2 = "foobar123";
  bool ok = MD5Password::validate(pw2, cipher);
  CPPUNIT_ASSERT_EQUAL(true, ok); // should verify

  String pw3 = "foobaz123";
  ok = MD5Password::validate(pw3, cipher);
  CPPUNIT_ASSERT_EQUAL(false, ok); // should fail to verify
}
