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

#ifndef __ccxx_ThreadLocalBuffer_hxx
#define __ccxx_ThreadLocalBuffer_hxx

#include <commonc++/Common.h++>
#include <commonc++/Buffer.h++>
#include <commonc++/ThreadLocal.h++>

namespace ccxx {

/**
 * Thread-local data buffer. The buffer exists as a separate instance for
 * each calling thread.
 *
 * @author Mark Lindner
 */
template <typename T> class ThreadLocalBuffer : public ThreadLocal<Buffer<T> >
{
 public:

  /**
   * Construct a new ThreadLocalBuffer with the given size.
   *
   * @param size The size of the buffer, in bytes.
   */
  ThreadLocalBuffer(size_t size)
    : _size(size)
  { }

  /** Destructor. */
  virtual ~ThreadLocalBuffer() { }

 protected:

  Buffer<T>* initialValue()
  { return(new Buffer<T>(_size)); }

 private:

  size_t _size;
};

typedef ThreadLocalBuffer<byte_t> ThreadLocalByteBuffer;

} // namespace ccxx

#endif // __ccxx_ThreadLocalBuffer_hxx
