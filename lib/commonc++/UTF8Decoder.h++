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

#ifndef __ccxx_UTF8Decoder_hxx
#define __ccxx_UTF8Decoder_hxx

#include <commonc++/UTFDecoder.h++>

namespace ccxx {

/**
 * A UTF-8 to UTF-16 string transcoder.
 * <p>
 * Since the String class provides facilities for converting text to/from
 * UTF-8, this class will generally not need to be used directly.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API UTF8Decoder : public UTFDecoder
{
 public:

  /** Construct a new UTF8Decoder. */
  UTF8Decoder();

  /** Destructor. */
  ~UTF8Decoder();

  /**
   * Transcode UTF-8 data to UTF-16.
   *
   * @param input A pointer to a pointer to the input buffer containing the
   * UTF-8 data. On return, the pointer will be incremented by the number of
   * bytes consumed from <i>input</i>.
   * @param inputCountLeft A pointer to the number of characters remaining to
   * be decoded from <i>input</i>. On return, the value will be decremented by
   * the number of characters consumed from <i>input</i>.
   * @param output A pointer to a pointer to the output buffer where the
   * UTF-16 data will be written. On return, the pointer will be
   * incremented by the number of UTF-16 code points written to <i>output</i>.
   * @param outputCountLeft A pointer to the number of elements remaining in
   * <i>output</i>. On return, the value will be decremented by the number
   * of elements that were written to <i>output</i>.
   * @return One of the status constants defined in UTFDecoder.
   */
  int decode(const char** input, int* inputCountLeft,
             char16_t** output, int* outputCountLeft);

  /**
   * Specifies whether decoding should stop as soon as a NUL character is
   * encountered in the input. By default, decoding continues until the entire
   * input buffer has been consumed. By default this setting is off.
   */
  void setStopDecodingAtNulChar(bool stopDecodingAtNulChar)
  { _stopDecodingAtNulChar = stopDecodingAtNulChar; }

  void reset();

  /**
   * Calculate the transcoded length of the UTF-16 data, without actually
   * transcoding the string.
   *
   * @param buf A pointer to the buffer containing the UTF-8 data.
   * @param length The length of the buffer.
   * @param maxLength If non-zero, indicates the maximum decoded length, in
   * UTF-16 characters.
   * @return On success, the decoded length of the UTF-8 string, as a count of
   * UTF-16 code points (including surrogates), not counting the NUL
   * terminator. On failure, one of the status constants defined in UTFDecoder.
   */
  static int decodedLength(const char* buf, int length, int maxLength = 0);

 private:

  int _bytesExpected;
  uint32_t _char32;
  bool _pendingChar;
  bool _stopDecodingAtNulChar;

  CCXX_COPY_DECLS(UTF8Decoder);
};

} // namespace ccxx

#endif // __ccxx_UTF8Decoder_hxx
