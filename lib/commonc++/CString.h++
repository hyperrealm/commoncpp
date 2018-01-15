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

#ifndef __ccxx_CString_hxx
#define __ccxx_CString_hxx

#include <commonc++/Common.h++>

namespace ccxx {

class AtomicCounter;

/**
 * An implicitly shared, reference-counted container for an immutable,
 * NUL-terminated C string. Instances of this class may be efficiently passed
 * by value.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API CString
{
 public:

  /** Construct a new, null CString. */
  CString();

  /**
   * Construct a new CString that will take ownership of the given C string.
   */
  explicit CString(const char* str);

  /** Copy constructor. */
  CString(const CString& other);

  /** Destructor. */
  virtual ~CString();

  /** Get a pointer to the character array. */
  inline const char* data() const
  { return(_str); }

  /** Get a pointer to the character array, reinterpreted as a byte array. */
  inline const byte_t* bytes() const
  { return(reinterpret_cast<const byte_t*>(_str)); }

  /** Get the length of the string. */
  size_t length() const;

  /** Cast operator. */
  inline operator const char*() const
  { return(_str); }

  /** Assignment operator. */
  CString& operator=(const CString& other);

  /** Test if the string is <b>NULL</b>. */
  inline bool isNull() const
  { return(_str == NULL); }

  /**
   * %Array index operator. Returns the character at the given offset in the
   * string.
   *
   * @param index The index.
   * @return The character at the specified index, as a Char.
   * @throw OutOfBoundsException If <i>index</I> is out of range.
   */
  char operator[](int index) const;

 private:

  void _release();

  const char* _str;
  AtomicCounter* _refs;
};

} // namespace ccxx

#endif // __ccxx_CString_hxx
