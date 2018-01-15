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

#include "commonc++/StreamSocket.h++"
#include "commonc++/SocketUtil.h++"
#include "commonc++/System.h++"
#include "commonc++/Private.h++"

#ifdef CCXX_OS_POSIX
#include <unistd.h>
#include <sys/select.h>
#endif

#include <cerrno>

namespace ccxx {

/*
 */

StreamSocket::StreamSocket(uint16_t port /* = 0 */)
  : Stream(),
    Socket()
{
  _laddr.setAddress(InetAddress::ANY);
  _laddr.setPort(port);
}

/*
 */

StreamSocket::~StreamSocket()
{
  close();
}

/*
 */

void StreamSocket::connect(const String& addr, uint16_t port)
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

  try
  {
    SocketUtil::connectSocket(_socket, (sockaddr *)_raddr, sz, _timeout);
  }
  catch(const IOException& ioex)
  {
    close();
    throw;
  }

  sz = (socklen_t)sizeof(sockaddr_in);
  if(::getsockname(_socket, (sockaddr *)_laddr, &sz) != 0)
    throw SocketException(System::getErrorString("getsockname"));

  setTimeout(_sotimeout);

  _connected = true;

  Stream::_init((FileHandle)(_socket), false, true, true);
}

/*
 */

void StreamSocket::connect(const SocketAddress& addr)
{
  if(_connected)
    throw SocketException("already connected");

  if(! isInitialized())
    throw SocketException("socket not initialized");

  _raddr = addr;
  socklen_t sz = (socklen_t)sizeof(sockaddr_in);

  try
  {
    SocketUtil::connectSocket(_socket, (sockaddr *)_raddr, sz, _timeout);
  }
  catch(const IOException& ioex)
  {
    close();
    throw;
  }

  if(::getsockname(_socket, (sockaddr *)_laddr, &sz) != 0)
    throw SocketException(System::getErrorString("getsockname"));

  _connected = true;

  Stream::_init((FileHandle)(_socket), false, true, true);
}

/*
 */

size_t StreamSocket::read(byte_t* buffer, size_t buflen)
{
  if(! _connected)
    throw SocketException("not connected");

  if(! _canRead)
    throw EOFException();

  int flags = MSG_NOSIGNAL;

  if(_timeout > 0)
    waitForIO(WaitRead);

  // socket ready to read; read as much as possible

  ssize_t b = 0;

  for(;;)
  {
    b = ::recv(_socket, (sockbufptr_t)buffer, buflen, flags);

    if((b == 0) || ((b < 0) && ((SOCKET_errno == ECONNRESET)
                                || (SOCKET_errno == ECONNABORTED))))
    {
      _connected = false;
      throw EOFException();
    }
    else if(b < 0)
    {
      if(SOCKET_errno == SOCKET_EINTR)
        continue;
      else if(SOCKET_errno == EWOULDBLOCK)
        throw TimeoutException();
      else
        throw SocketIOException(System::getErrorString("recv"));
    }
    else
      break;
  }

  return(static_cast<size_t>(b));
}

/*
 */

byte_t StreamSocket::readOOB()
{
  if(! _connected)
    throw SocketException("not connected");

  if(! _canRead)
    throw EOFException();

  byte_t oob;
  int flags = MSG_NOSIGNAL | MSG_OOB;

  for(;;)
  {
    ssize_t b = ::recv(_socket, (sockbufptr_t)&oob, 1, flags);

    if((b == 0) || ((b < 0) && ((SOCKET_errno == ECONNRESET)
                                || (SOCKET_errno == ECONNABORTED))))
    {
      _connected = false;
      throw EOFException();
    }
    else if(b < 0)
    {
      if(SOCKET_errno == SOCKET_EINTR)
        continue;
      else if(SOCKET_errno == EWOULDBLOCK)
        throw TimeoutException();
      else
        throw SocketIOException(System::getErrorString("recv"));
    }
    else
      return(oob);
  }
}

/*
 */

size_t StreamSocket::write(const byte_t* buffer, size_t buflen)
{
  if(! _connected)
    throw SocketException("not connected");

  if(! _canWrite)
    throw EOFException();

  int flags = MSG_NOSIGNAL;

  if(_timeout > 0)
    waitForIO(WaitWrite);

  // socket ready to write; write as much as possible

  ssize_t b = 0;

  for(;;)
  {
    b = ::send(_socket, (sockbufptr_t)buffer, buflen, flags);

    if((b == 0) || ((b < 0) && ((SOCKET_errno == ECONNRESET)
                                || (SOCKET_errno == ECONNABORTED))))
    {
      _connected = false;
      throw EOFException();
    }
    else if(b < 0)
    {
      if(SOCKET_errno == SOCKET_EINTR)
        continue;
      else if(SOCKET_errno == EWOULDBLOCK)
        throw TimeoutException();
      else
        throw SocketIOException(System::getErrorString("send"));
    }
    else
      break;
  }

  return(static_cast<size_t>(b));
}

/*
 */

