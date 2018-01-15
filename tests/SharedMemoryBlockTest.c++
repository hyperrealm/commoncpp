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

#include "SharedMemoryBlockTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/SharedMemoryBlock.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(SharedMemoryBlockTest);

/*
 */

CppUnit::Test *SharedMemoryBlockTest::suite()
{
  CCXX_TESTSUITE_BEGIN(SharedMemoryBlockTest);
  CCXX_TESTSUITE_TEST(SharedMemoryBlockTest, testSharedMemoryBlock);
  CCXX_TESTSUITE_END();
}

/*
 */

void SharedMemoryBlockTest::setUp()
{
}

/*
 */

void SharedMemoryBlockTest::tearDown()
{
}

/*
 */

void SharedMemoryBlockTest::testSharedMemoryBlock()
{
  try
  {
    SharedMemoryBlock shm("test", 128);

    shm.attach();

    byte_t *mem = shm.getBase();

    for(;;)
    {
      std::cout << "type a character & RETURN to modify memory, or v & RETURN"
                << "\nto see current value, or q to quit ";
      char c;
      std::cin >> c;

      if(c == 'q')
        break;

      if(c != 'v')
        *mem = c;

      std::cout << "current value: " << (int)*mem << '(' << (char)*mem
        << ')' << std::endl;
    }

    shm.detach();

  }
  catch(SystemException& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }

  // TODO
}
