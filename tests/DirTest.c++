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

#include "DirTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Dir.h++"
#include "commonc++/DateTimeFormat.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(DirTest);

/*
 */

CppUnit::Test *DirTest::suite()
{
  CCXX_TESTSUITE_BEGIN(DirTest);
  CCXX_TESTSUITE_TEST(DirTest, testDir);
  CCXX_TESTSUITE_END();
}

/*
 */

void DirTest::setUp()
{
}

/*
 */

void DirTest::tearDown()
{
}

/*
 */

void DirTest::testDir()
{
  // TODO

  Dir dir(".");
  DateTimeFormat fmt;

  try
  {
    FileAttributes attr;
    String name;

    dir.open(true, true);

    for(int z = 0; z < 2; z++)
    {
      std::cout << "--- pass #" << z << std::endl;
      while(dir.nextFile(name, attr))
      {
        std::cout << "file: " << name;

        if(attr.isDirectory())
          std::cout << " (dir)";

        std::cout << " size=" << attr.size;
        std::cout << " ";

        std::cout << " perm=" << attr.permissions.toString();
        std::cout << " ";

        DateTime dt(attr.modified);
        std::cout << fmt.format(dt);

        std::cout << std::endl;
      }

      dir.rewind();
    }

    dir.close();
  }
  catch(IOException& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}
