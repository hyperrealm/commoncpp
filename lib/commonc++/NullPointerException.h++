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

#ifndef __ccxx_NullPointerException_hxx
#define __ccxx_NullPointerException_hxx

#include <commonc++/Common.h++>

#include <exception>

namespace ccxx {

/**
 * An exception indicating an attempt to dereference a null pointer.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API NullPointerException : public std::exception
{
 public:

  /** Construct a new NullPointerException. */
  NullPointerException();

  inline const char* what() const throw()
  { return("null pointer dereferenced"); }

};

} // namespace ccxx

#endif // __ccxx_NullPointerException_hxx
