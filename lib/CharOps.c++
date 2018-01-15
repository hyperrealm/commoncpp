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

#include "commonc++/CharOps.h++"

#include <cstring>
#include <cstdlib>

#include "commonc++/Char.h++"

namespace ccxx {

/*
 */

// static
const uint_t CharOps::END = ~0;

// static
const char16_t CharOps::TRUE_REP[] = { char16_t('t'), char16_t('r'),
                                       char16_t('u'), char16_t('e'), 0 };

// static
const char16_t CharOps::FALSE_REP[] = { char16_t('f'), char16_t('a'),
                                        char16_t('l'), char16_t('s'),
                                        char16_t('e'), 0 };

// static
const char16_t CharOps::DIGITS[] = { char16_t('0'), char16_t('1'),
                                     char16_t('2'), char16_t('3'),
                                     char16_t('4'), char16_t('5'),
                                     char16_t('6'), char16_t('7'),
                                     char16_t('8'), char16_t('9'),
                                     char16_t('A'), char16_t('B'),
                                     char16_t('C'), char16_t('D'),
                                     char16_t('E'), char16_t('F'),
                                     char16_t('G'), char16_t('H'),
                                     char16_t('I'), char16_t('J'),
                                     char16_t('K'), char16_t('L'),
                                     char16_t('M'), char16_t('N'),
                                     char16_t('O'), char16_t('P'),
                                     char16_t('Q'), char16_t('R'),
                                     char16_t('S'), char16_t('T'),
                                     char16_t('U'), char16_t('V'),
                                     char16_t('W'), char16_t('X'),
                                     char16_t('Y'), char16_t('Z'), 0 };

/*
 */

// static
int CharOps::compare(const char16_t* a, const char16_t* b,
                     bool ignoreCase /* = false */)
{
  const char16_t* p = a, *q = b;

  while(*p && *q)
  {
    if(ignoreCase)
    {
      int diff = Char::toLowerCase(*p) - Char::toLowerCase(*q);
      if(diff != 0)
        return(diff);
    }
    else
    {
      if(*p != *q)
        return(*p - *q);
    }

    ++p;
    ++q;
  }

  return(*p - *q);
}

/*
 */

// static
int CharOps::compare(const char16_t a, const char16_t b,
                     bool ignoreCase /* = false */)
{
  if(ignoreCase)
    return(Char::toLowerCase(a) - Char::toLowerCase(b));
  else
    return(a - b);
}

/*
 */

// static

int CharOps::compare(const char16_t* a, const char16_t* b, uint_t n,
                     bool ignoreCase /* = false */)
{
  const char16_t* p = a, *q = b;

  for(uint_t i = 0; i < n; ++i, ++p, ++q)
  {
    int diff = *p - *q;
    if(diff != 0)
      return(diff);
  }

  return(0);
}

/*
 */

// static
char16_t* CharOps::move(char16_t* dest, char16_t* src, uint_t n)
{
  std::memmove(reinterpret_cast<void*>(dest), reinterpret_cast<void*>(src),
               n * sizeof(char16_t));

  return(dest);
}

/*
 */

// static
const char16_t* CharOps::find(const char16_t* str, char16_t c,
                              uint_t len /* = 0 */)
{
  const char16_t* p = str;

  for(uint_t i = 0; *p && ((len == 0) || (i < len)); ++i, ++p)
  {
    if(*p == c)
      return(p);
  }

  return(NULL);
}

/*
 */

// static
char16_t* CharOps::find(char16_t* str, char16_t c, uint_t len /* = 0 */)
{
  return(const_cast<char16_t*>(find(const_cast<const char16_t*>(str), c,
                                    len)));
}

/*
 */

// static
const char16_t* CharOps::find(const char16_t* str, const char16_t* s)
{
  uint_t slen = length(s);
  const char16_t* last = str + length(str) - slen;

  for(const char16_t* p = str; p <= last; ++p)
  {
    if(compare(p, s, slen) == 0)
      return(p);
  }

  return(NULL);
}

/*
 */

// static
char16_t* CharOps::find(char16_t* str, const char16_t* s)
{
  return(const_cast<char16_t*>(find(const_cast<const char16_t*>(str), s)));
}

/*
 */

// static
const char16_t* CharOps::findLast(const char16_t* str, char16_t c,
                                  uint_t fromIndex /* = END */)
{
  const char16_t* start = str + ((fromIndex == END) ? length(str) - 1
                                 : fromIndex);

  for(const char16_t* p = start; p >= str; --p)
  {
    if(*p == c)
      return(p);
  }

  return(NULL);
}

/*
 */

// static
char16_t* CharOps::findLast(char16_t* str, char16_t c,
                            uint_t fromIndex /* = END */)
{
  return(const_cast<char16_t*>(findLast(const_cast<const char16_t*>(str), c,
                                        fromIndex)));
}

/*
 */

// static
const char16_t* CharOps::findLast(const char16_t* str, const char16_t* s,
                                  uint_t fromIndex /* = END */)
{
  const char16_t* start = str + ((fromIndex == END) ? length(str) - 1
                                 : fromIndex) - length(s);
  start -= length(s);

  for(const char16_t* p = start; p >= str; --p)
  {
    if(compare(p, s) == 0)
      return(p);
  }

  return(NULL);
}

/*
 */

// static
char16_t* CharOps::findLast(char16_t* str, const char16_t* s,
                               uint_t fromIndex /* = END */)
{
  return(const_cast<char16_t*>(findLast(const_cast<const char16_t*>(str), s,
                                        fromIndex)));
}

/*
 */

// static
uint_t CharOps::length(const char16_t* str)
{
  uint_t len = 0;

  for(const char16_t* p = str; *p; ++p, ++len);

  return(len);
}

/*
 */

// static
char16_t* CharOps::copy(char16_t* dest, const char16_t* src, uint_t n)
{
  char16_t* p = dest;
  const char16_t* q = src;

  for(uint_t i = 0; *q && (i < n); ++p, ++q, ++i)
    *p = *q;

  return(dest);
}

/*
 */

// static
char16_t* CharOps::copyRaw(char16_t* dest, const char16_t* src, uint_t n)
{
  std::memcpy(reinterpret_cast<void*>(dest),
              reinterpret_cast<const void*>(src),
              n * sizeof(char16_t));

  return(dest);
}

/*
 */

// static
int32_t CharOps::toLong(const char16_t* str, int base /* = 10 */)
{
  char buf[64];

  return(::strtol(_toASCII(str, buf, sizeof(buf)), NULL, base));
}

/*
 */

// static
uint32_t CharOps::toULong(const char16_t* str, int base /* = 10 */)
{
  char buf[64];

  return(::strtoul(_toASCII(str, buf, sizeof(buf)), NULL, base));
}

/*
 */

// static
int64_t CharOps::toLongLong(const char16_t* str, int base /* = 10 */)
{
  char buf[64];

  return(::strtoll(_toASCII(str, buf, sizeof(buf)), NULL, base));
}

/*
 */

// static
uint64_t CharOps::toULongLong(const char16_t* str, int base /* = 10 */)
{
  char buf[64];

  return(::strtoull(_toASCII(str, buf, sizeof(buf)), NULL, base));
}

/*
 */

// static
float CharOps::toFloat(const char16_t* str)
{
  char buf[64];

  return(::strtof(_toASCII(str, buf, sizeof(buf)), NULL));
}

/*
 */

// static
double CharOps::toDouble(const char16_t* str)
{
  char buf[64];

  return(std::strtod(_toASCII(str, buf, sizeof(buf)), NULL));
}

/*
 */

// static
char* CharOps::_toASCII(const char16_t* str, char* buf, uint_t len)
{
  const char16_t* q = str;
  while(*q && Char::isWhitespace(*q)) ++q;

  --len; // leave room for NUL
  char* p = buf;
  for(; *q && len > 0; --len, ++p, ++q)
    *p = Char::toASCII(*q);

  *p = 0;

  return(buf);
}


} // namespace ccxx
