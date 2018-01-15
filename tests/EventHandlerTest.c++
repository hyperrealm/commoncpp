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

#include "EventHandlerTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/EventHandler.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(EventHandlerTest);

/*
 */

CppUnit::Test *EventHandlerTest::suite()
{
  CCXX_TESTSUITE_BEGIN(EventHandlerTest);
  CCXX_TESTSUITE_TEST(EventHandlerTest, testEventHandler);
  CCXX_TESTSUITE_END();
}

/*
 */

void EventHandlerTest::setUp()
{
}

/*
 */

void EventHandlerTest::tearDown()
{
}

/*
 */

void EventHandlerTest::testEventHandler()
{
  _handlerCalled = _eventReceived = false;

  EventHandlerDelegate<EventHandlerTest, String> handler(
    this, &EventHandlerTest::_handleEvent);

  String foo = "jello";

  handler(foo);

  CPPUNIT_ASSERT_EQUAL(true, _handlerCalled);
  CPPUNIT_ASSERT_EQUAL(true, _eventReceived);
}

/*
 */

void EventHandlerTest::_handleEvent(String msg)
{
  _handlerCalled = true;

  if(msg == "jello")
    _eventReceived = true;
}
