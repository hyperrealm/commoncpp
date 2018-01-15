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

#include "PermissionsTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Permissions.h++"
#include "commonc++/File.h++"
#include "commonc++/System.h++"

#include <iostream>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(PermissionsTest);

/*
 */

CppUnit::Test *PermissionsTest::suite()
{
  CCXX_TESTSUITE_BEGIN(PermissionsTest);
  CCXX_TESTSUITE_TEST(PermissionsTest, testPermissions);
  CCXX_TESTSUITE_END();
}

/*
 */

void PermissionsTest::setUp()
{
}

/*
 */

void PermissionsTest::tearDown()
{
}

/*
 */

void PermissionsTest::testPermissions()
{
  File *f[3];
  f[0] = new File("./test-rwx------");
  f[1] = new File("./test-rwxrwxr--");
  f[2] = new File("./test-rw-r--r--");

  Permissions perms[3];
  perms[0] = Permissions(Permissions::USER_READ_WRITE
                         | Permissions::USER_EXECUTE);
  perms[1] = Permissions(Permissions::USER_READ_WRITE
                         | Permissions::USER_EXECUTE
                         | Permissions::GROUP_READ
                         | Permissions::GROUP_WRITE
                         | Permissions::GROUP_EXECUTE
                         | Permissions::OTHERS_READ);
  perms[2] = Permissions(Permissions::USER_READ_WRITE
                         | Permissions::GROUP_READ
                         | Permissions::OTHERS_READ);

  for(int i = 0; i < 3; ++i)
  {
    try
    {
      if(f[i]->exists())
        f[i]->remove();

      f[i]->open(IOReadWrite, FileOpenElseCreate, perms[i]);

      f[i]->close();
    }
    catch(IOException &ex)
    {
      CCXX_TEST_FAIL_EXCEPTION(ex);
    }
  }

  for(int i = 0; i < 3; ++i)
    delete f[i];
}