void StreamSocket::writeOOB(byte_t data)
{
  if(! _connected)
    throw SocketException("not connected");

  if(! _canWrite)
    throw EOFException();

  int flags = MSG_NOSIGNAL | MSG_OOB;

  for(;;)
  {
    ssize_t b = ::send(_socket, (sockbufptr_t)&data, 1, flags);

    if((b == 0) || ((b < 0) && ((SOCKET_errno == ECONNRESET)
                                || (SOCKET_errno == ECONNABORTED))))
    {
      _connected = false;
      throw EOFException();
    }
    else if(b < 0)
    {
      if(SOCKET_errno == SOCKET_EINTR)
        continue;
      else if(SOCKET_errno == EWOULDBLOCK)
        throw TimeoutException();
      else
        throw SocketIOException(System::getErrorString("send"));
    }
    else
      break;
  }
}

/*
 */

size_t StreamSocket::read(MemoryBlock* vec, uint_t count)
{
  if(! _connected)
    throw SocketException("not connected");

  size_t total = 0;

#ifdef CCXX_OS_WINDOWS

  // Overridden to use recv() in a loop instead of ReadFile() with
  // overlapped I/O.

  if(_timeout >= 0)
    waitForIO(WaitRead);

  uint_t left = count;
  for(const MemoryBlock *blk = vec; left--; blk++)
  {
    size_t wanted = blk->getSize();
    ssize_t b = ::recv(_socket, (sockbufptr_t)(blk->getBase()), wanted, 0);

    if(b > 0) // some data was received
    {
      total += b;
      if(b < wanted)
        break; // not enough data read to fill current buffer, so stop reading

      // otherwise continue with next buffer
    }
    else // b <= 0; an error has occurred
    {
      if(total == 0) // nothing has been read so far
      {
        if((b == 0) || ((b < 0) && ((SOCKET_errno == ECONNRESET)
                                    || (SOCKET_errno == ECONNABORTED))))
        {
          _connected = false;
          throw EOFException();
        }
        else if(SOCKET_errno == EWOULDBLOCK)
          throw TimeoutException();
        else
          throw SocketIOException(System::getErrorString("recv"));
      }
      else // some data was received, so fall through
        break;
    }
  }

  // If an error has occurred during the above loop, but at least one
  // byte of data was successfully read, then ignore the error and
  // return successfully.  The next call to this function will cause
  // recv() to fail immediately (before any data has been read), and
  // thus an exception will be thrown.

#else

  total = Stream::read(vec, count);

  if(total == 0)
  {
    _connected = false;
    throw EOFException();
  }

#endif

  return(total);
}

/*
 */

size_t StreamSocket::write(const MemoryBlock* vec, uint_t count)
{
  if(! _connected)
    throw SocketException("not connected");

  size_t total = 0;

#ifdef CCXX_OS_WINDOWS

  // Overridden to use send() in a loop instead of WriteFile() with
  // overlapped I/O.

  if(_timeout >= 0)
    waitForIO(WaitWrite);

  uint_t left = count;
  for(const MemoryBlock *blk = vec; left--; blk++)
  {
    size_t wanted = blk->getSize();
    ssize_t b = ::send(_socket, (sockbufptr_t)(blk->getBase()), wanted, 0);

    if(b > 0) // some data was sent
    {
      total += b;
      if(b < wanted)
        break; // complete buffer not written, so stop writing

      // otherwise continue with next buffer
    }
    else // b <= 0; an error has occurred
    {
      if(total == 0) // nothing has been written so far
      {
        if((b == 0) || ((b < 0) && ((SOCKET_errno == ECONNRESET)
                                    || (SOCKET_errno == ECONNABORTED))))
        {
          _connected = false;
          throw EOFException();
        }
        else if(SOCKET_errno == EWOULDBLOCK)
          throw TimeoutException();
        else
          throw SocketIOException(System::getErrorString("send"));
      }
      else // some data was sent, so fall through
        break;
    }
  }

#else

  total = Stream::write(vec, count);

  if(total == 0)
  {
    _connected = false;
    throw EOFException();
  }

#endif

  return(total);
}

/*
 */

void StreamSocket::close(IOMode mode /* = IOReadWrite */)
{
  if(_socket == INVALID_SOCKET_HANDLE)
    return;

  switch(mode)
  {
    case IORead:
      if(_canRead)
      {
        ::shutdown(_socket, SHUT_RD);
        _canRead = false;
      }
      break;

    case IOWrite:
      if(_canWrite)
      {
        ::shutdown(_socket, SHUT_WR);
        _canWrite = false;
      }
      break;

    case IOReadWrite:
      if(_canRead || _canWrite)
      {
        ::shutdown(_socket, SHUT_RDWR);
        _canRead = _canWrite = false;
      }
      break;
  }

  if(!(_canRead || _canWrite))
    Socket::shutdown();
}

/*
 */

void StreamSocket::shutdown()
{
  close();
}

/*
 */

void StreamSocket::setTimeout(timespan_ms_t timeout)
{
  Socket::setTimeout(timeout);
  Stream::setTimeout(timeout);
}

/*
 */

void StreamSocket::setSocketHandle(SocketHandle handle)
{
  _socket = handle;
  Stream::_init((FileHandle)_socket, false, true, true);

  socklen_t sz = (socklen_t)sizeof(sockaddr_in);
  if(::getsockname(_socket, (sockaddr *)_laddr, &sz) != 0)
    _laddr.reset();

  sz = (socklen_t)sizeof(sockaddr_in);
  if(::getpeername(_socket, (sockaddr *)_raddr, &sz) != 0)
    _raddr.reset();

  _connected = true;
}


} // namespace ccxx
