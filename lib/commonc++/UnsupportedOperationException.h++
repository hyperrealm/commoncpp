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

#ifndef __ccxx_UnsupportedOperationException_hxx
#define __ccxx_UnsupportedOperationException_hxx

#include <commonc++/Common.h++>
#include <commonc++/Exception.h++>

namespace ccxx {

/**
 * An exception indicating that an attempted operation is not supported.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API UnsupportedOperationException : public Exception
{
 public:

  /**
   * Construct a new UnsupportedOperationException with an optional cause
   * message.
   */
  UnsupportedOperationException(String message = String::empty);

  inline virtual void write(std::ostream& stream) const
  { _write(stream, "UnsupportedOperationException"); }
};

} // namespace ccxx

#endif // __ccxx_UnsupportedOperationException_hxx
