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

#ifndef __ccxx_SQLValueBinder_hxx
#define __ccxx_SQLValueBinder_hxx

#include <commonc++/Common.h++>
#include <commonc++/Blob.h++>
#include <commonc++/OutOfBoundsException.h++>
#include <commonc++/SQLException.h++>

namespace ccxx {

class SQLQuery;

/**
 * A value binder for a SQLQuery object. This class provides for convenient
 * binding of values to consecutive query parameters through the chaining
 * of the left-shift operator, and extraction of values in consecutive fields
 * of query results through the chaining of the right-shift operator.
 */
class COMMONCPPDB_API SQLValueBinder
{
 public:

  /** Construct a new SQLValueBinder for the given SQLQuery. */
  SQLValueBinder(SQLQuery& query);

  /** Destructor. */
  ~SQLValueBinder();

  /**
   * Bind a boolean value to the next query parameter.
   *
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   * @throw OutOfBoundsException If all parameters have already been bound.
   */
  SQLValueBinder& operator<<(bool value);

  /**
   * Bind an integer value to the next query parameter.
   *
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   * @throw OutOfBoundsException If all parameters have already been bound.
   */
  SQLValueBinder& operator<<(int value);

  /**
   * Bind a 64-bit integer value to the next query parameter.
   *
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   * @throw OutOfBoundsException If all parameters have already been bound.
   */
  SQLValueBinder& operator<<(const int64_t& value);

  /**
   * Bind a double-precision floating point value to the next query parameter.
   *
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   * @throw OutOfBoundsException If all parameters have already been bound.
   */
  SQLValueBinder& operator<<(const double& value);

  /**
   * Bind a String value to the next query parameter.
   *
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   * @throw OutOfBoundsException If all parameters have already been bound.
   */
  SQLValueBinder& operator<<(const String& value);

  /**
   * Bind a string value to the next query parameter.
   *
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   * @throw OutOfBoundsException If all parameters have already been bound.
   */
  SQLValueBinder& operator<<(const char* value);

  /**
   * Bind a Blob value to the next query parameter.
   *
   * @param value The value to bind.
   * @throw SQLException If a database error occurs.
   * @throw OutOfBoundsException If all parameters have already been bound.
   */
  SQLValueBinder& operator<<(const Blob& value);

  /**
   * Extract a boolean value from the next field in the results.
   *
   * @param value The value to extract into.
   * @throw SQLException If a database error occurs.
   * @throw OutOfBoundsException If all fields have already been extracted.
   */
  SQLValueBinder& operator>>(bool& value);

  /**
   * Extract an integer value from the next field in the results.
   *
   * @param value The value to extract into.
   * @throw SQLException If a database error occurs.
   * @throw OutOfBoundsException If all fields have already been extracted.
   */
  SQLValueBinder& operator>>(int& value);

  /**
   * Extract a 64-bit integer value from the next field in the results.
   *
   * @param value The value to extract into.
   * @throw SQLException If a database error occurs.
   * @throw OutOfBoundsException If all fields have already been extracted.
   */
  SQLValueBinder& operator>>(int64_t& value);

  /**
   * Extract a double-precision floating point value from the next field in
   * the results.
   *
   * @param value The value to extract into.
   * @throw SQLException If a database error occurs.
   * @throw OutOfBoundsException If all fields have already been extracted.
   */
  SQLValueBinder& operator>>(double& value);

  /**
   * Extract a String value from the next field in the results.
   *
   * @param value The value to extract into.
   * @throw SQLException If a database error occurs.
   * @throw OutOfBoundsException If all fields have already been extracted.
   */
  SQLValueBinder& operator>>(String& value);

  /**
   * Extract a Blob value from the next field in the results.
   *
   * @param value The value to extract into.
   * @throw SQLException If a database error occurs.
   * @throw OutOfBoundsException If all fields have already been extracted.
   */
  SQLValueBinder& operator>>(Blob& value);

  /**
   * Reset the SQLValueBinder. The next binding/extraction will take place
   * at the first parameter/field.
   */
  void reset();

 private:

  void checkInputIndex();
  void checkOutputIndex();

  SQLQuery& _query;
  uint_t _index;

  CCXX_COPY_DECLS(SQLValueBinder);
};

} // namespace ccxx

#endif // __ccxx_SQLValueBinder_hxx
