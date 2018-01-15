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

#ifndef __ccxx_ScopedPtr_hxx
#define __ccxx_ScopedPtr_hxx

#include <commonc++/Common.h++>
#include <commonc++/NullPointerException.h++>

namespace ccxx {

/**
 * A non-copyable scoped pointer. Maintains a pointer to another
 * object, which is deleted automatically when the ScopedPtr itself
 * is destroyed.
 *
 * @author Mark Lindner
 */
template<class T> class ScopedPtr
{
 public:

  /** Construct a new, NULL ScopedPtr. */
  ScopedPtr()
    : _object(NULL)
  { }

  /** Construct a new ScopedPtr for the given object pointer. */
  explicit ScopedPtr(T* object)
    : _object(object)
  { }

  /** Destructor. */
  ~ScopedPtr()
  { delete _object; }

  /**
   * Dereference operator.
   *
   * @throw NullPointerException If this pointer is NULL.
   */
  T& operator*() const
  {
    if(! _object)
      throw NullPointerException();

    return(*_object);
  }

  /**
   * Pointer-to-member operator.
   *
   * @throw NullPointerException If this pointer is NULL.
   */
  T* operator->() const
  {
    if(! _object)
      throw NullPointerException();

    return(_object);
  }

  /** Reset the pointer. The old object, if any, is deleted. */
  void reset(T* object = NULL)
  {
    if(_object != object)
    {
      delete _object;
      _object = object;
    }
  }

  /** Get the value of the pointer. */
  T* get() const
  { return(_object); }

  /** Release ownership of the object. */
  T* release()
  {
    T* tmp = _object;
    _object = NULL;
    return(tmp);
  }

  /** Test if the pointer is NULL. */
  bool operator!() const
  { return(isNull()); }

  /** Boolean test operator. (Test if the pointer is non-NULL.) */
  operator const void*() const
  { return(_object ? this : NULL); }

  /** Test if the pointer is NULL. */
  bool isNull() const
  { return(_object == NULL); }

  /** Swap this pointer with another. */
  void swap(ScopedPtr& other)
  {
    T* tmp = other._object;
    other._object = _object;
    _object = tmp;
  }

 private:

  T* _object;

  CCXX_COPY_DECLS(ScopedPtr);
};

/** Swap two ScopedPtr values. */

template<class T> inline void swap(ScopedPtr<T>& a, ScopedPtr<T>& b)
{ a.swap(b); }

} // namespace ccxx

#endif // __ccxx_ScopedPtr_hxx
