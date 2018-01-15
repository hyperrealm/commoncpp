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

#include "commonc++/AllocationMap.h++"

namespace ccxx {

/*
 */

AllocationMap::AllocationMap(size_t size)
  : _bitset(size)
{
}

/*
 */

AllocationMap::~AllocationMap()
{
}

/*
 */

int AllocationMap::reserve()
{
  int pos = _bitset.nextClearBit(0);
  if(pos >= 0)
    _bitset.set(pos);

  return(pos);
}

/*
 */

bool AllocationMap::isReserved(uint_t index) const
{
  return(_bitset.isSet(index));
}

/*
 */

bool AllocationMap::release(uint_t index)
{
  if(! _bitset.isSet(index))
    return(false);

  _bitset.clear(index);
  return(true);
}


} // namespace ccxx
