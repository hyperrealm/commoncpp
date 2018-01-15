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

#include "commonc++/ByteOrder.h++"

#ifdef CCXX_OS_POSIX
#include <netinet/in.h>
#endif

namespace ccxx {

/*
 */

uint64_t ByteOrder::hostToNetwork(uint64_t n)
{
  return(((uint64_t)htonl((uint32_t)(n & 0xFFFFFFFF)) << 32)
         | ((uint64_t)htonl((uint32_t)(n >> 32))));
}

/*
 */

uint64_t ByteOrder::networkToHost(uint64_t n)
{
  return(((uint64_t)ntohl((uint32_t)(n & 0xFFFFFFFF)) << 32)
         | ((uint64_t)ntohl((uint32_t)(n >> 32))));
}

/*
 */

bool ByteOrder::isBigEndian()
{
  return(htonl(1) == 1);
}

/*
 */

void ByteOrder::_reverseBytes(byte_t* buf, size_t length)
{
  byte_t *p = buf, *q = buf + length - 1;

  while(p < q)
  {
    byte_t t = *p;
    *p = *q;
    *q = t;

    ++p;
    --q;
  }
}


} // namespace ccxx
