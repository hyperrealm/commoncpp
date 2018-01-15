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

#include "commonc++/SocketSelector.h++"

using namespace ccxx;

class TestConnection : public Connection
{
 public:

  TestConnection(int id);
  ~TestConnection() throw();

  inline int getID() const
  { return(_id); }

  inline int bumpCount()
  {
    return(++_rcount);
  }

 private:

  int _id;
  int _rcount;
};

class TestSelector : public SocketSelector
{
 public:

  TestSelector();
  ~TestSelector() throw();

  virtual Connection *connectionReady(const SocketAddress& address);
  virtual void dataReceived(Connection *conn);
  virtual void connectionTimedOut(Connection *conn);
  virtual void connectionClosed(Connection *conn);

 private:

  int _counter;
};

class SocketSelectorTest : public CppUnit::TestFixture
{
 public:

  static CppUnit::Test *suite();

  void setUp();
  void tearDown();

  void testSocketSelector();
};
