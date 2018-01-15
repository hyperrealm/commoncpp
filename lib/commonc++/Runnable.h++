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

#ifndef __ccxx_Runnable_hxx
#define __ccxx_Runnable_hxx

#include <commonc++/Common.h++>

namespace ccxx {

/**
 * A runnable object. See the Thread class for more information.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Runnable
{
 public:

  /** Destructor. */
  virtual ~Runnable() { }

  /** Entry point. */
  virtual void run() = 0;
};

/**
 * An adapter object that implements the Runnable interface and
 * delegates to an arbitrary method (one that must have no arguments or
 * return value) in some other object. This allows a class to
 * have multiple methods that will execute asynchronously in their own
 * threads. For example, consider the class:
 * <pre>
 * class MyClass
 * {
 *   public:
 *
 *   MyClass();
 *   ~MyClass();
 *
 *   void thread1();
 *   void thread2();
 * };
 * </pre>
 * Given an instance of MyClass, the following construct causes a new
 * thread to begin executing in the <b>thread1()</b> method of that
 * instance.
 *
 * <pre>
 * MyClass mc;
 *
 * RunnableDelegate&lt;MyClass&gt; runnable(&mc, &MyClass::thread1);
 * Thread thread(&amp;runnable);
 * thread.start();
 * </pre>
 *
 * @author Mark Lindner
 */
template<class T> class RunnableDelegate : public Runnable
{
 public:

  /**
   * Construct a new RunnableDelegate which will call a method on another
   * object.
   *
   * @param object The object.
   * @param func A member function (method) in the object which will be
   * called by <b>run()</b>.
   */
  RunnableDelegate(T* object, void (T::* func)())
    : _object(object),
      _func(func)
  {
  }

  /** Entry point. */
  void run()
  {
    (_object->*_func)();
  }

 private:

  T* _object;
  void (T::*_func)();
};

/**
 * An adapter object similar to RunnableDelegate that additionally
 * takes an arbitrary argument which will be passed to the member
 * function.
 *
 * @author Mark Lindner
 */
template<class T, typename A> class RunnableClosure : public Runnable
{
 public:

  /**
   * Construct a new RunnableClosure which will call a method on another
   * object with the given argument.
   *
   * @param object The object.
   * @param func A member function (method) in the object which will be
   * called by <b>run()</b>.
   * @param argument The argument that will be passed to the member function.
   */
  RunnableClosure(T* object, void (T::* func)(A), A argument)
    : _object(object),
      _func(func),
      _argument(argument)
  { }

  /** Entry point. */
  void run()
  { (_object->*_func)(_argument); }

 private:

  T* _object;
  void (T::*_func)(A);
  A _argument;
};

} // namespace ccxx

#endif // __ccxx_Runnable_hxx
