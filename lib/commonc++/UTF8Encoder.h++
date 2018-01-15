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

#ifndef __ccxx_UTF8Encoder_hxx
#define __ccxx_UTF8Encoder_hxx

#include <commonc++/Common.h++>

namespace ccxx {

/**
 * A UTF-16 to UTF-8 string transcoder.
 * <p>
 * Since the String class provides facilities for converting text to/from
 * UTF-8, this class will generally not need to be used directly.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API UTF8Encoder
{
 public:

  /** Construct a new UTF8Encoder. */
  UTF8Encoder();

  /** Destructor. */
  ~UTF8Encoder();

  /**
   * Transcode UTF-16 data ot UTF-8.
   *
   * @param input A pointer to a pointer to the input buffer containing the
   * UTF-16 data. On return, the pointer will be incremented by the number of
   * UTF-16 code points consumed from <i>input</i>.
   * @param inputCountLeft A pointer to the number of UTF-16 code points
   * remaining to be decoded from <i>input</i>. On return, the value will be
   * decremented by the number of UTF-16 code point consumed from <i>input</i>.
   * @param output A pointer to a pointer to the output buffer where the
   * UTF-8 data will be written. On return, the pointer will be
   * incremented by the number of characters written to <i>output</i>.
   * @param outputCountLeft A pointer to the number of characters remaining in
   * <i>output</i>. On return, the value will be decremented by the number
   * of characters that were written to <i>output</i>.
   * @return One of the status constants defined in UTFDecoder.
   */
  int encode(const char16_t** input, int* inputCountLeft,
             char** output, int* outputCountLeft);

  void reset();

  /**
   * Calculate the transcoded length of the UTF-8 data, without actually
   * transcoding the string.
   *
   * @return The decoded length of the UTF-8 string, in bytes, or -1 if the
   * input is invalid.
   */
  static int encodedLength(const char16_t* input, int length,
                           int maxLength = 0);

  /** A status indicating that all input has been successfully transcoded. */
  static const int STATUS_OK;

  /**
   * A status indicating that more input must be supplied to complete the
   * transcoding.
   */
  static const int STATUS_NEED_MORE_INPUT;

  /**
   * A status indicating that there is not enough room in the output buffer
   * to finish transcoding the input buffer.
   */
  static const int STATUS_OUTPUT_BUFFER_FULL;

  /**
   * A status indicating that transcoding cannot continue because invalid data
   * was encountered in the input buffer.
   */
  static const int STATUS_INVALID_INPUT;

 private:

  int outputChar(uint32_t char32, char** buf, int* length);

  char16_t _surrogate;

  CCXX_COPY_DECLS(UTF8Encoder);
};

} // namespace ccxx

#endif // __ccxx_UTF8Encoder_hxx
