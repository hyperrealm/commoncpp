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

#include "commonc++/AsyncIOTask.h++"

#ifdef HAVE_AIO_H
#include <aio.h>
#endif

#include "commonc++/System.h++"
#include "commonc++/UnsupportedOperationException.h++"

namespace ccxx {

/*
 */

AsyncIOTask::AsyncIOTask()
  : _file(CCXX_INVALID_FILE_HANDLE),
    _pending(false),
    _haveResult(false),
    _buffer(NULL)
{
#if defined(CCXX_OS_WINDOWS)
  CCXX_ZERO(_overlapped);
#elif defined(HAVE_AIO_H)
  _aiocb = new struct aiocb;
  CCXX_ZERO(*_aiocb);
#endif
}

/*
 */

AsyncIOTask::~AsyncIOTask()
{
#if defined(CCXX_OS_WINDOWS)
  if(_overlapped.hEvent != NULL)
    ::CloseHandle(_overlapped.hEvent);
#elif defined(HAVE_AIO_H)
  delete _aiocb;
#endif
}

/*
 */

void AsyncIOTask::init(FileHandle file, int64_t offset, byte_t *buf,
                       size_t buflen)
{
  _buffer = NULL;
  _init(file, offset, buf, buflen);
}

/*
 */

void AsyncIOTask::init(FileHandle file, int64_t offset, ByteBuffer *buf)
{
  _buffer = buf;
  _init(file, offset, buf->getPointer(), buf->getRemaining());
}

/*
 */

void AsyncIOTask::_init(FileHandle file, int64_t offset, byte_t *buf,
                        size_t buflen)
{
  _file = file;
  _result = 0;
  _pending = false;
  _haveResult = false;

#if defined(CCXX_OS_WINDOWS)

  _overlapped.Internal = 0;
  _overlapped.InternalHigh = 0;
  _overlapped.Offset = static_cast<LONG>(offset & 0xFFFFFFFF);
  _overlapped.OffsetHigh = static_cast<LONG>((offset >> 32) & 0xFFFFFFFF);

  if(_overlapped.hEvent == NULL)
    _overlapped.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
  else
    ::ResetEvent(_overlapped.hEvent);

  _length = static_cast<DWORD>(buflen);

#elif defined(CCXX_OS_ANDROID)

  throw UnsupportedOperationException();

#else

  CCXX_ZERO(*_aiocb);
  _aiocb->aio_fildes = file;
  _aiocb->aio_offset = offset;
  _aiocb->aio_buf = buf;
  _aiocb->aio_nbytes = buflen;
  _aiocb->aio_sigevent.sigev_notify = SIGEV_NONE;

#endif
}

/*
 */

bool AsyncIOTask::isCompleted()
{
  if(_haveResult)
    return(true);

  if(! _pending)
    return(false);

#if defined(CCXX_OS_WINDOWS)

  bool completed = (HasOverlappedIoCompleted(&_overlapped) == TRUE);

#elif defined(CCXX_OS_ANDROID)

  bool completed = false;

#else

  // We treat errors as "completed" also, so that we can report the error
  // in _collectResult().
  bool completed = (::aio_error(_aiocb) != EINPROGRESS);

#endif

  if(completed)
    _collectResult();

  return(completed);
}

/*
 */

size_t AsyncIOTask::getBytesTransferred()
{
  if(! _pending && !_haveResult)
    return(0);

  return(_result);
}

/*
 */

void AsyncIOTask::cancel()
{
  if(_pending && !_haveResult)
  {

#if defined(CCXX_OS_WINDOWS)

#if _WIN32_WINNT >= 0x0600 // Vista or later
    if(! ::CancelIoEx(_file), &_overlapped)
      throw IOException(System::getErrorString("CancelIoEx"));
#else
    if(! ::CancelIo(_file))
      throw IOException(System::getErrorString("CancelIo"));
#endif

#elif defined(CCXX_OS_ANDROID)

    throw UnsupportedOperationException();

#else

    int r = ::aio_cancel(_file, _aiocb);
    if((r == AIO_CANCELED) || (r == AIO_ALLDONE))
      return;
    else if(r == AIO_NOTCANCELED)
      throw IOException("operation not cancellable");
    else
      throw IOException(System::getErrorString("aio_cancel"));

#endif

    _pending = false;
  }
}

/*
 */

void AsyncIOTask::waitFor(timespan_ms_t timeout)
{
  if(_pending && !_haveResult)
  {
    if(timeout < 0)
      timeout = 0;

#if defined(CCXX_OS_WINDOWS)

    DWORD r = ::WaitForSingleObjectEx(_overlapped.hEvent,
                                      static_cast<DWORD>(timeout),
                                      TRUE);
    if(r != WAIT_OBJECT_0)
      throw TimeoutException();

#elif defined(CCXX_OS_ANDROID)

    throw UnsupportedOperationException();

#else

    struct timespec ts;
    ts.tv_sec = timeout / 1000;
    ts.tv_nsec = (timeout % 1000) * 1000000;

    for(;;)
    {
      aiocb *list = _aiocb;
      int r = ::aio_suspend(&list, 1, &ts);
      if(r < 0)
      {
        if(errno != EINTR)
          break;

        ts.tv_sec = ts.tv_nsec = 0; // no delay when restarting system call
        continue;
      }
      else
        throw TimeoutException();
    }
#endif

    _collectResult();
  }
}

/*
 */

void AsyncIOTask::_collectResult()
{
  if(! _haveResult)
  {
    // Collect the results.

    _result = 0;

#if defined(CCXX_OS_WINDOWS)

    DWORD bytes;
    if(! ::GetOverlappedResult(_file, &_overlapped, &bytes, TRUE))
    {
      switch(::GetLastError())
      {
        case ERROR_HANDLE_EOF:
          throw EOFException();
          break;
        case ERROR_OPERATION_ABORTED:
          throw InterruptedException();
          break;
        default:
          throw IOException("GetOverlappedResult");
      }
    }

#elif defined(CCXX_OS_ANDROID)

    int bytes = 0;

    throw UnsupportedOperationException();

#else

    int r = ::aio_error(_aiocb);
    if(r == EINPROGRESS)
      throw IOException("I/O operation in progress");
    else if(r == ECANCELED)
      throw InterruptedException();
    else if(r != 0)
      throw IOException(System::getErrorString("aio_*"));

    ssize_t bytes = ::aio_return(_aiocb);
    if(bytes < 0)
    {
      errno = bytes;
      throw IOException(System::getErrorString("aio_*"));
    }
    else if(bytes == 0)
      throw EOFException();

#endif

    _result = static_cast<size_t>(bytes);
    _haveResult = true;

    if(_buffer)
      _buffer->bump(_result);
  }
}

} // namespace ccxx
