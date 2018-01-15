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

#include "SemaphoreTest.h++"
#include "Pinwheel.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/ScopedLock.h++"
#include "commonc++/Thread.h++"
#include "commonc++/Runnable.h++"
#include "commonc++/System.h++"
#include "commonc++/Random.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(SemaphoreTest);

/*
 */

CppUnit::Test *SemaphoreTest::suite()
{
  CCXX_TESTSUITE_BEGIN(SemaphoreTest);
  CCXX_TESTSUITE_TEST(SemaphoreTest, testSemaphore);
  CCXX_TESTSUITE_END();
}

/*
 */

void SemaphoreTest::setUp()
{
  _sem = new Semaphore("semtest", 2);
}

/*
 */

void SemaphoreTest::tearDown()
{
  delete _sem;
}

/*
 */

void SemaphoreTest::testSemaphore()
{
  _counter = 0;
  _max = 0;
  _count = 0;

  _sem->init();

  RunnableDelegate<SemaphoreTest> r1(this, &SemaphoreTest::_thread1);
  RunnableDelegate<SemaphoreTest> r2(this, &SemaphoreTest::_thread2);
  RunnableDelegate<SemaphoreTest> r3(this, &SemaphoreTest::_thread3);

  Thread t1(&r1);
  Thread t2(&r2);
  Thread t3(&r3);

  t1.start();
  t2.start();
  t3.start();

  t1.join();
  t2.join();
  t3.join();

  CPPUNIT_ASSERT_EQUAL(60, _counter);
  CPPUNIT_ASSERT((_max >= 1) && (_max <= 2));
}

/*
 */

void SemaphoreTest::_thread1()
{
  Random rand;
  Pinwheel pinwheel;

  for(int i = 0; i < 20; i++)
  {
    int r = rand.nextInt(50);
    Thread::sleep(r);

    _sem->wait();
    pinwheel.spin();

    int x = ++_count;
    if(x > _max)
      _max = x;

    r = rand.nextInt(50);
    Thread::sleep(r);

    _counter++;
    --_count;

    _sem->signal();

    pinwheel.spin();
  }
}

/*
 */

void SemaphoreTest::_thread2()
{
  Random rand;
  Pinwheel pinwheel;

  for(int i = 0; i < 20; i++)
  {
    int r = rand.nextInt(100);
    Thread::sleep(r);

    _sem->wait();
    pinwheel.spin();

    int x = ++_count;
    if(x > _max)
      _max = x;

    r = rand.nextInt(50);
    Thread::sleep(r);

    _counter++;
    --_count;

    _sem->signal();
    pinwheel.spin();
  }
}

/*
 */

void SemaphoreTest::_thread3()
{
  Random rand;
  Pinwheel pinwheel;

  for(int i = 0; i < 20; i++)
  {
    int r = rand.nextInt(50);
    Thread::sleep(r);

    _sem->wait();
    pinwheel.spin();

    int x = ++_count;
    if(x > _max)
      _max = x;

    r = rand.nextInt(100);
    Thread::sleep(r);

    _counter++;
    --_count;

    _sem->signal();

    pinwheel.spin();
  }
}
