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

#include "SerialPortTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/SerialPort.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(SerialPortTest);

/*
 */

CppUnit::Test *SerialPortTest::suite()
{
  CCXX_TESTSUITE_BEGIN(SerialPortTest);
  CCXX_TESTSUITE_TEST(SerialPortTest, testSerialPort);
  CCXX_TESTSUITE_END();
}

/*
 */

void SerialPortTest::setUp()
{
}

/*
 */

void SerialPortTest::tearDown()
{
}

/*
 */

void SerialPortTest::testSerialPort()
{
#ifdef CCXX_OS_WINDOWS
  String device = "\\\\.\\COM2";
#else
  String device = "/dev/ttyS0";
#endif

  SerialPort port(device);
  String msg;
  CharBuffer buf(256);
  size_t len;

  try
  {
    port.open();
    port.setTimeout(2000);

    port.setBaudRate(SerialPort::Baud9600);
    port.setDataBits(SerialPort::DataBits8);
    port.setStopBits(SerialPort::StopBits1);
    port.setParity(SerialPort::ParityNone);
    port.setFlowControl(SerialPort::FlowOff);

    msg = "*0SL";

    CString cstr_msg = msg.toUTF8();
    len = port.writeFully((const byte_t *)cstr_msg.data(), cstr_msg.length());
    printf("wrote %lu bytes\n", len);

    try
    {
      for(;;)
      {
        buf.clear();
        buf.setLimit(22);
        len = port.readFully(buf);

        printf("read %lu bytes\n", len);

        buf.flip();
        printf("%*s\n", buf.getLimit(), buf.getPointer());
      }
    }
    catch(TimeoutException &tex) { }

    // latch relay #3

    msg = "*0OR3L";
    cstr_msg = msg.toUTF8();
    len = port.writeFully((const byte_t *)cstr_msg.data(), cstr_msg.length());
    printf("wrote %lu bytes\n", len);

    /*
    buf.clear();
    buf.setLimit(10);
    len = port.read(buf);

    printf("read %d bytes\n", len);

    buf.flip();
    printf("%*s\n", buf.getLimit(), buf.getPointer());
    */

    // read relay statuses

    msg = "*0SR";
    cstr_msg = msg.toUTF8();
    len = port.writeFully((const byte_t *)cstr_msg.data(), cstr_msg.length());
    printf("wrote %lu bytes\n", len);

    try
    {
      for(;;)
      {
        buf.clear();
        buf.setLimit(21);
        len = port.readFully(buf);

        printf("read %lu bytes\n", len);

        buf.flip();
        printf("%*s\n", buf.getLimit(), buf.getPointer());
      }
    }
    catch(TimeoutException &tex) { }

    port.close();
  }
  catch(IOException &ex)
  {
//    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}
