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

#include "commonc++/Stream.h++"
#include "commonc++/AsyncIOTask.h++"
#include "commonc++/System.h++"
#include "commonc++/UnsupportedOperationException.h++"

#ifdef CCXX_OS_POSIX
#ifdef HAVE_LSEEK64
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#endif

#include <unistd.h>
#include <sys/uio.h>

#include "commonc++/POSIX.h++"
#endif

#ifdef HAVE_AIO_H
#include <aio.h>
#endif

#include <cerrno>

namespace ccxx {

/*
 */

const uint_t Stream::MAX_IOBLOCK_COUNT = 16;

/*
 */

Stream::Stream()
  : _handle(CCXX_INVALID_FILE_HANDLE),
    _seekable(false),
    _canRead(false),
    _canWrite(false),
    _timeout(-1)
{
}

/*
 */

Stream::Stream(FileHandle handle, bool seekable /* = true */,
               bool readable /* = true */, bool writable /* = true */)
  : _handle(handle),
    _seekable(seekable),
    _canRead(readable),
    _canWrite(writable)
{
}

/*
 */

Stream::~Stream()
{
  close();
}

/*
 */

size_t Stream::read(ByteBuffer& buffer)
{
  size_t r = read(buffer.getPointer(), buffer.getRemaining());

  if(r > 0)
    buffer.bump(r);

  return(r);
}

/*
 */

size_t Stream::read(CharBuffer& buffer)
{
  size_t r = read(reinterpret_cast<byte_t *>(buffer.getPointer()),
                  buffer.getRemaining());

  if(r > 0)
    buffer.bump(r);

  return(r);
}

/*
 */

size_t Stream::read(byte_t* buffer, size_t buflen)
{
  if(! _canRead)
    throw EOFException();

#ifdef CCXX_OS_WINDOWS

  DWORD br = 0;

  HANDLE evt = ::CreateEvent(NULL, TRUE, FALSE, NULL);
  if(evt == NULL)
    throw IOException(System::getErrorString("CreateEvent"));

  OVERLAPPED ovl = { 0, 0, 0, 0, evt };

  if(_seekable)
  {
    // update the offset in the overlapped structure to be the current
    // file position

    int64_t pos = tell();
    ovl.Offset = (pos & 0xFFFFFFFF);
    ovl.OffsetHigh = ((pos >> 32) & 0xFFFFFFFF);
  }

  if(! ::ReadFile(_handle, static_cast<LPVOID>(buffer),
                  static_cast<DWORD>(buflen), &br, &ovl))
  {
    switch(::GetLastError())
    {
      case ERROR_IO_PENDING:
      {
        if(::WaitForSingleObjectEx(evt, _timeout >= 0 ? _timeout : INFINITE,
                                   TRUE)
           != WAIT_OBJECT_0)
        {
          // treat WAIT_FAILED as a timeout also

          ::CancelIo(_handle);
          ::CloseHandle(evt);
          throw TimeoutException();
        }

        BOOL r = ::GetOverlappedResult(_handle, &ovl, &br, FALSE);
        ::CloseHandle(evt);
        if(! r)
          throw IOException(System::getErrorString("GetOverlappedResult"));

        break;
      }

      case ERROR_BROKEN_PIPE:
        ::CancelIo(_handle);
        ::CloseHandle(evt);
        throw EOFException();
        break;

      case ERROR_HANDLE_EOF:
        throw EOFException();
        break;

      default:
        // some other error
        ::CancelIo(_handle);
        ::CloseHandle(evt);
        throw IOException(System::getErrorString("ReadFile"));
    }
  }
  else
    ::CloseHandle(evt);

  if(br == 0)
    throw TimeoutException();

  if(_seekable)
  {
    // update the seek position
    seek(br, SeekRelative);
  }

#else

  if(_timeout > 0)
    POSIX::waitForIO(_handle, _timeout, true);

  ssize_t br = 0;

  for(;;)
  {
    br = ::read(_handle, static_cast<void *>(buffer), buflen);

    if(br == 0)
      throw EOFException();
    else if(br < 0)
    {
      if(errno == EAGAIN)
        throw TimeoutException();
      else if(errno == EINTR)
        continue;
      else
        throw IOException(System::getErrorString("read"));
    }

    break;
  }

#endif

  return(static_cast<size_t>(br));
}

/*
 */

size_t Stream::readFully(byte_t* buffer, size_t buflen)
{
  size_t left = buflen;
  byte_t *pos = buffer;
  size_t total = 0;

  while(left > 0)
  {
    size_t n = read(pos, left);
    total += n;
    pos += n;
    left -= n;
  }

  return(total);
}

/*
 */

size_t Stream::readFully(ByteBuffer& buffer, size_t count /* = 0 */)
{
  size_t total = 0;

  if((count == 0) || (count > buffer.getRemaining()))
    count = buffer.getRemaining();

  size_t remain = count;

  while(remain > 0)
  {
    size_t r = read(buffer.getPointer(), remain);

    if(r > 0)
    {
      buffer.bump(r);
      total += r;
      remain -= r;
    }
  }

  return(total);
}

/*
 */

size_t Stream::readFully(CharBuffer& buffer, size_t count /* = 0 */)
{
  size_t total = 0;

  if((count == 0) || (count > buffer.getRemaining()))
    count = buffer.getRemaining();

  size_t remain = count;

  while(remain > 0)
  {
    size_t r = read(reinterpret_cast<byte_t *>(buffer.getPointer()), remain);

    if(r > 0)
    {
      buffer.bump(r);
      total += r;
      remain -= r;
    }
  }

  return(total);
}

/*
 */

size_t Stream::_readElemFully(byte_t* buffer, size_t size, size_t nelem,
                              size_t& partial)
{
  size_t pleft = (partial ? (size - partial) : 0);
  size_t r = readFully(buffer + partial, ((nelem * size) - partial));
  size_t n = 0;

  if(pleft)
  {
    if(r < pleft)
    {
      partial += r;
      return(0);
    }

    r -= pleft;
    ++n;
    partial = 0;
  }

  if(r > 0)
  {
    n += (r / size);
    partial = (r % size);
  }

  return(n);
}

/*
 */

size_t Stream::read(MemoryBlock* vec, uint_t count)
{
  if(! _canRead)
    throw EOFException();

  if((count < 1) || (count > MAX_IOBLOCK_COUNT))
    throw IOException();

#ifdef CCXX_OS_WINDOWS

  // Windows doesn't have scatter read like readv(); ReadFileScatter()
  // requires all the buffers to be the same size. So, we emulate it
  // with a loop.

  size_t total = 0;
  uint_t left = count;
  for(const MemoryBlock *blk = vec; left--; blk++)
  {
    DWORD br = 0;

    HANDLE evt = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    if(evt == NULL)
      throw IOException(System::getErrorString("CreateEvent"));

    OVERLAPPED ovl = { 0, 0, 0, 0, evt };

    if(_seekable)
    {
      // update the offset in the overlapped structure to be the current
      // file position

      int64_t pos = tell();
      ovl.Offset = (pos & 0xFFFFFFFF);
      ovl.OffsetHigh = ((pos >> 32) & 0xFFFFFFFF);
    }

    if(! ::ReadFile(_handle, (LPVOID)(blk->getBase()),
                    static_cast<DWORD>(blk->getSize()), &br, &ovl))
    {
      switch(::GetLastError())
      {
        case ERROR_IO_PENDING:
        {
          if(::WaitForSingleObjectEx(evt, _timeout >= 0 ? _timeout : INFINITE,
                                     TRUE)
             != WAIT_OBJECT_0)
          {
            // treat WAIT_FAILED as a timeout also

            ::CancelIo(_handle);
            ::CloseHandle(evt);
            throw TimeoutException();
          }

          BOOL r = ::GetOverlappedResult(_handle, &ovl, &br, FALSE);
          ::CloseHandle(evt);
          if(! r)
            throw IOException(System::getErrorString("GetOverlappedResult"));

          break;
        }

        case ERROR_HANDLE_EOF:
          throw EOFException();
          break;

        case ERROR_BROKEN_PIPE:
          ::CancelIo(_handle);
          ::CloseHandle(evt);
          throw EOFException();
          break;

        default:
          // some other error
          ::CancelIo(_handle);
          ::CloseHandle(evt);
          throw IOException(System::getErrorString("ReadFile"));
      }
    }
    else
      ::CloseHandle(evt);

    total += static_cast<size_t>(br);

    if(_seekable)
    {
      // update the seek position
      seek(br, SeekRelative);
    }

    if(br < blk->getSize()) // complete buffer wasn't written, so abort
      break;
  }

  return(total);

#else

  iovec *dv, iov[count];
  const MemoryBlock *sv;
  uint_t i;

  for(dv = iov, sv = vec, i = 0; i < count; dv++, sv++, i++)
  {
    dv->iov_base = (void *)(sv->getBase());
    dv->iov_len = sv->getSize();
  }

  if(_timeout > 0)
    POSIX::waitForIO(_handle, _timeout, true);

  ssize_t br = 0;

  for(;;)
  {
    br = ::readv(_handle, iov, count);

    if(br < 0)
    {
      if(errno == EAGAIN)
        throw TimeoutException();
      else if(errno == EINTR)
        continue;
      else
        throw IOException(System::getErrorString("readv"));
    }
    else if(br == 0)
      throw EOFException();

    break;
  }

  return(static_cast<size_t>(br));
#endif
}

/*
 */

size_t Stream::_readElem(byte_t* buffer, size_t size, size_t nelem,
                         size_t& partial)
{
  size_t pleft = (partial ? (size - partial) : 0);
  size_t r = read(buffer + partial, ((nelem * size) - partial));
  size_t n = 0;

  if(pleft)
  {
    if(r < pleft)
    {
      partial += r;
      return(0);
    }

    r -= pleft;
    ++n;
    partial = 0;
  }

  if(r > 0)
  {
    n += (r / size);
    partial = (r % size);
  }

  return(n);
}

/*
 */

size_t Stream::write(ByteBuffer& buffer)
{
  size_t w = write(buffer.getPointer(), buffer.getRemaining());

  if(w > 0)
    buffer.bump(w);

  return(w);
}

/*
 */

size_t Stream::write(CharBuffer& buffer)
{
  size_t w = write(reinterpret_cast<byte_t *>(buffer.getPointer()),
                   buffer.getRemaining());

  if(w > 0)
    buffer.bump(w);

  return(w);
}

/*
 */

size_t Stream::write(const byte_t* buffer, size_t buflen)
{
  if(! _canWrite)
    throw EOFException();

#ifdef CCXX_OS_WINDOWS

  DWORD bw = 0;

  HANDLE evt = ::CreateEvent(NULL, TRUE, FALSE, NULL);
  if(evt == NULL)
    throw IOException(System::getErrorString("CreateEvent"));

  OVERLAPPED ovl = { 0, 0, 0, 0, evt };

  if(_seekable)
  {
    // update the offset in the overlapped structure to be the current
    // file position

    int64_t pos = tell();
    ovl.Offset = (pos & 0xFFFFFFFF);
    ovl.OffsetHigh = ((pos >> 32) & 0xFFFFFFFF);
  }

  if(! ::WriteFile(_handle, (LPVOID)(buffer),
                   static_cast<DWORD>(buflen), &bw, &ovl))
  {
    switch(::GetLastError())
    {
      case ERROR_IO_PENDING:
      {
        if(::WaitForSingleObjectEx(evt, _timeout >= 0 ? _timeout : INFINITE,
                                   TRUE)
           != WAIT_OBJECT_0)
        {
          // treat WAIT_FAILED as a timeout also

          ::CancelIo(_handle);
          ::CloseHandle(evt);
          throw TimeoutException();
        }

        BOOL r = ::GetOverlappedResult(_handle, &ovl, &bw, FALSE);
        ::CloseHandle(evt);
        if(! r)
          throw IOException(System::getErrorString("GetOverlappedResult"));

        break;
      }

      case ERROR_BROKEN_PIPE:
        ::CancelIo(_handle);
        ::CloseHandle(evt);
        throw EOFException();
        break;

      default:
        // some other error
        ::CancelIo(_handle);
        ::CloseHandle(evt);
        throw IOException(System::getErrorString("WriteFile"));
    }
  }
  else
    ::CloseHandle(evt);

  if(_seekable)
  {
    // update the seek position
    seek(bw, SeekRelative);
  }

#else

  if(_timeout > 0)
    POSIX::waitForIO(_handle, _timeout, false);

  ssize_t bw = 0;

  for(;;)
  {
    bw = ::write(_handle, static_cast<const void *>(buffer), buflen);

    if(bw < 0)
    {
      if(errno == EAGAIN)
        throw TimeoutException();
      else if(errno == EINTR)
        continue;
      else
        throw IOException(System::getErrorString("write"));
    }

    break;
  }

#endif

  return(static_cast<size_t>(bw));
}

/*
 */

size_t Stream::read(byte_t* buffer, size_t buflen, int64_t offset,
                    AsyncIOTask& task)
{
  if(offset < 0)
    offset = 0;

  task.init(_handle, offset, buffer, buflen);
  return(_readAsync(buffer, buflen, offset, task));
}

/*
 */

size_t Stream::read(ByteBuffer& buffer, int64_t offset, AsyncIOTask& task)
{
  if(offset < 0)
    offset = 0;

  task.init(_handle, offset, &buffer);

  int r = _readAsync(buffer.getPointer(), buffer.getRemaining(), offset, task);

  if(r > 0)
    buffer.bump(r);

  return(r);
}

/*
 */

size_t Stream::_readAsync(byte_t* buffer, size_t buflen, int64_t offset,
                          AsyncIOTask& task)
{
#if defined(CCXX_OS_WINDOWS)

  DWORD br = 0;

  if(! ::ReadFile(_handle, static_cast<LPVOID>(buffer),
                  static_cast<DWORD>(buflen), &br,
                  task.getControlBlock()))
  {
    switch(::GetLastError())
    {
      case ERROR_IO_PENDING:
        // normal and expected
        task.setPending(true);
        return(0);

      case ERROR_HANDLE_EOF:
        throw EOFException();
        break;

      default:
        throw IOException(System::getErrorString("ReadFile"));
    }
  }
  else
  {
    // I/O finished synchronously, but we still must mark the task as
    // pending so that isCompleted() will return true.
    task.setPending(true);
    return(br);
  }

#elif defined(CCXX_OS_ANDROID)

  throw UnsupportedOperationException();

#else

  int r = aio_read(task.getControlBlock());
  if(r < 0)
    throw IOException(System::getErrorString("aio_read"));

  task.setPending(true);
  return(0);

#endif
}

/*
 */

size_t Stream::write(const byte_t* buffer, size_t buflen, int64_t offset,
                     AsyncIOTask& task)
{
  if(offset < 0)
    offset = 0;

  task.init(_handle, offset, const_cast<byte_t *>(buffer), buflen);
  return(_writeAsync(buffer, buflen, offset, task));
}

/*
 */

size_t Stream::write(ByteBuffer& buffer, int64_t offset, AsyncIOTask& task)
{
  if(offset < 0)
    offset = 0;

  task.init(_handle, offset, &buffer);
  size_t r = _writeAsync(buffer.getPointer(), buffer.getRemaining(), offset,
                         task);
  if(r > 0)
    buffer.bump(r);

  return(r);
}

/*
 */

size_t Stream::_writeAsync(const byte_t* buffer, size_t buflen, int64_t offset,
                           AsyncIOTask& task)
{
#if defined(CCXX_OS_WINDOWS)

  DWORD bw = 0;

  if(! ::WriteFile(_handle, (LPVOID)(buffer),
                   static_cast<DWORD>(buflen), &bw,
                   task.getControlBlock()))
  {
    switch(::GetLastError())
    {
      case ERROR_IO_PENDING:
        // normal and expected
        task.setPending(true);
        return(0);

      default:
        throw IOException(System::getErrorString("WriteFile"));
    }
  }
  else
  {
    // I/O finished synchronously, but we still must mark the task as
    // pending so that isCompleted() will return true.
    task.setPending(true);
    return(bw);
  }

#elif defined(CCXX_OS_ANDROID)

  throw UnsupportedOperationException();

#else

  int r = aio_write(task.getControlBlock());
  if(r < 0)
    throw IOException(System::getErrorString("aio_write"));

  task.setPending(true);
  return(0);

#endif
}

/*
 */

size_t Stream::writeFully(ByteBuffer& buffer)
{
  size_t total = 0;

  while(buffer.getRemaining() > 0)
  {
    size_t r = write(buffer.getPointer(), buffer.getRemaining());

    if(r > 0)
    {
      buffer.bump(r);
      total += r;
    }
  }

  return(total);
}

/*
 */

size_t Stream::writeFully(CharBuffer& buffer)
{
  size_t total = 0;

  while(buffer.getRemaining() > 0)
  {
    size_t r = write(reinterpret_cast<byte_t *>(buffer.getPointer()),
                     buffer.getRemaining());

    if(r > 0)
    {
      buffer.bump(r);
      total += r;
    }
  }

  return(total);
}

/*
 */

size_t Stream::writeFully(const byte_t* buffer, size_t buflen)
{
  size_t left = buflen;
  const byte_t *pos = buffer;
  size_t total = 0;

  while(left > 0)
  {
    size_t n = write(pos, left);
    total += n;
    pos += n;
    left -= n;
  }

  return(total);
}

/*
 */

size_t Stream::_writeElem(const byte_t* buffer, size_t size, size_t nelem,
                          size_t& partial)
{
  size_t pleft = (partial ? (size - partial) : 0);
  size_t r = write(buffer + partial, ((nelem * size) - partial));
  size_t n = 0;

  if(pleft)
  {
    if(r < pleft)
    {
      partial += r;
      return(0);
    }

    r -= pleft;
    ++n;
    partial = 0;
  }

  if(r > 0)
  {
    n += (r / size);
    partial = (r % size);
  }

  return(n);
}

/*
 */

size_t Stream::_writeElemFully(const byte_t* buffer, size_t size, size_t nelem,
                               size_t& partial)
{
  size_t pleft = (partial ? (size - partial) : 0);
  size_t r = writeFully(buffer + partial, ((nelem * size) - partial));
  size_t n = 0;

  if(pleft)
  {
    if(r < pleft)
    {
      partial += r;
      return(0);
    }

    r -= pleft;
    ++n;
    partial = 0;
  }

  if(r > 0)
  {
    n += (r / size);
    partial = (r % size);
  }

  return(n);
}

/*
 */

size_t Stream::write(const MemoryBlock* vec, uint_t count)
{
  if(! _canWrite)
    throw EOFException();

  if((count < 1) || (count > MAX_IOBLOCK_COUNT))
    throw IOException();

#ifdef CCXX_OS_WINDOWS

  // Windows doesn't have gather write like writev(); WriteFileGather()
  // requires all the buffers to be the same size. So, we emulate it
  // with a loop.

  size_t total = 0;
  uint_t left = count;
  for(const MemoryBlock *blk = vec; left--; blk++)
  {
    DWORD bw = 0;

    HANDLE evt = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    if(evt == NULL)
      throw IOException(System::getErrorString("CreateEvent"));

    OVERLAPPED ovl = { 0, 0, 0, 0, evt };

    if(_seekable)
    {
      // update the offset in the overlapped structure to be the current
      // file position

      int64_t pos = tell();
      ovl.Offset = (pos & 0xFFFFFFFF);
      ovl.OffsetHigh = ((pos >> 32) & 0xFFFFFFFF);
    }

    if(! ::WriteFile(_handle, (LPVOID)(blk->getBase()),
                     static_cast<DWORD>(blk->getSize()), &bw, &ovl))
    {
      switch(::GetLastError())
      {
        case ERROR_IO_PENDING:
        {
          if(::WaitForSingleObjectEx(evt, _timeout >= 0 ? _timeout : INFINITE,
                                     TRUE)
             != WAIT_OBJECT_0)
          {
            // treat WAIT_FAILED as a timeout also

            ::CancelIo(_handle);
            ::CloseHandle(evt);
            throw TimeoutException();
          }

          BOOL r = ::GetOverlappedResult(_handle, &ovl, &bw, FALSE);
          ::CloseHandle(evt);
          if(! r)
            throw IOException(System::getErrorString("GetOverlappedResult"));

          break;
        }

        case ERROR_BROKEN_PIPE:
          ::CancelIo(_handle);
          ::CloseHandle(evt);
          throw EOFException();
          break;

        default:
          // some other error
          ::CancelIo(_handle);
          ::CloseHandle(evt);
          throw IOException(System::getErrorString("WriteFile"));
      }
    }
    else
      ::CloseHandle(evt);

    total += static_cast<size_t>(bw);

    if(_seekable)
    {
      // update the seek position
      seek(bw, SeekRelative);
    }

    if(bw < blk->getSize()) // complete buffer wasn't written, so abort
      break;
  }

  return(total);

#else

  iovec *dv, iov[count];
  const MemoryBlock *sv;
  uint_t i;

  for(dv = iov, sv = vec, i = 0; i < count; dv++, sv++, i++)
  {
    dv->iov_base = (void *)(sv->getBase());
    dv->iov_len = sv->getSize();
  }

  if(_timeout > 0)
    POSIX::waitForIO(_handle, _timeout, false);

  ssize_t bw = 0;

  for(;;)
  {
    bw = ::writev(_handle, iov, count);

    if(bw < 0)
    {
      if(errno == EAGAIN)
        throw TimeoutException();
      else if(errno == EINTR)
        continue;
      else
        throw IOException(System::getErrorString("writev"));
    }

    break;
  }

  return(static_cast<size_t>(bw));

#endif
}

/*
 */

int64_t Stream::seek(int64_t offset, SeekMode mode /* = SeekAbsolute */)
{
  if(! _seekable)
    throw IOException("Unsupported operation");

#ifdef CCXX_OS_WINDOWS

  LONG offLo = static_cast<LONG>(offset & 0xFFFFFFFF);
  LONG offHi = static_cast<LONG>((offset >> 32) & 0xFFFFFFFF);
  DWORD whence = 0;

  switch(mode)
  {
    case SeekAbsolute:
      whence = FILE_BEGIN;
      break;

    case SeekRelative:
      whence = FILE_CURRENT;
      break;

    case SeekEnd:
      whence = FILE_END;
      break;
  }

  DWORD off = ::SetFilePointer(_handle, offLo, &offHi, whence);
  if(off == 0xFFFFFFFF)
  {
    if(::GetLastError() != NO_ERROR)
      throw IOException(System::getErrorString("SetFilePointer"));
  }

  return(static_cast<int64_t>(off) | (static_cast<int64_t>(offHi) << 32));

#else

  int whence = 0;

  switch(mode)
  {
    case SeekAbsolute:
      whence = SEEK_SET;
      break;

    case SeekRelative:
      whence = SEEK_CUR;
      break;

    case SeekEnd:
      whence = SEEK_END;
      break;
  }

#ifdef HAVE_LSEEK64
  off64_t off = ::lseek64(_handle, static_cast<off64_t>(offset), whence);
#else
  off_t off = ::lseek(_handle, static_cast<off_t>(offset), whence);
#endif

  if(off < 0)
    throw IOException(System::getErrorString("lseek"));

  return(static_cast<int64_t>(off));

#endif
}

/*
 */

int64_t Stream::tell()
{
  return(seek(0, SeekRelative));
}

/*
 */

void Stream::close(IOMode mode /* = IOReadWrite */)
{
  if(!(_canRead || _canWrite))
    return; // already closed

  switch(mode)
  {
    case IORead:
      _canRead = false;
      break;

    case IOWrite:
      _canWrite = false;
      break;

    case IOReadWrite:
      _canRead = false;
      _canWrite = false;
      break;
  }

  if(!(_canRead || _canWrite))
  {
#ifdef CCXX_OS_WINDOWS

    ::CloseHandle(_handle);
    _handle = NULL;

#else

    ::close(_handle);
    _handle = -1;

#endif
  }
}

/*
 */

void Stream::_init(FileHandle handle, bool seekable, bool readable,
                   bool writable)
{
  _handle = handle;
  _seekable = seekable;
  _canRead = readable;
  _canWrite = writable;
}

/*
 */

void Stream::setTimeout(timespan_ms_t timeout)
{
  _timeout = timeout;
}


} // namespace ccxx
