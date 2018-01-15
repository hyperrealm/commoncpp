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

#include "commonc++/UTF8Decoder.h++"

#include <cstring>

namespace ccxx {

/*
 */

UTF8Decoder::UTF8Decoder()
  : _bytesExpected(0)
  , _char32(0)
  , _pendingChar(false)
  , _stopDecodingAtNulChar(false)
{
}

/*
 */

UTF8Decoder::~UTF8Decoder()
{
}

/*
 */

int UTF8Decoder::decode(const char** input, int* inputCountLeft,
                        char16_t** output, int* outputCountLeft)
{
  if(_pendingChar)
  {
    int status = outputChar(_char32, output, outputCountLeft);
    if(status != STATUS_OK)
      return(status);

    _pendingChar = false;
    bool nulChar = (_char32 == 0);
    _char32 = 0;

    if(_stopDecodingAtNulChar && nulChar)
      return(STATUS_OK);
  }

  while(*inputCountLeft > 0)
  {
    char c = **input;
    ++*input;
    --*inputCountLeft;

    if(_bytesExpected > 0)
    {
      if((c & 0xC0) != 0x80)
        return(STATUS_INVALID_INPUT);

      _char32 <<= 6;
      _char32 |= (c & 0x3F);

      if(--_bytesExpected == 0)
      {
        _pendingChar = true;
        int status = outputChar(_char32, output, outputCountLeft);
        if(status != STATUS_OK)
          return(status);

        _pendingChar = false;
        bool nulChar = (_char32 == 0);
        _char32 = 0;

        if(_stopDecodingAtNulChar && nulChar)
          return(STATUS_OK);
      }
    }
    else
    {
      if((c & 0x80) == 0)
      {
        _char32 = c;
        _pendingChar = true;
        int status = outputChar(_char32, output, outputCountLeft);
        if(status != STATUS_OK)
          return(status);

        _pendingChar = false;
        bool nulChar = (_char32 == 0);
        _char32 = 0;

        if(_stopDecodingAtNulChar && nulChar)
          return(STATUS_OK);
      }
      else if((c & 0xE0) == 0xC0)
      {
        _bytesExpected = 1;
        _char32 = (c & 0x1F);
      }
      else if((c & 0xF0) == 0xE0)
      {
        _bytesExpected = 2;
        _char32 = (c & 0x0F);
      }
      else if((c & 0xF8) == 0xF0)
      {
        _bytesExpected = 3;
        _char32 = (c & 0x07);
      }
      else
        return(STATUS_INVALID_INPUT);
    }
  }

  return((_bytesExpected > 0) ? STATUS_NEED_MORE_INPUT : STATUS_OK);
}

/*
 */

void UTF8Decoder::reset()
{
  _bytesExpected = 0;
  _char32 = 0;
  _pendingChar = false;
}

/*
 */

// static
int UTF8Decoder::decodedLength(const char* input, int length,
                               int maxLength /* = 0 */)
{
  if((input == NULL) || (length == 0))
    return(0);

  int count = 0;
  int bytesExpected = 0;

  for(const char *p = input; length > 0; ++p, --length)
  {
    if(bytesExpected > 0)
    {
      if((*p & 0xC0) != 0x80)
        return(STATUS_INVALID_INPUT);

      if(--bytesExpected == 0)
      {
        if((maxLength > 0) && (count == maxLength))
          break;

        ++count;
      }
    }
    else
    {
      if((*p & 0x80) == 0)
      {
        if((maxLength > 0) && (count == maxLength))
          break;

        ++count;
      }
      else if((*p & 0xE0) == 0xC0)
        bytesExpected = 1;
      else if((*p & 0xF0) == 0xE0)
        bytesExpected = 2;
      else if((*p & 0xF8) == 0xF0)
        bytesExpected = 3;
      else if((*p & 0xFC) == 0xF8)
        bytesExpected = 4;
      else
        return(STATUS_INVALID_INPUT);
    }
  }

  if(bytesExpected > 0)
    return(STATUS_INVALID_INPUT);

  return(count);
}


} // namespace ccxx
