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

#ifndef __ccxx_IOException_hxx
#define __ccxx_IOException_hxx

#include <commonc++/Common.h++>
#include <commonc++/Exception.h++>

namespace ccxx {

/**
 * An exception indicating an I/O error.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API IOException : public Exception
{
 public:

  /** Construct a new IOException with an optional cause message. */
  IOException(String message = String::empty);

  virtual ~IOException() throw();

  inline virtual void write(std::ostream& stream) const
  { _write(stream, "IOException"); }
};

/**
 * An exception indicating an end-of-file condition for a file stream or
 * a broken connection for a socket or pipe.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API EOFException : public IOException
{
 public:

  /** Construct a new EOFException with an optional cause message. */
  EOFException(String message = String::empty);

  inline void write(std::ostream& stream) const
  { _write(stream, "EOFException"); }
};

/**
 * An exception indicating that a blocking operation has timed out.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API TimeoutException : public IOException
{
 public:

  /** Construct a new TimeoutException witih ano optional cause message. */
  TimeoutException(String message = String::empty);

  inline void write(std::ostream& stream) const
  { _write(stream, "TimeoutException"); }
};

/**
 * An exception indicating a file or directory was not found.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API PathNotFoundException : public IOException
{
 public:

  /** Construct a new PathNotFoundException with an optional cause message. */
  PathNotFoundException(String message = String::empty);

  inline void write(std::ostream& stream) const
  { _write(stream, "PathNotFoundException"); }
};

} // namespace ccxx

#endif // __ccxx_IOException_hxx
