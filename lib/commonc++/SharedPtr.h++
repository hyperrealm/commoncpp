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

#ifndef __ccxx_SharedPtr_hxx
#define __ccxx_SharedPtr_hxx

#include <commonc++/Common.h++>
#include <commonc++/AtomicCounter.h++>
#include <commonc++/NullPointerException.h++>

namespace ccxx {

/**
 * A threadsafe, reference-counting smart pointer.
 *
 * See sections 13.6.3.1 and 25.7 of <i>The C++ Programming Language</i>.
 *
 * @author Mark Lindner
 */
template<class T> class SharedPtr
{
 public:

  /** Construct a new SharedPtr for the given object pointer. */
  SharedPtr(T* object = NULL)
    : _object(object), _refs(new AtomicCounter(1))
  { }

  /** Copy constructor. */
  SharedPtr(const SharedPtr& other)
    : _object(other._object),
      _refs(other._refs)
  {
    ++(*_refs);
  }

  /** Destructor. */
  ~SharedPtr()
  {
    _release();
  }

  /**
   * Static cast. Performs a static cast on the object held by this
   * SharedPtr, and returns a new SharedPtr of the target type.
   */
  template<class U> SharedPtr<U> staticCast()
  { return SharedPtr<U>(*this, false); }

  /**
   * Dynamic cast. Performs a dynamic cast on the object held by this
   * SharedPtr, and returns a new SharedPtr of the target type.
   */
  template<class U> SharedPtr<U> dynamicCast()
  { return SharedPtr<U>(*this); }

  /** Assignment operator. */
  SharedPtr& operator=(const SharedPtr& other)
  {
    if(other._object == _object)
      return(*this);

    _release();
    _object = other._object;
    _refs = other._refs;
    ++(*_refs);
    return(*this);
  }

  /** Assignment operator. */
  SharedPtr& operator=(T* object)
  {
    if(_object != object)
    {
      _release();
      _object = object;
      _refs = new AtomicCounter(1);
    }

    return(*this);
  }

  /** Pointer-to-member operator. */
  T* operator->()
  { return(_object); }

  /** Pointer-to-member operator. */
  const T* operator->() const
  { return(_object); }

  /**
   * Dereference operator.
   *
   * @throw NullPointerException If this pointer is NULL.
   */
  T& operator*()
  {
    if(! _object)
      throw NullPointerException();

    return(*_object);
  }

  /** Dereference operator.
   *
   * @throw NullPointerException If this pointer is NULL.
   */
  const T& operator*() const
  {
    if(! _object)
      throw NullPointerException();
    return(*_object);
  }

  /** Get the value of the pointer. */
  T* get() const
  { return(_object); }

  /** Test if the pointer is NULL. */
  bool operator!() const
  { return(isNull()); }

  /** Boolean test operator. (Test if the pointer is non-NULL.) */
  operator const void *() const
  { return(_object ? this : NULL); }

  /** Test if the pointer is NULL. */
  bool isNull() const
  { return(_object == NULL); }

  /** Equality operator. Compares the object pointers. */
  bool operator==(const SharedPtr &other) const
  { return(_object == other._object); }

  /** Inequality operator. Compares the object pointers. */
  bool operator!=(const SharedPtr &other) const
  { return(! operator==(other)); }

  /** Get the reference count for this pointer. */
  int getRefCount() const
  { return(_object ? _refs->get() : 0); }

 private:

  template<class U> friend class SharedPtr;

  template<typename U>
    explicit SharedPtr(const SharedPtr<U> &other)
      : _object(dynamic_cast<T *>(other._object))
  {
    if(_object)
    {
      _refs = other._refs;
      ++(*_refs);
    }
    else
      _refs = new AtomicCounter(1);
  }

  template<typename U>
    SharedPtr(const SharedPtr<U> &other, bool tag)
      : _object(static_cast<T *>(other._object)),
        _refs(other._refs)
  {
    ++(*_refs);
  }

  void _release()
  {
    if(--(*_refs) == 0)
    {
      delete _object;
      delete _refs;
    }
  }

  T* _object;
  AtomicCounter* _refs;
};

} // namespace ccxx

#endif // __ccxx_SharedPtr_hxx
