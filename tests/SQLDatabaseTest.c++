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

#include "SQLDatabaseTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/File.h++"
#include "commonc++/ScopedPtr.h++"
#include "commonc++/SQLDatabase.h++"
#include "commonc++/SQLQuery.h++"
#include "commonc++/SQLValueBinder.h++"

#include <fstream>
#include <iostream>
#include <string>

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(SQLDatabaseTest);

/*
 */

CppUnit::Test *SQLDatabaseTest::suite()
{
  CCXX_TESTSUITE_BEGIN(SQLDatabaseTest);
  CCXX_TESTSUITE_TEST(SQLDatabaseTest, testInsert);
  CCXX_TESTSUITE_TEST(SQLDatabaseTest, testSelect);
  CCXX_TESTSUITE_TEST(SQLDatabaseTest, testValueBinder);
  CCXX_TESTSUITE_TEST(SQLDatabaseTest, testTransaction);
  CCXX_TESTSUITE_END();
}

/*
 */

void SQLDatabaseTest::setUp()
{
  SQLDatabase db;

  try
  {
    File::remove("./testdata/testdb.db");
    db.open("./testdata/testdb.db", true);
    uint_t n = db.executeSQL("CREATE TABLE BOOKS ("
                             "  TITLE STRING NOT NULL,"
                             "  AUTHOR STRING NOT NULL,"
                             "  QTY INTEGER NOT NULL,"
                             "  PRICE REAL);");
    CPPUNIT_ASSERT_EQUAL(0U, n);
  }
  catch(const Exception &ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}

/*
 */

void SQLDatabaseTest::tearDown()
{
  File::remove("./testdata/testdb.db");
}

/*
 */

void SQLDatabaseTest::testInsert()
{
  uint_t n = 0;

  try
  {
    SQLDatabase db;
    db.open("./testdata/testdb.db", true);

    // Database must not be closed before queries go out of scope!
    {
      ScopedPtr<SQLQuery> insertQuery(
        db.prepareQuery("INSERT INTO BOOKS"
                        " (TITLE, AUTHOR, QTY, PRICE)"
                        " VALUES (?, ?, ?, ?)"));

      CPPUNIT_ASSERT_EQUAL(4U, insertQuery->getParameterCount());
      CPPUNIT_ASSERT_EQUAL(String("INSERT INTO BOOKS"
                                  " (TITLE, AUTHOR, QTY, PRICE)"
                                  " VALUES (?, ?, ?, ?)"),
                           insertQuery->toSQL());

      insertQuery->bind(1, "Treasure Island");
      insertQuery->bind(2, "Robert Louis Stevenson");
      insertQuery->bind(3, 5);
      insertQuery->bind(4, 29.99);
      insertQuery->execute();

      insertQuery->reset();
      insertQuery->bind(1, "Snow Crash");
      insertQuery->bind(2, "Neal Stephenson");
      insertQuery->bind(3, 8);
      insertQuery->bind(4, 9.99);
      insertQuery->execute();

      n = db.executeSQL("SELECT * FROM BOOKS");
      CPPUNIT_ASSERT_EQUAL(2U, n);

      n = db.executeSQL("SELECT * FROM BOOKS WHERE PRICE = 9.99");
      CPPUNIT_ASSERT_EQUAL(1U, n);

      n = db.executeSQL("SELECT * FROM BOOKS WHERE PRICE = 0.0");
      CPPUNIT_ASSERT_EQUAL(0U, n);
    }

    db.close();
  }
  catch(const Exception &ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}

/*
 */

void SQLDatabaseTest::testSelect()
{
  try
  {
    SQLDatabase db;
    db.open("./testdata/testdb.db", true);

    populateData(&db);

    // Database must not be closed before queries go out of scope!
    {
      ScopedPtr<SQLQuery> selectQuery(
        db.prepareQuery("SELECT * FROM BOOKS"
                        " WHERE TITLE = :TITLE"));

      selectQuery->bind(":TITLE", "Snow Crash");
      selectQuery->execute();

      CPPUNIT_ASSERT_EQUAL(true, selectQuery->next());
      CPPUNIT_ASSERT_EQUAL(4U, selectQuery->getColumnCount());
      CPPUNIT_ASSERT_EQUAL(String("BOOKS"), selectQuery->getTableName(1));
      CPPUNIT_ASSERT_EQUAL(String("BOOKS"), selectQuery->getTableName(4));

      CPPUNIT_ASSERT_EQUAL(String("TITLE"), selectQuery->getColumnName(1));
      CPPUNIT_ASSERT_EQUAL(String("AUTHOR"), selectQuery->getColumnName(2));
      CPPUNIT_ASSERT_EQUAL(String("QTY"), selectQuery->getColumnName(3));
      CPPUNIT_ASSERT_EQUAL(String("PRICE"), selectQuery->getColumnName(4));

      CPPUNIT_ASSERT_EQUAL(String("Snow Crash"), selectQuery->getString(1));
      CPPUNIT_ASSERT_EQUAL(String("Neal Stephenson"),
                           selectQuery->getString(2));
      CPPUNIT_ASSERT_EQUAL(8, selectQuery->getInt(3));
      CPPUNIT_ASSERT_EQUAL(9.99, selectQuery->getDouble(4));

      CPPUNIT_ASSERT_EQUAL(0, selectQuery->getInt(1));
      CPPUNIT_ASSERT_EQUAL(String("8"), selectQuery->getString(3));
      CPPUNIT_ASSERT_EQUAL(String("9.99"), selectQuery->getString(4));

      CPPUNIT_ASSERT_EQUAL(false, selectQuery->next());
    }

    db.close();
  }
  catch(const Exception &ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}

/*
 */

void SQLDatabaseTest::testValueBinder()
{
  try
  {
    SQLDatabase db;
    db.open("./testdata/testdb.db", true);

    populateData(&db);

    // Database must not be closed before queries go out of scope!
    {
      ScopedPtr<SQLQuery> selectQuery(
        db.prepareQuery("SELECT * FROM BOOKS"
                        " WHERE TITLE = ?"));

      SQLValueBinder binder(*selectQuery);
      binder << "Treasure Island";

      selectQuery->execute();
      CPPUNIT_ASSERT_EQUAL(true, selectQuery->next());

      binder.reset();
      String title, author;
      int qty = 0;
      double price = 0.0;

      binder >> title >> author >> qty >> price;
      CPPUNIT_ASSERT_EQUAL(String("Treasure Island"), title);
      CPPUNIT_ASSERT_EQUAL(String("Robert Louis Stevenson"), author);
      CPPUNIT_ASSERT_EQUAL(5, qty);
      CPPUNIT_ASSERT_EQUAL(29.99, price);
    }

    db.close();
  }
  catch(const Exception &ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}

/*
 */

void SQLDatabaseTest::testTransaction()
{
  try
  {
    SQLDatabase db;
    db.open("./testdata/testdb.db", true);

    populateData(&db);

    db.begin();
    db.executeSQL("DELETE FROM BOOKS");
    db.rollback();

    uint_t n = db.executeSQL("SELECT * FROM BOOKS");
    CPPUNIT_ASSERT_EQUAL(2U, n);

    db.begin();
    db.executeSQL("DELETE FROM BOOKS");
    db.commit();

    n = db.executeSQL("SELECT * FROM BOOKS");
    CPPUNIT_ASSERT_EQUAL(0U, n);

    db.close();
  }
  catch(const Exception &ex)
  {
    CCXX_TEST_FAIL_EXCEPTION(ex);
  }
}

/*
 */

void SQLDatabaseTest::populateData(SQLDatabase *db)
{
  db->executeSQL(
    "INSERT INTO BOOKS(TITLE, AUTHOR, QTY, PRICE)"
    " VALUES ('Treasure Island', 'Robert Louis Stevenson', 5, 29.99)");
  db->executeSQL(
    "INSERT INTO BOOKS(TITLE, AUTHOR, QTY, PRICE)"
    " VALUES ('Snow Crash', 'Neal Stephenson', 8, 9.99)");
}
