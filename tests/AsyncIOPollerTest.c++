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

#include "AsyncIOPollerTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/AsyncIOPoller.h++"
#include "commonc++/File.h++"
#include "commonc++/AsyncIOTask.h++"
#include "commonc++/Random.h++"
#include "commonc++/Thread.h++"

#include <iostream>
#include <vector>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(AsyncIOPollerTest);

/*
 */


CppUnit::Test *AsyncIOPollerTest::suite()
{
  CCXX_TESTSUITE_BEGIN(AsyncIOPollerTest);
  CCXX_TESTSUITE_TEST(AsyncIOPollerTest, testAsyncPoll);
  CCXX_TESTSUITE_END();
}

/*
 */

void AsyncIOPollerTest::setUp()
{
}

/*
 */

void AsyncIOPollerTest::tearDown()
{
}

/*
 */

void AsyncIOPollerTest::testAsyncPoll()
{
  std::vector<int> blocks;
  for(int i = 0; i < 16; ++i)
    blocks.push_back(i);

  Random rand;
  File file("./testdata/asynciotest.bin");
  AsyncIOTask tasks[16];
  bool ok[16];

  for(int i = 0; i < 16; ++i)
    ok[i] = false;

  try
  {
    file.open(IORead);
    byte_t buf[16][128];

    while(! blocks.empty())
    {
      int x = rand.nextInt(blocks.size());
      std::vector<int>::iterator iter = blocks.begin() + x;
      int segment = *iter;
      blocks.erase(iter);

      std::cout << "reading segment " << segment << std::endl;

      int r = file.read(buf[segment], 128, segment * 128, tasks[segment]);
      std::cout << "return value: " << r << std::endl;
    }

    std::vector<int> unfinished;
    for(int i = 0; i < 16; ++i)
      unfinished.push_back(i);

    AsyncIOPoller poller;
    for(int i = 0; i < 16; ++i)
      poller.addTask(&tasks[i]);

    while(! unfinished.empty())
    {
      std::cout << "polling..." << std::endl;
      if(poller.poll(100))
      {
        for(std::vector<int>::iterator iter = unfinished.begin();
            iter != unfinished.end();
          )
        {
          int segment = *iter;

          if(tasks[segment].isCompleted())
          {
            poller.removeTask(&tasks[segment]);

            size_t count = tasks[segment].getBytesTransferred();

            if(count == 128)
            {
              bool matched = true;
              for(int j = 0; j < 128; ++j)
              {
                if(buf[segment][j] != segment)
                {
                  matched = false;
                  break;
                }
              }

              if(matched)
                ok[segment] = true;
            }

            std::cout << "segment " << segment << " completed; "
                      << count << " bytes transferred; matched? "
                      << ok[segment] << std::endl;

            iter = unfinished.erase(iter);
          }
          else
            ++iter;
        }
      }
    }

    file.close();

    for(int i = 0; i < 16; ++i)
      CPPUNIT_ASSERT_EQUAL(true, ok[i]);

  }
  catch(IOException &ioex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ioex);
  }
}

