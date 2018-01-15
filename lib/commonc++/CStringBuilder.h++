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

#ifndef __ccxx_CStringBuilder_hxx
#define __ccxx_CStringBuilder_hxx

#include <commonc++/Common.h++>
#include <commonc++/CString.h++>

namespace ccxx {

/**
 * A utility class for constructing C-style strings. CStringBuilder
 * is a simple wrapper around a raw character buffer and provides
 * some basic append operations which write strings and integer
 * values into the buffer, but which will never write past the end of the
 * buffer. When append operations have been completed, a call to the
 * terminate() method should be made to write the final NUL terminator
 * character, for which the final byte in the buffer is always reserved.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API CStringBuilder
{
 public:

  /**
   * Construct a new CStringBuilder for the specified character buffer.
   *
   * @param buf The buffer.
   * @param size The size of the buffer.
   */
  CStringBuilder(char* buf, size_t size);

  /** Destructor. */
  ~CStringBuilder();

  /**
   * Clear the buffer. This method resets the write position to
   * the beginning of the buffer.
   */
  CStringBuilder& clear();

  /**
   * Append a C string to the buffer.
   *
   * @param s The string to append.
   * @param length The number of bytes to append, or 0 to append the
   * entire string. If the buffer cannot accommodate the requested number
   * of characters, the method appends as many characters as possible.
   */
  CStringBuilder& append(const char* s, size_t length = 0);

  /**
   * Append a CString to the buffer.
   *
   * @param s The string to append.
   * @param length The number of bytes to append, or 0 to append the
   * entire string. If the buffer cannot accommodate the requested number
   * of characters, the method appends as many characters as possible.
   */
  CStringBuilder& append(const CString& s, size_t length = 0);

  /** Append a character to the buffer. */
  CStringBuilder& append(char c);

  /**
   * Append the string representation of a signed integer to the buffer.
   * If the buffer cannot accommodate the data, the request is ignored.
   *
   * @param val The value to append.
   * @param width The formatting width for the value.
   * @param zeroPad A flag indicating whether the value should be padded
   * on the right with zeroes (<b>true</b>) or spaces (<b>false</b>).
   **/
  CStringBuilder& append(int val, size_t width, bool zeroPad = true);

  /**
   * Append the string representation of an unsigned integer to the
   * buffer. If the buffer cannot accommodate the data, the request
   * is ignored.
   *
   * @param val The value to append.
   * @param width The formatting width for the value.
   * @param zeroPad A flag indicating whether the value should be padded
   * on the right with zeroes (<b>true</b>) or spaces (<b>false</b>).
   */
  CStringBuilder& append(uint_t val, size_t width, bool zeroPad = true);

  /** Get a pointer to the current write position for the buffer. */
  inline char* getPosition() const
  { return(_pos); }

  /**
   * Get the remaining capacity of the buffer, that is, the number of
   * characters that can still be written.
   */
  inline size_t getRemaining() const
  { return(_left); }

  /** Determine if the buffer has space remaining for additional characters. */
  inline bool hasRemaining() const
  { return(_left > 0); }

  /**
   * Get the current length of the buffer, that is, the number of
   * characters currently in the buffer.
   */
  inline size_t getLength() const
  { return(_pos - _buf); }

  /**
   * Get the current length of the buffer, that is, the number of
   * characters currently in the buffer.
   */
  inline size_t length() const
  { return(getLength()); }

  /** Get a pointer to the beginning of the buffer.  */
  inline char *c_str() const
  { return(_buf); }

  /**
   * Bump the write position by the given number of characters.
   *
   * @param delta The number of characters by which to bump the write
   * position. If the delta would move the write position past the end
   * of the buffer, the request is ignored.
   */
  bool bump(size_t delta);

  /** NUL-terminate the buffer. */
  void terminate();

  /** Append operator. */
  inline CStringBuilder& operator+=(const char *s)
  { return(append(s)); }

  /** Append operator. */
  inline CStringBuilder& operator+=(char c)
  { return(append(c)); }

 private:

  CCXX_COPY_DECLS(CStringBuilder);

  template<typename T> char* _formatValue(T value, size_t width, char padChar,
                                          char* buf, size_t bufsz);

  char *_buf;
  size_t _length;
  char *_pos;
  size_t _left;
};

} // namespace ccxx

#endif // __ccxx_CStringBuilder_hxx
