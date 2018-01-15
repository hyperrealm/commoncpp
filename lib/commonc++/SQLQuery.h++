/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2014  Mark A Lindner

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

#ifndef __ccxx_SQLQuery_hxx
#define __ccxx_SQLQuery_hxx

#include <commonc++/Common.h++>
#include <commonc++/Blob.h++>
#include <commonc++/InterruptedException.h++>
#include <commonc++/SQLException.h++>
#include <commonc++/String.h++>
#include <commonc++/Variant.h++>

namespace ccxx {

class SQLDatabase; // fwd decl
class SQLPrivate; // fwd decl

/**
 * An SQL prepared statement that can be efficiently executed multiple times.
 * New values may be bound to the statement's parameters (if any) using the
 * various <code>bind*()</code> methods. Once the statement is executed, the
 * <code>next()</code> method is used to iterate over the result set,
 * and the various <code>get*()</code> methods are used to extract the values
 * from each row of the result set.
 *
 * A SQLQuery must be reset via a call to <code>reset()</code> before it can
 * be executed again.
 *
 * An SQLValueBinder may optionally be used to express the parameter value
 * binding and result value extraction in a concise way.
 *
 * SQLQuery objects should be destroyed before the SQLDatabase that they are
 * associated with is closed.
 *
 * @author Mark Lindner
 */
class COMMONCPPDB_API SQLQuery
{
  friend class SQLDatabase;

 public:

  /** Destructor. */
  ~SQLQuery();

  /**
   * Reset the query so that it can be executed again.
   *
   * @throw SQLException If a database error occurs.
   */
  void reset();

  /**
   * Get the SQL statement represented by this query.
   *
   * @return The SQL statement, as a String.
   * @throw SQLException If a database error occurs.
   */
  String toSQL();

  /**
   * Bind a null value to the designated parameter in the query.
   *
   * @param index The index of the parameter (where 1 refers to the first
   * parameter).
   * @throw SQLException If a database error occurs.
   */
  void bind(uint_t index);

  /**
   * Bind a null value to the designated parameter in the query.
   *
   * @param name The name of the parameter.
   * @throw SQLException If a database error occurs.
   */
  void bind(const String& name);

  /**
   * Bind an integer value to the designated parameter in the query.
   *
   * @param index The index of the parameter (where 1 refers to the first
   * parameter).
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   */
  void bind(uint_t index, int value);

  /**
   * Bind an integer value to the designated parameter in the query.
   *
   * @param name The name of the parameter.
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   */
  void bind(const String& name, int value);

  /**
   * Bind a 64-bit integer value to the designated parameter in the
   * query.
   *
   * @param index The index of the parameter (where 1 refers to the first
   * parameter).
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   */
  void bind(uint_t index, const int64_t& value);

  /**
   * Bind a 64-bit integer value to the designated parameter in the
   * statement.
   *
   * @param name The name of the parameter.
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   */
  void bind(const String& name, const int64_t& value);

  /**
   * Bind a double-precision floating point value to the designated
   * parameter in the query.
   *
   * @param index The index of the parameter (where 1 refers to the first
   * parameter).
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   */
  void bind(uint_t index, const double& value);

  /**
   * Bind a double-precision floating point value to the designated
   * parameter in the query.
   *
   * @param name The name of the parameter.
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   */
  void bind(const String& name, const double& value);

  /**
   * Bind a String value to the designated parameter in the query.
   *
   * @param index The index of the parameter (where 1 refers to the first
   * parameter).
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   */
  void bind(uint_t index, const String& value);

  /**
   * Bind a String value to the designated parameter in the query.
   *
   * @param name The name of the parameter.
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   */
  void bind(const String& name, const String& value);

  /**
   * Bind a string value to the designated parameter in the query.
   *
   * @param index The index of the parameter (where 1 refers to the first
   * parameter).
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   */
  void bind(uint_t index, const char* value);

  /**
   * Bind a string value to the designated parameter in the query.
   *
   * @param name The name of the parameter.
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   */
  void bind(const String& name, const char* value);

