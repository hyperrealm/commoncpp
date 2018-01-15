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

#include "ThreadLocalTest.h++"
#include "Pinwheel.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/ThreadLocal.h++"
#include "commonc++/Thread.h++"
#include "commonc++/Runnable.h++"
#include "commonc++/System.h++"
#include "commonc++/Random.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(ThreadLocalTest);

/*
 */

CppUnit::Test *ThreadLocalTest::suite()
{
  CCXX_TESTSUITE_BEGIN(ThreadLocalTest);
  CCXX_TESTSUITE_TEST(ThreadLocalTest, testThreadLocal);
  CCXX_TESTSUITE_END();
}

/*
 */

void ThreadLocalTest::setUp()
{
}

/*
 */

void ThreadLocalTest::tearDown()
{
}

/*
 */

void ThreadLocalTest::testThreadLocal()
{
  _ok = true;

  RunnableDelegate<ThreadLocalTest> r(this, &ThreadLocalTest::_thread);

  Thread t1(&r);
  Thread t2(&r);
  Thread t3(&r);

  t1.start();
  t2.start();
  t3.start();

  t1.join();
  t2.join();
  t3.join();

  CPPUNIT_ASSERT(_ok);
}

/*
 */

void ThreadLocalTest::_thread()
{
  int32_t cur = 0;
  Random rand;
  Pinwheel pinwheel;

  int delay = rand.nextInt(10) * 200;

  for(int i = 0; i < 3; i++)
  {
    pinwheel.spin();
    Thread::sleep(delay);

    int32_t& val = *_counter;
    if(val != cur)
      _ok = false;

    val++;
    cur++;
  }
}
