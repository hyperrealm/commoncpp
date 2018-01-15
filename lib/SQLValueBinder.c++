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

#include "commonc++/SQLValueBinder.h++"
#include "commonc++/SQLQuery.h++"

namespace ccxx {

/*
 */

SQLValueBinder::SQLValueBinder(SQLQuery& query)
  : _query(query)
  , _index(1)
{
}

/*
 */

SQLValueBinder::~SQLValueBinder()
{
}

/*
 */

SQLValueBinder& SQLValueBinder::operator<<(bool value)
{
  checkInputIndex();
  _query.bind(_index, value ? 1 : 0);
  ++_index;

  return(*this);
}

/*
 */

SQLValueBinder& SQLValueBinder::operator<<(int value)
{
  checkInputIndex();
  _query.bind(_index, value);
  ++_index;

  return(*this);
}

/*
 */

SQLValueBinder& SQLValueBinder::operator<<(const int64_t& value)
{
  checkInputIndex();
  _query.bind(_index, value);
  ++_index;

  return(*this);
}

/*
 */

SQLValueBinder& SQLValueBinder::operator<<(const double& value)
{
  checkInputIndex();
  _query.bind(_index, value);
  ++_index;

  return(*this);
}

/*
 */

SQLValueBinder& SQLValueBinder::operator<<(const String& value)
{
  checkInputIndex();
  _query.bind(_index, value);
  ++_index;

  return(*this);
}

/*
 */

SQLValueBinder& SQLValueBinder::operator<<(const char* value)
{
  checkInputIndex();
  _query.bind(_index, value);
  ++_index;

  return(*this);
}

/*
 */

SQLValueBinder& SQLValueBinder::operator<<(const Blob& value)
{
  checkInputIndex();
  _query.bind(_index, value);
  ++_index;

  return(*this);
}

/*
 */

SQLValueBinder& SQLValueBinder::operator>>(bool& value)
{
  checkOutputIndex();
  value = (_query.getInt(_index) != 0);
  ++_index;

  return(*this);
}

/*
 */

SQLValueBinder& SQLValueBinder::operator>>(int& value)
{
  checkOutputIndex();
  value = _query.getInt(_index);
  ++_index;

  return(*this);
}

/*
 */

SQLValueBinder& SQLValueBinder::operator>>(int64_t& value)
{
  checkOutputIndex();
  value = _query.getInt64(_index);
  ++_index;

  return(*this);
}

/*
 */

SQLValueBinder& SQLValueBinder::operator>>(double& value)
{
  checkOutputIndex();
  value = _query.getDouble(_index);
  ++_index;

  return(*this);
}

/*
 */

SQLValueBinder& SQLValueBinder::operator>>(String& value)
{
  checkOutputIndex();
  value = _query.getString(_index);
  ++_index;

  return(*this);
}

/*
 */

SQLValueBinder& SQLValueBinder::operator>>(Blob& value)
{
  checkOutputIndex();
  value = _query.getBlob(_index);
  ++_index;

  return(*this);
}

/*
 */

void SQLValueBinder::reset()
{
  _index = 1;
}

/*
 */

void SQLValueBinder::checkInputIndex()
{
  if((_index == 0) || (_index > _query.getParameterCount()))
    throw OutOfBoundsException();
}

/*
 */

void SQLValueBinder::checkOutputIndex()
{
  if((_index == 0) || (_index > _query.getColumnCount()))
    throw OutOfBoundsException();
}


} // namespace ccxx
