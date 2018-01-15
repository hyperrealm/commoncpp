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

#include "RandomTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Random.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(RandomTest);

/*
 */

CppUnit::Test *RandomTest::suite()
{
  CCXX_TESTSUITE_BEGIN(RandomTest);
  CCXX_TESTSUITE_TEST(RandomTest, testRandom);
  CCXX_TESTSUITE_END();
}

/*
 */

void RandomTest::setUp()
{
}

/*
 */

void RandomTest::tearDown()
{
}

/*
 */

void RandomTest::testRandom()
{
  Random rand;

  std::cout << "\nRandom numbers in range 0 - 9\n";
  for(int i = 0; i < 3; i++)
  {
    for(int rr = 0; rr < 30; rr++)
    {
      uint_t val = rand.nextInt(10);
      std::cout << val << " ";
    }
    std::cout << '\n';
  }

  std::cout << "\nRandom floating point numbers in range 0 - 1\n";

  for(int i = 0; i < 20; i++)
  {
    double val = rand.nextDouble();
    std::cout << val << " ";
  }
  std::cout << '\n';

  std::cout << "\nDoes this look correct? (y/n) ";
  char c;
  std::cin >> c;
  CPPUNIT_ASSERT(tolower(c) == 'y');
}
