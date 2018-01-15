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

#ifndef __ccxx_JavaThreadLocalBuffer_h
#define __ccxx_JavaThreadLocalBuffer_h

#include <commonc++/Common.h++>
#include <commonc++/JavaBuffer.h++>
#include <commonc++/ThreadLocal.h++>

namespace ccxx {

class JavaVirtualMachine; // fwd ref

/**
 * Thread-local Java direct buffer. The buffer exists as a separate
 * instance for each calling thread.
 *
 * @author Mark Lindner
 */
class COMMONCPPJVM_API JavaThreadLocalBuffer : public ThreadLocal<JavaBuffer>
{
 public:

  /**
   * Construct a new ThreadLocalJavaBuffer with the given size.
   *
   * @param jvm The Java Virtual Machine in which to create the buffer.
   * @param size The size of the buffer, in bytes.
   */
  JavaThreadLocalBuffer(JavaVirtualMachine* jvm, size_t size);

  /** Destructor. */
  ~JavaThreadLocalBuffer();

 protected:

  JavaBuffer* initialValue();

 private:

  JavaVirtualMachine* _jvm;
  size_t _size;
};

} // namespace ccxx

#endif // __ccxx_JavaThreadLocalBuffer_hxx
