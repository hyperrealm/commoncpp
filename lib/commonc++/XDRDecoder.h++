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

#ifndef __ccxx_XDRDecoder_hxx
#define __ccxx_XDRDecoder_hxx

#include <commonc++/DataReader.h++>

namespace ccxx {

class Variant;

/**
 * A decoder for XDR, the eXternal Data Representation format. XDR is
 * a scheme for encoding primitive datatypes in a
 * platform-independent manner. For more information, see <a
 * href="http://www.faqs.org/rfcs/rfc1832.html" target="_top">RFC 1832</a>.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API XDRDecoder
{
 public:

  /**
   * Construct a new XDRDecoder that will decode values from the given
   * DataReader.
   *
   * @param reader The DataReader.
   */
  XDRDecoder(DataReader& reader);

  /** Destructor. */
  ~XDRDecoder();

  /**
   * Decode a boolean value.
   *
   * @param v The value to decode into.
   * @return The XDRDecoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDRDecoder& decode(bool& v)
  { int vv; decode(vv); v = static_cast<bool>(vv); return(*this); }

  /**
   * Decode a byte value.
   *
   * @param v The value to decode into.
   * @return The XDRDecoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDRDecoder& decode(byte_t& v)
  { uint32_t vv; decode(vv); v = static_cast<byte_t>(vv); return(*this); }

  /**
   * Decode a char value.
   *
   * @param v The value to decode into.
   * @return The XDRDecoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDRDecoder& decode(char& v)
  { uint32_t vv; decode(vv); v = static_cast<bool>(vv); return(*this); }

  /**
   * Decode a signed 16-bit integer value.
   *
   * @param v The value to decode into.
   * @return The XDRDecoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDRDecoder& decode(int16_t& v)
  { uint32_t vv; decode(vv); v = static_cast<int16_t>(vv); return(*this); }

  /**
   * Decode an unsigned 16-bit integer value.
   *
   * @param v The value to decode into.
   * @return The XDRDecoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDRDecoder& decode(uint16_t& v)
  { uint32_t vv; decode(vv); v = static_cast<uint16_t>(vv); return(*this); }

  /**
   * Decode a signed 32-bit integer value.
   *
   * @param v The value to decode into.
   * @return The XDRDecoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDRDecoder& decode(int32_t& v)
  { _reader >> v; return(*this); }

  /**
   * Decode an unsigned 32-bit integer value.
   *
   * @param v The value to decode into.
   * @return The XDRDecoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDRDecoder& decode(uint32_t& v)
  { _reader >> v; return(*this); }

  /**
   * Decode a signed 64-bit integer value.
   *
   * @param v The value to decode into.
   * @return The XDRDecoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDRDecoder& decode(int64_t& v)
  { _reader >> v; return(*this); }

  /**
   * Decode an unsigned 64-bit integer value.
   *
   * @param v The value to decode into.
   * @return The XDRDecoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDRDecoder& decode(uint64_t& v)
  { _reader >> v; return(*this); }

  /**
   * Decode a floating point value.
   *
   * @param v The value to decode into.
   * @return The XDRDecoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDRDecoder& decode(float& v)
  { _reader >> v; return(*this); }

  /**
   * Decode a double precision floating point value.
   *
   * @param v The value to decode into.
   * @return The XDRDecoder.
   * @throw IOException If an I/O error occcurs.
   */
  inline XDRDecoder& decode(double& v)
  { _reader >> v; return(*this); }

  /**
   * Decode a string.
   *
   * @param s The character array to decode into.
   * @param maxlen The maximum length of the string to decode. If the
   * encoded string is longer than this length, an IOException will be
   * thrown.
   * @return The XDRDecoder.
   * @throw IOException If an I/O error occcurs.
   */
  XDRDecoder& decode(char* s, size_t maxlen = 0);

  /**
   * Decode a string.
   *
   * @param s The String to decode into.
   * @param maxlen The maximum length of the string to decode. If the
   * encoded string is longer than this length, an IOException will be
   * thrown.
   * @return The XDRDecoder.
   * @throw IOException If an I/O error occcurs.
   */
  XDRDecoder& decode(String& s, size_t maxlen = 0);

  /**
   * Decode a block of opaque data.
   *
   * @param v The byte array to decode into.
   * @param len The number of bytes to decode.
   * @return The XDRDecoder.
   * @throw IOException If an I/O error occcurs.
   */
  XDRDecoder& decode(byte_t* v, size_t len);

  /**
   * Decode an arbitrary value into a Variant.
   *
   * @param v The Variant to decode into.
   * @return The XDRDecoder.
   * @throw IOException If an I/O error occcurs.
   */
  XDRDecoder& decode(Variant& v);

 private:

  DataReader& _reader;
};

} // namespace ccxx

#endif // __ccxx_XDRDecoder_hxx
