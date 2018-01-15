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

#include "AtomicCounterTest.h++"
#include "Pinwheel.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Thread.h++"
#include "commonc++/Runnable.h++"
#include "commonc++/System.h++"
#include "commonc++/Random.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(AtomicCounterTest);

/*
 */

CppUnit::Test *AtomicCounterTest::suite()
{
  CCXX_TESTSUITE_BEGIN(AtomicCounterTest);
  CCXX_TESTSUITE_TEST(AtomicCounterTest, testAtomicCounter);
  CCXX_TESTSUITE_TEST(AtomicCounterTest, testIncrDecr);
  CCXX_TESTSUITE_END();
}

/*
 */

void AtomicCounterTest::setUp()
{
}

/*
 */

void AtomicCounterTest::tearDown()
{
}

/*
 */

void AtomicCounterTest::testAtomicCounter()
{
  // test increment/decrement

  CPPUNIT_ASSERT_EQUAL(int32_t(0), (int32_t)_counter);

  _counter++;
  CPPUNIT_ASSERT_EQUAL(int32_t(1), (int32_t)_counter);

  _counter += 50;
  CPPUNIT_ASSERT_EQUAL(int32_t(51), (int32_t)_counter);

  _counter--;
  CPPUNIT_ASSERT_EQUAL(int32_t(50), (int32_t)_counter);

  _counter -= 50;
  CPPUNIT_ASSERT_EQUAL(int32_t(0), _counter.get());

  _counter.set(1);
  CPPUNIT_ASSERT_EQUAL(int32_t(1), (int32_t)_counter);

  _counter = 0;
  CPPUNIT_ASSERT_EQUAL(int32_t(0), (int32_t)_counter);

  int32_t old = _counter.swap(5);
  CPPUNIT_ASSERT_EQUAL(0, old);
  CPPUNIT_ASSERT_EQUAL(int32_t(5), (int32_t)_counter);

  // test concurrent access

  _val = 0;
  _lockOK = false;

  _counter = 0; // reset
  RunnableDelegate<AtomicCounterTest> r1(this, &AtomicCounterTest::_thread1);

  Thread t1(&r1);
  Thread t2(&r1);

  t1.start();
  t2.start();

  t1.join();
  t2.join();

  CPPUNIT_ASSERT_EQUAL(0, _val);
  CPPUNIT_ASSERT_EQUAL(true, _lockOK);
}

/*
 */

void AtomicCounterTest::testIncrDecr()
{
  _counter = 0;
  int32_t r;

  r = _counter++;
  CPPUNIT_ASSERT_EQUAL(0, r);

  r = ++_counter;
  CPPUNIT_ASSERT_EQUAL(2, r);

  r = _counter--;
  CPPUNIT_ASSERT_EQUAL(2, r);

  r = --_counter;
  CPPUNIT_ASSERT_EQUAL(0, r);
}

/*
 */

void AtomicCounterTest::_thread1()
{
  Random rand;
  Pinwheel pinwheel;

  for(int i = 0; i < 20; i++)
  {
    int32_t v = _counter.testAndSet(1, 0);

    if(v == 0)
    {
      // we locked it

      _lockOK = true;

      if(_val == 0)
        _val++;
      else
        _val += 50;

      // sleep for a while
      Thread::sleep(rand.nextInt(50));

      if(_val == 1)
        _val--;
      else
        _val -= 20;

      _counter.set(0); // unlock
    }

    Thread::sleep(rand.nextInt(75));
    pinwheel.spin();
  }
}
