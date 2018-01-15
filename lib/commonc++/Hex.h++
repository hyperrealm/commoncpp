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

#ifndef __ccxx_Hex_hxx
#define __ccxx_Hex_hxx

#include <commonc++/Common.h++>
#include <commonc++/Blob.h++>
#include <commonc++/Buffer.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * Routines for ASCII hex encoding and decoding. %Hex encoding consists of
 * encoding each byte as two ASCII characters that represent its value in
 * hexadecimal; for example the value 0x3B would be encoded as the characters
 * "3B".
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Hex
{
 public:

  /**
   * Hex-encode a buffer of data. If the output buffer is too small, no
   * encoding is done, and the required size of the output buffer is
   * returned.
   *
   * @param input The input buffer.
   * @param output The output buffer.
   * @param lowercase A flag indicating whether lowercase characters should
   * be used for hexadecimal digits ('a' - 'f').
   * @return The number of characters written to the output buffer. If the
   * value returned is greater than the size of the output buffer, it
   * indicates that the output buffer is too small.
   */
  static size_t encode(ByteBuffer& input, CharBuffer& output,
                       bool lowercase = false);

  /**
   * Hex-encode an array of bytes. If the output array is too small, no
   * decoding is done, and the required length of the output array is
   * returned.
   *
   * @param input The input array.
   * @param inputLen The length of the input array.
   * @param output The output array.
   * @param outputLen The length of the output array.
   * @param lowercase A flag indicating whether lowercase characters should
   * be used for hexadecimal digits ('a' - 'f').
   * @return The number of bytes written to the output array. If the value
   * returned is greater than <b>outputLen</b>, it indicates that the
   * output array is too small.
   */
  static size_t encode(const byte_t* input, size_t inputLen, char* output,
                       size_t outputLen, bool lowercase = false);

  /**
   * Hex-encode a Blob, returning the result as a String.
   * @param lowercase A flag indicating whether lowercase characters should
   * be used for hexadecimal digits ('a' - 'f').
   *
   * @param input The Blob to encode.
   * @return The hex encoding of the Blob, as a String.
   */
  static String encode(const Blob& input, bool lowercase = false);

  /**
   * Hex-decode a buffer of data. If the output buffer is too small, no
   * decoding is done, and the required size of the output buffer is
   * returned.
   *
   * @param input The input buffer.
   * @param output The output buffer.
   * @return The number of bytes written to the output buffer. If the value
   * returned is greater than the size of the output buffer, it indicates
   * that the output buffer is too small.
   */
  static size_t decode(CharBuffer& input, ByteBuffer& output);

  /**
   * Hex-decode an array of bytes. If the output array is too small, no
   * decoding is done, and the required length of the output array is
   * returned.
   *
   * @param input The input array.
   * @param inputLen The length of the input array.
   * @param output The output array.
   * @param outputLen The length of the output array.
   * @return The number of bytes written to the output array. If the value
   * returned is greater than <b>outputLen</b>, it indicates that the
   * output array is too small.
   */
  static size_t decode(const char* input, size_t inputLen, byte_t* output,
                       size_t outputLen);

  /**
   * Hex-decode a String, returning the result as a Blob.
   *
   * @param input The String to decode.
   * @return The hex decoding of the String, as a Blob, or
   * <code>Blob::null</code> if the input is invalid.
   */
  static Blob decode(const String& input);

  /**
   * Encode an integer value (which should be in the range 0 - 15) to a
   * hexadecimal digit.
   *
   * @param i The value to encode.
   * @param lowercase A flag indicating whether the hex digits A - F should
   * be encoded in upper- or lower-case.
   * @return The hex digit, as a character. If <b>i</b> is out of range,
   * '0' is returned.
   */
  static char encodeDigit(uint_t i, bool lowercase = false);

  /**
   * Decode a hexadecimal digit (one of the characters '0' - '9',
   * 'A' - 'F', or 'a' - 'f') to an integer value in the range 0 - 15.
   *
   * @param c The hex digit to decode.
   * @return The integer value. If <b>c</b> is not a valid hexadecimal
   * digit, 0 is returned.
   */
  static uint_t decodeDigit(char c);

  /**
   * Decode a hexadecimal digit (one of the characters '0' - '9',
   * 'A' - 'F', or 'a' - 'f') to an integer value in the range 0 - 15.
   *
   * @param c The hex digit to decode.
   * @return The integer value. If <b>c</b> is not a valid hexadecimal
   * digit, 0 is returned.
   */
  static uint_t decodeDigit(char16_t c);

 private:

  Hex(); // not supported
  CCXX_COPY_DECLS(Hex);
};

} // namespace ccxx

#endif // __ccxx_Hex_hxx
