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

#include "commonc++/SocketUtil.h++"
#include "commonc++/System.h++"
#include "commonc++/Private.h++"

#ifdef CCXX_OS_POSIX
#include <unistd.h>
#endif

#include <cerrno>
#include <cstdlib>

namespace ccxx {

/*
 */

const uint_t SocketUtil::WAIT_READ = 0x01;
const uint_t SocketUtil::WAIT_WRITE = 0x02;

/*
 */

bool SocketUtil::startConnect(SocketHandle socket, struct sockaddr* addr,
                              socklen_t addrlen)
{
  while(::connect(socket, addr, addrlen) != 0)
  {
    if(SOCKET_errno == SOCKET_EINTR)
      continue;
    else if((SOCKET_errno == EWOULDBLOCK) || (SOCKET_errno == EINPROGRESS))
      return(false);
    else if(SOCKET_errno == ECONNREFUSED)
      throw ConnectionRefusedException();
    else
      throw SocketException(System::getErrorString("connect"));
  }

  return(true);
}

/*
 */

void SocketUtil::finishConnect(SocketHandle socket)
{
  int val = 0;
  socklen_t len = sizeof(int);

  if(::getsockopt(socket, SOL_SOCKET, SO_ERROR, (sockbufptr_t)&val, &len) != 0)
  {
    closeSocket(socket);
    throw SocketException(System::getErrorString("getsockopt"));
  }

  if(val != 0)
  {
    closeSocket(socket);

#ifdef CCXX_OS_WINDOWS
    ::SetLastError(val);
#else
    errno = val;
#endif

    if(val == ECONNREFUSED)
      throw ConnectionRefusedException();
    else
      throw SocketException(System::getErrorString("connect"));
  }
}

/*
 */

void SocketUtil::connectSocket(SocketHandle socket, struct sockaddr* addr,
                               socklen_t addrlen, int timeout)
{

  if(! startConnect(socket, addr, addrlen))
  {
    try
    {
      waitForIO(socket, (WAIT_READ | WAIT_WRITE), timeout);
    }
    catch(IOException &tex)
    {
      closeSocket(socket);
      throw;
    }
  }

  finishConnect(socket);
}

/*
 */

void SocketUtil::waitForIO(SocketHandle socket, uint_t mode, int timeout)
{
  fd_set fds;
  struct timeval tv;
  div_t dv;
  time_ms_t end = System::currentTimeMillis() + timeout;

  for(;;)
  {
    if(timeout >= 0)
    {
      dv = div(static_cast<int>(end - System::currentTimeMillis()), 1000);
      tv.tv_sec = dv.quot;
      tv.tv_usec = dv.rem * 1000;
    }

    FD_ZERO(&fds);
    FD_SET(socket, &fds);

    int r = ::select(FD_SETSIZE, ((mode & WAIT_READ) ? &fds : NULL),
                     ((mode & WAIT_WRITE) ? &fds : NULL),
                     NULL, (timeout < 0 ? NULL : &tv));
    if(r < 0)
    {
      if(SOCKET_errno == SOCKET_EINTR)
        continue;
      else
        throw IOException(System::getErrorString("select"));
    }
    else if(r == 0)
      throw TimeoutException();
    else
      break;
  }
}

/*
 */

void SocketUtil::closeSocket(SocketHandle socket)
{
#ifdef CCXX_OS_WINDOWS
  ::closesocket(socket);
#else
  ::close(socket);
#endif
}


} // namespace ccxx
