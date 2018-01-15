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

#ifndef __ccxx_StaticObjectPool_hxx
#define __ccxx_StaticObjectPool_hxx

#include <commonc++/Common.h++>
#include <commonc++/ObjectPool.h++>

#include <cstring>
#include <typeinfo>

namespace ccxx {

/**
 * An object pool that allocates (presumably fixed-size) objects in
 * a contiguous memory buffer.
 *
 * This class is not suitable for objects which allocate additional
 * member data on the heap, as this data will be created each time
 * an object is reserved from the pool and destroyed when it is
 * released back to the pool; use DynamicObjectPool in this case.
 *
 * @author Mark Lindner
 */
template <typename T> class StaticObjectPool : public ObjectPool<T>
{
 public:

  /**
   * Construct a new StaticObjectPool of the given size.
   *
   * @param size The number of objects to allocate in the pool.
   */
  StaticObjectPool(uint_t size);

  /**
   * Destructor. Deallocates all of the objects in the pool. Destructors
   * will <i>not</i> be called for any objects still allocated from the
   * pool; therefore it is a programmig error to destroy a StaticObjectPool
   * before releasing all of its objects.
   */
  ~StaticObjectPool();

  /**
   * Reserve an object from the pool. The object's constructor is called
   * before the object is returned.
   *
   * @return A pointer to the reserved object.
   * @throw ObjectPoolException If there are no more objects available in
   * the pool.
   */
  T* reserve();

  /**
   * Release an object back to the pool. The object's destructor is called
   * before the object is released back to the pool.
   *
   * @param elem The object to release.
   * @throw ObjectPoolException If <i>elem</i> is <b>NULL</b>.
   */
  void release(T* elem);

 private:

  struct Link { Link* next; };
  size_t _elemsz;
  Link* _freeList;
  byte_t* _heap;
};

#include <commonc++/StaticObjectPoolImpl.h++>

} // namespace ccxx

#endif // __ccxx_StaticObjectPool_hxx
