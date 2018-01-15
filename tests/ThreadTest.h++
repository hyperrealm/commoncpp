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

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <cppunit/TestSuite.h>

#include "commonc++/Thread.h++"

using namespace ccxx;

class MyTestThread : public Thread
{
 public:

  MyTestThread();

  inline bool isCancelled()
  { return(_cancelled); }

 protected:

  void run();

  bool _cancelled;
};

class ThreadTest : public CppUnit::TestFixture
{
 public:

  static CppUnit::Test *suite();

  void setUp();
  void tearDown();

  void testThread();
  void testMainThread();
  void testThreadCancel();

 private:

  void thread1();
  void thread2();

  int _value;
};
