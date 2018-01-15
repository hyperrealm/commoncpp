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

#ifndef __ccxx_RefSet_hxx
#define __ccxx_RefSet_hxx

#include <map>

#include <commonc++/Common.h++>

namespace ccxx {

/**
 * A reference-counted set. The set allows duplicate values, but it
 * reference counts them, so that a value must be removed the same number
 * of times that it was added in order to actually be removed from the set.
 * RefSet is a subclass of <b>std::map<T, int></b>.
 *
 * @author Mark Lindner
 */
template<typename T> class RefSet : public std::map<T, int>
{
 public:

  /** Construct a new RefSet. */
  RefSet();

  /** Destructor. */
  ~RefSet();

  /**
   * Add a value to the set. If the value is already in the set, its
   * reference count is incremented.
   *
   * @param value The value to add.
   * @return <b>true</b> if the value was added for the first time,
   * <b>false</b> if value was already in the set.
   */
  bool add(T value);

  /**
   * Remove a value from the set. Decrements the reference count for the
   * value, and removes the value from the set if the reference count has
   * reached 0.
   *
   * @param value The value to remove.
   * @return <b>true</b> if the value was removed from the set, <b>false</b>
   * if the value was not in the set or if the reference count for the
   * value has not reached 0.
   */
  bool remove(T value);

  /**
   * Determine if a value is present in the set.
   *
   * @param value The value.
   * @return <b>true</b> if the value is in the set, <b>false</b> otherwise.
   */
  bool contains(T value) const;

  /**
   * Get the reference count for a given value.
   *
   * @param value The value.
   * @return The reference count for the value, or 0 if the value is not in
   * the set.
   */
  int refCount(T value) const;
};

#include <commonc++/RefSetImpl.h++>

} // namespace ccxx

#endif // __ccxx_RefSet_hxx
