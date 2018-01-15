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

#ifndef __ccxx_DataFormatException_hxx
#define __ccxx_DataFormatException_hxx

#include <commonc++/Common.h++>
#include <commonc++/IOException.h++>

namespace ccxx {

/**
 * An exception indicating a data format error.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API DataFormatException : public IOException
{
 public:

  /** Construct a new DataFormatException with an optional cause message. */
  DataFormatException(String message = String::empty);

  inline void write(std::ostream& stream) const
  { _write(stream, "DataFormatException"); }
};

} // namespace ccxx

#endif // __ccxx_DataFormatException_hxx
