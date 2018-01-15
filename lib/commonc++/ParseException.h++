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

#ifndef __ccxx_ParseException_hxx
#define __ccxx_ParseException_hxx

#include <commonc++/Common.h++>
#include <commonc++/Exception.h++>

namespace ccxx {

/**
 * An exception indicating a parsing error.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API ParseException : public Exception
{
 public:

  /**
   * Construct a new ParseException with the given cause message.
   *
   * @param message The cause message.
   * @param line The line number of the error.
   * @param character The character position (within the line) of the error.
   */
  ParseException(String message, int line = 0, int character = 0);

  /** Get the line number of the error. */
  inline int getLine() const
  { return(_line); }

  /** Get the character position (within the line) of the error. */
  inline int getCharacter() const
  { return(_char); }

  void write(std::ostream& stream) const;

 private:

  int _line;
  int _char;
};

} // namespace ccxx

#endif // __ccxx_ParseException_hxx
