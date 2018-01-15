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

#include "commonc++/SocketAddress.h++"
#include "commonc++/Network.h++"
#include "commonc++/System.h++"
#include "commonc++/Private.h++"

#ifdef CCXX_OS_POSIX
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <cstring>
#include <cerrno>

namespace ccxx {

/*
 */

SocketAddress::SocketAddress()
{
  reset();
}

/*
 */

SocketAddress::SocketAddress(const InetAddress& address, uint16_t port)
{
  reset();
  setAddress(address);
  setPort(port);
}

/*
 */

SocketAddress::SocketAddress(const struct sockaddr& other)
{
  _saddr = other;
}

/*
 */

SocketAddress::SocketAddress(const struct sockaddr_in& other)
{
  _insaddr = other;
}

/*
 */

SocketAddress::SocketAddress(const SocketAddress& other)
{
  std::memcpy(&_insaddr, &(other._insaddr), sizeof(sockaddr_in));
}

/*
 */

SocketAddress::~SocketAddress()
{
}

/*
 */

SocketAddress& SocketAddress::operator=(const struct sockaddr& other)
{
  _saddr = other;

  return(*this);
}

/*
 */

SocketAddress& SocketAddress::operator=(const struct sockaddr_in& other)
{
  _insaddr = other;

  return(*this);
}

/*
 */

SocketAddress& SocketAddress::operator=(const SocketAddress& other)
{
  std::memcpy(&_insaddr, &(other._insaddr), sizeof(sockaddr_in));

  return(*this);
}

/*
 */

void SocketAddress::reset()
{
  CCXX_ZERO(_insaddr);
  _insaddr.sin_family = AF_INET;
}

/*
 */

InetAddress SocketAddress::getAddress() const
{
  return(InetAddress(ByteOrder::networkToHost(static_cast<uint32_t>(
                                                _insaddr.sin_addr.s_addr))));
}

/*
 */

void SocketAddress::setAddress(const InetAddress& address)
{
  _insaddr.sin_addr.s_addr = ByteOrder::hostToNetwork(address.getAddress());
}

/*
 */

void SocketAddress::setHost(const String& host)
{
  InetAddress ipaddr(host);
  ipaddr.resolve();

  _insaddr.sin_addr.s_addr = ByteOrder::hostToNetwork(ipaddr.getAddress());
}

/*
 */

String SocketAddress::toString() const
{
  String s;
  s << getAddress().toIPString() << ":" << static_cast<int>(getPort());

  return(s);
}

/*
 */

bool SocketAddress::operator==(const SocketAddress& other) const
{
  return((std::memcmp(&_insaddr, &(other._insaddr), sizeof(sockaddr_in)))
         == 0);
}


} // namespace ccxx