  /**
   * Bind a Blob value to the designated parameter in the query.
   *
   * @param index The index of the parameter (where 1 refers to the first
   * parameter).
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   */
  void bind(uint_t index, const Blob& value);

  /**
   * Bind a Blob value to the designated parameter in the query.
   *
   * @param name The name of the parameter.
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   */
  void bind(const String& name, const Blob& value);

  /**
   * Clear the parameter value bindings for the query.
   *
   * @throw SQLException If a database error occurs.
   */
  void clear();

  /**
   * Get the number of parameters in the query.
   *
   * @throw SQLException If a database error occurs.
   */
  uint_t getParameterCount();

  /**
   * Execute the query.
   *
   * @throw SQLException If a database error occurs.
   * @throw InterruptedException If the operation was interrupted via
   * a call to SQLDatabase::interrupt().
   */
  void execute();

  /**
   * Advance to the next result.
   *
   * @throw SQLException If a database error occurs.
   * @throw InterruptedException If the operation was interrupted via
   * a call to SQLDatabase::interrupt().
   * @throw TimeoutException If a timeout occurs while waiting for
   * database locks. The call should be retried in this case.
   * @return <b>true</b> if successful, or <b>false</b> if there are no
   * more results.
   */
  bool next();

  /**
   * Get the number of columns in the results.
   *
   * @throw SQLException If a database error occurs.
   * @return The column count.
   */
  uint_t getColumnCount();

  /**
   * Get the value for the given column in the current row of the
   * results.
   *
   * @param column The column number (counting from 1).
   * @return The value, as a Variant.
   * @throw SQLException If a database error occurs.
   */
  Variant getColumnValue(uint_t column);

  /**
   * Get the unaliased name for the given column in the results.
   *
   * @param column The column number (counting from 1).
   * @return The column name.
   * @throw SQLException If a database error occurs.
   */
  String getColumnName(uint_t column);

  /**
   * Get the aliased name for the given column in the results.
   *
   * @param column The column number (counting from 1).
   * @return The aliased column name.
   * @throw SQLException If a database error occurs.
   */
  String getAliasedColumnName(uint_t column);

  /**
   * Get the name of the table for the given column in the results.
   *
   * @param column The column number (counting from 1).
   * @return The table name.
   * @throw SQLException If a database error occurs.
   */
  String getTableName(uint_t column);

  /**
   * Get the value for the given column in the current row of the
   * results, as an integer.
   *
   * @param column The column number (counting from 1).
   * @return The value, as an int.
   * @throw SQLException If a database error occurs.
   */
  int getInt(uint_t column);

  /**
   * Get the value for the given column in the current row of the
   * results, as a 64-bit integer.
   *
   * @param column The column number (counting from 1).
   * @return The value, as an int64_t.
   * @throw SQLException If a database error occurs.
   */
  int64_t getInt64(uint_t column);

  /**
   * Get the value for the given column in the current row of the
   * results, as a double-precision floating point value.
   *
   * @param column The column number (counting from 1).
   * @return The value, as a double.
   * @throw SQLException If a database error occurs.
   */
  double getDouble(uint_t column);

  /**
   * Get the value for the given column in the current row of the
   * results, as a String.
   *
   * @param column The column number (counting from 1).
   * @return The value, as a String.
   * @throw SQLException If a database error occurs.
   */
  String getString(uint_t column);

  /**
   * Get the value for the given column in the current row of the
   * results, as a Blob.
   *
   * @param column The column number (counting from 1).
   * @return The value, as a Blob.
   * @throw SQLException If a database error occurs.
   */
  Blob getBlob(uint_t column);

 private:

  SQLQuery(const SQLPrivate& private_);

  void prime();
  void _next();

  String _getString(uint_t column);
  Blob _getBlob(uint_t column);

  SQLPrivate* _private;
  int _result;
  bool _primed;
  bool _hasData;

  CCXX_COPY_DECLS(SQLQuery);
};

} // namespace ccxx

#endif // __ccxx_SQLQuery_hxx
