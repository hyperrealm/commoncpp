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

#include "commonc++/CStringBuilder.h++"

#include <algorithm>
#include <cstring>

namespace ccxx {

/*
 */

CStringBuilder::CStringBuilder(char *buf, size_t size)
  : _buf(buf),
    _length(size),
    _pos(buf),
    _left(size - 1)
{
}

/*
 */

CStringBuilder::~CStringBuilder()
{
}

/*
 */

CStringBuilder & CStringBuilder::clear()
{
  _pos = _buf;
  _left = _length - 1;
  return(*this);
}

/*
 */

CStringBuilder & CStringBuilder::append(const char *s, size_t length /* = 0 */)
{
  // append at most length chars, or all chars if length == 0

  if(s && _left)
  {
    const char *p = s;

    if(! length)
    {
      while(*p && _left)
      {
        *_pos = *p;
        ++p;
        ++_pos;
        --_left;
      }
    }
    else
    {
      while(*p && _left && length)
      {
        *_pos = *p;
        ++p;
        ++_pos;
        --_left;
        --length;
      }
    }
  }

  return(*this);
}

/*
 */

CStringBuilder& CStringBuilder::append(const CString &s,
                                       size_t length /* = 0 */)
{
  return(append(s.data(), length));
}

/*
 */

CStringBuilder& CStringBuilder::append(char c)
{
  if((_left >= 1) && c)
  {
    *_pos = c;
    ++_pos;
    --_left;
  }

  return(*this);
}

/*
 */

CStringBuilder& CStringBuilder::append(int val, size_t width,
                                       bool zeroPad /* = true */)
{
  char buf[32];
  char *p = _formatValue(val, width, (zeroPad ? '0' : ' '), buf, sizeof(buf));

  return(append(p));
}

/*
 */

CStringBuilder& CStringBuilder::append(uint_t val, size_t width,
                                       bool zeroPad /* = true */)
{
  char buf[32];
  char *p = _formatValue(val, width, (zeroPad ? '0' : ' '), buf, sizeof(buf));

  return(append(p));
}

/*
 */

bool CStringBuilder::bump(size_t delta)
{
  if(delta < _left)
  {
    _left -= delta;
    _pos += delta;
    return(true);
  }
  else
    return(false);
}

/*
 */

void CStringBuilder::terminate()
{
  *_pos = NUL;
}

/*
 */

template <typename T> char * CStringBuilder::_formatValue(
  T value, size_t width, char padChar, char *buf, size_t bufsz)
{
  bool neg = false;
  char *p = buf + bufsz;

  if(value < 0)
  {
    neg = true;
    value = -value;
  }

  *(--p) = 0;

  do
  {
    *(--p) = '0' + (value % 10);
    value /= 10;
  }
  while(value > 0);

  if(padChar)
  {
    char *first = buf + bufsz - width - (neg ? 1 : 0);
    if(first >= buf)
    {
      while(p >= first)
        *(--p) = padChar;
    }
  }

  if(neg)
    *(--p) = '-';

  return(p);
}


} // namespace ccxx
