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

#ifndef __ccxx_Hash_hxx
#define __ccxx_Hash_hxx

#include <commonc++/Common.h++>

namespace ccxx {

/**
 * A collection of hash functions.
 *
 * @author Mark Lindner
 */

class COMMONCPP_API Hash
{
 public:

  /**
   * Calculate a hash using an algorithm proposed by Donald E Knuth
   * in <i>The Art Of Computer Programming Volume 3</i>, under the topic of
   * sorting and search in Chapter 6.4.
   *
   * @param data A pointer to the data.
   * @param length The length of the data.
   * @return A 32-bit hash of the data.
   */
  static uint32_t DEKHash(const byte_t* data, size_t length);

  /**
   * Calculate a hash using an algorithm produced by Professor Daniel
   * J Bernstein and shown first to the world on the USENET newsgroup
   * <i>comp.lang.c</i>. It is one of the most efficient hash
   * functions ever published.
   *
   * @param data A pointer to the data.
   * @param length The length of the data.
   * @return A 32-bit hash of the data.
   */
  static uint32_t DJBHash(const byte_t* data, size_t length);

  /**
   * Calculate a hash using an algorithm that is based on work by
   * Peter J Weinberger of AT&T Bell Labs, but is optimized for
   * 32-bit processors. It's the hash function widely used on most
   * UNIX systems.
   *
   * @param data A pointer to the data.
   * @param length The length of the data.
   * @return A 32-bit hash of the data.
   */
  static uint32_t ELFHash(const byte_t* data, size_t length);

  /**
   * Calculate a hash using an algorithm that is used in the open
   * source SDBM project.
   *
   * @param data A pointer to the data.
   * @param length The length of the data.
   * @return A 32-bit hash of the data.
   */
  static uint32_t SDBMHash(const byte_t* data, size_t length);

 private:

  Hash();

  CCXX_COPY_DECLS(Hash);
};

} // namespace ccxx

#endif // __ccxx_Hash_hxx
