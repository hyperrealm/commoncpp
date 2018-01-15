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

#ifdef CCXX_OS_WINDOWS
#ifdef FD_SETSIZE
#undef FD_SETSIZE
#endif
#define FD_SETSIZE 1024
#endif

#include "commonc++/Socket.h++"
#include "commonc++/System.h++"
#include "commonc++/Private.h++"
#include "commonc++/SocketUtil.h++"

#ifdef CCXX_OS_POSIX
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#endif

#include <fcntl.h>
#include <cerrno>

namespace ccxx {

#ifndef INADDR_NONE
#define INADDR_NONE ((in_addr_t)-1)
#endif // INADDR_NONE

/*
 */

Socket::Socket(NetProtocol type)
  : _type(type),
    _socket(INVALID_SOCKET_HANDLE),
    _sotimeout(0),
    _connected(false),
    _reuseAddr(false)
{
}

/*
 */

Socket::~Socket()
{
  shutdown();
}

/*
 */

void Socket::init()
{
  if(_socket != INVALID_SOCKET_HANDLE)
    return; // already initialized

  _socket = ::socket(AF_INET, (_type == ProtoUDP ? SOCK_DGRAM : SOCK_STREAM),
                     0);

#ifdef CCXX_OS_WINDOWS
  if(_socket == INVALID_SOCKET)
#else
  if(_socket < 0)
#endif
    throw SocketIOException(System::getErrorString("socket"));

#ifdef CCXX_OS_WINDOWS

  BOOL b = (_reuseAddr ? TRUE : FALSE);
  if(::setsockopt(_socket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *)&b,
                  sizeof(b)) == SOCKET_ERROR)
    throw SocketException(System::getErrorString("setsockopt"));

#else

  int v = (_reuseAddr ? 1 : 0);

  if(::setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&v, sizeof(v))
     != 0)
    throw SocketException(System::getErrorString("setsockopt"));

#endif

  if(::bind(_socket, (sockaddr *)_laddr, sizeof(sockaddr_in)) != 0)
  {
    if(errno == EADDRINUSE)
      throw AddressInUseException(String() << "Address already in use: "
                                  << _laddr.toString());
    else
      throw SocketException(System::getErrorString("bind"));
  }

  setTimeout(_sotimeout);
}

/*
 */

void Socket::connect(const String& addr, uint16_t port)
{
  // no-op by default
}

/*
 */

void Socket::connect(const SocketAddress& addr)
{
  // no-op by default
}

/*
 */

void Socket::shutdown()
{
  if(_socket != INVALID_SOCKET_HANDLE)
  {
    ::shutdown(_socket, SHUT_RDWR);
    SocketUtil::closeSocket(_socket);
    _socket = INVALID_SOCKET_HANDLE;
  }

  _connected = false;
  _laddr.reset();
  _raddr.reset();
}

/*
 */

void Socket::setTimeout(timespan_ms_t timeout)
{
  bool blocking = (timeout < 0);

  if(isInitialized())
  {
#ifdef CCXX_OS_WINDOWS

    u_long arg = (blocking ? 0 : 1);
    if(::ioctlsocket(_socket, FIONBIO, &arg) != 0)
      throw SocketException(System::getErrorString("ioctlsocket"));

#else

    int flags;

    if((flags = ::fcntl(_socket, F_GETFL, 0)) == -1)
      throw SocketException(System::getErrorString("fcntl"));

    blocking ? (flags &= ~O_NONBLOCK) : (flags |= O_NONBLOCK);

    if(::fcntl(_socket, F_SETFL, flags) == -1)
      throw SocketException(System::getErrorString("fcntl"));

#endif
  }

  _sotimeout = timeout;
}

/*
 */

void Socket::setReceiveBufSize(size_t size)
{
  if(! isInitialized())
    throw SocketException("socket not initialized");

  int v = (int)size;

  if(::setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (char *)&v, sizeof(v)) != 0)
    throw SocketException(System::getErrorString("setsockopt"));
}

