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

/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 */

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#include "commonc++/MD5Digest.h++"

#include <cstring>

#include "commonc++/ByteOrder.h++"

namespace ccxx {

// The four core functions - F1 is optimized somewhat

// #define F1(x, y, z) (x & y | ~x & z)
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

// This is the central step in the MD5 algorithm.
#define MD5STEP(f, w, x, y, z, data, s)                         \
  (w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x)

/*
 */

MD5Digest::MD5Digest()
{
  reset();
}

/*
 */

MD5Digest::~MD5Digest()
{
}

/*
 */

void MD5Digest::reset()
{
  _buf[0] = 0x67452301;
  _buf[1] = 0xefcdab89;
  _buf[2] = 0x98badcfe;
  _buf[3] = 0x10325476;

  _bits[0] = 0;
  _bits[1] = 0;

  std::memset(_in, 0, sizeof(_in));
}

/*
 */

void MD5Digest::update(const byte_t* buf, size_t len)
{
  uint32_t t;

  // Update bitcount

  t = _bits[0];
  if((_bits[0] = t + (static_cast<uint32_t>(len) << 3)) < t)
    ++_bits[1]; // Carry from low to high
  _bits[1] += (len >> 29);

  t = (t >> 3) & 0x3F; // Bytes already in shsInfo->data

  // Handle any leading odd-sized chunks

  if(t)
  {
    byte_t* p = reinterpret_cast<byte_t*>(_in) + t;

    t = 64 - t;
    if(len < t)
    {
      std::memcpy(p, buf, len);
      return;
    }
    std::memcpy(p, buf, t);
    if(ByteOrder::isBigEndian())
      _byteReverse(_in, 16);
    transform(_buf, _in);
    buf += t;
    len -= t;
  }

  // Process data in 64-byte chunks

  while(len >= 64)
  {
    std::memcpy(_in, buf, sizeof(_in));
    if(ByteOrder::isBigEndian())
      _byteReverse(_in, 16);
    transform(_buf, _in);
    buf += 64;
    len -= 64;
  }

  // Handle any remaining bytes of data.

  std::memcpy(_in, buf, len);
}

/*
 */

void MD5Digest::finish(byte_t* digest)
{
  unsigned int count;
  byte_t* p;

  // Compute number of bytes mod 64
  count = (_bits[0] >> 3) & 0x3F;

  // Set the first char of padding to 0x80.  This is safe since there is
  // always at least one byte free

  p = reinterpret_cast<byte_t*>(_in) + count;
  *p++ = 0x80;

  // Bytes of padding needed to make 64 bytes
  count = 64 - 1 - count;

  // Pad out to 56 mod 64
  if(count < 8)
  {
    // Two lots of padding:  Pad the first block to 64 bytes
    std::memset(p, 0, count);
    if(ByteOrder::isBigEndian())
      _byteReverse(_in, 16);
    transform(_buf, _in);

    // Now fill the next block with 56 bytes
    std::memset(_in, 0, 56);
  }
  else
  {
    // Pad block to 56 bytes
    std::memset(p, 0, count - 8);
  }

  if(ByteOrder::isBigEndian())
    _byteReverse(_in, 14);

  // Append length in bits and transform
  _in[14] = _bits[0];
  _in[15] = _bits[1];

  transform(_buf, _in);
  if(ByteOrder::isBigEndian())
    _byteReverse(_buf, 4);
  std::memcpy(digest, _buf, 16);

  reset();
}

/*
 */

void MD5Digest::_byteReverse(uint32_t* buf, int count)
{
  for(; --count; ++buf)
    ByteOrder::reverseBytes(*buf);
}

/*
 */

void MD5Digest::transform(uint32_t buf[4], uint32_t const in[16])
{
  register uint32_t a, b, c, d;

  a = buf[0];
  b = buf[1];
  c = buf[2];
  d = buf[3];

  MD5STEP (F1, a, b, c, d, in[0]  + 0xd76aa478, 7);
  MD5STEP (F1, d, a, b, c, in[1]  + 0xe8c7b756, 12);
  MD5STEP (F1, c, d, a, b, in[2]  + 0x242070db, 17);
  MD5STEP (F1, b, c, d, a, in[3]  + 0xc1bdceee, 22);
  MD5STEP (F1, a, b, c, d, in[4]  + 0xf57c0faf, 7);
  MD5STEP (F1, d, a, b, c, in[5]  + 0x4787c62a, 12);
  MD5STEP (F1, c, d, a, b, in[6]  + 0xa8304613, 17);
  MD5STEP (F1, b, c, d, a, in[7]  + 0xfd469501, 22);
  MD5STEP (F1, a, b, c, d, in[8]  + 0x698098d8, 7);
  MD5STEP (F1, d, a, b, c, in[9]  + 0x8b44f7af, 12);
  MD5STEP (F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
  MD5STEP (F1, b, c, d, a, in[11] + 0x895cd7be, 22);
  MD5STEP (F1, a, b, c, d, in[12] + 0x6b901122, 7);
  MD5STEP (F1, d, a, b, c, in[13] + 0xfd987193, 12);
  MD5STEP (F1, c, d, a, b, in[14] + 0xa679438e, 17);
  MD5STEP (F1, b, c, d, a, in[15] + 0x49b40821, 22);

  MD5STEP (F2, a, b, c, d, in[1]  + 0xf61e2562, 5);
  MD5STEP (F2, d, a, b, c, in[6]  + 0xc040b340, 9);
  MD5STEP (F2, c, d, a, b, in[11] + 0x265e5a51, 14);
  MD5STEP (F2, b, c, d, a, in[0]  + 0xe9b6c7aa, 20);
  MD5STEP (F2, a, b, c, d, in[5]  + 0xd62f105d, 5);
  MD5STEP (F2, d, a, b, c, in[10] + 0x02441453, 9);
  MD5STEP (F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
  MD5STEP (F2, b, c, d, a, in[4]  + 0xe7d3fbc8, 20);
  MD5STEP (F2, a, b, c, d, in[9]  + 0x21e1cde6, 5);
  MD5STEP (F2, d, a, b, c, in[14] + 0xc33707d6, 9);
  MD5STEP (F2, c, d, a, b, in[3]  + 0xf4d50d87, 14);
  MD5STEP (F2, b, c, d, a, in[8]  + 0x455a14ed, 20);
  MD5STEP (F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
  MD5STEP (F2, d, a, b, c, in[2]  + 0xfcefa3f8, 9);
  MD5STEP (F2, c, d, a, b, in[7]  + 0x676f02d9, 14);
  MD5STEP (F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

  MD5STEP (F3, a, b, c, d, in[5]  + 0xfffa3942, 4);
  MD5STEP (F3, d, a, b, c, in[8]  + 0x8771f681, 11);
  MD5STEP (F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
  MD5STEP (F3, b, c, d, a, in[14] + 0xfde5380c, 23);
  MD5STEP (F3, a, b, c, d, in[1]  + 0xa4beea44, 4);
  MD5STEP (F3, d, a, b, c, in[4]  + 0x4bdecfa9, 11);
  MD5STEP (F3, c, d, a, b, in[7]  + 0xf6bb4b60, 16);
  MD5STEP (F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
  MD5STEP (F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
  MD5STEP (F3, d, a, b, c, in[0]  + 0xeaa127fa, 11);
  MD5STEP (F3, c, d, a, b, in[3]  + 0xd4ef3085, 16);
  MD5STEP (F3, b, c, d, a, in[6]  + 0x04881d05, 23);
  MD5STEP (F3, a, b, c, d, in[9]  + 0xd9d4d039, 4);
  MD5STEP (F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
  MD5STEP (F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
  MD5STEP (F3, b, c, d, a, in[2]  + 0xc4ac5665, 23);

  MD5STEP (F4, a, b, c, d, in[0]  + 0xf4292244, 6);
  MD5STEP (F4, d, a, b, c, in[7]  + 0x432aff97, 10);
  MD5STEP (F4, c, d, a, b, in[14] + 0xab9423a7, 15);
  MD5STEP (F4, b, c, d, a, in[5]  + 0xfc93a039, 21);
  MD5STEP (F4, a, b, c, d, in[12] + 0x655b59c3, 6);
  MD5STEP (F4, d, a, b, c, in[3]  + 0x8f0ccc92, 10);
  MD5STEP (F4, c, d, a, b, in[10] + 0xffeff47d, 15);
  MD5STEP (F4, b, c, d, a, in[1]  + 0x85845dd1, 21);
  MD5STEP (F4, a, b, c, d, in[8]  + 0x6fa87e4f, 6);
  MD5STEP (F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
  MD5STEP (F4, c, d, a, b, in[6]  + 0xa3014314, 15);
  MD5STEP (F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
  MD5STEP (F4, a, b, c, d, in[4]  + 0xf7537e82, 6);
  MD5STEP (F4, d, a, b, c, in[11] + 0xbd3af235, 10);
  MD5STEP (F4, c, d, a, b, in[2]  + 0x2ad7d2bb, 15);
  MD5STEP (F4, b, c, d, a, in[9]  + 0xeb86d391, 21);

  buf[0] += a;
  buf[1] += b;
  buf[2] += c;
  buf[3] += d;
}


} // namespace ccxx
