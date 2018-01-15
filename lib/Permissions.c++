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

#include "commonc++/Permissions.h++"

#ifdef CCXX_OS_WINDOWS

#define S_IRUSR 0400
#define S_IWUSR 0200
#define S_IXUSR 0100
#define S_IRGRP 0040
#define S_IWGRP 0020
#define S_IXGRP 0010
#define S_IROTH 0004
#define S_IWOTH 0002
#define S_IXOTH 0001

#else

#include <sys/stat.h>

#endif

namespace ccxx {

const int Permissions::USER_READ = S_IRUSR,
  Permissions::USER_WRITE = S_IWUSR,
  Permissions::USER_READ_WRITE = (S_IRUSR | S_IWUSR),
  Permissions::USER_EXECUTE = S_IXUSR,
  Permissions::USER_ALL = (S_IRUSR | S_IWUSR | S_IXUSR),
  Permissions::GROUP_READ = S_IRGRP,
  Permissions::GROUP_WRITE = S_IWGRP,
  Permissions::GROUP_READ_WRITE = (S_IRGRP | S_IWGRP),
  Permissions::GROUP_EXECUTE = S_IXGRP,
  Permissions::GROUP_ALL = (S_IRGRP | S_IWGRP | S_IXGRP),
  Permissions::OTHERS_READ = S_IROTH,
  Permissions::OTHERS_WRITE = S_IWOTH,
  Permissions::OTHERS_READ_WRITE = (S_IROTH | S_IWOTH),
  Permissions::OTHERS_EXECUTE = S_IXOTH,
  Permissions::OTHERS_ALL = (S_IROTH | S_IWOTH | S_IXOTH),
  Permissions::ALL_READ_WRITE = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP
                                 | S_IROTH | S_IWOTH),
  Permissions::ALLBITS = (USER_ALL | GROUP_ALL | OTHERS_ALL);

/*
 */

const Permissions Permissions::defaultFilePerms(Permissions::USER_ALL
                                                | Permissions::GROUP_READ);

const Permissions Permissions::defaultDirPerms(Permissions::USER_ALL
                                               | Permissions::GROUP_READ
                                               | Permissions::GROUP_EXECUTE);

/*
 */

Permissions::Permissions(uint_t mask /* = 0 */)
  : _mask(mask & ALLBITS)
{
}

/*
 */

Permissions::Permissions(const Permissions& other)
  : _mask(other._mask)
{
}

/*
 */

Permissions::~Permissions()
{
}

/*
 */

Permissions& Permissions::operator=(const Permissions& other)
{
  _mask = other._mask;
  return(*this);
}

/*
 */

String Permissions::toString() const
{
  String s = "---------";

  for(int i = 2, x = 0; i >=0; --i)
  {
    unsigned int b = ((_mask >> (i * 3)) & 0x07);

    if(b & 4)
      s[x] = 'r';
    ++x;

    if(b & 2)
      s[x] = 'w';
    ++x;

    if(b & 1)
      s[x] = 'x';
    ++x;
  }

  return(s);
}


} // namespace ccxx
