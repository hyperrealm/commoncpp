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

#ifndef __ccxx_DynamicObjectPool_hxx
#define __ccxx_DynamicObjectPool_hxx

#include <commonc++/Common.h++>
#include <commonc++/ObjectPool.h++>

#include <typeinfo>

namespace ccxx {

/**
 * An object pool that dynamically allocates its
 * objects. DynamicObjectPool allocates objects on the heap, either
 * all at once when the pool is constructed, or on an as-needed
 * basis, up to the specified limit.
 *
 * The template parameter <b>T</b> must be a class which has a default
 * constructor. This constructor is used to create new instances of
 * the object.
 *
 * @author Mark Lindner
 */
template <class T> class DynamicObjectPool : public ObjectPool<T>
{
 public:

  /**
   * Construct a new DynamicObjectPool of the given size.
   *
   * @param size The maximum number of objects to allocate in the pool.
   * @param resetFunc A function that will be used to "reset" objects to
   * their initial state when they are returned to the pool.
   * @param lazy A flag indicating whether objects will be allocated as
   * needed or initially all at once.
   */
  DynamicObjectPool(uint_t size, void (*resetFunc)(T*), bool lazy = false);

  /**
   * Destructor. Deletes all of the objects managed the pool,
   * whether or not they are reserved.
   */
  ~DynamicObjectPool();

  /**
   * Reserve an object from the pool.
   *
   * @return A pointer to the reserved object.
   * @throw ObjectPoolException If there are no more objects available in
   * the pool.
   */
  T* reserve();

  /**
   * Release an object back to the pool. The object's reset member function
   * is called before the object is released back to the pool.
   *
   * @param elem The object to release.
   * @throw ObjectPoolException If <i>elem</i> is <b>NULL</b>.
   */
  void release(T* elem);

 private:

  struct Link
  {
    T data;
    Link *next;
  };

  void (*_resetFunc)(T*);
  Link* _list;
  Link* _freeList;
  Link* _usedList;
};

#include <commonc++/DynamicObjectPoolImpl.h++>

} // namespace ccxx

#endif // __ccxx_DynamicObjectPool_hxx
