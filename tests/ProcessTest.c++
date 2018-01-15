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

#include "ProcessTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/BasicBufferedStream.h++"
#include "commonc++/Process.h++"
#include "commonc++/Stream.h++"
#include "commonc++/String.h++"
#include "commonc++/System.h++"
#include "commonc++/Thread.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(ProcessTest);

/*
 */

CppUnit::Test *ProcessTest::suite()
{
  CCXX_TESTSUITE_BEGIN(ProcessTest);
  CCXX_TESTSUITE_TEST(ProcessTest, testProcess);
  CCXX_TESTSUITE_TEST(ProcessTest, testProcessInfo);
  CCXX_TESTSUITE_TEST(ProcessTest, testWorkingDir);
  CCXX_TESTSUITE_END();
}

/*
 */

void ProcessTest::setUp()
{
}

/*
 */

void ProcessTest::tearDown()
{
}

/*
 */

void ProcessTest::testProcess()
{
  StringVec args;

#ifdef CCXX_OS_POSIX
  args << "ls" << "-a" << "-l" << "-CsF";

  Process proc("/bin/ls", args);
#else

  Process proc("\\windows\\system32\\ipconfig.exe", args);

#endif

  try
  {
    proc.start();
  }
  catch(SystemException ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }

  Stream& instrm = proc.getInputStream();
  instrm.close(); // we're not going to write anything to the process

  Stream& strm = proc.getOutputStream();
  BufferedCharStream bstrm(strm);

  try
  {
    ByteBuffer buf(16);
    for(;;)
    {
      buf.clear();
      puts("reading");
      strm.read(buf);

      buf.flip();
      printf("buf flipped; remaining is %d\n", buf.getRemaining());

      char cbuf[17];
      strncpy(cbuf, (char *)buf.getBase(), buf.getRemaining());
      cbuf[buf.getRemaining()] = 0;
//      printf("%*s", buf.getRemaining(), buf.getBase());
      puts(cbuf);
      fflush(stdout);
    }
  }

  /*
  try
  {
    for(;;)
    {
      std::string s;

      bstrm >> s;

      std::cout << "got: " << s << std::endl;
    }
  }
  */

  catch(EOFException& ex)
  {
    std::cout << "EOF" << std::endl;
  }
  catch(IOException& ex)
  {
    std::cout << "IOException: " << ex.getMessage() << std::endl;
  }

  ExitStatus es;

  proc.waitFor(es);

  std::cout << "exit code: " << es.getExitCode() << std::endl;
  std::cout << "exit signal: " << es.getExitSignal() << std::endl;
  std::cout << "normal exit? " << es.isNormal() << std::endl;
  std::cout << "user time: " << es.getUserTime() << std::endl;
  std::cout << "kernel time: " << es.getKernelTime() << std::endl;

  // try terminating proc

  StringVec args2;
  args2 << "sleep" << "30";

  Process proc2("/bin/sleep", args2);

  try
  {
    proc2.start();
  }
  catch(SystemException ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }

  proc2.getInputStream().close();
  proc2.getOutputStream().close();
  proc2.getErrorStream().close();

  std::cout << "started sleep cmd" << std::endl;

  Thread::sleep(2000);
  proc2.stop();

  ExitStatus es2;
  std::cout << "waiting for exit" << std::endl;
  proc2.waitFor(es2);

  std::cout << "exit code: " << es2.getExitCode() << std::endl;
  std::cout << "exit signal: " << es2.getExitSignal() << std::endl;
  std::cout << "normal exit? " << es2.isNormal() << std::endl;
  std::cout << "user time: " << es2.getUserTime() << std::endl;
  std::cout << "kernel time: " << es2.getKernelTime() << std::endl;

  // TODO
}

/*
 */

void ProcessTest::testProcessInfo()
{
  std::cout << "process ID:        " << Process::currentProcessID() << '\n';

  String prog = Process::getProgramName();

  std::cout << "program name:      " << (prog ? prog : String::nullRep)
            << '\n';
  std::cout << "executable path:   " << Process::getExecutablePath() << '\n';
  std::cout << "working dir:       " << Process::getWorkingDir() << '\n';

  std::cout << "has console?       " << (Process::hasConsole() ? "yes" : "no")
            << '\n';

  std::cout << "\nDoes this look correct? (y/n) ";
  char c;
  std::cin >> c;

  CPPUNIT_ASSERT(tolower(c) == 'y');
}

/*
 */

void ProcessTest::testWorkingDir()
{
  String orig = Process::getWorkingDir();
  String tmp = System::getTempDir();

  Process::setWorkingDir(tmp);

  String cur = Process::getWorkingDir();

  CPPUNIT_ASSERT(cur != orig);
  Process::setWorkingDir(orig);

  cur = Process::getWorkingDir();
  CPPUNIT_ASSERT(cur == orig);
}
