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

#ifndef __ccxx_TestUtils_hxx
#define __ccxx_TestUtils_hxx

#include <commonc++/String.h++>

class TestUtils
{
 public:

  static bool compareFiles(const ccxx::String &path1,
                           const ccxx::String &path2);

  static void failWithMessage(const ccxx::String &message);
};

#define CCXX_TESTSUITE_BEGIN(CLASS)                             \
  CppUnit::TestSuite *tests = new CppUnit::TestSuite(#CLASS)

#define CCXX_TESTSUITE_TEST(CLASS, METHOD)                              \
  tests->addTest(new CppUnit::TestCaller<CLASS>(#METHOD,                \
                                                &CLASS::METHOD))
#define CCXX_TESTSUITE_END()                    \
  return(tests)

#define CCXX_TEST_FAIL_EXCEPTION(EX) \
  TestUtils::failWithMessage(EX.toString())

#endif // __ccxx_TestUtils_hxx
