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

#include "commonc++/NetworkInterface.h++"

namespace ccxx {

/*
 */

const uint32_t NetworkInterface::FLAG_UP          = 1 << 0;
const uint32_t NetworkInterface::FLAG_BROADCAST   = 1 << 1;
const uint32_t NetworkInterface::FLAG_LOOPBACK    = 1 << 2;
const uint32_t NetworkInterface::FLAG_PPP         = 1 << 3;
const uint32_t NetworkInterface::FLAG_RUNNING     = 1 << 4;
const uint32_t NetworkInterface::FLAG_MULTICAST   = 1 << 5;

const uint32_t NetworkInterface::FLAG_MASK        = 0x3F;

/*
 */

NetworkInterface::NetworkInterface()
  : _mtu(0),
    _metric(0),
    _prefixLength(0),
    _flags(0)
{
}

/*
 */

NetworkInterface::~NetworkInterface()
{
}

/*
 */

void NetworkInterface::setNetmask(const InetAddress& addr)
{
  _netmask = addr;

  uint32_t mask = addr.getAddress();
  _prefixLength = 0;
  while(mask != 0)
  {
    ++_prefixLength;
    mask <<= 1;
  }
}


} // namespace ccxx
