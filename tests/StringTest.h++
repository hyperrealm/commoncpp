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

#include "commonc++/String.h++"

using namespace ccxx;

class StringTest : public CppUnit::TestFixture
{
  public:

  static CppUnit::Test *suite();

  void setUp();
  void tearDown();

  void testAssignment();
  void testAppend();
  void testAppendToNull();
  void testInsert();
  void testEquals();
  void testPrepend();
  void testCompare();
  void testTrim();
  void testReplace();
  void testRemove();
  void testIndexOf();
  void testContains();
  void testUpperLowerCase();
  void testNumeric();
  void testIndex();
  void testStartsEndsWith();
  void testNull();
  void testPadFill();
  void testConcat();
  void testReserve();
  void testTokenize();

  private:

  String _s0, _s1;
};
