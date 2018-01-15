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

#ifndef __ccxx_Buffer_hxx
#define __ccxx_Buffer_hxx

#include <commonc++/Common.h++>
#include <commonc++/AbstractBuffer.h++>
#include <commonc++/OutOfBoundsException.h++>

#include <cstring>
#include <algorithm>

namespace ccxx {

/**
 * A buffer for storing a contiguous sequence of elements. The
 * buffer has a <i>limit</i>, which defines a subrange of the entire
 * buffer that is available for reading or writing, and a
 * <i>position</i>, which is the offset of the next element to be
 * read or written.
 *
 * @author Mark Lindner
 */
template <typename T> class Buffer : public AbstractBuffer<T>
{
 public:

  /**
   * Construct a new <b>Buffer</b> with the given size.
   *
   * @param size The capacity of the buffer, in elements.
   */
  Buffer(uint_t size);

  /** Construct a new Buffer that will wrap the given array.
   *
   * @param data The array.
   * @param size The number of elements in the array.
   * @param takeOwnership A flag indicating whether the Buffer should take
   * ownership of this array and delete[] it in its destructor.
   */
  Buffer(T* data, size_t size, bool takeOwnership = true);

  /** Destructor. */
  virtual ~Buffer();

  /**
   * Clear the buffer. Sets the position to 0 and the limit to the end of
   * the buffer.
   */
  virtual void clear();

  /**
   * Flip the buffer. Sets the limit to the current position, and the
   * position to 0.
   */
  void flip();

  /** Rewind the buffer. Sets the position to 0. */
  void rewind();

  /** Set the limit. */
  void setLimit(uint_t limit);

  /** Set the position. */
  void setPosition(uint_t pos);

  /**
   * Bump (advance) the position by the given number of elements,
   * or to the limit, whichever occurs first.
   *
   * @param delta The number of elements.
   * @return The new position.
   */
  uint_t bump(uint_t delta);

  /**
   * Advance the position by the given number of elements, or to the
   * limit, whichever occurs first, filling the "skipped" elements
   * with zeroes.
   *
   * @param delta The number of elements.
   * @return The new position.
   */
  uint_t skip(uint_t delta);

  /**
   * Copy an item into the buffer at the current position and bump the
   * position by 1.
   *
   * @param item The item.
   * @return <b>true</b> if the buffer accepted the item, <b>false</b> if
   * there was not enough room.
   */
  bool put(const T& item);

  /**
   * Copy an item from the buffer at the current position and bump the
   * position by 1.
   *
   * @param item The item.
   * @return <b>true</b> if the buffer produced the item, <b>false</b> if
   * there was not enough data available.
   */
  bool get(T * item);

  /**
   * Copy an array of items into the buffer starting at the current
   * position, and bump the position by the number of items.
   *
   * @param items The items to place in the buffer.
   * @param count The number of items.
   * @return <b>true</b> if the buffer accepted the items, <b>false</b> if
   * there was not enough room.
   */
  bool put(const T* items, uint_t count);

  /**
   * Fill the buffer with a given item.
   *
   * @param item The item to fill with.
   * @param count The number of elements to fill. If 0 or greater than
   * the number of elements before the limit, the buffer is filled up to
   * the limit.
   */
  void fill(const T& item, uint_t count = 0);

  /**
   * Copy an array of items from the buffer starting at the current
   * position, and bump the position by the number of items.
   *
   * @param items The items to copy into.
   * @param count The number of items.
   * @return <b>true</b> if the buffer produced the items, <b>false</b> if
   * there was not enough data available.
   */
  bool get(T* items, uint_t count);

  /**
   * Scan forward from the current position for an element equal to the
   * given value.
   *
   * @param item The item to scan for.
   * @return The number of elements between the current position and the
   * matching item, if found; otherwise -1.
   */
  int peek(const T& item) const;

  /** Get a pointer to the element at the current position. */
  inline T* getPointer()
  { return(this->_data + this->_pos); }

  /** Get a pointer to the next element to be read or written. */
  inline const T* getPointer() const
  { return(this->_data + this->_pos); }

  /** Get the limit. */
  inline uint_t getLimit() const
  { return(_limit); }

  /**
   * Get the number of elements available to be read or written.
   *
   * @return The number of elements between the position and the limit.
   */
  inline uint_t getRemaining() const
  { return(_limit - _pos); }

  /**
   * Test if there are any elements available to be read or written.
   *
   * @return <b>true</b> if there are elements available, <b>false</b>
   * otherwise.
   */
  inline bool hasRemaining() const
  { return(_limit > _pos); }

  /** Get the position of the next element to be read or written. */
  inline uint_t getPosition() const
  { return(_pos); }

  /**
   * Get a reference to the element at the specified index.
   *
   * @param index The index of the element.
   * @throw OutOfBoundsException If the index is out of bounds.
   */
  T& operator[](int index);

  /**
   * Get a copy of the element at the specified index.
   *
   * @param index The index of the element.
   * @throw OutOfBoundsException If the index is out of bounds.
   */
  T operator[](int index) const;

 protected:

  /** The limit. */
  uint_t _limit;
  /** The position. */
  uint_t _pos;

 private:

  CCXX_COPY_DECLS(Buffer);
};

#include <commonc++/BufferImpl.h++>

typedef Buffer<byte_t> ByteBuffer;
typedef Buffer<char> CharBuffer;

} // namespace ccxx

#endif // __ccxx_Buffer_hxx
