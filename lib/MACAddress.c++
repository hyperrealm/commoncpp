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

#include "commonc++/MACAddress.h++"

#include <cstdio>
#include <cstring>

namespace ccxx {

/*
 */

MACAddress::MACAddress()
{
  std::memset(_octets, 0, 6);
}

/*
 */

MACAddress::MACAddress(const byte_t octets[6])
{
  std::memcpy(_octets, octets, 6);
}

/*
 */

MACAddress::~MACAddress()
{
}

/*
 */

void MACAddress::getOctets(byte_t octets[6]) const
{
  std::memcpy(octets, _octets, 6);
}

/*
 */

void MACAddress::setOctets(const byte_t octets[6])
{
  std::memcpy(_octets, octets, 6);
  _str = String::null;
}

/*
 */

byte_t MACAddress::operator[](int index) const
{
  if((index < 0) || (index >= 6))
    throw OutOfBoundsException();

  return(_octets[index]);
}

/*
 */

byte_t& MACAddress::operator[](int index)
{
  if((index < 0) || (index >= 6))
    throw OutOfBoundsException();

  return(_octets[index]);
}

/*
 */

bool MACAddress::operator==(const MACAddress& other) const
{
  return(std::memcmp(_octets, other._octets, 6));
}

/*
 */

String MACAddress::toString() const
{
  if(! _str)
  {
    char buf[32];
    snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", _octets[0],
             _octets[1], _octets[2], _octets[3], _octets[4], _octets[5]);
    _str = buf;
  }

  return(_str);
}

/*
 */

bool MACAddress::operator!() const
{
  return((_octets[0] == 0) && (_octets[1] == 0) && (_octets[2] == 0)
         && (_octets[3] == 0) && (_octets[4] == 0) && (_octets[5] == 0));
}


} // namespace ccxx
