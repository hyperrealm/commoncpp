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

#include "commonc++/InetAddress.h++"
#include "commonc++/DynamicArray.h++"
#include "commonc++/Network.h++"
#include "commonc++/System.h++"
#include "commonc++/Private.h++"

#ifdef CCXX_OS_POSIX
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <cerrno>

namespace ccxx {

const InetAddress InetAddress::ANY(static_cast<uint32_t>(INADDR_ANY));

/*
 */

HostNotFoundException::HostNotFoundException(
  String message /* = String::empty */)
  : IOException(message)
{
}

/*
 */

InetAddress::InetAddress()
  : _addr(0),
    _resolved(false)
{
}

/*
 */

InetAddress::InetAddress(uint32_t addr)
  : _addr(addr),
    _resolved(true)
{
}

/*
 */

InetAddress::InetAddress(byte_t addr[4])
  : _resolved(true)
{
  uint32_t ipaddr;

  ipaddr = addr[0];
  ipaddr <<= 8;
  ipaddr |= addr[1];
  ipaddr <<= 8;
  ipaddr |= addr[2];
  ipaddr <<= 8;
  ipaddr |= addr[3];

  _addr = ipaddr;
}

/*
 */

InetAddress::InetAddress(const String &host)
  : _addr(0),
    _host(host)
{
  _resolved = _fromDotSeparated();
}

/*
 */

InetAddress::~InetAddress()
{
}

/*
 */

void InetAddress::resolve()
{
  if(_resolved)
    return;

  if(!_host)
    throw HostNotFoundException("invalid hostname");

  if(! _fromDotSeparated())
  {
    struct hostent he, *rhe;
    int herr = 0;
    DynamicArray<char> buf(256);

    for(;;)
    {
      CString cstr_host = _host.toUTF8();
      if(Network::getHostByName(cstr_host.data(), &he, buf.data(), buf.size(),
                                &rhe, &herr) != 0)
      {
        if(((errno == ERANGE) || (errno == EINVAL)) && (buf.size() < 4096))
        {
          // resize buffer & try again
          buf.resize(buf.size() * 2);
        }
        else
          throw HostNotFoundException(System::getErrorString("gethostbyname"));
      }
      else if(herr != 0)
        throw HostNotFoundException();
      else
        break;
    }

    in_addr_t ipaddr;
    std::memcpy(static_cast<void *>(&ipaddr), static_cast<void *>(he.h_addr),
                static_cast<size_t>(he.h_length));
    _addr = ByteOrder::networkToHost(static_cast<uint32_t>(ipaddr));
  }

  _resolved = true;
}

/*
 */

String InetAddress::getHost() const
{
  if(_resolved && _host.isNull())
  {
    struct hostent he, *rhe;
    int herr = 0;
    DynamicArray<char> buf(256);
    in_addr_t ipaddr = ByteOrder::hostToNetwork(_addr);

    for(;;)
    {
      if(Network::getHostByAddr((char *)&ipaddr, sizeof(in_addr_t), AF_INET,
                                &he, buf.data(), buf.size(), &rhe, &herr) != 0)
      {
        if(((errno == ERANGE) || (errno == EINVAL)) && (buf.size() < 4096))
        {
          // resize buffer & try again
          buf.resize(buf.size() * 2);
        }
        else
          throw HostNotFoundException(System::getErrorString("gethostbyname"));
      }
      else if(herr != 0)
        throw HostNotFoundException();
      else
        break;
    }

    _host = he.h_name;
  }

  return(_host);
}

/*
 */

void InetAddress::setHost(const String& host)
{
  _resolved = false;
  _host = host;
  resolve();
}

/*
 */

void InetAddress::setAddress(uint32_t addr)
{
  _addr = addr;
  _host = String::null;
  _resolved = true;
}

/*
 */

String InetAddress::toString() const
{
  return(_host.isNull() ? toIPString() : _host);
}

/*
 */

String InetAddress::toIPString() const
{
  struct in_addr inaddr;
  inaddr.s_addr = ByteOrder::hostToNetwork(_addr);
  String s = ::inet_ntoa(inaddr);
  return(s);
}

/*
 */

bool InetAddress::isMulticast() const
{
  return((_addr & 0xF0000000) == 0xE0000000);
}

/*
 */

char InetAddress::getNetworkClass() const
{
  if((_addr & 0x80000000) == 0x00000000)
    return('A');
  else if((_addr & 0xC0000000) == 0x80000000)
    return('B');
  else if((_addr & 0xE0000000) == 0xC0000000)
    return('C');
  else if((_addr & 0xF0000000) == 0xE0000000)
    return('D');
  else if((_addr & 0xF0000000) == 0xF0000000)
    return('E');
  else
    return('?');
}

/*
 */

bool InetAddress::isNonroutable() const
{
  return(((_addr & 0xFF000000) == 0x0A000000)       // 10.0.0.0/8
         || ((_addr & 0xFFF00000) == (0xAC100000))  // 172.16.0.0/12
         || ((_addr & 0xFFFF0000) == 0xC0A80000));  // 192.168.0.0./16
}

/*
 */

bool InetAddress::isLinkLocal() const
{
  return((_addr & 0xFFFF0000) == 0xA9FE0000);  // 169.254.0.0/16
}

/*
 */

bool InetAddress::operator==(const InetAddress& other) const
{ return(_addr == other._addr); }

/*
 */

InetAddress & InetAddress::operator|=(const InetAddress& other)
{
  _addr |= other._addr;

  return(*this);
}

/*
 */

InetAddress & InetAddress::operator&=(const InetAddress& other)
{
  _addr &= other._addr;

  return(*this);
}

/*
 */

bool InetAddress::_fromDotSeparated()
{
  CString cstr_host = _host.toUTF8();
  for(const char *p = cstr_host.data(); *p; p++)
  {
    if(!(isdigit((int)*p) || *p == '.'))
      return(false);
  }

  in_addr_t ipaddr;

#ifdef HAVE_INET_ATON

  struct in_addr inaddr;
  int r = ::inet_aton(cstr_host.data(), &inaddr);
  ipaddr = (r != 0) ? inaddr.s_addr : INADDR_ANY;

#else

  in_addr_t inaddr;
  if(((inaddr = ::inet_addr(cstr_host.data())) != INADDR_NONE)
     || (_host == "255.255.255.255"))
    ipaddr = inaddr;
  else
    ipaddr = INADDR_ANY;

#endif

  _addr = ByteOrder::networkToHost(static_cast<uint32_t>(ipaddr));

  return(true);
}


} // namespace ccxx
