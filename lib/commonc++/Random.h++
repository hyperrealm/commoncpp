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

#ifndef __ccxx_Random_hxx
#define __ccxx_Random_hxx

#include <commonc++/Common.h++>

namespace ccxx {

/**
 * A random number generator.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Random
{
 public:

  /**
   * Construct a new random number generator with a default seed which
   * is based on a combination of the current thread ID, process ID,
   * and system time.
   */
  Random();

  /** Construct a new random number generator with the given seed. */
  Random(uint32_t seed);

  /** Destructor. */
  ~Random();

  /**
   * Generate a random integer.
   *
   * @param range The desired range of the value to be returned.
   * @return An integer in the range [0, <b>range</b>).
   */
  uint32_t nextInt(uint32_t range);

  /**
   * Generate a random double value.
   *
   * @return A double in the range [0.0, 1.0).
   */
  double nextDouble();

  /**
   * Fill a byte buffer with random bytes.
   *
   * @param buf The buffer to fill.
   * @param len The length of the buffer.
   * @return <b>true</b> on success, <b>false</b> otherwise.
   */
  bool getBytes(byte_t* buf, size_t len);

 private:

  uint32_t _state;

  CCXX_COPY_DECLS(Random);
};

} // namespace ccxx

#endif // __ccxx_Random_hxx
