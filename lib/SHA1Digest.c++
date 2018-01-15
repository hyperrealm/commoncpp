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

// Adapted from FPL-licensed code by Paul E. Jones <paulej@packetizer.com>

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#include "commonc++/SHA1Digest.h++"

namespace ccxx {

/*
 */

SHA1Digest::SHA1Digest()
{
  reset();
}

/*
 */

SHA1Digest::~SHA1Digest()
{
}

/*
 */

void SHA1Digest::reset()
{
  _length = 0;
  _offset = 0;

  _h[0] = 0x67452301;
  _h[1] = 0xEFCDAB89;
  _h[2] = 0x98BADCFE;
  _h[3] = 0x10325476;
  _h[4] = 0xC3D2E1F0;
}

/*
 */

void SHA1Digest::update(const byte_t* buf, size_t len)
{
  if(len == 0)
    return;

  while(len--)
  {
    _block[_offset++] = (*buf & 0xFF);

    _length += 8;
    if(_length == 0) // overflow?
      throw IOException("maximum message size exceeded");

    if(_offset == 64)
      processBlock();

    ++buf;
  }
}

/*
 */

void SHA1Digest::finish(byte_t* digest)
{
  padBlock();

  byte_t *p = digest;

  for(int i = 0; i < 5; i++)
  {
    *p++ = (_h[i] >> 24) & 0xFF;
    *p++ = (_h[i] >> 16) & 0xFF;
    *p++ = (_h[i] >>  8) & 0xFF;
    *p++ = (_h[i]      ) & 0xFF;
  }

  reset();
}

/*
 */

void SHA1Digest::processBlock()
{
  // Constants defined for SHA-1
  static const uint32_t k[] = { 0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC,
                                0xCA62C1D6 };
  int t;
  uint32_t tmp, w[80];

  // Initialize the first 16 words in the array W.
  for(t = 0; t < 16; ++t)
  {
    w[t] =  ((uint32_t)_block[t * 4    ]) << 24;
    w[t] |= ((uint32_t)_block[t * 4 + 1]) << 16;
    w[t] |= ((uint32_t)_block[t * 4 + 2]) <<  8;
    w[t] |= ((uint32_t)_block[t * 4 + 3]);
  }

  for(t = 16; t < 80; ++t)
    w[t] = rotateLeft(w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16], 1);

  uint32_t a = _h[0];
  uint32_t b = _h[1];
  uint32_t c = _h[2];
  uint32_t d = _h[3];
  uint32_t e = _h[4];

  for(t = 0; t < 20; ++t)
  {
    tmp = rotateLeft(a, 5) + ((b & c) | ((~b) & d)) + e + w[t] + k[0];
    tmp &= 0xFFFFFFFF;
    e = d;
    d = c;
    c = rotateLeft(b, 30);
    b = a;
    a = tmp;
  }

  for(t = 20; t < 40; ++t)
  {
    tmp = rotateLeft(a, 5) + (b ^ c ^ d) + e + w[t] + k[1];
    tmp &= 0xFFFFFFFF;
    e = d;
    d = c;
    c = rotateLeft(b, 30);
    b = a;
    a = tmp;
  }

  for(t = 40; t < 60; ++t)
  {
    tmp = (rotateLeft(a, 5) + ((b & c) | (b & d) | (c & d)) + e + w[t] + k[2]);
    tmp &= 0xFFFFFFFF;
    e = d;
    d = c;
    c = rotateLeft(b, 30);
    b = a;
    a = tmp;
  }

  for(t = 60; t < 80; ++t)
  {
    tmp = rotateLeft(a, 5) + (b ^ c ^ d) + e + w[t] + k[3];
    tmp &= 0xFFFFFFFF;
    e = d;
    d = c;
    c = rotateLeft(b, 30);
    b = a;
    a = tmp;
  }

  _h[0] = (_h[0] + a) & 0xFFFFFFFF;
  _h[1] = (_h[1] + b) & 0xFFFFFFFF;
  _h[2] = (_h[2] + c) & 0xFFFFFFFF;
  _h[3] = (_h[3] + d) & 0xFFFFFFFF;
  _h[4] = (_h[4] + e) & 0xFFFFFFFF;

  _offset = 0;
}

/*
 */

void SHA1Digest::padBlock()
{
  // Check if the current message block is too small to hold the
  // initial padding bits and length. If so, pad the block, process
  // it, and then continue padding into a second block.

  if(_offset > 55)
  {
    _block[_offset++] = 0x80;

    while(_offset < 64)
      _block[_offset++] = 0;

    processBlock();

    while(_offset < 56)
      _block[_offset++] = 0;
  }
  else
  {
    _block[_offset++] = 0x80;

    while(_offset < 56)
      _block[_offset++] = 0;
  }

  // Store the message length as the last 8 octets.
  _block[56] = (_length >> 56) & 0xFF;
  _block[57] = (_length >> 48) & 0xFF;
  _block[58] = (_length >> 40) & 0xFF;
  _block[59] = (_length >> 32) & 0xFF;
  _block[60] = (_length >> 24) & 0xFF;
  _block[61] = (_length >> 16) & 0xFF;
  _block[62] = (_length >>  8) & 0xFF;
  _block[63] = (_length      ) & 0xFF;

  processBlock();
}

/*
 */

uint32_t SHA1Digest::rotateLeft(uint32_t word, int bits)
{
  return((word << bits) & 0xFFFFFFFF) | ((word & 0xFFFFFFFF) >> (32 - bits));
}


} // namespace ccxx
