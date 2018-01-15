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

#include "ThreadTest.h++"
#include "Pinwheel.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Runnable.h++"
#include "commonc++/System.h++"
#include "commonc++/Thread.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(ThreadTest);

/*
 */

CppUnit::Test *ThreadTest::suite()
{
  CCXX_TESTSUITE_BEGIN(ThreadTest);
  CCXX_TESTSUITE_TEST(ThreadTest, testThread);
  CCXX_TESTSUITE_TEST(ThreadTest, testMainThread);
  CCXX_TESTSUITE_TEST(ThreadTest, testThreadCancel);
  CCXX_TESTSUITE_END();
}

/*
 */

void ThreadTest::setUp()
{
  _value = 0;
}

/*
 */

void ThreadTest::tearDown()
{
}

/*
 */

void ThreadTest::testMainThread()
{
  Thread* thr = Thread::currentThread();
  // TODO
}

/*
 */

void ThreadTest::testThread()
{
  RunnableDelegate<ThreadTest> r(this, &ThreadTest::thread1);

  Thread thr1(&r);

  thr1.start();

  thr1.join();

  CPPUNIT_ASSERT_EQUAL(5, _value);
}

/*
 */

void ThreadTest::testThreadCancel()
{
  MyTestThread tt;

  tt.start();
  Thread::sleep(3);
  tt.stop();
  tt.join();

  CPPUNIT_ASSERT_EQUAL(true, tt.isCancelled());
}

/*
 */

void ThreadTest::thread1()
{
  Pinwheel pinwheel;

  for(int i = 0; i < 5; i++)
  {
    Thread::sleep(1000);
    _value++;
    pinwheel.spin();
  }
}

/*
 */

MyTestThread::MyTestThread()
  : _cancelled(false)
{
}

/*
 */

void MyTestThread::run()
{
  for(int i = 0; i < 10; i++)
  {
    Thread::sleep(1);
    if(testCancel())
    {
      _cancelled = true;
      break;
    }
  }
}
