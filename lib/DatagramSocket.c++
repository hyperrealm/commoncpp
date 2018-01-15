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

#include "commonc++/DatagramSocket.h++"
#include "commonc++/System.h++"
#include "commonc++/Private.h++"

#ifdef CCXX_OS_POSIX
#include <sys/select.h>
#endif

#include <cerrno>

namespace ccxx {

/*
 */

const size_t DatagramSocket::MAX_DATAGRAM_SIZE = 16384;

/*
 */

DatagramSocket::DatagramSocket(uint16_t port /* = 0 */)
  : Socket(ProtoUDP)
{
  _laddr.setAddress(InetAddress::ANY);
  _laddr.setPort(port);
}

/*
 */

DatagramSocket::~DatagramSocket()
{
}

/*
 */

void DatagramSocket::connect(const String &addr, uint16_t port)
{
  if(_connected)
    throw SocketException("already connected");

  if(! isInitialized())
    throw SocketException("socket not initialized");

  socklen_t sz = (socklen_t)sizeof(sockaddr_in);

  if(! addr)
    throw SocketException("NULL address");

  InetAddress ipaddr(addr);
  ipaddr.resolve();

  _raddr.setAddress(ipaddr);
  _raddr.setPort(port);

  while(::connect(_socket, (sockaddr *)_raddr, sz) < 0)
  {
    if(SOCKET_errno == SOCKET_EINTR)
      continue;
    else
    {
      shutdown();
      throw SocketIOException(System::getErrorString());
    }
  }

  sz = (socklen_t)sizeof(sockaddr_in);
  ::getsockname(_socket, (sockaddr *)_laddr, &sz);

  _connected = true;
}

/*
 */

void DatagramSocket::connect(const SocketAddress& addr)
{
  if(_connected)
    throw SocketException("already connected");

  if(! isInitialized())
    throw SocketException("socket not initialized");

  socklen_t sz = (socklen_t)sizeof(sockaddr_in);

  _raddr = addr;

  while(::connect(_socket, (sockaddr *)_raddr, sz) < 0)
  {
    if(SOCKET_errno == SOCKET_EINTR)
      continue;
    else
    {
      shutdown();
      throw SocketIOException(System::getErrorString());
    }
  }

  sz = (socklen_t)sizeof(sockaddr_in);
  ::getsockname(_socket, (sockaddr *)_laddr, &sz);

  _connected = true;
}

/*
 */

size_t DatagramSocket::send(const byte_t* buffer, size_t buflen)
{
  if(buflen == 0)
    return(0); // nothing to send

  int flags = MSG_NOSIGNAL, b = 0;

  if(_sotimeout > 0)
    waitForIO(WaitWrite);

  for(;;)
  {
    b = ::sendto(_socket, (const sockbufptr_t)buffer, buflen, flags,
                 (_connected ? NULL : (struct sockaddr *)_raddr),
                 (_connected ? 0 : sizeof(sockaddr_in)));

    if(b == 0)
      throw EOFException();
    else if(b < 0)
    {
      if(errno == SOCKET_EINTR)
        continue;
      else if((errno == EWOULDBLOCK)
#ifdef EAGAIN
              || (errno == EAGAIN)
#endif
        )
        throw TimeoutException();
      else
        throw SocketIOException(System::getErrorString("sendto"));
    }
    else
      break;
  }

  return(static_cast<size_t>(b));
}

/*
 */

size_t DatagramSocket::send(ByteBuffer& buffer)
{
  size_t b = send(buffer.getPointer(), buffer.getRemaining());
  buffer.bump(b);
  return(b);
}

/*
 */

size_t DatagramSocket::send(ByteBuffer& buffer, const SocketAddress& dest)
{
  _raddr = dest;

  return(send(buffer));
}

/*
 */

size_t DatagramSocket::send(const byte_t* buffer, size_t buflen,
                            const SocketAddress& dest)
{
  _raddr = dest;

  return(send(buffer, buflen));
}

/*
 */

size_t DatagramSocket::receive(byte_t* buffer, size_t buflen)
{
  if(buflen == 0)
    return(0); // nothing to receive

  socklen_t sz = (socklen_t)sizeof(sockaddr_in);
  int flags = MSG_NOSIGNAL, b = 0;

  if(_sotimeout > 0)
    waitForIO(WaitRead);

  for(;;)
  {
    b = ::recvfrom(_socket, (sockbufptr_t)buffer, buflen, flags,
                   (_connected ? NULL : (struct sockaddr *)_raddr),
                   (_connected ? 0 : &sz));

    if(b == 0)
    {
      _connected = false;
      throw EOFException();
    }
    else if(b < 0)
    {
      if(errno == SOCKET_EINTR)
        continue;
      else if((errno == EWOULDBLOCK)
#ifdef EAGAIN
              || (errno == EAGAIN)
#endif
        )
        throw TimeoutException();
      else
        throw SocketIOException(System::getErrorString("recvfrom"));
    }
    else
      break;
  }

  return(static_cast<size_t>(b));
}

/*
 */

size_t DatagramSocket::receive(ByteBuffer& buffer)
{
  size_t b = receive(buffer.getPointer(), buffer.getRemaining());
  buffer.bump(b);
  return(b);
}

/*
 */

size_t DatagramSocket::receive(byte_t* buffer, size_t buflen,
                               SocketAddress& source)
{
  size_t sz = receive(buffer, buflen);

  source = _raddr;

  return(sz);
}

/*
 */

size_t DatagramSocket::receive(ByteBuffer& buffer, SocketAddress& source)
{
  size_t sz = receive(buffer);

  source = _raddr;

  return(sz);
}

/*
 */

void DatagramSocket::setBroadcast(bool flag)
{
  int v = flag ? 1 : 0;

  if(::setsockopt(_socket, SOL_SOCKET, SO_BROADCAST, (char *)(&v), sizeof(v))
     != 0)
    throw SocketException(System::getErrorString("setsockopt"));
}

/*
 */

bool DatagramSocket::getBroadcast() const
{
  int v;
  socklen_t len = sizeof(v);

  if(::getsockopt(_socket, SOL_SOCKET, SO_BROADCAST, (char *)(&v), &len) != 0)
    throw SocketException(System::getErrorString("getsockopt"));

  return(v == 1);
}


} // namespace ccxx
