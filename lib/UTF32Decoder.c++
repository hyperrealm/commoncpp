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

#include "commonc++/UTF32Decoder.h++"

namespace ccxx {

/*
 */

UTF32Decoder::UTF32Decoder()
{
}

/*
 */

UTF32Decoder::~UTF32Decoder()
{
}

/*
 */

int UTF32Decoder::decode(const char32_t** input, int* inputCountLeft,
                         char16_t** output, int* outputCountLeft)
{
  while(*inputCountLeft > 0)
  {
    int status = outputChar(**input, output, outputCountLeft);
    if(status != STATUS_OK)
      return(status);

    ++*input;
    --*inputCountLeft;
  }

  return(STATUS_OK);
}

/*
 */

void UTF32Decoder::reset()
{
}

/*
 */

// static
int UTF32Decoder::decodedLength(const char32_t* input, int length,
                                int maxLength /* = 0 */)
{
  if((input == NULL) || (length == 0))
    return(0);

  int count = 0;

  for(const char32_t* p = input; length > 0; ++p, --length)
  {
    int charLen = 0;

    if(*p < 0x10000)
      charLen = 1;
    else if(*p < 0x110000)
      charLen = 2;
    else
      return(STATUS_INVALID_INPUT);

    if((maxLength > 0) && ((count + charLen) > maxLength))
      break;

    count += charLen;
  }

  return(count);
}


} // namespace ccxx
