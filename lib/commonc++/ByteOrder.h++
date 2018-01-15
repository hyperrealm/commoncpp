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

#ifndef __ccxx_ByteOrder_hxx
#define __ccxx_ByteOrder_hxx

#include <commonc++/Common.h++>

#ifdef CCXX_OS_POSIX
#include <netinet/in.h>
#endif

namespace ccxx {

/**
 * Byte-order conversion routines.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API ByteOrder
{
 public:

  /** Convert an unsigned 16-bit integer from host to network byte-order. */
  inline static uint16_t hostToNetwork(uint16_t n)
  { return(htons(n)); }

  /** Convert an unsigned 32-bit integer from host to network byte-order. */
  inline static uint32_t hostToNetwork(uint32_t n)
  { return(htonl(n)); }

  /** Convert an unsigned 64-bit integer from host to network byte-order. */
  static uint64_t hostToNetwork(uint64_t n);

  /** Convert an unsigned 16-bit integer from network to host byte-order. */
  inline static uint16_t networkToHost(uint16_t n)
  { return(ntohs(n)); }

  /** Convert an unsigned 32-bit integer from network to host byte-order. */
  inline static uint32_t networkToHost(uint32_t n)
  { return(ntohl(n)); }

  /** Convert an unsigned 64-bit integer from network to host byte-order. */
  static uint64_t networkToHost(uint64_t n);

  /**
   * Determine if the host system is big-endian.
   *
   * @return <b>true</b> if the host system is big-endian, <b>false</b>
   * otherwise.
   */
  static bool isBigEndian();

  /**
   * Determine if the host system is little-endian.
   *
   * @return <b>true</b> if the host system is little-endian, <b>false</b>
   * otherwise.
   */
  inline static bool isLittleEndian()
  { return(!isBigEndian()); }

  /**
   * Get the endianness of the host system.
   *
   * @return The endianness.
   */
  inline static Endianness getEndianness()
  { return(isBigEndian() ? BigEndian : LittleEndian); }

  /**
   * Reverse the bytes in a value. This method may be used to toggle the
   * endianness of an arbitrary value.
   *
   * @param value The value.
   * @return The value.
   */
  template<typename T> static T& reverseBytes(T& value)
  {
    _reverseBytes(reinterpret_cast<byte_t*>(&value), sizeof(T));
    return(value);
  }

 private:

  static void _reverseBytes(byte_t* buf, size_t length);

  ByteOrder(); // not supported
  CCXX_COPY_DECLS(ByteOrder);
};

} // namespace ccxx

#endif // __ccxx_ByteOrder_hxx
