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

#include "ConditionVarTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/ConditionVar.h++"
#include "commonc++/Random.h++"
#include "commonc++/Runnable.h++"
#include "commonc++/System.h++"
#include "commonc++/Thread.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(ConditionVarTest);

/*
 */

CppUnit::Test *ConditionVarTest::suite()
{
  CCXX_TESTSUITE_BEGIN(ConditionVarTest);
  CCXX_TESTSUITE_TEST(ConditionVarTest, testConditionVar);
  CCXX_TESTSUITE_END();
}

/*
 */

void ConditionVarTest::setUp()
{
}

/*
 */

void ConditionVarTest::tearDown()
{
}

/*
 */

void ConditionVarTest::testConditionVar()
{
  _counter = 0;
  _flag = false;

  RunnableDelegate<ConditionVarTest> r1(this, &ConditionVarTest::_thread1);
  RunnableDelegate<ConditionVarTest> r2(this, &ConditionVarTest::_thread2);

  Thread t1(&r1);
  Thread t2(&r2);

  t1.start();
  t2.start();

  t1.join();
  t2.join();

  CPPUNIT_ASSERT_EQUAL(5, _counter);
}

/*
 */

void ConditionVarTest::_thread1()
{
  Random rand;

  for(int i = 0; i < 10; i++)
  {
    int r = rand.nextInt(300);
    Thread::sleep(r);

    _mutex.lock();
    if(i % 2 == 1) // odd
      _flag = true;

    //std::cout << "notify!" << std::endl;
    _condvar.notify();
    _mutex.unlock();
  }
}

/*
 */

void ConditionVarTest::_thread2()
{
  for(int i = 0; i < 10; i++)
  {
    _mutex.lock();

    if(! _condvar.wait(_mutex, 2000))
      break;

    //std::cout << "notified!" << std::endl;

    if(_flag)
    {
      _counter++;
      _flag = false;
    }

    _mutex.unlock();
  }
}
