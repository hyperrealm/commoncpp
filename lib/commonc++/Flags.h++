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

#ifndef __ccxx_Flags_hxx
#define __ccxx_Flags_hxx

#include <commonc++/Common.h++>
#include <commonc++/OutOfBoundsException.h++>

namespace ccxx {

/**
 * A wrapper template for primitive integer types that represent flag
 * bitmasks.
 *
 * @author Mark Lindner
 */
template<typename T> class Flags
{
 public:

  /** Construct a new Flags object with an initial value of 0. */
  Flags(const T& flags = 0);

  /** Destructor. */
  ~Flags();

  /**
   * Test if the flag at the given index is set.
   *
   * @param index The flag index.
   * @return <b>true</b> if the flag is set, <b>false</b> otherwise.
   */
  bool isSet(int index) const;

  /**
   * Set or clear the flag at the given index.
   *
   * @param index The flag index.
   * @param on The new state for the flag.
   */
  void set(int index, bool on = true);

  /**
   * Compare the flags to a mask, returning true if any of the bits in the
   * mask are set in the flags.
   *
   * @param mask The bitmask.
   * @return <b>true</b> if the bitwise-AND of the flags and the mask is
   * nonzero.
   */
  bool anySet(const T& mask) const;

  /**
   * Compare the flags to a mask, returning true if all of the bits in the
   * mask are set in the flags.
   *
   * @param mask The bitmask.
   * @return <b>true</b> if the bitwise-AND of the flags and the mask is
   * equal to the mask.
   */
  bool allSet(const T& mask) const;

  /** Index operator. Equivalent to isSet(). */
  bool operator[](int index) const;

  class BitRef;

  /**
   * Index operator. Returns a reference to the given bit.
   *
   * @throw OutOfBoundsException If <code>index</code> is invalid.
   */
  BitRef operator[](int index);

  /**
   * Clear the flag at the given index.
   *
   * @param index The flag index.
   */
  void clear(int index);

  /** Clear all of the flags.
   */
  void clear();

  /** Cast operator. */
  operator T() const
  { return(_flags); }

  /** Assignment operator. */
  Flags& operator=(const T& flags);

  /** @cond INTERNAL */
  class BitRef
  {
   public:

    BitRef(Flags& flags, int index);
    BitRef& operator=(bool value);

   private:

    Flags& _flags;
    int _index;
  };
  /** @endcond */

 private:

  T _flags;
};

#include <commonc++/FlagsImpl.h++>

} // namespace ccxx

#endif // __ccxx_Flags_hxx

