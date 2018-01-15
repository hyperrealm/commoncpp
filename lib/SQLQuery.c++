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

#include "commonc++/SQLQuery.h++"

#include "SQLPrivate.h++"

namespace ccxx {

/*
 */

SQLQuery::SQLQuery(const SQLPrivate& private_)
  : _private(new SQLPrivate(private_)),
    _result(SQLITE_DONE),
    _primed(false),
    _hasData(false)
{
}

/*
 */

SQLQuery::~SQLQuery()
{
  if(_private->stmt)
    ::sqlite3_finalize(_private->stmt);

  delete _private;
}

/*
 */

void SQLQuery::reset()
{
  int r = ::sqlite3_reset(_private->stmt);
  if(r != SQLITE_OK)
    throw SQLException(String() << "Failed to reset statement: "
                       << ::sqlite3_errmsg16(_private->db));
}

/*
 */

String SQLQuery::toSQL()
{
  _private->checkValid();

  const char *t = ::sqlite3_sql(_private->stmt);

  if(! t)
    throw SQLException("invalid prepared statement");

  return(String(t));
}

/*
 */

void SQLQuery::bind(uint_t index)
{
  _private->checkValid();

  if(::sqlite3_bind_null(_private->stmt, index) != SQLITE_OK)
    throw SQLException(String() << "Failed to bind value: "
                       << ::sqlite3_errmsg16(_private->db));
}

/*
 */

void SQLQuery::bind(const String& name)
{
  _private->checkValid();

  CString cstr_name = name.toUTF8();
  int index = ::sqlite3_bind_parameter_index(_private->stmt, cstr_name.data());
  if(index == 0)
    throw SQLException(String() << "No such parameter: " << name);

  if(::sqlite3_bind_null(_private->stmt, index) != SQLITE_OK)
    throw SQLException(String() << "Failed to bind value: "
                       << ::sqlite3_errmsg16(_private->db));
}

/*
 */

void SQLQuery::bind(uint_t index, int value)
{
  _private->checkValid();

  if(::sqlite3_bind_int(_private->stmt, index, value) != SQLITE_OK)
    throw SQLException(String() << "Failed to bind value: "
                       << ::sqlite3_errmsg16(_private->db));
}

/*
 */

void SQLQuery::bind(const String& name, int value)
{
  _private->checkValid();

  CString cstr_name = name.toUTF8();
  int index = ::sqlite3_bind_parameter_index(_private->stmt, cstr_name.data());
  if(index == 0)
    throw SQLException(String() << "No such parameter: " << name);

  if(::sqlite3_bind_int(_private->stmt, index, value) != SQLITE_OK)
    throw SQLException(String() << "Failed to bind value: "
                       << ::sqlite3_errmsg16(_private->db));
}

/*
 */

void SQLQuery::bind(uint_t index, const int64_t &value)
{
  _private->checkValid();

  if(::sqlite3_bind_int64(_private->stmt, index, value) != SQLITE_OK)
    throw SQLException(String() << "Failed to bind value: "
                       << ::sqlite3_errmsg16(_private->db));
}

/*
 */

void SQLQuery::bind(const String& name, const int64_t &value)
 
{
  _private->checkValid();

  CString cstr_name = name.toUTF8();
  int index = ::sqlite3_bind_parameter_index(_private->stmt, cstr_name.data());
  if(index == 0)
    throw SQLException(String() << "No such parameter: " << name);

  if(::sqlite3_bind_int64(_private->stmt, index, value) != SQLITE_OK)
    throw SQLException(String() << "Failed to bind value: "
                       << ::sqlite3_errmsg16(_private->db));
}

/*
 */

void SQLQuery::bind(uint_t index, const double &value)
{
  _private->checkValid();

  if(::sqlite3_bind_double(_private->stmt, index, value) != SQLITE_OK)
    throw SQLException(String() << "Failed to bind value: "
                       << ::sqlite3_errmsg16(_private->db));
}

/*
 */

void SQLQuery::bind(const String& name, const double &value)
{
  _private->checkValid();

  CString cstr_name = name.toUTF8();
  int index = ::sqlite3_bind_parameter_index(_private->stmt, cstr_name.data());
  if(index == 0)
    throw SQLException(String() << "No such parameter: " << name);

  if(::sqlite3_bind_double(_private->stmt, index, value) != SQLITE_OK)
    throw SQLException(String() << "Failed to bind value: "
                       << ::sqlite3_errmsg16(_private->db));
}

/*
 */

void SQLQuery::bind(uint_t index, const String& value)
{
  _private->checkValid();

  if(::sqlite3_bind_text16(_private->stmt, index, value.data(),
                           value.length() * sizeof(char16_t), SQLITE_TRANSIENT)
     != SQLITE_OK)
    throw SQLException(String() << "Failed to bind value: "
                       << ::sqlite3_errmsg16(_private->db));
}

/*
 */

void SQLQuery::bind(uint_t index, const char *value)
{
  _private->checkValid();

  if(::sqlite3_bind_text(_private->stmt, index, value, -1,
                         SQLITE_TRANSIENT) != SQLITE_OK)
    throw SQLException(String() << "Failed to bind value: "
                       << ::sqlite3_errmsg16(_private->db));
}

/*
 */

void SQLQuery::bind(const String& name, const String& value)
{
  _private->checkValid();

  CString cstr_name = name.toUTF8();
  int index = ::sqlite3_bind_parameter_index(_private->stmt, cstr_name.data());
  if(index == 0)
    throw SQLException(String() << "No such parameter: " << name);

  if(::sqlite3_bind_text16(_private->stmt, index, value.data(),
                           value.length() * sizeof(char16_t), SQLITE_TRANSIENT)
     != SQLITE_OK)
    throw SQLException(String() << "Failed to bind value: "
                       << ::sqlite3_errmsg16(_private->db));
}

/*
 */

void SQLQuery::bind(const String& name, const char* value)
{
  _private->checkValid();

  CString cstr_name = name.toUTF8();
  int index = ::sqlite3_bind_parameter_index(_private->stmt, cstr_name.data());
  if(index == 0)
    throw SQLException(String() << "No such parameter: " << name);

  if(::sqlite3_bind_text(_private->stmt, index, value, -1,
                         SQLITE_TRANSIENT) != SQLITE_OK)
    throw SQLException(String() << "Failed to bind value: "
                       << ::sqlite3_errmsg16(_private->db));
}

/*
 */

void SQLQuery::bind(uint_t index, const Blob& value)
{
  _private->checkValid();

  if(::sqlite3_bind_text(_private->stmt, index,
                         reinterpret_cast<const char *>(value.getData()),
                         value.getLength(), SQLITE_TRANSIENT) != SQLITE_OK)
    throw SQLException(String() << "Failed to bind value: "
                       << ::sqlite3_errmsg16(_private->db));
}

/*
 */

void SQLQuery::bind(const String& name, const Blob& value)
{
  _private->checkValid();

  CString cstr_name = name.toUTF8();
  int index = ::sqlite3_bind_parameter_index(_private->stmt, cstr_name.data());
  if(index == 0)
    throw SQLException(String() << "No such parameter: " << name);

  if(::sqlite3_bind_text(_private->stmt, index,
                         reinterpret_cast<const char *>(value.getData()),
                         value.getLength(), SQLITE_TRANSIENT) != SQLITE_OK)
    throw SQLException(String() << "Failed to bind value: "
                       << ::sqlite3_errmsg16(_private->db));
}

/*
 */

void SQLQuery::clear()
{
  _private->checkValid();

  ::sqlite3_clear_bindings(_private->stmt);
}

/*
 */

uint_t SQLQuery::getParameterCount()
{
  _private->checkValid();

  return(::sqlite3_bind_parameter_count(_private->stmt));
}

/*
 */

void SQLQuery::execute()
{
  _private->checkValid();
  prime();
}

/*
 */

void SQLQuery::prime()
{
  _next(); // prefetch first row
  _primed = true;

  if(_result == SQLITE_INTERRUPT)
    throw InterruptedException();
  else if(!((_result == SQLITE_DONE) || (_result == SQLITE_ROW)))
    throw SQLException(String() << ::sqlite3_errmsg16(_private->db));
}

/*
 */

bool SQLQuery::next()
{
  _private->checkValid();

  if(! _primed)
    _next();
  else
    _primed = false;

  switch(_result)
  {
    case SQLITE_DONE:
      ::sqlite3_reset(_private->stmt);
      _hasData = false;
      return(false);

    case SQLITE_BUSY:
      throw TimeoutException();
      break;

    case SQLITE_ROW:
      _hasData = true;
      break;

    case SQLITE_INTERRUPT:
      ::sqlite3_reset(_private->stmt);
      throw InterruptedException();
      break;

    default:
      throw SQLException(String() << ::sqlite3_errmsg16(_private->db));
  }

  return(true);
}

/*
 */

void SQLQuery::_next()
{
  if(_private->stmt)
    _result = ::sqlite3_step(_private->stmt);
}

/*
 */

uint_t SQLQuery::getColumnCount()
{
  _private->checkValid();

  return(static_cast<uint_t>(::sqlite3_column_count(_private->stmt)));
}

/*
 */

String SQLQuery::getColumnName(uint_t column)
{
  _private->checkValid();

  if(column == 0)
    return String::null;

  const char *s = ::sqlite3_column_origin_name(_private->stmt, --column);
  if(! s)
    throw SQLException("memory allocation error");

  return s;
}

/*
 */

String SQLQuery::getAliasedColumnName(uint_t column)
{
  _private->checkValid();

  if(column == 0)
    return String::null;

  const char *s = ::sqlite3_column_name(_private->stmt, --column);
  if(! s)
    throw SQLException("memory allocation error");

  return s;
}

/*
 */

String SQLQuery::getTableName(uint_t column)
{
  _private->checkValid();

  if(column == 0)
    return String::null;

  const char *s = ::sqlite3_column_table_name(_private->stmt, --column);
  if(! s)
    throw SQLException("memory allocation error");

  return s;
}

/*
 */

Variant SQLQuery::getColumnValue(uint_t column)
{
  _private->checkValid();

  Variant value;

  if(_hasData && (column != 0))
  {
    int type = ::sqlite3_column_type(_private->stmt, --column);
    switch(type)
    {
      case SQLITE_INTEGER:
        value = (int64_t)::sqlite3_column_int64(_private->stmt, --column);
        break;

      case SQLITE_FLOAT:
        value = ::sqlite3_column_double(_private->stmt, --column);
        break;

      case SQLITE_TEXT:
        value = _getString(--column);

      case SQLITE_BLOB:
        value = _getBlob(--column);
        break;

      case SQLITE_NULL:
      default:
        // do nothing
        break;
    }
  }

  return(value); // null if unchanged
}

/*
 */

int SQLQuery::getInt(uint_t column)
{
  _private->checkValid();

  if(! _hasData || (column == 0))
    return 0;

  return(::sqlite3_column_int(_private->stmt, --column));
}

/*
 */

int64_t SQLQuery::getInt64(uint_t column)
{
  _private->checkValid();

  if(! _hasData || (column == 0))
    return(INT64_CONST(0));

  return(::sqlite3_column_int64(_private->stmt, --column));
}

/*
 */

double SQLQuery::getDouble(uint_t column)
{
  _private->checkValid();

  if(! _hasData || (column == 0))
    return(0.0);

  return(::sqlite3_column_double(_private->stmt, --column));
}

/*
 */

String SQLQuery::getString(uint_t column)
{
  _private->checkValid();

  if(! _hasData || (column == 0))
    return(String::null);

  return(_getString(--column));
}

/*
 */

Blob SQLQuery::getBlob(uint_t column)
{
  _private->checkValid();

  if(! _hasData || (column == 0))
    return(Blob::null);

  return(_getBlob(--column));
}

/*
 */

String SQLQuery::_getString(uint_t column)
{
  const char16_t *t = reinterpret_cast<const char16_t *>(
    ::sqlite3_column_text16(_private->stmt, column));
  size_t len = static_cast<size_t>(
    ::sqlite3_column_bytes16(_private->stmt, column) / 2);
  return(String(t, 0, len));
}

/*
 */

Blob SQLQuery::_getBlob(uint_t column)
{
  const byte_t *b = ::sqlite3_column_text(_private->stmt, column);
  size_t len = static_cast<size_t>(::sqlite3_column_bytes(_private->stmt,
                                                          column));
  return(Blob(b, len));
}


} // namespace ccxx
