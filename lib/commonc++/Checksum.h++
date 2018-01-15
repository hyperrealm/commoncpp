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

#ifndef __ccxx_Checksum_hxx
#define __ccxx_Checksum_hxx

#include <commonc++/Common.h++>

namespace ccxx {

/**
 * A base class for checksum generators.
 *
 * @author Mark Lindner
 */
class Checksum
{
 public:

  /** Destructor. */
  virtual ~Checksum();

  /**
   * Update the checksum with more data.
   *
   * @param buf A pointer to the data.
   * @param len The length of the data.
   */
  virtual void update(const byte_t* buf, size_t len) = 0;

  /**
   * Finish generating the checksum.
   *
   * @return The checksum.
   */
  uint32_t finish();

  /** Reset the object for a new checksum. */
  void reset();

 protected:

  /** Constructor. */
  Checksum();

  /** The current checksum value. */
  uint32_t _sum;

 private:

  CCXX_COPY_DECLS(Checksum);
};

} // namespace ccxx

#endif // __ccxx_Checksum_hxx
