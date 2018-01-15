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

#include "commonc++/UTFDecoder.h++"

namespace ccxx {

// static
const int UTFDecoder::STATUS_OK = 0;
const int UTFDecoder::STATUS_NEED_MORE_INPUT = -1;
const int UTFDecoder::STATUS_OUTPUT_BUFFER_FULL = -2;
const int UTFDecoder::STATUS_INVALID_INPUT = -3;

/*
 */

UTFDecoder::UTFDecoder()
{
}

/*
 */

UTFDecoder::~UTFDecoder()
{
}

/*
 */

int UTFDecoder::outputChar(char32_t char32, char16_t** buf, int* left)
{
  if(char32 < 0x10000)
  {
    if(*left < 1)
      return(STATUS_OUTPUT_BUFFER_FULL);

    **buf = static_cast<char16_t>(char32 & 0xFFFF);
    ++*buf;
    --*left;
  }
  else if(char32 < 0x110000)
  {
    // surrogate pair
    if(*left < 2)
      return(STATUS_OUTPUT_BUFFER_FULL);

    char32 -= 0x10000;
    char16_t lo = static_cast<char16_t>(0xDC00 + (char32 & 0x3FF));
    char32 >>= 10;
    **buf = static_cast<char16_t>(0xD800 + (char32 & 0x3FF));
    ++*buf;
    --*left;

    **buf = lo;
    ++*buf;
    --*left;
  }
  else
    return(STATUS_INVALID_INPUT);

  return(STATUS_OK);
}


} // namespace ccxx
