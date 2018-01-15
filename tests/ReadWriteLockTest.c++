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

#include "ReadWriteLockTest.h++"
#include "Pinwheel.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/ReadWriteLock.h++"
#include "commonc++/Thread.h++"
#include "commonc++/Random.h++"
#include "commonc++/System.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(ReadWriteLockTest);

/*
 */

CppUnit::Test *ReadWriteLockTest::suite()
{
  CCXX_TESTSUITE_BEGIN(ReadWriteLockTest);
  CCXX_TESTSUITE_TEST(ReadWriteLockTest, testReadWriteLock);
  CCXX_TESTSUITE_END();
}

/*
 */

void ReadWriteLockTest::setUp()
{
}

/*
 */

void ReadWriteLockTest::tearDown()
{
}

/*
 */

void ReadWriteLockTest::testReadWriteLock()
{
  _numReaders = _numWriters = 0;
  _value = 0;
  _writeOK = _readOK = true;

  RunnableDelegate<ReadWriteLockTest> rd(this, &ReadWriteLockTest::_reader);
  RunnableDelegate<ReadWriteLockTest> wr(this, &ReadWriteLockTest::_writer);

  Thread rt1(&rd);
  Thread rt2(&rd);

  Thread wt1(&wr);
  Thread wt2(&wr);

  rt1.start();
  rt2.start();
  wt1.start();
  wt2.start();

  rt1.join();
  rt2.join();
  wt1.join();
  wt2.join();

  CPPUNIT_ASSERT_EQUAL(true, _writeOK);
  CPPUNIT_ASSERT_EQUAL(true, _readOK);
  CPPUNIT_ASSERT_EQUAL(0, _numReaders);
  CPPUNIT_ASSERT_EQUAL(0, _numWriters);
  CPPUNIT_ASSERT_EQUAL(20, _value);
}

/*
 */

void ReadWriteLockTest::_reader()
{
  Random rand;
  Pinwheel pinwheel;

  Thread::sleep(rand.nextInt(200));

  for(int i = 0; i < 10; i++)
  {
    int r = rand.nextInt(50);
    Thread::sleep(r * 2);

    // std::cout << "lock read" << std::endl;
    _lock.lockRead();
    // std::cout << "lock read DONE" << std::endl;

    _numReaders++;
    if((_numWriters != 0) || (_numReaders < 1))
      _readOK = false;

    Thread::sleep(30);

    _numReaders--;
    _lock.unlock();

    // std::cout << "unlock" << std::endl;

    pinwheel.spin();
  }
}

/*
 */

void ReadWriteLockTest::_writer()
{
  Random rand;
  Pinwheel pinwheel;

  Thread::sleep(rand.nextInt(200));

  for(int i = 0; i < 10; i++)
  {
    int r = rand.nextInt(50);
    Thread::sleep(r * 2);

    // std::cout << "lock write" << std::endl;
    _lock.lockWrite();
    // std::cout << "lock write DONE" << std::endl;

    _numWriters++;
    if((_numWriters != 1) || (_numReaders != 0))
      _writeOK = false;
    else
      _value++;

    Thread::sleep(25);

    _numWriters--;
    _lock.unlock();
    // std::cout << "unlock" << std::endl;

    pinwheel.spin();
  }
}
