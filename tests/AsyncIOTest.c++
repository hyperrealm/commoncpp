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

#include "AsyncIOTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/AsyncIOPoller.h++"
#include "commonc++/File.h++"
#include "commonc++/AsyncIOTask.h++"
#include "commonc++/Random.h++"
#include "commonc++/ServerSocket.h++"
#include "commonc++/StreamSocket.h++"
#include "commonc++/Thread.h++"

#include <iostream>
#include <vector>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(AsyncIOTest);

/*
 */

CppUnit::Test *AsyncIOTest::suite()
{
  CCXX_TESTSUITE_BEGIN(AsyncIOTest);
  CCXX_TESTSUITE_TEST(AsyncIOTest, testAsyncFileRead);
  CCXX_TESTSUITE_TEST(AsyncIOTest, testAsyncFileWrite);
  // AsyncIO does not work reliably/portably with sockets...
  //  CCXX_TESTSUITE_TEST(AsyncIOTest, testAsyncSocketRead);
  //  CCXX_TESTSUITE_TEST(AsyncIOTest, testAsyncSocketReadFailure);
  //  CCXX_TESTSUITE_TEST(AsyncIOTest, testAsyncSocketWrite);
  //  CCXX_TESTSUITE_TEST(AsyncIOTest, testAsyncSocketWriteRead);
  CCXX_TESTSUITE_END();
}

/*
 */

void AsyncIOTest::setUp()
{
}

/*
 */

void AsyncIOTest::tearDown()
{
}

/*
 */

void AsyncIOTest::testAsyncFileRead()
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

    while(! unfinished.empty())
    {
      for(std::vector<int>::iterator iter = unfinished.begin();
          iter != unfinished.end();
        )
      {
        int segment = *iter;

        if(tasks[segment].isCompleted())
        {
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

      Thread::sleep(100);
    }

    file.close();

    for(int i = 0; i < 16; ++i)
      CPPUNIT_ASSERT(ok[i] == true);
  }
  catch(IOException &ioex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ioex);
  }
}

/*
 */

void AsyncIOTest::testAsyncFileWrite()
{
  std::vector<int> blocks;
  for(int i = 0; i < 16; ++i)
    blocks.push_back(i);

  Random rand;
  File file("./testdata/asyncwrite.bin");
  AsyncIOTask tasks[16];

  try
  {
    file.open(IOWrite, FileTruncateElseCreate);
    byte_t buf[16][128];

    for(int i = 0; i < 16; ++i)
      std::memset(buf[i], i, 128);

    // first fill file with 128*16 NUL bytes
    for(int i = 0; i < 16; ++i)
      file.write(buf[0], 128);

    while(! blocks.empty())
    {
      int x = rand.nextInt(blocks.size());
      std::vector<int>::iterator iter = blocks.begin() + x;
      int segment = *iter;
      blocks.erase(iter);

      std::cout << "writing segment " << segment << " at offset "
                << (segment * 128) << std::endl;

      int r = file.write(buf[segment], 128, segment * 128, tasks[segment]);
      std::cout << "return value: " << r << std::endl;
    }

    std::vector<int> unfinished;
    for(int i = 0; i < 16; ++i)
      unfinished.push_back(i);

    while(! unfinished.empty())
    {
      for(std::vector<int>::iterator iter = unfinished.begin();
          iter != unfinished.end();
        )
      {
        int segment = *iter;

        if(tasks[segment].isCompleted())
        {
          size_t count = tasks[segment].getBytesTransferred();

          std::cout << "segment " << segment << " completed; "
                    << count << " bytes transferred" << std::endl;

          iter = unfinished.erase(iter);
        }
        else
          ++iter;
      }

      Thread::sleep(100);
    }

    file.close();

    CPPUNIT_ASSERT(TestUtils::compareFiles("./testdata/asyncwrite.bin",
                                           "./testdata/asynciotest.bin")
                   == true);
  }
  catch(IOException &ioex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ioex);
  }
}

/*
 */

void AsyncIOTest::testAsyncSocketRead()
{
  RunnableClosure<AsyncIOTest, bool> runnable(this, &AsyncIOTest::_server,
                                              true);

  Thread thread(&runnable);

  thread.start();

  try
  {
    ByteBuffer buf(256);

    StreamSocket sock;
    sock.init();
    sock.connect("localhost", 40407);
    sock.setTimeout(-1);

    AsyncIOTask task;

    int r = sock.read(buf, 0LL, task);
    CPPUNIT_ASSERT(r == 0 || r == 18);

    if(r == 0)
    {
      AsyncIOPoller poller;
      poller.addTask(&task);

      bool ok = poller.poll(-1);
      CPPUNIT_ASSERT(ok);

      ok = task.isCompleted();
      CPPUNIT_ASSERT(ok);

      r = task.getBytesTransferred();
      CPPUNIT_ASSERT(r == 18);
    }

    sock.close();
  }
  catch(Exception& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }

  thread.join();
}

/*
 */

