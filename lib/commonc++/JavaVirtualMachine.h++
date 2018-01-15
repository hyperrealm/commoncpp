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

#ifndef __ccxx_JavaVirtualMachine_hxx
#define __ccxx_JavaVirtualMachine_hxx

#include <commonc++/Common.h++>
#include <commonc++/Exception.h++>
#include <commonc++/SearchPath.h++>
#include <commonc++/ThreadLocal.h++>
#include <commonc++/JavaException.h++>
#include <commonc++/JavaContext.h++>

#include <jni.h>

namespace ccxx {

/**
 * An object that encapsulates (most of) the Java Native Interface
 * (JNI) to a Java Virtual Machine. Since the JNI Environment
 * pointers are stored in thread-local storage, instances of this
 * class may be used concurrently from multiple native threads,
 * provided that each native thread attaches to the JVM via
 * <b>attachThread()</b> before performing any JNI operations and
 * detaches via <b>detachThread()</b> afterwards.
 *
 * <p> For more information about the JNI, see <a
 * href="http://java.sun.com/docs/books/jni/html/jniTOC.html"
 * target="_top">The Java Native Interface Programmer's Guide and
 * Specification</a>.
 *
 * @author Mark Lindner
 */
class COMMONCPPJVM_API JavaVirtualMachine
{
 public:

  /**
   * Construct and initialize a new Java Virtual Machine.
   *
   * @param classPath The path for locating classes and JAR files.
   * @param linkerPath The path for locating native libraries.
   * @param initialHeapSize The initial JVM heap size, in megabytes.
   * @param maxHeapSize The maximum JVM heap size, in megabytes.
   * @param server A flag indicating whether the Server VM (<b>true</b>)
   * or Client VM (<b>false</b>) should be used.
   * @param verbose A flag indicating whether verbose mode should be enabled.
   * @param nojit A flag indicating whether the Just-in-Time (JIT) compiler
   * should be disabled.
   */
  JavaVirtualMachine(const SearchPath& classPath = SearchPath::empty,
                     const SearchPath& linkerPath = SearchPath::empty,
                     uint_t initialHeapSize = 0, uint_t maxHeapSize = 0,
                     bool server = false, bool verbose = false,
                     bool nojit = false);

  /**
   * Construct a JavaVirtualMachine object for the given JavaVM instance.
   *
   * @param jvm The JavaVM instance.
   */
  JavaVirtualMachine(JavaVM* jvm);

  /** Destructor. Shuts down and destroys the Java Virtual Machine. */
  ~JavaVirtualMachine();

  /**
   * Attach the calling native thread to the Java Virtual Machine if it is
   * not already attached, and return the JavaContext for the calling thread.
   *
   * @throw JavaException If an error occurs.
   */
  JavaContext attachThread();

  /**
   * Detach the calling native thread from the Java Virtual Machine. If the
   * thread is already detached, the method has no effect.
   *
   * @throw JavaException If an error occurs.
   */
  void detachThread();

  /** Determine if verbose mode is enabled for this JVM. */
  inline bool isVerbose() const
  { return(_verbose); }

 private:

  class JavaContextPtr : public ThreadLocal<JavaContext>
  {
   protected:

    JavaContext* initialValue()
    { return(new JavaContext(NULL)); }
  };

  JavaVM* _jvm;
  JavaContextPtr _context;
  bool _verbose;

  CCXX_COPY_DECLS(JavaVirtualMachine);
};

} // namespace ccxx

#endif // __ccxx_JavaVirtualMachine_hxx
