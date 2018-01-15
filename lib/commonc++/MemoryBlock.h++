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

#ifndef __ccxx_MemoryBlock_hxx
#define __ccxx_MemoryBlock_hxx

#include <commonc++/Common.h++>
#include <commonc++/Integers.h++>
#include <commonc++/OutOfBoundsException.h++>

namespace ccxx {

/**
 * An encapsulation of a block of memory. The class supports various
 * bounds-checked memory operations.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API MemoryBlock
{
 public:

  /**
   * Construct a new MemoryBlock.
   *
   * @param base The pointer to the beginning of the memory block.
   * @param size The size of the memory block, in bytes.
   */
  MemoryBlock(byte_t* base, size_t size);

  MemoryBlock();
  /** Destructor. */
  virtual ~MemoryBlock();

  /** Zero the memory block. */
  inline void zero()
  { fill(0); }

  /** Fill the memory block with the given byte value. */
  void fill(byte_t value);

  /** Get a pointer to the beginning of the memory block. */
  inline byte_t* getBase()
  { return(_base); }

  /** Get a pointer to the beginning of the memory block. */
  inline const byte_t* getBase() const
  { return(_base); }

  /** Get a pointer to the end of the memory block. */
  inline const byte_t *getEnd() const
  { return(_base + _size); }

  /** Get the size of the memory block. */
  inline size_t getSize() const
  { return(_size); }

  /** Equality operator. */
  bool operator==(const MemoryBlock& other) const;

  /** Inequality operator. */
  inline bool operator!=(const MemoryBlock& other) const
  { return(! operator==(other)); }

  /**
   * Bounds-checked access.
   *
   * @throw OutOfBoundsException If the index is out of bounds.
   */
  byte_t operator[](int index) const;

  /**
   * Bounds-checked access.
   *
   * @throw OutOfBoundsException If the index is out of bounds.
   */
  byte_t& operator[](int index);

  /**
   * Copy data from another memory block to this one. The number of
   * bytes copied is the minimum of the sizes of the two blocks. The
   * blocks may not overlap.
   *
   * @param other The block to copy from.
   * @return <b>true</b> if the copy succeeded, <b>false</b> if the
   * blocks overlap.
   */
  bool copyFrom(const MemoryBlock& other);

  /**
   * Copy data from another (possibly overlapping) memory block to
   * this one. The number of bytes copied is the minimum of the
   * sizes of the two blocks.
   *
   * @param other The block to copy from. Note that the copy may result
   * in both blocks being modified, if the source area overlaps the target
   * area.
   */
  void moveFrom(MemoryBlock& other);

  /**
   * Find the first occurrence of a given byte value in the memory block,
   * starting at a given index.
   *
   * @param val The value to search for.
   * @param startIndex The index to begin searching at.
   * @return The index of the first matching value, or -1 if not found.
   */
  int indexOf(byte_t val, uint_t startIndex = 0) const;

  /** Test if the base of the block is NULL. */
  inline bool operator!() const
  { return(isNull()); }

  /**
   * Test if the memory block is <b>null</b>.
   *
   * @return <b>true</b> if the memory block is <b>null</b>, <b>false</b>
   * otherwise.
   */
  bool isNull() const;

  /** The <b>null</b> memory block. */
  static const MemoryBlock null;
  
 protected:

  /** A pointer to the beginning of the memory block. */
  byte_t* _base;
  /** The size of the memory block, in bytes. */
  size_t _size;

 private:

//  CCXX_COPY_DECLS(MemoryBlock);
};

} // namespace ccxx

#endif // __ccxx_MemoryBlock_hxx
