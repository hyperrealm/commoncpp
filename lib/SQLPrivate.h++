/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2009  Mark A Lindner

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

#include <commonc++/Common.h++>

#include <sqlite3.h>

namespace ccxx {

/** @cond INTERNAL */
class SQLPrivate
{
  public:

  SQLPrivate(sqlite3 *db = NULL, sqlite3_stmt *stmt = NULL)
    : db(db), stmt(stmt)
  {
  }

  SQLPrivate(const SQLPrivate& other)
    : db(other.db), stmt(other.stmt)
  {
  }

  void checkValid() throw(SQLException)
  {
    if(! stmt)
      throw SQLException("invalid statement");
  }

  sqlite3 *db;
  sqlite3_stmt *stmt;
};
/** @endcond */


} // namespace ccxx
