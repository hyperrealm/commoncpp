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

#ifndef __ccxx_SQLException_hxx
#define __ccxx_SQLException_hxx

#include <commonc++/Common.h++>
#include <commonc++/IOException.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * An exception indicating a SQL database error.
 *
 * @author Mark Lindner
 */
class COMMONCPPDB_API SQLException : public IOException
{
 public:

  /** Construct a new SQLException with an optional cause message. */
  SQLException(String message = String::empty, String sql = String::null);

  virtual ~SQLException() throw();

  inline void write(std::ostream& stream) const
  { _write(stream, "SQLException"); }

  /** Get the SQL statement associated with this exception. */
  inline String getSQL() const
  { return(_sql); }

 private:

  String _sql;
};

} // namespace ccxx

#endif // __ccxx_SQLException_hxx
