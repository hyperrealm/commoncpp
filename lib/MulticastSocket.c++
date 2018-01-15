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

#include "commonc++/MulticastSocket.h++"
#include "commonc++/ByteOrder.h++"
#include "commonc++/System.h++"

#ifdef CCXX_OS_WINDOWS
#include <ws2tcpip.h>
#endif

namespace ccxx {

/*
 */

uint8_t MulticastSocket::TTL_HOST         =   0;
uint8_t MulticastSocket::TTL_SUBNET       =   1;
uint8_t MulticastSocket::TTL_SITE         =  32;
uint8_t MulticastSocket::TTL_REGION       =  64;
uint8_t MulticastSocket::TTL_CONTINENT    = 128;
uint8_t MulticastSocket::TTL_UNRESTRICTED = 255;

/*
 */

MulticastSocket::MulticastSocket(uint16_t port /* = 0 */)
  : DatagramSocket(port)
{
}

/*
 */

MulticastSocket::~MulticastSocket()
{
}

/*
 */

void MulticastSocket::join(const String& address)
{
  join(InetAddress(address));
}

/*
 */

void MulticastSocket::join(const InetAddress& address)
{
  verifyAddress(address);

  struct in_addr ipaddr = { ByteOrder::hostToNetwork(address.getAddress()) };
  struct ip_mreq mreq;

  std::memcpy(&mreq.imr_multiaddr, &ipaddr, sizeof(ipaddr));
  mreq.imr_interface.s_addr = ByteOrder::hostToNetwork(
    InetAddress::ANY.getAddress());

  if(::setsockopt(_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (sockbufptr_t)&mreq,
                  sizeof(mreq)) < 0)
    throw SocketException(System::getErrorString("setsockopt"));
}

/*
 */

void MulticastSocket::join(const String& address,
                           const NetworkInterface& ixface)
{
  join(InetAddress(address), ixface);
}

/*
 */

void MulticastSocket::join(const InetAddress& address,
                           const NetworkInterface& ixface)
{
  verifyAddress(address);

  struct in_addr ipaddr = { ByteOrder::hostToNetwork(address.getAddress()) };
  struct ip_mreq mreq;

  std::memcpy(&mreq.imr_multiaddr, &ipaddr, sizeof(ipaddr));
  mreq.imr_interface.s_addr = ByteOrder::hostToNetwork(
    ixface.getAddress().getAddress());

  if(::setsockopt(_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (sockbufptr_t)&mreq,
                  sizeof(mreq)) < 0)
    throw SocketException(System::getErrorString("setsockopt"));
}

/*
 */

void MulticastSocket::leave(const String& address)
{
  leave(InetAddress(address));
}

/*
 */

void MulticastSocket::leave(const InetAddress& address)
{
  verifyAddress(address);

  struct in_addr ipaddr = { ByteOrder::hostToNetwork(address.getAddress()) };
  struct ip_mreq mreq;

  std::memcpy(&mreq.imr_multiaddr, &ipaddr, sizeof(ipaddr));
  mreq.imr_interface.s_addr = ByteOrder::hostToNetwork(
    InetAddress::ANY.getAddress());

  if(::setsockopt(_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (sockbufptr_t)&mreq,
                  sizeof(mreq)) < 0)
    throw SocketException(System::getErrorString("setsockopt"));
}

/*
 */

void MulticastSocket::leave(const String& address,
                            const ccxx::NetworkInterface& ixface)
{
  leave(InetAddress(address), ixface);
}

/*
 */

void MulticastSocket::leave(const InetAddress& address,
                            const ccxx::NetworkInterface& ixface)
{
  verifyAddress(address);

  struct in_addr ipaddr = { ByteOrder::hostToNetwork(address.getAddress()) };
  struct ip_mreq mreq;

  std::memcpy(&mreq.imr_multiaddr, &ipaddr, sizeof(ipaddr));
  mreq.imr_interface.s_addr = ByteOrder::hostToNetwork(
    ixface.getAddress().getAddress());

  if(::setsockopt(_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (sockbufptr_t)&mreq,
                  sizeof(mreq)) < 0)
    throw SocketException(System::getErrorString("setsockopt"));
}

/*
 */

void MulticastSocket::setTTL(uint8_t ttl)
{
  uint8_t val = ttl;

  if(::setsockopt(_socket, IPPROTO_IP, IP_MULTICAST_TTL, (sockbufptr_t)&val,
                  sizeof(val)) < 0)
    throw SocketException(System::getErrorString("setsockopt"));
}

/*
 */

uint8_t MulticastSocket::getTTL() const
{
  uint8_t val;
  socklen_t len;

  if(::getsockopt(_socket, IPPROTO_IP, IP_MULTICAST_TTL, (sockbufptr_t)&val,
                  &len) < 0)
    throw SocketException(System::getErrorString("getsockopt"));

  return(val);
}

/*
 */

void MulticastSocket::setLoopbackEnabled(bool enabled)
{
  u_char val = static_cast<u_char>(enabled);

  if(::setsockopt(_socket, IPPROTO_IP, IP_MULTICAST_LOOP, (sockbufptr_t)&val,
                  sizeof(val)) < 0)
    throw SocketException(System::getErrorString("setsockopt"));
}

/*
 */

bool MulticastSocket::isLoopbackEnabled() const
{
  u_char val;
  socklen_t len;

  if(::getsockopt(_socket, IPPROTO_IP, IP_MULTICAST_LOOP, (sockbufptr_t)&val,
                  &len) < 0)
    throw SocketException(System::getErrorString("getsockopt"));

  return(val ? true : false);
}

/*
 */

void MulticastSocket::verifyAddress(const InetAddress& address)
{
  if(! address.isMulticast())
    throw SocketException(String() << "not a multicast address: "
                          << address.toIPString());
}


} // namespace ccxx
