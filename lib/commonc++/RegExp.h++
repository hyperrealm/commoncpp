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

#ifndef __ccxx_RegExp_hxx
#define __ccxx_RegExp_hxx

#include <commonc++/Common.h++>
#include <commonc++/String.h++>

#include <sys/types.h>

namespace ccxx {

/**
 * A regular expression substring match. This class represents the portion
 * of a string that matched a subgroup in a regular expression.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API RegExpMatch
{
  friend class RegExp;

 public:

  /** Constructor. */
  RegExpMatch()
    : rm_so(-1), rm_eo(-1)
  { }

  /** Determine if this match is valid. */
  inline bool isValid() const
  { return(rm_so >= 0); }

  /** Get the start index of the substring. */
  inline uint_t getStartIndex() const
  { return(rm_so >= 0 ? rm_so : 0); }

  /** Get the end index of the substring. If both the start and end index
   * are 0, a non-match is indicated.
   */
  inline uint_t getEndIndex() const
  { return((rm_eo >= 0) && (rm_so >= 0) ? rm_eo : 0); }

 private:

  int rm_so;
  int rm_eo;
};

/**
 * A regular expression.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API RegExp
{
 public:

  /** Construct a new RegExp. */
  RegExp();

  /** Copy constructor. */
  RegExp(const RegExp& other);

  /** Destructor. */
  ~RegExp();

  /**
   * Set the regular expression pattern.
   *
   * @param pattern The pattern.
   * @param caseInsensitive A flag indicating whether pattern matches
   * using this pattern should be case-insensitive.
   * @throw ParseException If the pattern is invalid.
   */
  void setPattern(const char* pattern, bool caseInsensitive = false)
  { setPattern(String(pattern), caseInsensitive); }

  /**
   * Set the regular expression pattern.
   *
   * @param pattern The pattern.
   * @param caseInsensitive A flag indicating whether pattern matches
   * using this pattern should be case-insensitive.
   * @throw ParseException If the pattern is invalid.
   */
  void setPattern(const String& pattern, bool caseInsensitive = false);

  /** Get the regular expression pattern. */
  inline String getPattern() const
  { return(_pattern); }

  /** Get a String representation of the RegExp. Equivalent to getPattern(). */
  inline String toString() const
  { return(getPattern()); }

  /**
   * Perform a pattern match against some text.
   *
   * @param text The text.
   * @return <b>true</b> if the text matches the pattern, <b>false</b>
   * otherwise.
   */
  bool match(const String& text) const;

  /**
   * Perform a pattern match against some text, returning submatches.
   *
   * @param text The text.
   * @param matches The objects in which the submatches will be recorded.
   * @param numMatches The number of submatches.
   * @return <b>true</b> if the text matches the pattern, <b>false</b>
   * otherwise.
   */
  bool match(const String& text, RegExpMatch matches[], int numMatches) const;

  /** Assignment operator. */
  RegExp& operator=(const RegExp& other);

  /** Determine if this regular expression has a null pattern. */
  inline bool isNull() const
  { return(_pattern.isNull()); }

  /** Determine if this regular expression has a null pattern. */
  inline bool operator!() const
  { return(_pattern.isNull()); }

  /**
   * Escape all metacharacters in a String.
   *
   * @param text The String to escape.
   * @return A copy of the string with all metacharacters escaped.
   */
  static String escapeMeta(const String& text);

 private:

  String _pattern;
  void *_regex;
  bool _compiled;
};

} // namespace ccxx

#endif // __ccxx_RegExp_hxx
