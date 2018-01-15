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

#include "commonc++/PulseTimer.h++"

#include "Pinwheel.h++"

using namespace ccxx;

class PulseTimerTest : public CppUnit::TestFixture
{
 public:

  static CppUnit::Test *suite();

  void setUp();
  void tearDown();

  void testPulseTimer();

 private:

  class MyPulseTimer : public PulseTimer
  {
   public:

    MyPulseTimer();
    ~MyPulseTimer() throw();

    void pulse(const time_ms_t &now);

    inline int getCount() { return(_count); }
    inline time_ms_t getTime(int i) { return(_pulseTimes[i]); }

   private:

    int _count;
    time_ms_t _pulseTimes[5];
    Pinwheel _pinwheel;
  };

};