void AsyncIOTest::testAsyncSocketReadFailure()
{
  RunnableClosure<AsyncIOTest, bool> runnable(this, &AsyncIOTest::_server,
                                              false);

  Thread thread(&runnable);

  thread.start();

  try
  {
    ByteBuffer buf(256);

    StreamSocket sock;
    sock.init();
    sock.connect("localhost", 40407);
    sock.setTimeout(-1);

    AsyncIOTask task;

    int r = sock.read(buf, 0LL, task);
    CPPUNIT_ASSERT(r == 0 || r == 18);

    if(r == 0)
    {
      AsyncIOPoller poller;
      poller.addTask(&task);

      bool ok = poller.poll(-1);
      CPPUNIT_ASSERT(ok);
      bool eof = false;

      try
      {
        ok = task.isCompleted();
        CPPUNIT_ASSERT(ok);
      }
      catch(EOFException& eofex)
      {
        // This is expected
        eof = true;
      }

      CPPUNIT_ASSERT(eof);

      r = task.getBytesTransferred();
      CPPUNIT_ASSERT(r == 0);
    }

    sock.close();
  }
  catch(Exception& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }

  thread.join();
}

/*
 */


void AsyncIOTest::testAsyncSocketWriteRead()
{
//  RunnableClosure<AsyncIOTest, bool> runnable(this, &AsyncIOTest::_server,
//                                              true);

//  Thread thread(&runnable);

//  readFirst = true;

//  thread.start();

  try
  {
    ByteBuffer writeBuf(256), readBuf(256);

    StreamSocket sock;
    sock.init();
    sock.connect("localhost", 40407);
    sock.setTimeout(300);

    AsyncIOPoller poller;
    AsyncIOTask taskw;
    AsyncIOTask taskr;

    // Kick off a read even though we will not
    // receive anything until we transmit something

    int r = sock.read(readBuf, 0LL, taskr);
    CPPUNIT_ASSERT(r == 0 || r == 18);
    std::cout << "sock.read returned " << r << std::endl << std::endl;

    if (r == 0)
    {
      std::cout << "adding taskr to poller" << std::endl;
      poller.addTask(&taskr);
    }

    // Kick off a write.
    writeBuf.put((const byte_t *)"this is a test\r\n", 16);
    writeBuf.flip();

    printf("trying to send %d bytes\n", writeBuf.getRemaining());

    int w = sock.write(writeBuf, 0LL, taskw);
    CPPUNIT_ASSERT(w == 0 || w == 18);

    if(w == 0)
    {
      std::cout << "adding taskw to poller" << std::endl;
      poller.addTask(&taskw);
    }

    do
    {
      std::cout << "entering poller.poll()" << std::endl;
      bool ok = poller.poll(-1);
      std::cout << "poller returned: " << ok << std::endl;
      CPPUNIT_ASSERT(ok);
      bool eof = false;
      bool okw = false;
      bool okr = false;

      try
      {
        std::cout << "is taskw done?" << std::endl;
        okw = taskw.isCompleted();
        if (okw)
        {
          std::cout << "taskw.isCompleted returned true" << std::endl;
          poller.removeTask(&taskw);
        }
        else
          std::cout << "taskw.isCompleted returned false" << std::endl;

        std::cout << "is taskr done?" << std::endl;
        okr = taskr.isCompleted();
        if (okr)
        {
          std::cout << "taskr.isCompleted returned true" << std::endl;
          poller.removeTask(&taskr);
        }
        else
          std::cout << "taskr.isCompleted returned false" << std::endl;
      }
      catch(EOFException& eofex)
      {
        // This is expected
        eof = true;
      }
    }
    while(poller.getTaskCount() > 0);

    //CPPUNIT_ASSERT(eof);

    r = taskr.getBytesTransferred();
    printf("taskr.getBytesTransferred = %d\n", r);

    w = taskw.getBytesTransferred();
    printf("taskw.getBytesTransferred = %d\n", w);
    //CPPUNIT_ASSERT(w == 0);

    printf("Starting read portion of the test\n");
    AsyncIOTask task;

    CPPUNIT_ASSERT(r == 0 || r == 18);

    if(r == 0)
    {
      poller.addTask(&task);

      bool ok = poller.poll(-1);
      std::cout << "poller.poll returned : " << ok << std::endl;
      CPPUNIT_ASSERT(ok);
      bool eof = false;

      try
      {
        ok = task.isCompleted();
        std::cout << "task.isCompleted returned : " << ok << std::endl;
        if(ok)
          std::cout << "bytes transferred: " << task.getBytesTransferred()
                    << std::endl;
        CPPUNIT_ASSERT(ok);
      }
      catch(EOFException& eofex)
      {
        // This is expected
        eof = true;
      }

//      CPPUNIT_ASSERT(eof);

      r = task.getBytesTransferred();
      std::cout << "task.getBytesTransferred returned : " << r << std::endl;

      CPPUNIT_ASSERT(r == 0);
    }

    sock.close();
  }
  catch(Exception& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }

//  thread.join();
}

/*
 */

void AsyncIOTest::_server(bool writeDataToClient)
{
  try
  {
    ServerSocket ssock(40407);
    ssock.setReuseAddress(true);
    ssock.init();
    ssock.listen();

    ByteBuffer buf(2048);

    StreamSocket sock;

    ssock.accept(sock);

    if(writeDataToClient)
    {
      Thread::sleep(1000);
      static const char *message = "Hello from server!"; // 18 bytes

      sock.writeFully((byte_t*)message, strlen(message));
    }

    sock.close();
  }
  catch(Exception& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}
