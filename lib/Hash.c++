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

#include "commonc++/Hash.h++"

namespace ccxx {

/*
 */

uint32_t Hash::DEKHash(const byte_t *data, size_t length)
{
  if(!data || !length)
    return(0);

  uint32_t hash = static_cast<uint32_t>(length);
  size_t n = 0;

  for(const byte_t *p = data; n < length; ++p, ++n)
    hash = ((hash << 5) ^ (hash >> 27)) ^ *p;

  return(hash & 0x7FFFFFFF);
}

/*
 */

uint32_t Hash::DJBHash(const byte_t *data, size_t length)
{
  if(!data || !length)
    return(0);

  uint32_t hash = 5381;
  size_t n = 0;

  for(const byte_t *p = data; n < length; ++p, ++n)
    hash = ((hash << 5) + hash) + *p;

  return(hash & 0x7FFFFFFF);
}

/*
 */

uint32_t Hash::ELFHash(const byte_t *data, size_t length)
{
  if(!data || !length)
    return(0);

  uint32_t hash = 0, x = 0;
  size_t n = 0;

  for(const byte_t *p = data; n < length; ++p, ++n)
  {
    hash = (hash << 4) + *p;
    x = (hash & 0xF0000000);
    if(x != 0)
    {
      hash ^= (x >> 24);
      hash &= ~x;
    }
  }

  return(hash & 0x7FFFFFFF);
}

/*
 */

uint32_t Hash::SDBMHash(const byte_t *data, size_t length)
{
  if(!data || !length)
    return(0);

  uint32_t hash = 0;
  size_t n = 0;

  for(const byte_t *p = data; n < length; ++p, ++n)
    hash = *p + (hash << 6) + (hash << 16) - hash;

  return(hash & 0x7FFFFFFF);
}


} // namespace ccxx
