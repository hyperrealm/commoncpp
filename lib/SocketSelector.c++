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

#include "commonc++/SocketSelector.h++"
#include "commonc++/ByteOrder.h++"
#include "commonc++/ScopedLock.h++"
#include "commonc++/SocketUtil.h++"
#include "commonc++/System.h++"

#ifdef CCXX_OS_POSIX
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#include <cerrno>
#include <list>

namespace ccxx {

/*
 */

class SocketSelector::ConnectionList : public std::list<Connection *>
{
};

/*
 */

SocketSelector::SocketSelector(uint_t maxConnections /* = 64 */,
                               timespan_ms_t defaultIdleLimit /* = 0 */)
  : _connections(new ConnectionList()),
    _mutex(true),
    _pool(maxConnections == 0 ? 1 : maxConnections),
    _idleLimit(defaultIdleLimit),
    _ssock(NULL)
{
}

/*
 */

SocketSelector::~SocketSelector()
{
  delete _connections;
}

/*
 */

bool SocketSelector::init(ServerSocket* socket)
{
  if(_ssock != NULL)
    return(socket == _ssock); // ensure idempotence

  if((socket == NULL) || !socket->isListening())
    return(false);

  _ssock = socket;


#ifdef CCXX_OS_WINDOWS

  // TODO: implement wakeup() mechanism for Windows

#else

  if(::pipe(_wakePipe) != 0)
    return(false);

  ::fcntl(_wakePipe[0], F_SETFL, O_NONBLOCK);
  ::fcntl(_wakePipe[1], F_SETFL, O_NONBLOCK);

#endif

  return(true);
}

/*
 */

void SocketSelector::cleanup()
{
  ScopedLock lock(_mutex);

  for(ConnectionList::const_iterator iter = _connections->begin();
      iter != _connections->end();
      ++iter)
  {
    Connection *conn = *iter;
    _connectionClosed(conn);
  }

  _connections->clear();

#ifdef CCXX_OS_WINDOWS

  // TODO: implement wakeup() mechanism for Windows

#else

  ::close(_wakePipe[0]);
  ::close(_wakePipe[1]);

#endif
}

/*
 */

void SocketSelector::wakeup()
{
#ifdef CCXX_OS_WINDOWS

  // TODO: implement wakeup() mechanism for Windows

#else

  static const char data[1] = { '!' };

  if(_wakeFlag.testAndSet(1, 0) == 0)
    if(::write(_wakePipe[1], data, sizeof(data)) != 1) { /* ignore */ }

#endif
}

/*
 */

size_t SocketSelector::getConnectionCount() const
{
  return(_connections->size());
}

/*
 */

void SocketSelector::dataReceivedOOB(Connection* connection)
{
  // no-op
}

/*
 */

void SocketSelector::run()
{
  if(! _ssock)
  {
    // Not properly initialized; return immediately.
    return;
  }

  SocketHandle ms = _ssock->getSocketHandle();
  fd_set readfd, writefd, exceptfd;
  struct timeval tv;

  while(! testCancel())
  {
    // populate the fd sets

    FD_ZERO(&readfd);
    FD_ZERO(&writefd);
    FD_ZERO(&exceptfd);

    // server-socket specific:
    FD_SET(ms, &readfd);

    // wakeup()-specific:

#ifdef CCXX_OS_WINDOWS

    // TODO: implement wakeup() mechanism for Windows

#else

    FD_SET(_wakePipe[0], &readfd);

#endif

    for(ConnectionList::const_iterator iter = _connections->begin();
        iter != _connections->end();
        ++iter)
    {
      Connection *conn = *iter;
      SocketHandle fd = conn->getSocket()->getSocketHandle();

      if(! conn->isReadHigh())
        FD_SET(fd, &readfd);

      if(! conn->isWriteLow())
        FD_SET(fd, &writefd);

      if(! conn->getOOBFlag())
        FD_SET(fd, &exceptfd);
    }

    // select on fd sets

  SELECT:
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int r = ::select(FD_SETSIZE, &readfd, &writefd, &exceptfd, &tv);

    if(r < 0)
    {
      if(r == EINTR)
        goto SELECT;

      // otherwise an unrecoverable error
      break;
    }

    // now we may have descriptors ready

    time_ms_t now = System::currentTimeMillis();

    // server-socket specific:
    // check if master socket is ready for read

    if((r > 0) && FD_ISSET(ms, &readfd))
    {
      // a new connection is pending
      StreamSocket *sock = NULL;

      try
      {
        sock = _pool.reserve();
        _ssock->accept(*sock);

        Connection *conn = connectionReady(sock->getRemoteAddress());
        if(! conn)
        {
          sock->close();
          _pool.release(sock);
        }
        else
        {
          conn->attach(this, sock);
          conn->setTimestamp(now);
          _connections->push_back(conn);
        }
      }
      catch(const ObjectPoolException &)
      {
        // too many connections
        SocketUtil::closeSocket(::accept(ms, NULL, NULL));
      }
      catch(const SocketException &)
      {
        // accept failed
        _pool.release(sock);
      }
    }

#ifndef CCXX_OS_WINDOWS

    // wakeup()-specific:

    if((r > 0) && FD_ISSET(_wakePipe[0], &readfd))
    {
      char buf;
      if(::read(_wakePipe[0], &buf, 1) != 1) { /* ignore */ }
      _wakeFlag.set(0);
    }

#endif

    // now check all active connections

    ScopedLock lock(_mutex);

    for(ConnectionList::iterator iter = _connections->begin();
        iter != _connections->end();
      )
    {
      Connection *conn = *iter;
      StreamSocket *sock = conn->getSocket();
      SocketHandle fd = sock->getSocketHandle();

      if(conn->isClosePending())
      {
        // had a pending close; so perform it now
        _connectionClosed(conn);
      }
      else if((r > 0) && FD_ISSET(fd, &readfd))
      {
        try
        {
          bool rcvd = false;

          conn->_readLock.enter();

          // read as much data as possible
          conn->read();
          conn->setTimestamp(now);

          if(! conn->isReadLow())
            rcvd = true;

          conn->setOOBFlag(false);

          conn->_readLock.leave();

          if(rcvd)
            dataReceived(conn);
        }
        catch(const EOFException &)
        {
          _connectionClosed(conn);
        }
        catch(const IOException& ex)
        {
          exceptionOccurred(conn, ex);
        }
      }
      else if(FD_ISSET(fd, &writefd))
      {
        try
        {
          bool low = false;

          conn->_writeLock.enter();

          // write as much data as possible
          conn->write();
          low = conn->isWriteLow();

          conn->_writeLock.leave();

          if(low)
            dataSent(conn);
        }
        catch(const EOFException &)
        {
          _connectionClosed(conn);
        }
        catch(const IOException& ex)
        {
          exceptionOccurred(conn, ex);
        }
      }
      else if((r > 0) && FD_ISSET(fd, &exceptfd))
      {
        try
        {
          conn->_readLock.enter();

          conn->readOOB();
          conn->setOOBFlag(true);
          dataReceivedOOB(conn);

          conn->_readLock.leave();
        }
        catch(const IOException& ex)
        {
          exceptionOccurred(conn, ex);
        }
      }
      else
      {
        // descriptor NOT set...check if connection timed out
        if((_idleLimit > 0) && ((now - conn->getTimestamp())
                                > static_cast<int64_t>(_idleLimit)))
        {
          _connectionTimedOut(conn);
        }
      }

      // if socket is no longer connected, remove connection from list
      if(! sock->isConnected())
        iter = _connections->erase(iter);
      else
        ++iter;
    }
  }
}

/*
 */

void SocketSelector::_connectionClosed(Connection* conn)
{
  StreamSocket* sock = conn->getSocket();

  conn->close(true);
  _pool.release(sock);

  connectionClosed(conn);
}

/*
 */

void SocketSelector::_connectionTimedOut(Connection* conn)
{
  StreamSocket* sock = conn->getSocket();

  conn->close(true);
  _pool.release(sock);

  connectionTimedOut(conn);
}

/*
 */

void SocketSelector::exceptionOccurred(Connection* connection,
                                    const IOException& ex)
{
  connection->close(true);
}

/*
 */

void SocketSelector::dataSent(Connection* connection)
{
  // no-op
}

/*
 */

uint_t SocketSelector::writeAll(const byte_t* buf, size_t count)
{
  ScopedLock lock(_mutex);

  uint_t n = 0;

  for(ConnectionList::iterator iter = _connections->begin();
      iter != _connections->end();
      ++iter)
  {
    Connection *conn = *iter;

    if(conn->writeData(buf, count))
      ++n;
  }

  return(n);
}

/*
 */

const size_t Connection::DEFAULT_BUFFER_SIZE = 4096;

// static
const bool Connection::_isSameEndianness = ByteOrder::isBigEndian();

/*
 */

Connection::Connection(size_t bufferSize /* = DEFAULT_BUFFER_SIZE */)
  : _socket(NULL)
  , readBuffer(bufferSize)
  , writeBuffer(bufferSize)
  , _readLoMark(1)
  , _readHiMark(bufferSize)
  , _writeLoMark(1)
  , _writeHiMark(bufferSize)
  , _oobFlag(false)
  , _closePending(false)
  , _oobData(0)
  , _lastRecv(INT64_CONST(0))
{
}

/*
 */

Connection::~Connection()
{
}

/*
 */

bool Connection::writeData(ByteBuffer& buffer)
{
  ScopedLock guard(_writeLock);

  if(writeBuffer.getFree() < buffer.getRemaining())
    return(false);

  writeBuffer.write(buffer);

  _selector->wakeup();

  return(true);
}

/*
 */

bool Connection::writeData(const byte_t* buf, size_t count)
{
  ScopedLock lock(_writeLock);

  if(writeBuffer.getFree() < count)
    return(false);

  writeBuffer.write(buf, count);

  _selector->wakeup();

  return(true);
}

/*
 */

bool Connection::writeLine(const String& text)
{
  ScopedLock lock(_writeLock);

  CString cstr_text = text.toUTF8();

  if(writeBuffer.getFree() < (cstr_text.length() + 2))
    return(false);

  writeBuffer.write((const byte_t *)(cstr_text.data()), cstr_text.length());
  writeBuffer.write((const byte_t *)"\r\n", 2);

  _selector->wakeup();

  return(true);
}

/*
 */

void Connection::beginWrite()
{
  _writeLock.lock();
}

/*
 */

void Connection::endWrite()
{
  _writeLock.unlock();

  _selector->wakeup();
}

/*
 */

void Connection::cancelWrite()
{
  _writeLock.unlock();
}

/*
 */

size_t Connection::readData(ByteBuffer& buffer, bool fully /* = true */)
{
  ScopedLock lock(_readLock);

  size_t left = readBuffer.getRemaining();

  if(left == 0 || (fully && (left < buffer.getRemaining())))
    return(0);

  return(readBuffer.read(buffer));
}

/*
 */

size_t Connection::readData(byte_t* buf, size_t count, bool fully /* = true */)
{
  ScopedLock lock(_readLock);

  size_t left = readBuffer.getRemaining();

  if(left == 0 || (fully && (left < count)))
    return(0);

  return(readBuffer.read(buf, count));
}

/*
 */

size_t Connection::readLine(String& text, size_t maxLen)
{
  ScopedLock lock(_readLock);

  text.setLength(0);
  bool found = false;
  size_t len = readBuffer.peek('\n', maxLen, found);

  if(len > 0)
  {
    size_t ext = readBuffer.getReadExtent();
    size_t left = len;

    if(left > ext)
    {
      text.append((const char *)readBuffer.getReadPos(), ext);
      left -= ext;
      readBuffer.advanceReadPos(ext);
    }

    text.append((const char *)readBuffer.getReadPos(), left);
    readBuffer.advanceReadPos(left);
  }

  return(len);
}

/*
 */

uint32_t Connection::readUInt32()
{
  ScopedLock lock(_readLock);

  if(readBuffer.getRemaining() < sizeof(uint32_t))
    return(0);

  uint32_t val;

  readBuffer.read(reinterpret_cast<byte_t *>(&val), sizeof(val));
  if(!_isSameEndianness)
    ByteOrder::reverseBytes(val);

  return(val);
}

/*
 */

uint64_t Connection::readUInt64()
{
  ScopedLock lock(_readLock);

  if(readBuffer.getRemaining() < sizeof(uint64_t))
    return(0);

  uint64_t val;

  readBuffer.read(reinterpret_cast<byte_t *>(&val), sizeof(val));
  if(!_isSameEndianness)
    ByteOrder::reverseBytes(val);

  return(val);
}

/*
 */

void Connection::setReadLowWaterMark(size_t count)
{
  if((count > 0) && (count < _readHiMark))
    _readLoMark = count;
}

/*
 */

void Connection::setReadHighWaterMark(size_t count)
{
  if((count > _readLoMark) && (count <= readBuffer.getSize()))
    _readHiMark = count;
}

/*
 */

void Connection::setWriteLowWaterMark(size_t count)
{
  if((count > 0) && (count < _writeHiMark))
    _writeLoMark = count;
}

/*
 */

void Connection::setWriteHighWaterMark(size_t count)
{
  if((count > _writeLoMark) && (count <= readBuffer.getSize()))
    _writeHiMark = count;
}

/*
 */

void Connection::attach(SocketSelector* selector, StreamSocket* socket)
{
  _socket = socket;
  _selector = selector;
  _closePending = false;
}

/*
 */

bool Connection::isClosePending() const
{
  return(_closePending && writeBuffer.isEmpty());
}

/*
 */

void Connection::read()
{
  readBuffer.write(*_socket);
}

/*
 */

void Connection::write()
{
  writeBuffer.read(*_socket);
}

/*
 */

void Connection::readOOB()
{
  _oobData = _socket->readOOB();
}

/*
 */

void Connection::close(bool immediate /* = false */)
{
  if(_socket)
  {
    if(immediate)
      _socket->close();
    else
      _closePending = true;
  }
}

/*
 */

bool Connection::isReadLow() const
{
  ScopedLock lock(_readLock);

  return(readBuffer.getRemaining() < _readLoMark);
}

/*
 */

bool Connection::isReadHigh() const
{
  ScopedLock lock(_readLock);

  return(readBuffer.getRemaining() >= _readHiMark);
}

/*
 */

bool Connection::isWriteLow() const
{
  ScopedLock lock(_writeLock);

  return(writeBuffer.getRemaining() < _writeLoMark);
}

/*
 */

bool Connection::isWriteHigh() const
{
  ScopedLock lock(_writeLock);

  return(writeBuffer.getRemaining() >= _writeHiMark);
}

/*
 */

size_t Connection::getBytesAvailableToRead() const
{
  ScopedLock lock(_readLock);

  return(readBuffer.getRemaining());
}

/*
 */

size_t Connection::getBytesAvailableToWrite() const
{
  ScopedLock lock(_readLock);

  return(writeBuffer.getRemaining());
}

} // namespace ccxx
