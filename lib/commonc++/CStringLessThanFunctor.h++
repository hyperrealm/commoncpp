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

#ifndef __ccxx_CStringLessThanFunctor_hxx
#define __ccxx_CStringLessThanFunctor_hxx

#include <commonc++/Common.h++>

#include <functional>

namespace ccxx {

/**
 * A "less than" comparator for C strings. May be used as a template
 * argument to <b>std::map</b> for maps which use C string keys.
 *
 * @author Mark Lindner
 */
class CStringLessThanFunctor
  : public std::binary_function<const char*, const char*, bool>
{
 public:

  /**
   * Construct a new CStringLessThanFunctor with the given case sensitivity.
   *
   * @param caseSensitive <b>true</b> if string comparisons should be
   * case-sensitive, <b>false</b> otherwise.
   */
  CStringLessThanFunctor(bool caseSensitive = true);

  /**
   * Compare two C strings.
   *
   * @return <b>true</b> if the strings are equal, <b>false</b> otherwise.
   */
  bool operator()(const char* a, const char* b) const;

 private:

  bool _caseSensitive;
};

} // namespace ccxx

#endif // __ccxx_CStringLessThanFunctor_hxx
