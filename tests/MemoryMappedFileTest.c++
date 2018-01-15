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

#include "MemoryMappedFileTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/MemoryMappedFile.h++"

#include <cstring>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(MemoryMappedFileTest);

/*
 */

CppUnit::Test *MemoryMappedFileTest::suite()
{
  CCXX_TESTSUITE_BEGIN(MemoryMappedFileTest);
  CCXX_TESTSUITE_TEST(MemoryMappedFileTest, testMemoryMappedFile);
  CCXX_TESTSUITE_END();
}

/*
 */

void MemoryMappedFileTest::setUp()
{
}

/*
 */

void MemoryMappedFileTest::tearDown()
{
}

/*
 */

void MemoryMappedFileTest::testMemoryMappedFile()
{
  MemoryMappedFile mmf("./testdata/menu.xml");

  try
  {
    mmf.open(0, true); // read-only

    byte_t *base = mmf.getBase();

    char buf[129];
    std::memcpy(buf, base, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = 0;

    mmf.close();

    // std::cout << "size: " << mmf.getSize() << std::endl;

    char buf2[129];

    FILE *fp = fopen("./testdata/menu.xml", "r");
    int r = fread(buf2, sizeof(buf2) - 1, 1, fp);
    fclose(fp);

    buf2[sizeof(buf) - 1] = 0;

    if(r == 1)
      CPPUNIT_ASSERT(!::strncmp(buf, buf2, sizeof(buf) - 1));

    //std::cout << buf << std::endl;
  }
  catch(IOException &ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}
