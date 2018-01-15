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

#include "commonc++/UTF8Encoder.h++"

#include <stdio.h> // remove

namespace ccxx {

// static
const int UTF8Encoder::STATUS_OK = 0;
const int UTF8Encoder::STATUS_NEED_MORE_INPUT = -1;
const int UTF8Encoder::STATUS_OUTPUT_BUFFER_FULL = -2;
const int UTF8Encoder::STATUS_INVALID_INPUT = -3;

UTF8Encoder::UTF8Encoder()
  : _surrogate(0)
{
}

/*
 */

UTF8Encoder::~UTF8Encoder()
{
}

/*
 */

int UTF8Encoder::encode(const char16_t** input, int* inputCountLeft,
                        char** output, int* outputCountLeft)
{
//  printf("encode: %d in, %d out\n", *inputCountLeft, *outputCountLeft);

  while(*inputCountLeft > 0)
  {
//    printf("char16: %d\n", **input);

    // UTF16 -> UTF32
    if((**input & 0xFC00) == 0xD800)
      _surrogate = **input;
    else
    {
      int32_t char32;

      if(_surrogate != 0)
      {
        uint32_t hi = _surrogate - 0xD800; // high 10 bits
        uint32_t lo = **input - 0xDC00; // low 10 bits
        char32 = ((hi << 10) | lo) + 0x10000;
        _surrogate = 0;
      }
      else
        char32 = **input;

      int status = outputChar(char32, output, outputCountLeft);
      if(status != STATUS_OK)
        return(status);
    }

    ++*input;
    --*inputCountLeft;
  }

  return((_surrogate != 0) ? STATUS_NEED_MORE_INPUT : STATUS_OK);
}

/*
 */

void UTF8Encoder::reset()
{
  _surrogate = 0;
}

/*
 */

int UTF8Encoder::outputChar(uint32_t char32, char** buf, int* left)
{
//  printf("outputChar %d, left= %d\n", char32, *left);
  int bits = 0;

  // UTF32 -> UTF8
  if(char32 < 0x80)
  {
    // 0xxxxxxx => 0xxxxxxx

    if(*left < 1)
      return(STATUS_OUTPUT_BUFFER_FULL);

    **buf = (static_cast<char>(char32 & 0x7F));
  }
  else if(char32 < 0x800)
  {
    // 00000yyy yyxxxxxx => 110yyyyy 10xxxxxx

    if(*left < 2)
      return(STATUS_OUTPUT_BUFFER_FULL);

    **buf = static_cast<char>(0xC0 & ((char32 >> 6) & 0x1F));
    bits = 6;
  }
  else if(char32 < 0x10000)
  {
    // zzzzyyyy yyxxxxxx => 1110zzzz 10yyyyyy 10xxxxxx

    if(*left < 3)
      return(STATUS_OUTPUT_BUFFER_FULL);

    **buf = static_cast<char>(0xE0 & ((char32 >> 12) & 0x0F));
    bits = 12;
  }
  else if(char32 < 0x110000)
  {
    // 000wwwzz zzzzyyyy yyxxxxxx => 11110www 10zzzzzz 10yyyyyy 10xxxxxx

    if(*left < 4)
      return(STATUS_OUTPUT_BUFFER_FULL);

    **buf = static_cast<char>(0xF0 & ((char32 >> 18) & 0x07));
    bits = 18;
  }
  else
  {
    // invalid character
    return(STATUS_INVALID_INPUT);
  }

  ++*buf;
  --*left;

  // if bits=6, take shifted at 0
  // if bits=12, take shifted at 6, 0
  // if bits=18, take shifted at 12, 6, 0

  if(bits > 0)
  {
    do
    {
      bits -= 6;
      **buf = static_cast<char>(0x80 & ((char32 >> bits) & 0x3F));
      ++*buf;
      --*left;
    }
    while(bits > 0);
  }

  return(STATUS_OK);
}

/*
 */

// static
int UTF8Encoder::encodedLength(const char16_t* input, int length,
                               int maxLength /* = 0 */)
{
  if((input == NULL) || (length == 0))
    return(0);

  int count = 0;
  bool expectLowSurrogate = false;

  // High surrogate: D800-DBFF (110110xxxxxxxxxx)
  // Low  surrogate: DC00-DFFF (110111xxxxxxxxxx)

  for(const char16_t *p = input; length > 0; ++p, --length)
  {
    int charLen = 0;

    if(*p < 0x80)
      charLen = 1;
    else if(*p < 0x800)
      charLen = 2;
    else
    {
      char16_t mask = (*p & 0xFC00);
      // high surrogate introduces code point >= 0xFFFF
      if(mask == 0xD800) // High surrogate
      {
        if(expectLowSurrogate)
          return(STATUS_INVALID_INPUT); // Decoding error.

        expectLowSurrogate = true;
        continue;
      }
      else if(mask == 0xDC00) // Low surrogate
      {
        if(!expectLowSurrogate)
          return(STATUS_INVALID_INPUT); // Decoding error.

        expectLowSurrogate = false;
        charLen = 4;
      }
    }

    if((maxLength > 0) && ((count + charLen) > maxLength))
      break;

    count += charLen;
  }

  if((length == 0) && expectLowSurrogate)
    return(STATUS_NEED_MORE_INPUT);

  return(count);
}


} // namespace ccxx
