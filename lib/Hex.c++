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

#include "commonc++/Hex.h++"
#include "commonc++/Char.h++"

#include <cstring>
#include <cctype>

namespace ccxx {

/*
 */

char Hex::encodeDigit(uint_t i, bool lowercase /* = false */)
{
  if(i > 15)
    return('0');
  else
    return(i < 10 ? i + '0' : i + (lowercase ? 'a' : 'A') - 10);
}

/*
 */

uint_t Hex::decodeDigit(char c)
{
  if((c >= '0') && (c <= '9'))
    return(c - '0');
  else if((c >= 'A') && (c <= 'F'))
    return(c - 'A' + 10);
  else if((c >= 'a') && (c <= 'f'))
    return(c - 'a' + 10);
  else
    return(0);
}

/*
 */

uint_t Hex::decodeDigit(char16_t c)
{
  if(!Char::isLowASCII(c))
    return(0);

  return(decodeDigit(static_cast<char>(c)));
}

/*
 */

size_t Hex::encode(ByteBuffer& input, CharBuffer& output,
                   bool lowercase /* = false */)
{
  size_t outlen = output.getRemaining();

  size_t len = encode(input.getPointer(), input.getRemaining(),
                      output.getPointer(), outlen, lowercase);

  if((len > 0) && (len <= outlen))
    output.bump(len);

  return(len);
}

/*
 */

size_t Hex::encode(const byte_t* input, size_t inputLen, char* output,
                   size_t outputLen, bool lowercase /* = false */)
{
  if(! input || ! inputLen)
    return(0);

  size_t r = inputLen * 2;

  if((output == NULL) || (outputLen < (r)))
    return(r);

  size_t left = inputLen;
  char *q = output;

  for(const byte_t *p = input; left--; p++)
  {
    uint_t lo = (*p & 0x0F);
    uint_t hi = ((*p >> 4) & 0x0F);

    *(q++) = encodeDigit(hi, lowercase);
    *(q++) = encodeDigit(lo, lowercase);
  }

  return(r);
}

/*
 */

String Hex::encode(const Blob& input, bool lowercase /* = false */)
{
  String s;

  size_t length = input.getLength();

  if(length > 0)
  {
    s.reserve(length * 2);

    size_t i = 0;
    for(const byte_t *p = input.getData(); i < length; ++p, ++i)
    {
      int lo = (*p & 0x0F);
      int hi = ((*p >> 4) & 0x0F);

      s << encodeDigit(hi, lowercase) << encodeDigit(lo, lowercase);
    }
  }
  else
    s = String::nullRep;

  return(s);
}

/*
 */

size_t Hex::decode(CharBuffer& input, ByteBuffer& output)
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

size_t Hex::decode(const char* input, size_t inputLen, byte_t* output,
                   size_t outputLen)
{
  if(! input)
    return(0);

  if((inputLen % 2) != 0)
    return(0);

  size_t r = inputLen / 2;

  if(outputLen < r)
    return(r);

  size_t left = inputLen;
  byte_t *q = output;
  for(const char *p = input; left > 0; left -= 2)
  {
    if(! isxdigit(*p))
      return(0);

    uint_t x = decodeDigit(*p) & 0x0F;
    ++p;
    x <<= 4;

    if(! isxdigit(*p))
      return(0);

    x |= decodeDigit(*p) & 0x0F;
    ++p;

    *(q++) = static_cast<byte_t>(x);
  }

  return(r);
}

/*
 */

Blob Hex::decode(const String& input)
{
  Blob blob;

  size_t inputLen = input.getLength();

  if((inputLen % 2) != 0)
    return(Blob::null);

  size_t outputLen = inputLen / 2;

  if(outputLen > 0)
  {
    blob.setLength(outputLen);
    byte_t *q = blob.getData();

    const char16_t* data = input.data();
    int left = inputLen;

    for(const char16_t* p = data; left > 0; left -= 2)
    {
      if(!Char::isDigit(*p))
        return(Blob::null);

      uint_t x = decodeDigit(*p) & 0x0F;
      ++p;
      x <<= 4;

      if(!Char::isDigit(*p))
        return(Blob::null);

      x |= decodeDigit(*p) & 0x0F;
      ++p;

      *(q++) = static_cast<byte_t>(x);
    }
  }

  return(blob);
}


} // namespace ccxx
