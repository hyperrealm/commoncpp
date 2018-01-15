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

#ifndef __ccxx_Exception_hxx
#define __ccxx_Exception_hxx

#include <commonc++/String.h++>

#include <exception>
#include <iostream>
#include <sstream>

namespace ccxx {

/**
 * A general-purpose exception.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Exception : public std::exception
{
 public:

  /** Construct a new Exception with an optional cause message. */
  Exception(String message = String::empty);

  /** Destructor. */
  virtual ~Exception() throw()
  { }

  /** Get the message associated with the exception. */
  inline String getMessage() const
  { return(_message); }

  /** Get the message associated with the exception. */
  virtual const char* what() const throw();

  /** Write a textual representation of the exception to a stream. */
  inline virtual void write(std::ostream& stream) const
  { _write(stream, "Exception"); }

  /** Get a textual representation of the exception as a String. */
  inline String toString() const
  {
    std::ostringstream ss;
    write(ss);
    return String(ss.str().c_str());
  }

 protected:

  /** @cond INTERNAL */
  void _write(std::ostream& stream, const char* name) const;
  /** @endcond */

  /** The cause message. */
  String _message;
  mutable CString _what;
};

/** Write an exception to a stream. */
inline std::ostream& operator<<(std::ostream& stream, const Exception& ex)
{
  ex.write(stream);
  return(stream);
}

} // namespace ccxx

#endif // __ccxx_Exception_hxx
