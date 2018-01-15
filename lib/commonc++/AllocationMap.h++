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

#ifndef __ccxx_AllocationMap_hxx
#define __ccxx_AllocationMap_hxx

#include <commonc++/Common.h++>
#include <commonc++/BitSet.h++>

namespace ccxx {

/**
 * An index allocation map.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API AllocationMap
{
 public:

  /**
   * Construct a new AllocationMap with the given size.
   *
   * @param size The number of indexes to allocate in the map.
   */
  AllocationMap(size_t size);

  /** Destructor. */
  ~AllocationMap();

  /**
   * Reserve an index from the map.
   *
   * @return The reserved index, or -1 if no free index is available.
   */
  int reserve();

  /**
   * Release an index.
   *
   * @param index The index to release.
   * @return <b>true</b> if the index was released, <b>false</b> if the
   * index wasn't reserved or if the index is out of range.
   */
  bool release(uint_t index);

  /**
   * Determine if a given index is reserved.
   *
   * @param index The index to test.
   * @return <b>true</b> if the index is reserved, <b>false</b> if it is
   * available.
   */
  bool isReserved(uint_t index) const;

  /**
   * Get the size of the map.
   *
   * @return The number of indexes in the map.
   */
  inline uint_t getSize() const
  { return(_bitset.getSize()); }

 private:

  BitSet _bitset;
};

} // namespace ccxx

#endif // __ccxx_AllocationMap_hxx
