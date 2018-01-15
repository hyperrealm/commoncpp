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

#include "commonc++/ServerSocket.h++"
#include "commonc++/System.h++"
#include "commonc++/Private.h++"

#include <cerrno>

namespace ccxx {

/*
 */

ServerSocket::ServerSocket(uint16_t port, uint_t backlog /* = 3 */)
  : _backlog(backlog),
    _listening(false)
{
  _laddr.setAddress(InetAddress::ANY);
  _laddr.setPort(port);
}

/*
 */

ServerSocket::ServerSocket(uint16_t port, const NetworkInterface& ixface,
                           uint_t backlog /* = 3 */)
  : _backlog(backlog),
    _listening(false)
{
  _laddr.setAddress(ixface.getAddress());
  _laddr.setPort(port);
}

/*
 */

ServerSocket::~ServerSocket()
{
}

/*
 */

void ServerSocket::init()
{
  Socket::init();
  setTimeout(-1);
}

/*
 */

void ServerSocket::shutdown()
{
  Socket::shutdown();
  _listening = false;
}

/*
 */

void ServerSocket::listen()
{
  if(! isInitialized())
    throw SocketException("socket not initialized");

  if(! _listening)
  {
    if(::listen(_socket, static_cast<int>(_backlog)) != 0)
    {
      if(errno == EADDRINUSE)
        throw AddressInUseException(String() << "Address already in use: "
                                    << _laddr.toString());
      else
        throw SocketException(System::getErrorString("listen"));
    }

    _listening = true;
  }
}

/*
 */

void ServerSocket::accept(StreamSocket& socket)
{
  if(socket.isInitialized())
    throw SocketException("socket already initialized");

  socklen_t sz = (socklen_t)sizeof(sockaddr_in);
  SocketHandle sd;

  try
  {
    waitForIO(WaitRead);
  }
  catch(TimeoutException &tex)
  {
    throw tex;
  }
  catch(IOException &ex)
  {
    throw SocketException(ex.getMessage());
  }

  while((sd = ::accept(_socket, (sockaddr *)socket._raddr, &sz))
#ifdef CCXX_OS_WINDOWS
    == INVALID_SOCKET
#else
    < 0
#endif
    )
  {
    if((SOCKET_errno == EWOULDBLOCK)
#ifdef EAGAIN
       || (SOCKET_errno == EAGAIN)
#endif
      )
      throw TimeoutException();

    if(SOCKET_errno != SOCKET_EINTR)
      throw SocketException(System::getErrorString("accept"));
  }

  socket.setSocketHandle(sd);
}


} // namespace ccxx
