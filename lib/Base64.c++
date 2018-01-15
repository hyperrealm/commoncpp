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

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#include "commonc++/Base64.h++"

#include <cstring>
#include <cctype>

namespace ccxx {

static const char *__base64
  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int __tuplesPerLine = 19;
static int __bytesPerLine = __tuplesPerLine * 3;

/*
 */

size_t Base64::encode(ByteBuffer& input, CharBuffer& output)
{
  size_t outlen = output.getRemaining();

  size_t len = encode(input.getPointer(), input.getRemaining(),
                      output.getPointer(), outlen);

  if((len > 0) && (len <= outlen))
    output.bump(len);

  return(len);
}

/*
 */

size_t Base64::encode(const byte_t* input, size_t inputLen,
                      char* output, size_t outputLen)
{
  if(! input || ! inputLen)
    return(0);

  if(! output)
  {
    // no buffer supplied; return size required
    // which is 4/3 of the input size + 1 byte for each 19x3 bytes of input

    return((((inputLen + 2) / 3) * 4) + ((inputLen + __bytesPerLine - 1)
                                         / __bytesPerLine));
  }

  char *pos = output;
  size_t left = inputLen;
  uint32_t tuple = 0;
  int t = 0, tc = 0;
  size_t len = 0;

  for(const byte_t *p = input; left--; p++)
  {
    if(t > 0)
      tuple <<= 8;
    tuple |= (*p & 0xFF);

    if(++t == 3)
    {
      *(pos++) = encodeByte((tuple >> 18) & 0x3F);
      *(pos++) = encodeByte((tuple >> 12) & 0x3F);
      *(pos++) = encodeByte((tuple >>  6) & 0x3F);
      *(pos++) = encodeByte((tuple      ) & 0x3F);

      len += 4;
      t = 0;
      tuple = 0;

      if(++tc == __tuplesPerLine)
      {
        *(pos++) = '\n';
        len++;
        tc = 0;
      }
    }
  }

  if(t > 0)
  {
    tuple <<= 8;
    if(t == 1)
      tuple <<= 8;

    *(pos++) = encodeByte((tuple >> 18) & 0x3F);
    *(pos++) = encodeByte((tuple >> 12) & 0x3F);

    if(t == 2)
      *(pos++) = encodeByte((tuple >> 6) & 0x3F);

    *(pos++) = '=';
    if(t == 1)
      *(pos++) = '=';

    len += 4;
    tc++;
  }

  if(tc > 0)
  {
    *pos = '\n';
    len++;
  }

  return(len);
}

/*
 */

size_t Base64::decode(CharBuffer& input, ByteBuffer& output)
{
  size_t outlen = output.getRemaining();

  size_t len = decode(input.getPointer(), input.getRemaining(),
                      output.getPointer(), outlen);

  if((len > 0) && (len <= outlen))
    output.bump(len);

  return(len);
}

/*
 */

size_t Base64::decode(const char* input, size_t inputLen,
                      byte_t* output, size_t outputLen)
{
  if(! input)
    return(0);

  if(! output)
  {
    // no buffer supplied; return (worst-case) size required
    return(((inputLen + 3) / 4) * 3);
  }

  uint32_t tuple = 0;
  size_t len = 0;
  byte_t *pos = output;
  int t = 0, x = 0;

  for(const char *p = input; *p; p++)
  {
    if(isspace(*p))
      continue; // skip whitespace

    int32_t b = 0;

    if(*p == '=')
    {
      if(x < 2)
        x++;
      else
        return(0); // decoding error
    }
    else
    {
      b = decodeByte(*p);
      if(b < 0)
        return(0); // decoding error
    }

    if(t > 0)
      tuple <<= 6;
    tuple |= (b & 0x3F);

    if(++t == 4)
    {
      *(pos++) = ((tuple >> 16) & 0xFF);
      len++;

      if(x < 2)
      {
        *(pos++) = ((tuple >> 8) & 0xFF);
        len++;
      }

      if(x < 1)
      {
        *(pos++) = (tuple & 0xFF);
        len++;
      }

      t = 0;
      tuple = 0;
    }
  }

  if(t > 0)
    return(0); // truncated input

  return(len);
}

/*
 */

int32_t Base64::decodeByte(char c)
{
  const char *p = std::strchr(__base64, c);

  return(p ? (p - __base64) : -1);
}

/*
 */

char Base64::encodeByte(int32_t b)
{
  if(b < 0 || b >= 64)
    return(0);

  return(__base64[b]);
}


} // namespace ccxx
