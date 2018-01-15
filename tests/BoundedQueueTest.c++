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

#include "BoundedQueueTest.h++"
#include "Pinwheel.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Thread.h++"
#include "commonc++/Runnable.h++"
#include "commonc++/Random.h++"
#include "commonc++/System.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(BoundedQueueTest);

/*
 */

CppUnit::Test *BoundedQueueTest::suite()
{
  CCXX_TESTSUITE_BEGIN(BoundedQueueTest);
  CCXX_TESTSUITE_TEST(BoundedQueueTest, testQueue);
  CCXX_TESTSUITE_END();
}

/*
 */

void BoundedQueueTest::setUp()
{
  _queue = new BoundedQueue<int>(10);
}

/*
 */

void BoundedQueueTest::tearDown()
{
  delete _queue;
}

/*
 */

void BoundedQueueTest::testQueue()
{
  RunnableDelegate<BoundedQueueTest> prod(this, &BoundedQueueTest::_producer);
  RunnableDelegate<BoundedQueueTest> cons(this, &BoundedQueueTest::_consumer);

  Thread tp(&prod);
  Thread tc(&cons);

  _count = 0;
  _boundsOK = true;

  tp.start();
  tc.start();

  tp.join();
  tc.join();

  CPPUNIT_ASSERT_EQUAL(50, _count);
  CPPUNIT_ASSERT_EQUAL(true, _boundsOK);
}

/*
 */

void BoundedQueueTest::_producer()
{
  Random rand;
  Pinwheel pinwheel;

  try
  {
    for(int i = 1; i <= 50; i++)
    {
      int r = rand.nextInt(10);

      Thread::sleep(r);

      _queue->put(i);

      int sz = _queue->getSize();
      if((sz < 0) || (sz > 10))
        _boundsOK = false;

      pinwheel.spin();
    }
  }
  catch(InterruptedException &ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
  catch(TimeoutException &tex)
  {
  }
}

/*
 */

void BoundedQueueTest::_consumer()
{
  Random rand;
  int last = 0;

  try
  {
    for(;;)
    {
      int r = rand.nextInt(50);
      Thread::sleep(10 + r); // make consumer slower than producer

      int v = _queue->tryTake(1000);

      if(v == (last + 1))
        _count++;

      last = v;

      int sz = _queue->getSize();
      if((sz < 0) || (sz > 10))
        _boundsOK = false;
    }
  }
  catch(InterruptedException &ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
  catch(TimeoutException &tex)
  {
  }
}
