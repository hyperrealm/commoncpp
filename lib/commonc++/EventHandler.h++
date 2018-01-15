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

#ifndef __ccxx_EventHandler_hxx
#define __ccxx_EventHandler_hxx

#include <commonc++/Common.h++>

namespace ccxx {

/**
 * A functor for event-handler callbacks. See EventHandlerDelegate for
 * an implementation that delegates to a method in an object.
 *
 * @author Mark Lindner
 */
template<class E> class EventHandler
{
 public:

  /** Destructor. */
  virtual ~EventHandler() { }

  /**
   * Function-call operator. The method simply delegates to <b>invoke()</b>.
   *
   * @param event The event.
   */
  inline void operator() (E event)
  { invoke(event); }

 protected:

  /**
   * Invoke the handler with an event.
   *
   * @param event The event.
   */
  virtual void invoke(E event) = 0;
};

/**
 * An implementation of EventHandler that delegates to a method in an
 * object. The object can be thought of as a pointer to an event handler
 * method that can be invoked as if it were a function; it allows
 * an object to declare one or more event-handling methods to be used as
 * callbacks.
 *
 * @author Mark Lindner
 */
template<class T, class E> class EventHandlerDelegate
  : public EventHandler<E>
{
 public:

  /**
   * Construct a new EventHandlerDelegate for an object.
   *
   * @param object The object.
   * @param func A member function (method) in the object which will serve
   * as the event handler.
   */
  EventHandlerDelegate(T* object, void (T::* func)(E))
    : _object(object), _func(func)
  {
  }

 protected:

  inline void invoke(E event)
  { (_object->*_func)(event); }

 private:

  T* _object;
  void (T::*_func)(E);
};

} // namespace ccxx

#endif // __ccxx_EventHandler_hxx
