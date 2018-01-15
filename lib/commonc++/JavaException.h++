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

#ifndef __ccxx_JavaException_hxx
#define __ccxx_JavaException_hxx

#include <commonc++/Common.h++>
#include <commonc++/Exception.h++>

namespace ccxx {

/**
 * A base class for all Java Virtual Machine exceptions.
 *
 * @author Mark Lindner
 */
class COMMONCPPJVM_API JavaException : public Exception
{
 public:

  /** Construct a new JavaException with an optional cause message. */
  JavaException(String message = String::empty);

  inline virtual void write(std::ostream& stream) const
  { _write(stream, "JavaException"); }
};

/**
 * An exception indicating that a Java method signature string was
 * malformed.
 */
class COMMONCPPJVM_API JavaSignatureException : public JavaException
{
 public:

  JavaSignatureException(String message = String::empty);

  inline void write(std::ostream& stream) const
  { _write(stream, "JavaSignatureException"); }
};

/**
 * An exception indicating that a requested Java class was not found.
 *
 * @author Mark Lindner
 */
class COMMONCPPJVM_API JavaClassNotFoundException : public JavaException
{
 public:

  JavaClassNotFoundException(String message = String::empty);

  inline void write(std::ostream& stream) const
  { _write(stream, "JavaClassNotFoundException"); }
};

/**
 * An exception indicating that a requested method of a Java class
 * was not found.
 *
 * @author Mark Lindner
 */
class COMMONCPPJVM_API JavaMethodNotFoundException : public JavaException
{
 public:

  JavaMethodNotFoundException(String message = String::empty);

  inline void write(std::ostream& stream) const
  { _write(stream, "JavaMethodNotFoundException"); }
};

/**
 * An exception indicating that a requested field of a Java class
 * was not found.
 *
 * @author Mark Lindner
 */
class COMMONCPPJVM_API JavaFieldNotFoundException : public JavaException
{
 public:

  JavaFieldNotFoundException(String message = String::empty);

  inline void write(std::ostream& stream) const
  { _write(stream, "JavaFieldNotFoundException"); }
};

/**
 * An exception indicating that an error occurred while
 * instantiating a Java class.
 *
 * @author Mark Lindner
 */
class COMMONCPPJVM_API JavaInstantiationException : public JavaException
{
 public:

  JavaInstantiationException(String message = String::empty);

  inline void write(std::ostream& stream) const
  { _write(stream, "JavaInstantiationException"); }
};

/**
 * An exception indicating that a Java exception occurred while invoking
 * a Java method.
 *
 * @author Mark Lindner
 */
class COMMONCPPJVM_API JavaInvocationException : public JavaException
{
 public:

  JavaInvocationException(String message = String::empty);

  inline void write(std::ostream& stream) const
  { _write(stream, "JavaInvocationException"); }
};

/**
 * An exception indicating an attempt at an invalid field access.
 *
 * @author Mark Lindner
 */
class COMMONCPPJVM_API JavaFieldAccessException : public JavaException
{
 public:

  JavaFieldAccessException(String message = String::empty);

  inline void write(std::ostream& stream) const
  { _write(stream, "JavaFieldAccessException"); }
};

/**
 * An exception indicating an attempt to store a value of an inappropriate
 * type into a Java array.
 *
 * @author Mark Lindner
 */
class COMMONCPPJVM_API JavaArrayAccessException : public JavaException
{
 public:

  JavaArrayAccessException(String message = String::empty);

  inline void write(std::ostream& stream) const
  { _write(stream, "JavaArrayAccessException"); }
};

/**
 * An exception indicating exhaustion of heap space in the Java Virtual
 * Machine.
 *
 * @author Mark Lindner
 */
class COMMONCPPJVM_API JavaOutOfMemoryException : public JavaException
{
 public:

  JavaOutOfMemoryException(String message = String::empty);

  inline void write(std::ostream& stream) const
  { _write(stream, "JavaOutOfMemoryException"); }
};

/**
 * An exception indicating an illegal monitor state.
 *
 * @author Mark Lindner
 */
class COMMONCPPJVM_API JavaMonitorException : public JavaException
{
 public:

  JavaMonitorException(String message = String::empty);

  inline void write(std::ostream& stream) const
  { _write(stream, "JavaMonitorException"); }
};

/**
 * An exception indicating a native thread attach or detach operation
 * failed.
 *
 * @author Mark Lindner
 */
class COMMONCPPJVM_API JavaThreadException : public JavaException
{
 public:

  JavaThreadException(String message = String::empty);

  inline void write(std::ostream& stream) const
  { _write(stream, "JavaThreadException"); }
};

/**
 * An exception indicating that a call was made from a native thread that
 * is not attached to the JVM.
 *
 * @author Mark Lindner
 */
class COMMONCPPJVM_API JavaNotAttachedException : public JavaException
{
 public:

  JavaNotAttachedException(String message = String::empty);

  inline void write(std::ostream& stream) const
  { _write(stream, "JavaNotAttachedException"); }
};

/**
 * An exception indicating that the raw bytecode for a class could not
 * be loaded because of a class format, bytecode verification, or security
 * error.
 *
 * @author Mark Lindner
 */
class COMMONCPPJVM_API JavaClassLoaderException : public JavaException
{
 public:

  /**
   * Construct a new JavaClassLoaderException with an optional cause
   * message.
   */
  JavaClassLoaderException(String message = String::empty);

  inline void write(std::ostream& stream) const
  { _write(stream, "JavaClassLoaderException"); }
};

} // namespace ccxx

#endif // __ccxx_JavaException_hxx
