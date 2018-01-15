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

#include "commonc++/ParseException.h++"

namespace ccxx {

/*
 */

ParseException::ParseException(String message /* = String::empty */,
                               int line /* = 0 */, int character /* = 0 */)
  : Exception(message),
    _line(line),
    _char(character)
{
}

/*
 */

void ParseException::write(std::ostream& stream) const
{
  stream << "ParseException";
  if(_line)
  {
    stream << " at " << _line;
    if(_char)
          stream << ':' << _char;
  }
  stream << ": " << _message;
}


} // namespace ccxx
