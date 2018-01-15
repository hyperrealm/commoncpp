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

#include "CriticalSectionTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/CriticalSection.h++"
#include "commonc++/ScopedLock.h++"
#include "commonc++/Thread.h++"
#include "commonc++/Random.h++"
#include "commonc++/Runnable.h++"
#include "commonc++/System.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(CriticalSectionTest);

/*
 */

CppUnit::Test *CriticalSectionTest::suite()
{
  CCXX_TESTSUITE_BEGIN(CriticalSectionTest);
  CCXX_TESTSUITE_TEST(CriticalSectionTest, testCriticalSection);
  CCXX_TESTSUITE_END();
}

/*
 */

void CriticalSectionTest::setUp()
{
}

/*
 */

void CriticalSectionTest::tearDown()
{
}

/*
 */

void CriticalSectionTest::testCriticalSection()
{
  _counter = 0;

  RunnableDelegate<CriticalSectionTest> r1(this,
                                           &CriticalSectionTest::_thread1);
  RunnableDelegate<CriticalSectionTest> r2(this,
                                           &CriticalSectionTest::_thread2);

  Thread t1(&r1);
  Thread t2(&r2);

  t1.start();
  t2.start();

  t1.join();
  t2.join();

  CPPUNIT_ASSERT_EQUAL(0, _counter);
}

/*
 */

void CriticalSectionTest::_thread1()
{
  Random rand;

  for(int i = 0; i < 20; i++)
  {
    int r = rand.nextInt(100);
    Thread::sleep(r);

    synchronized(_crit)
    {
      if(_counter == 0)
      {
        ++_counter;

        r = rand.nextInt(50);
        Thread::sleep(r);

        if(_counter == 1)
          --_counter;
      }
    }
  }
}

/*
 */

void CriticalSectionTest::_thread2()
{
  Random rand;

  for(int i = 0; i < 20; i++)
  {
    int r = rand.nextInt(100);
    Thread::sleep(r);

    synchronized(_crit)
    {
      if(_counter == 0)
      {
        _counter -= 5;

        r = rand.nextInt(50);
        Thread::sleep(r);

        if(_counter == -5)
          _counter += 5;
      }
    }
  }
}
