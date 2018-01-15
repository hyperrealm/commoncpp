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

#ifndef __ccxx_XDREncoder_hxx
#define __ccxx_XDREncoder_hxx

#include <commonc++/DataWriter.h++>

namespace ccxx {

class Variant;

/**
 * An encoder for XDR, the eXternal Data Representation format. XDR is
 * a scheme for encoding primitive datatypes in a
 * platform-independent manner. For more information, see <a
 * href="http://www.faqs.org/rfcs/rfc1832.html" target="_top">RFC 1832</a>.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API XDREncoder
{
 public:

  /**
   * Construct a new XDREncoder that will encode values to the given
   * DataWriter.
   *
   * @param writer The DataWriter.
   */
  XDREncoder(DataWriter& writer);

  /** Destructor. */
  ~XDREncoder();

  /**
   * Encode a boolean value.
   *
   * @param v The value to encode.
   * @return The XDREncoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDREncoder& encode(bool v)
  { return(encode(static_cast<uint32_t>(v))); }

  /**
   * Encode a byte value.
   *
   * @param v The value to encode.
   * @return The XDREncoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDREncoder& encode(byte_t v)
  { return(encode(static_cast<uint32_t>(v))); }

  /**
   * Encode a char value.
   *
   * @param v The value to encode.
   * @return The XDREncoder.
   * @throw IOException If an I/O error occcurs.
   */
  XDREncoder& encode(char v)
  { return(encode(static_cast<int32_t>(v))); }

  /**
   * Encode a signed 16-bit integer value.
   *
   * @param v The value to encode.
   * @return The XDREncoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDREncoder& encode(int16_t v)
  { return(encode(static_cast<int32_t>(v))); }

  /**
   * Encode an unsigned 16-bit integer value.
   *
   * @param v The value to encode.
   * @return The XDREncoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDREncoder& encode(uint16_t v)
  { return(encode(static_cast<uint32_t>(v))); }

  /**
   * Encode a signed 32-bit integer value.
   *
   * @param v The value to encode.
   * @return The XDREncoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDREncoder& encode(int32_t v)
  { _writer << v; return(*this); }

  /**
   * Encode an unsigned 32-bit integer value.
   *
   * @param v The value to encode.
   * @return The XDREncoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDREncoder& encode(uint32_t v)
  { _writer << v; return(*this); }

  /**
   * Encode a signed 64-bit integer value.
   *
   * @param v The value to encode.
   * @return The XDREncoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDREncoder& encode(const int64_t& v)
  { _writer << v; return(*this); }

  /**
   * Encode an unsigned 64-bit integer value.
   *
   * @param v The value to encode.
   * @return The XDREncoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDREncoder& encode(const uint64_t& v)
  { _writer << v; return(*this); }

  /**
   * Encode a floating point value.
   *
   * @param v The value to encode.
   * @return The XDREncoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDREncoder& encode(float v)
  { _writer << v; return(*this); }

  /**
   * Encode a double-precision floating point value.
   *
   * @param v The value to encode.
   * @return The XDREncoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDREncoder& encode(const double& v)
  { _writer << v; return(*this); }

  /**
   * Encode a string.
   *
   * @param s The character array to encode.
   * @param maxlen The maximum number of characters to encode.
   * @return The XDREncoder.
   * @throw IOException If an I/O error occcurs.
   */
  XDREncoder& encode(const char* s, size_t maxlen = 0);

  /**
   * Encode a string.
   *
   * @param s The String to encode.
   * @param maxlen The maximum number of characters to encode.
   * @return The XDREncoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDREncoder& encode(const String& s, size_t maxlen = 0);

  /**
   * Encode a block of opaque data.
   *
   * @param v The byte array to encode.
   * @param len The number of bytes to encode.
   * @return The XDREncoder.
   * @throw IOException If an I/O error occcurs.
   */
  XDREncoder& encode(const byte_t* v, size_t len);

  /**
   * Encode an arbitrary Variant value.
   *
   * @param v The Variant to encode.
   * @return The XDREncoder.
   * @throw IOException If an I/O error occcurs.
   */
  XDREncoder& encode(const Variant& v);

 private:

  DataWriter& _writer;
};

} // namespace ccxx

#endif // __ccxx_XDREncoder_hxx
