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

#ifndef __ccxx_Base64_hxx
#define __ccxx_Base64_hxx

#include <commonc++/Common.h++>
#include <commonc++/Buffer.h++>

namespace ccxx {

/**
 * Base-64 encoding and decoding routines.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Base64
{
 public:

  /**
   * Base64-encode the contents of a buffer.
   *
   * @param input The buffer containing the input data.
   * @param output The output buffer.
   * @return The number of characters written to the output buffer,
   * or 0 if an encoding error occurred. If there was not enough
   * room in the output buffer, the value returned will indicate the
   * amount of space needed.
   */
  static size_t encode(ByteBuffer& input, CharBuffer& output);

  /**
   * Base64-encode a block of bytes.
   *
   * @param input The input buffer.
   * @param inputLen The length of the input buffer.
   * @param output The output buffer.
   * @param outputLen The length of the output buffer.
   * @return The number of characters written to the output buffer, or 0
   * if an encoding error occurred. If the output buffer was too small,
   * the value returned will be greater than outputLen, and will indicate
   * the size required.
   */
  static size_t encode(const byte_t* input, size_t inputLen,
                       char* output, size_t outputLen);

  /**
   * Base64-decode the contents of a buffer.
   *
   * @param input The buffer containing the input data.
   * @param output The output buffer. If NULL, the function returns the
   * amount of space needed.
   * @return The number of characters written to the output buffer,
   * or 0 if a decoding error occurred. If there was not enough
   * room in the output buffer, the value returned will indicate the
   * amount of space needed.
   */
  static size_t decode(CharBuffer& input, ByteBuffer& output);

  /**
   * Base64-decode a block of characters.
   *
   * @param input The input buffer.
   * @param inputLen The length of the input buffer.
   * @param output The output buffer.
   * @param outputLen The length of the output buffer. If NULL, the function
   * returns the amount of space needed.
   * @return The number of characters written to the output buffer, or 0
   * if a decoding error occurred. If the output buffer was too small,
   * the value returned will be greater than outputLen, and will indicate
   * the size required.
   */
  static size_t decode(const char* input, size_t inputLen,
                       byte_t* output, size_t outputLen);

 private:

  static int32_t decodeByte(char c);
  static char encodeByte(int32_t b);

  Base64(); // not supported
  CCXX_COPY_DECLS(Base64);
};

} // namespace ccxx

#endif // __ccxx_Base64_hxx
