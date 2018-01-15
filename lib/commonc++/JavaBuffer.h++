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

#ifndef __ccxx_JavaBuffer_hxx
#define __ccxx_JavaBuffer_hxx

#include <commonc++/Common.h++>
#include <commonc++/Buffer.h++>
#include <commonc++/JavaVirtualMachine.h++>

namespace ccxx {

/**
 * An implementation of Buffer representing a Java direct buffer.
 *
 * @author Mark Lindner
 */
class COMMONCPPJVM_API JavaBuffer : public ByteBuffer
{
 public:

  /**
   * Construct a new JavaBuffer.
   *
   * @param jvm The JavaVirtualMachine in which the buffer is being
   * created.
   * @param size The size of the buffer, in bytes.
   * @throw JavaException If the buffer cannot be constructed.
   */
  JavaBuffer(JavaVirtualMachine* jvm, size_t size);

  /** Destructor. */
  ~JavaBuffer();

  /**
   * @throw JavaException If the buffer cannot be resized.
   */
  void setSize(size_t newSize);

  /** Get the JVM object representing the Java direct buffer. */
  inline jobject getJavaObject()
  { return(_javaBuffer); }

 protected:

  /** @cond INTERNAL */
  jobject _javaBuffer;
  JavaVirtualMachine *_jvm;
  /** @endcond */

 private:

  void destroyJavaBuffer();
  void createJavaBuffer();
};

} // namespace ccxx

#endif // __ccxx_JavaBuffer_hxx
