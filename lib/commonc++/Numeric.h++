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

#ifndef __ccxx_Numeric_hxx
#define __ccxx_Numeric_hxx

#include <limits>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include <commonc++/OutOfBoundsException.h++>

namespace ccxx {

/**
 * %Numeric and bitwise utility functions. Algorithms taken from
 * http://aggregate.org/MAGIC
 *
 * @author Mark Lindner
 */
class Numeric
{
 public:

  /**
   * Calculate the next higher power of 2 for a value.
   *
   * @param value The value.
   * @return The smallest power of 2 that is greater than or equal to the
   * value, or 0 if the value is 0 or greater than 2^31.
   */
  static uint32_t nextPowerOf2(uint32_t value);

  /**
   * Find the least significant bit that is set in a value.
   *
   * @param value The value.
   * @return A value with the same least significant bit set as the input,
   * but with all other bits clear.
   */
  static uint32_t leastSignificantSetBit(uint32_t value);

  /**
   * Find the least significant bit that is set in a value.
   *
   * @param value The value.
   * @return A value with the same least significant bit set as the input,
   * but with all other bits clear.
   */
  static uint64_t leastSignificantSetBit(uint64_t value);

  /**
   * Find the most significant bit that is set in a value.
   *
   * @param value The value.
   * @return A value with the same most significant bit set as the input,
   * but with all other bits clear.
   */
  static uint32_t mostSignificantSetBit(uint32_t value);

  /**
   * Find the most significant bit that is set in a value.
   *
   * @param value The value.
   * @return A value with the same most significant bit set as the input,
   * but with all other bits clear.
   */
  static uint64_t mostSignificantSetBit(uint64_t value);

  /**
   * Count the number of bits that are set in a value.
   *
   * @param value The value.
   * @return The count of bits that are set in the value.
   */
  static uint_t countSetBits(uint32_t value);

  /**
   * Count the number of bits that are set in a value.
   *
   * @param value The value.
   * @return The count of bits that are set in the value.
   */
  static uint_t countSetBits(uint64_t value);

  /**
   * Count the number of leading zeroes in a value.
   *
   * @param value The value.
   * @return The number of leading zeroes in the value.
   */
  static uint_t countLeadingZeroes(uint32_t value);

  /**
   * Count the number of leading zeroes in a value.
   *
   * @param value The value.
   * @return The number of leading zeroes in the value.
   */
  static uint_t countLeadingZeroes(uint64_t value);

  /**
   * Count the number of trailing zeroes in a value.
   *
   * @param value The value.
   * @return The number of trailing zeroes in the value.
   */
  static uint_t countTrailingZeroes(uint32_t value);

  /**
   * Count the number of trailing zeroes in a value.
   *
   * @param value The value.
   * @return The number of trailing zeroes in the value.
   */
  static uint_t countTrailingZeroes(uint64_t value);

 private:

  Numeric();
  CCXX_COPY_DECLS(Numeric);
};

/**
 * A range-checked numeric cast. If the value is outside the range of
 * the target type, an exception is thrown.
 *
 * @param value The value to cast.
 * @return The value cast to the target type.
 * @throw OutOfBoundsException If the value is outside the range of the
 * target type.
 */
template<typename S, typename T>
  inline T numeric_cast(S value)
{
  if((value < std::numeric_limits<T>::min())
     || (value > std::numeric_limits<T>::max()))
    throw OutOfBoundsException();
  else
    return(static_cast<T>(value));
}

/**
 * A range-clipping numeric cast. If the value is outside the range of
 * the target type, it is clipped.
 *
 * @param value The value to cast.
 * @return The value cast to the target type.
 */
template<typename S, typename T>
  inline T numeric_clipping_cast(S value)
{
  if(value < std::numeric_limits<T>::min())
    return(std::numeric_limits<T>::min());
  else if(value > std::numeric_limits<T>::max())
    return(std::numeric_limits<T>::max());
  else
    return(static_cast<T>(value));
}

/**
 * A numeric range-clipping function. If the value is outside the specified
 * range, it is clipped.
 *
 * @param value The value to cast.
 * @param min The lower bound of the allowed range.
 * @param max The upper bound of the allowed range.
 * @return The (possibly clipped) value.
 */
template<typename T>
  inline T numeric_clip(T value, T min, T max)
{
  if(value < min)
    return(min);
  else if(value > max)
    return(max);
  else return(value);
}

} // namespace ccxx

#endif // __ccxx_Numeric_hxx
