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

#include "MutexTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Mutex.h++"
#include "commonc++/Thread.h++"
#include "commonc++/Runnable.h++"
#include "commonc++/System.h++"
#include "commonc++/Random.h++"
#include "commonc++/ScopedLock.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(MutexTest);

/*
 */

CppUnit::Test *MutexTest::suite()
{
  CCXX_TESTSUITE_BEGIN(MutexTest);
  CCXX_TESTSUITE_TEST(MutexTest, testMutex);
  CCXX_TESTSUITE_TEST(MutexTest, testMutexTimeout);
  CCXX_TESTSUITE_END();
}

/*
 */

void MutexTest::setUp()
{
}

/*
 */

void MutexTest::tearDown()
{
}

/*
 */

void MutexTest::testMutex()
{
  _counter = 0;

  RunnableDelegate<MutexTest> r1(this, &MutexTest::_thread1);
  RunnableDelegate<MutexTest> r2(this, &MutexTest::_thread2);

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

void MutexTest::testMutexTimeout()
{
  RunnableDelegate<MutexTest> r1(this, &MutexTest::_thread3);
  RunnableDelegate<MutexTest> r2(this, &MutexTest::_thread4);

  Thread t1(&r1);
  Thread t2(&r2);

  _locked = false;

  t1.start();
  t2.start();

  t1.join();
  t2.join();

  CPPUNIT_ASSERT_EQUAL(false, _locked); // should have timed out
}

/*
 */

void MutexTest::_thread1()
{
  Random rand;

  for(int i = 0; i < 20; i++)
  {
    int r = rand.nextInt(100);
    Thread::sleep(r);

    synchronized(_mutex)
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

void MutexTest::_thread2()
{
  Random rand;

  for(int i = 0; i < 20; i++)
  {
    int r = rand.nextInt(100);
    Thread::sleep(r);

    synchronized(_mutex)
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

/*
 */

void MutexTest::_thread3()
{
  _mutex.lock();
  Thread::sleep(3500);
  _mutex.unlock();
}

/*
 */

void MutexTest::_thread4()
{
  Thread::sleep(1000);

  _locked = _mutex.tryLock(1000);

  if(_locked)
    _mutex.unlock();
}
