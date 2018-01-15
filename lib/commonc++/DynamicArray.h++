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

#ifndef __ccxx_DynamicArray_hxx
#define __ccxx_DynamicArray_hxx

#include <commonc++/Common.h++>
#include <commonc++/OutOfBoundsException.h++>

#include <cstring>

namespace ccxx {

/**
 * A simple object wrapper for dynamically-allocated arrays.
 * See section 17.5.4 of <i>The C++ Programming Language</i>. The array
 * is heap-allocated and can be resized after construction. An array of
 * length zero is considered to be "null".
 *
 * @author Mark Lindner
 */
template <typename T> class DynamicArray
{
 public:

  typedef T value_type;
  typedef T* iterator;
  typedef const T* const_iterator;
  typedef T& reference;
  typedef const T& const_reference;

  /**
   * Construct a new DynamicArray with the given initial size.
   *
   * @param size The initial size, as a number of elements.
   */
  DynamicArray(uint_t size)
    : _size(0),
      _data(NULL)
  {
    resize(size, false);
  }

  /** Destructor. */
  ~DynamicArray()
  {
    delete[] _data;
  }

  /**
   * Resize the array to a new size. Allocates a new array, and optionally
   * copies as many elements from the old array as will fit to the new one.
   *
   * @param size The new size, as a number of elements.
   * @param copy If <b>true</b>, copy the elements from the old array.
   */
  void resize(uint_t size, bool copy = false)
  {
    if(size != _size)
    {
      T *old = _data;
      _size = size;
      _data = (_size > 0) ? new T[_size] : NULL;

      if(old)
      {
        if(copy)
          std::memcpy(reinterpret_cast<void *>(_data),
                      reinterpret_cast<void *>(old),
                      _size * sizeof(T));
        delete[] old;
      }
    }
  }

  /**
   * Release ownership of the array.
   *
   * @return The array.
   */
  T* release()
  {
    T* data = _data;
    _data = NULL;
    _size = 0;

    return(data);
  }

  /** Determine if this array is NULL. */
  bool isNull() const
  { return(_data == NULL); }

  /**
   * Get the underlying array.
   *
   * @return A pointer to the beginning of the array.
   */
  T* data()
  { return(_data); }

  /**
   * Get the underlying array.
   *
   * @return A pointer to the beginning of the array.
   */
  const T* data() const
  { return(_data); }

  /**
   * Get the size of the array.
   *
   * @return The size, in elements.
   */
  uint_t size() const
  { return(_size); }

  /**
   * Index operator.
   *
   * @throw OutOfBoundsException If the index is invalid.
   */
  T& operator[](int index)
  {
    if((index < 0) || (index >= static_cast<int>(_size)))
      throw OutOfBoundsException();

    return(_data[index]);
  }

  /** Index operator.
   *
   * @throw OutOfBoundsException If the index is invalid.
   */
  T operator[](int index) const
  {
    if((index < 0) || (index >= static_cast<int>(_size)))
      throw OutOfBoundsException();

    return(_data[index]);
  }

  /** Cast operator. */
  operator T*()
  { return(_data); }

  /** Cast operator. */
  operator const T*() const
  { return(_data); }

  /** Unary NOT operator. */
  bool operator!() const
  { return(isNull()); }

  iterator begin()
  { return(_data); }

  const_iterator begin() const
  { return(_data); }

  iterator end()
  { return(_data + _size); }

  const_iterator end() const
  { return(_data + _size); }

 private:

  uint_t _size;
  T* _data;
};

} // namespace ccxx

#endif // __ccxx_DynamicArray_hxx
