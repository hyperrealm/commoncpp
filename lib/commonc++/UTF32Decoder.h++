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

#ifndef __ccxx_UTF32Decoder_hxx
#define __ccxx_UTF32Decoder_hxx

#include <commonc++/UTFDecoder.h++>

namespace ccxx {

/**
 * A UTF-32 to UTF-16 string transcoder.
 * <p>
 * Since the String class provides facilities for converting text to/from
 * UTF-32, this class will generally not need to be used directly.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API UTF32Decoder : public UTFDecoder
{
 public:

  /** Construct a new UTF32Decoder. */
  UTF32Decoder();

  /** Destructor. */
  ~UTF32Decoder();

  /**
   * Transcode UTF-32 data to UTF-16.
   *
   * @param input A pointer to a pointer to the input buffer containing the
   * UTF-32 data. On return, the pointer will be incremented by the number of
   * UTF-32 characters consumed from <i>input</i>.
   * @param inputCountLeft A pointer to the number of UTF-32 characters
   * remaining to be decoded from <i>input</i>. On return, the value will be
   * decremented by the number of UTF-32 characters consumed from <i>input</i>.
   * @param output A pointer to a pointer to the output buffer where the
   * UTF-16 data will be written. On return, the pointer will be
   * incremented by the number of UTF-16 code points written to <i>output</i>.
   * @param outputCountLeft A pointer to the number of elements remaining in
   * <i>output</i>. On return, the value will be decremented by the number
   * of elements that were written to <i>output</i>.
   * @return One of the status constants defined in UTFDecoder.
   */
  int decode(const char32_t** input, int* inputCountLeft,
             char16_t** output, int* outputCountLeft);

  void reset();

  /**
   * Calculate the transcoded length of the UTF-16 data, without actually
   * transcoding the string.
   *
   * @param buf A pointer to the buffer containing the UTF-32 data.
   * @param length The length of the buffer.
   * @param maxLength If non-zero, indicates the maximum decoded length, in
   * UTF-16 characters.
   * @return On success, the decoded length of the UTF-32 string, as a count
   * of UTF-16 code points (including surrogates), not counting the NUL
   * terminator. On failure, one of the status constants defined in UTFDecoder.
   */
  static int decodedLength(const char32_t* buf, int length, int maxLength = 0);

 private:

  CCXX_COPY_DECLS(UTF32Decoder);
};

} // namespace ccxx

#endif // __ccxx_UTF32Decoder_hxx
