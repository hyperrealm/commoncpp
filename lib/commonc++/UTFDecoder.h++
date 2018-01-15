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

#ifndef __ccxx_UTFDecoder_hxx
#define __ccxx_UTFDecoder_hxx

#include <commonc++/Common.h++>

namespace ccxx {

/**
 * A base class for transcoders that decode strings to UTF-16 from a different
 * UTF encoding.
 */
class COMMONCPP_API UTFDecoder
{
 public:

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

  /**
   * Resets the decoder to an initial state.
   */
  virtual void reset() = 0;

  /**
   * Destructor.
   */
  virtual ~UTFDecoder();

 protected:

  /** Constructor. */
  UTFDecoder();

  /**
   * Outputs a UTF-32 character as a single UTF-16 character or surrogate
   * pair.
   *
   * @param char32 The character to output.
   * @param buf The output buffer.
   * @param length The remaining number of elements in the output buffer.
   * @return A status code.
   */
  int outputChar(char32_t char32, char16_t** buf, int* length);

 private:

  CCXX_COPY_DECLS(UTFDecoder);
};

} // namespace ccxx

#endif // __ccxx_UTFDecoder_hxx
