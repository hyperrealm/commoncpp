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

#include "commonc++/UUID.h++"
#include "commonc++/Hex.h++"
#include "commonc++/Random.h++"

#if defined(HAVE_UUID_UUID_H)
#include <uuid/uuid.h>
#elif defined(HAVE_UUID_H)
#include <uuid.h>
#elif defined(SYS_UUID_H)
#include <sys/uuid.h>
#endif

#ifdef CCXX_OS_WINDOWS
typedef UUID WIN32_UUID_TYPE;
#endif

namespace ccxx {

/*
 */

UUID::UUID()
{
#if defined(CCXX_OS_WINDOWS)

  if(FAILED(::UuidCreate(reinterpret_cast<WIN32_UUID_TYPE *>(&_uuid))))
    _createRandomUUID(_uuid);

#elif defined(HAVE_UUID_GENERATE)

  ::uuid_generate(_uuid);

#elif defined(HAVE_UUID_CREATE)

  uuid_t u;
  uint32_t status;

  uuid_create(&u, &status);
  if(status == uuid_s_ok)
    std::memcpy(&_uuid, &u, 16);
  else
    _createRandomUUID(_uuid);

#else

  _createRandomUUID(_uuid);

#endif
}

/*
 */

UUID::~UUID()
{
}

/*
 */

String UUID::toString() const
{
  // example: B5F8DB80-3CCA-14F8-1E78-930269370000

  char buf[8 + 1 + 4 + 1 + 4 + 1 + 4 + 1 + 12 + 1];

  const byte_t *u = _uuid;
  char *p = buf;

  Hex::encode(u, 4, p, 8);
  u += 4;
  p += 8;
  *p = '-';
  ++p;

  for(uint_t i = 0; i < 3; ++i)
  {
    Hex::encode(u, 2, p, 4);
    u += 2;
    p += 4;
    *p = '-';
    ++p;
  }

  Hex::encode(u, 6, p, 12);
  p += 12;
  *p = NUL;

  return(String(buf));
}

/*
 */

void UUID::_createRandomUUID(byte_t* uuid)
{
  Random rand;
  rand.getBytes(uuid, 16);

  uuid[6] &= 0x0F;
  uuid[6] |= 0x40;

  uuid[8] &= 0x3F;
  uuid[8] |= 0x80;
}


} // namespace ccxx
