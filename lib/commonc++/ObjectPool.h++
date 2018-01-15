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

#ifndef __ccxx_ObjectPool_hxx
#define __ccxx_ObjectPool_hxx

#include <commonc++/Common.h++>
#include <commonc++/Exception.h++>

namespace ccxx {

/**
 * An exception indicating an ObjectPool error.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API ObjectPoolException : public Exception
{
 public:

  /** Construct a new ObjectPoolException with an optional cause message. */
  ObjectPoolException(String message = String::empty)
    : Exception(message)
  { }

  inline void write(std::ostream& stream) const
  { _write(stream, "ObjectPoolException"); }
};

/**
 * A base class for object pools. See DynamicObjectPool and
 * StaticObjectPool for concrete implementations.
 *
 * @author Mark Lindner
 */

template <typename T> class ObjectPool
{
 public:

  /**
   * Constructor.
   *
   * @param size The number of objects to allocate in the pool.
   */
  ObjectPool(uint_t size)
    : _size(size),
      _avail(size)
  { }

  /** Destructor. */
  virtual ~ObjectPool()
  { }

  /**
   * Reserve an object from the pool.
   *
   * @return The reserved object.
   * @throw ObjectPoolException If no objects are available in the pool.
   */
  virtual T* reserve() = 0;

  /**
   * Release an object back to the pool. The object's destructor is called
   * before the object is released back to the pool.
   *
   * @param elem The object to release.
   * @throw ObjectPoolException If <i>elem</i> is <b>NULL</b>.
   */
  virtual void release(T* elem) = 0;

  /**
   * Get the size of the pool, i.e., the total number of objects (both
   * reserved and available) in the pool.
   */
  inline uint_t getSize() const
  { return(_size); }

  /** Get the number of available (unreserved) objects in the pool. */
  inline uint_t getAvailable() const
  { return(_avail); }

 protected:

  /**
   * The size of the pool (i.e., the maximum number of objects managed
   * by the pool).
   */
  uint_t _size;

  /** The number of objects currently available (not reserved) in the pool. */
  uint_t _avail;
};

} // namespace ccxx

#endif // __ccxx_ObjectPool_hxx
