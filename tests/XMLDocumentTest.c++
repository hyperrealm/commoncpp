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

#include "XMLDocumentTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/XMLDocument.h++"
#include "commonc++/XMLElement.h++"
#include "commonc++/XMLElementConstIterator.h++"

#include <fstream>
#include <iostream>
#include <string>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(XMLDocumentTest);

/*
 */

CppUnit::Test *XMLDocumentTest::suite()
{
  CCXX_TESTSUITE_BEGIN(XMLDocumentTest);
  CCXX_TESTSUITE_TEST(XMLDocumentTest, testParse);
  CCXX_TESTSUITE_TEST(XMLDocumentTest, testOutput);
  CCXX_TESTSUITE_TEST(XMLDocumentTest, testElements);
  CCXX_TESTSUITE_TEST(XMLDocumentTest, testAttributes);
  CCXX_TESTSUITE_END();
}

/*
 */

void XMLDocumentTest::setUp()
{
}

/*
 */

void XMLDocumentTest::tearDown()
{
}

/*
 */

void XMLDocumentTest::testParse()
{
  using ccxx::XMLDocument;

  try
  {
    puts("\nHERE\n");
    std::ifstream stream("testdata/xmltest.xml");

    XMLDocument doc;

    doc.read(stream);

    stream.close();
    puts("DONE");
  }
  catch(ParseException& ex)
  {
    puts("\nParseException\n");
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
  catch(IOException& ex)
  {
    puts("\nIOException\n");
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
  catch(...)
  {
    puts("\n unknown Exception\n");
  }
}

void XMLDocumentTest::testOutput()
{
 using ccxx::XMLDocument;

  std::ofstream stream("testdata/out.xml");
  XMLDocument doc;

  _parseDocument(doc, "testdata/xmltest.xml");

  try
  {
    std::ofstream ostream("testdata/out.xml");
    doc.write(ostream, 2);
    ostream.close();
  }
  catch(IOException& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}

void XMLDocumentTest::testAttributes()
{
  using ccxx::XMLDocument;
  using ccxx::XMLElement;

  XMLDocument doc;
  _parseDocument(doc, "testdata/xmltest.xml");

  const XMLElement &root = doc.getRoot();

  const XMLElement &attrs = root["attributes"];
  CPPUNIT_ASSERT_EQUAL(false, attrs.isNull());
  int i = 0;

  XMLElementConstIterator iter(attrs, "attr");
  while(iter.hasNext())
  {
    const XMLElement &elem = iter.next();

    std::cout << "iterating: element = " << elem.getName() << std::endl;

    bool f;

    switch(i)
    {
      case 0:
      {
        bool bval;
        f = elem.getAttribute("value", bval);
        CPPUNIT_ASSERT_EQUAL(true, f);
        CPPUNIT_ASSERT_EQUAL(true, bval);
        break;
      }

      case 1:
      {
        int ival;
        f = elem.getAttribute("value", ival);
        CPPUNIT_ASSERT_EQUAL(true, f);
        CPPUNIT_ASSERT_EQUAL(12345678, ival);
        break;
      }

      case 2:
      {
        double dval;
        f = elem.getAttribute("value", dval);
        CPPUNIT_ASSERT_EQUAL(true, f);
        CPPUNIT_ASSERT_EQUAL(-3.141592654, dval);
        break;
      }

      case 3:
      {
        int64_t lval;
        f = elem.getAttribute("value", lval);
        CPPUNIT_ASSERT_EQUAL(true, f);
        CPPUNIT_ASSERT_EQUAL(INT64_CONST(1125899906842624), lval);
        break;
      }

      case 4:
      {
        String sval;
        f = elem.getAttribute("value", sval);
        CPPUNIT_ASSERT_EQUAL(true, f);
        CPPUNIT_ASSERT_EQUAL(String("Hello, world!"), sval);
        break;
      }
    }

    ++i;
  }
}

void XMLDocumentTest::testElements()
{
  using ccxx::XMLDocument;
  using ccxx::XMLElement;

  XMLDocument doc;
  _parseDocument(doc, "testdata/xmltest.xml");

  const XMLElement &elem = doc.find("test.deeply.nested.item");
  CPPUNIT_ASSERT_EQUAL(false, elem.isNull());

  CPPUNIT_ASSERT_EQUAL(String("Jello"), elem.getContent());

  uint_t count = doc.getRoot().getChildCount();

  CPPUNIT_ASSERT_EQUAL(2U, count);

  XMLElement &root = doc.getRoot();

  XMLElement &e1 = root["deeply"];
  CPPUNIT_ASSERT_EQUAL(false, e1.isNull());
  XMLElement &e2 = e1["nested"];
  CPPUNIT_ASSERT_EQUAL(false, e2.isNull());
  XMLElement &e3 = e2["item"];
  CPPUNIT_ASSERT_EQUAL(false, e3.isNull());

  String content = e3.getContent();
  std::cout << "content is: " << content << std::endl;
  CPPUNIT_ASSERT_EQUAL(String("Jello"), content);
}

void XMLDocumentTest::_parseDocument(ccxx::XMLDocument& doc,
                                     const std::string& file)
{
  using ccxx::XMLDocument;
  using ccxx::XMLElement;

  try
  {
    std::ifstream stream(file.c_str());
    doc.read(stream);
    stream.close();
  }
  catch(IOException& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
  catch(ParseException& ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}

