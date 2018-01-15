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

#ifndef __ccxx_AsyncIOTask_hxx
#define __ccxx_AsyncIOTask_hxx

#include <commonc++/Common.h++>

#include <commonc++/Buffer.h++>
#include <commonc++/IOException.h++>
#include <commonc++/InterruptedException.h++>

#ifdef CCXX_OS_POSIX
struct aiocb;
#endif

namespace ccxx {

/**
 * An object representing an asynchronous I/O operation.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API AsyncIOTask
{
  friend class Stream;
  friend class AsyncIOPoller;

 public:

  /**
   * Construct a new AsyncIOTask. The to collect the results of the task,
   * either:
   * <ul>
   * <li>Periodically call isCompleted() until that method returns <b>true</b>,
   * or
   * <li>Call waitFor() to block until the operation completes.
   * </ul>
   */
  AsyncIOTask();

  /** Destructor. */
  ~AsyncIOTask();

  /**
   * Determine if the asynchronous I/O task has completed (either
   * successfully or with an error); and, if so, determine the result
   * of the operation. If the I/O task involved a ByteBuffer, that
   * buffer's position is bumped by the number of bytes that were
   * transferred.
   *
   * @return <b>true</b> if the task has completed; <b>false</b> if it is
   * still in progress.
   * @throw IOException If an I/O error occurred during the operation.
   * @throw InterruptedException If the I/O task was cancelled via a call
   * to cancel().
   */
  bool isCompleted();

  /**
   * Attempt to cancel the asynchronous I/O task. Whether a given I/O
   * task is actually cancellable is implementation-defined. Note that
   * due to limitations in the Win32 API, on Windows versions prior to Vista
   * this call will cancel <i>all</i> pending asynchronous I/O
   * operations for the stream associated with this task.
   *
   * @throw IOException If the task could not be cancelled.
   */
  void cancel();

  /**
   * Wait for an asynchronous I/O task to complete.
   *
   * @param timeout The maximum amount of time to wait, in milliseconds.
   * @throw TimeoutException If the operation times out.
   * @throw IOException If an I/O error occurred during the operation.
   * @throw InterruptedException If the I/O task was cancelled via a call
   * to cancel().
   */
  void waitFor(timespan_ms_t timeout);

  /**
   * Get the number of bytes transferred by the I/O task. This method
   * should only be called <i>after</i> it has been determined that the
   * task has completed via a call to isCompleted().
   *
   * @return The number of bytes that were transferred. Returns 0 if
   * the operation is still in progress, or if its result has not yet
   * been collected via a call to isCompleted().
   */
  size_t getBytesTransferred();

 private:

  void init(FileHandle file, int64_t offset, byte_t* buf, size_t buflen);
  void init(FileHandle file, int64_t offset, ByteBuffer* buf);

  void _init(FileHandle file, int64_t offset, byte_t* buf, size_t buflen);

  void _collectResult();

  inline void setPending(bool pending)
  { _pending = pending; }

  inline FileHandle getFileHandle()
  { return(_file); }

#if defined(CCXX_OS_WINDOWS)
  inline LPOVERLAPPED getControlBlock()
  { return(&_overlapped); }
#elif !defined(CCXX_OS_ANDROID)
  inline struct aiocb *getControlBlock()
  { return (_aiocb); }
#endif

  FileHandle _file;
  size_t _result;
  bool _pending;
  bool _haveResult;
  ByteBuffer* _buffer;
#if defined(CCXX_OS_WINDOWS)
  DWORD _length;
  mutable OVERLAPPED _overlapped;
#elif !defined(CCXX_OS_ANDROID)
  mutable struct aiocb* _aiocb;
#endif
};

} // namespace ccxx

#endif // __ccxx_AsyncIOTask_hxx