/*
 */

size_t Socket::getReceiveBufSize() const
{
  if(! isInitialized())
    throw SocketException("socket not initialized");

  int v;
  socklen_t len = sizeof(v);

  if(::getsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (char *)(&v), &len) != 0)
    throw SocketException(System::getErrorString("getsockopt"));

  return(static_cast<size_t>(v));
}

/*
 */

void Socket::setSendBufSize(size_t size)
{
  if(! isInitialized())
    throw SocketException("socket not initialized");

  int v = (int)size;

  if(::setsockopt(_socket, SOL_SOCKET, SO_SNDBUF, (char *)(&v), sizeof(v))
     != 0)
    throw SocketException(System::getErrorString("setsockopt"));
}

/*
 */

size_t Socket::getSendBufSize() const
{
  if(! isInitialized())
    throw SocketException("socket not initialized");

  int v;
  socklen_t len = sizeof(v);

  if(::getsockopt(_socket, SOL_SOCKET, SO_SNDBUF, (char *)(&v), &len) != 0)
    throw SocketException(System::getErrorString("getsockopt"));

  return(static_cast<size_t>(v));
}

/*
 */

void Socket::setLingerTime(timespan_s_t timeout)
{
  if(! isInitialized())
    throw SocketException("socket not initialized");

  struct linger li = { (timeout >= 0), timeout };

  if(::setsockopt(_socket, SOL_SOCKET, SO_LINGER, (char *)(&li), sizeof(li))
     != 0)
    throw SocketException(System::getErrorString("setsockopt"));
}

/*
 */

timespan_s_t Socket::getLingerTime() const
{
  if(! isInitialized())
    throw SocketException("socket not initialized");

  struct linger li;
  socklen_t len = sizeof(li);

  if(::getsockopt(_socket, SOL_SOCKET, SO_LINGER, (char *)(&li), &len) != 0)
    throw SocketException(System::getErrorString("setsockopt"));

  return(li.l_onoff ? li.l_linger : -1);
}

/*
 */

void Socket::setKeepAlive(bool enable)
{
  if(! isInitialized())
    throw SocketException("socket not initialized");

  int v = (enable ? 1 : 0);

  if(::setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&v, sizeof(v))
     != 0)
    throw SocketException(System::getErrorString("setsockopt"));
}

/*
 */

bool Socket::getKeepAlive()
{
  if(! isInitialized())
    throw SocketException("socket not initialized");

  int v;
  socklen_t len = sizeof(v);

  if(::getsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&v, &len) != 0)
    throw SocketException(System::getErrorString("getsockopt"));

  return(v == 1);
}

/*
 */

void Socket::setReuseAddress(bool enable)
{
  if(isInitialized())
    throw SocketException("socket already initialized");

  _reuseAddr = enable;
}

/*
 */

bool Socket::getReuseAddress() const
{
  if(! isInitialized())
    throw SocketException("socket not initialized");

  int v;
  socklen_t len = sizeof(v);

  if(::getsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&v, &len) != 0)
    throw SocketException(System::getErrorString("getsockopt"));

  return(v == 1);
}

/*
 */

void Socket::setTCPDelay(bool enable)
{
  if(! isInitialized())
    throw SocketException("socket not initialized");

  int v = (enable ? 0 : 1);

  if(::setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&v, sizeof(v))
     != 0)
    throw SocketException(System::getErrorString("setsockopt"));
}

/*
 */

bool Socket::getTCPDelay()
{
  if(! isInitialized())
    throw SocketException("socket not initialized");

  int v;
  socklen_t len = sizeof(v);

  if(::getsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&v, &len) != 0)
    throw SocketException(System::getErrorString("getsockopt"));

  return(v == 0);
}

/*
 */

void Socket::waitForIO(IOWaitMode mode)
{
  SocketUtil::waitForIO(_socket, (mode == WaitRead ? SocketUtil::WAIT_READ
                                  : SocketUtil::WAIT_WRITE), _sotimeout);
}


} // namespace ccxx
