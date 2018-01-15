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

#ifndef __ccxx_JavaScope_hxx
#define __ccxx_JavaScope_hxx

#include <commonc++/Common.h++>
#include <commonc++/JavaContext.h++>

namespace ccxx {

/**
 * A convenience class for creating lexical Java scopes in native code.
 * JavaScope creates a new JVM stack frame in its constructor and releases
 * it in its destructor.
 *
 * A block of code can be conveniently wrapped in a new scope by using
 * the <b>java_scoped()</b> macro, as follows:
 *
 * <pre>
 * JavaContext ctx = ...;
 * int capacity = ...;
 *
 * java_scoped(ctx, capacity)
 * {
 *   // scoped code here
 * }
 * </pre>
 *
 * With this approach, any Java local references that are created within the
 * scoped block will be automatically freed when control leaves the block.
 *
 * @author Mark Lindner
 */
class /* COMMONCPP_API */ JavaScope
{
 public:

  /**
   * Construct a new JavaScope for the given JavaContext and local reference
   * capacity. A new stack frame is created in the JVM.
   */
  JavaScope(JavaContext& context, int capacity)
    : _context(context),
      _once(true)
  {
    _context.pushLocalFrame(capacity);
  }

  /** Destructor. Pops the stack frame. */
  ~JavaScope()
  {
    _context.popLocalFrame();
  }

  /** @cond INTERNAL */
  inline bool testOnce()
  { bool f = _once; _once = false; return(f); }
  /** @endcond */

 private:

  JavaContext& _context;
  bool _once;

  CCXX_COPY_DECLS(JavaScope);
}

} // namespace ccxx

/**
 * @def java_scoped(JC)
 *  Introduce a code block that is within a new Java VM scope for the
 * JavaContext JC and with initial capacity CAP.
 */

#ifdef java_scoped
#undef java_scoped
#endif java_scoped

#if (defined CCXX_OS_WINDOWS) && (defined _MSC_VER) && (_MSC_VER < 1400)

#define java_scoped(JC, CAP)                                            \
  for(JavaScope JC ## _scope(JC, CAP); JC ## _scope.testOnce();)

#else

#define java_scoped(JC, CAP)                                            \
  for(JavaScope CCXX_OS_UNIQUE_VARNAME(JC, __LINE__, scope)(JC, CAP);   \
      CCXX_OS_UNIQUE_VARNAME(JC, __LINE__, scope).testOnce();)

#endif

#endif // __ccxx_JavaScope_hxx
