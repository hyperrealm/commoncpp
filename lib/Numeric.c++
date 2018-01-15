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

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#include "commonc++/Numeric.h++"

namespace ccxx {

/*
 */

// static
uint32_t Numeric::nextPowerOf2(uint32_t value)
{
  if(value == 0)
    return(0);

  value |= (value >> 1);
  value |= (value >> 2);
  value |= (value >> 4);
  value |= (value >> 8);
  value |= (value >> 16);
  return(++value);
}

/*
 */

// static
uint32_t Numeric::leastSignificantSetBit(uint32_t value)
{
  return(value & -value);
}

/*
 */

// static
uint64_t Numeric::leastSignificantSetBit(uint64_t value)
{
  return(value & -value);
}

/*
 */

// static
uint32_t Numeric::mostSignificantSetBit(uint32_t value)
{
  value |= (value >> 1);
  value |= (value >> 2);
  value |= (value >> 4);
  value |= (value >> 8);
  value |= (value >> 16);
  return(value & ~(value >> 1));
}

/*
 */

// static
uint64_t Numeric::mostSignificantSetBit(uint64_t value)
{
  value |= (value >> 1);
  value |= (value >> 2);
  value |= (value >> 4);
  value |= (value >> 8);
  value |= (value >> 16);
  value |= (value >> 32);
  return(value & ~(value >> 1));
}

/*
 */

// static
uint_t Numeric::countSetBits(uint32_t value)
{
  value -= ((value >> 1) & 0x55555555);
  value = (((value >> 2) & 0x33333333) + (value & 0x33333333));
  value = (((value >> 4) + value) & 0x0F0F0F0F);
  value += (value >> 8);
  value += (value >> 16);
  return(value & 0x0000003F);
}

/*
 */

// static
uint_t Numeric::countSetBits(uint64_t value)
{
  value -= ((value >> 1) & 0x5555555555555555LL);
  value = (((value >> 2) & 0x3333333333333333LL)
           + (value & 0x3333333333333333LL));
  value = (((value >> 4) + value) & 0x0F0F0F0F0F0F0F0FLL);
  value += (value >> 8);
  value += (value >> 16);
  value += (value >> 32);
  return(static_cast<uint_t>(value & 0x000000FF));
}

/*
 */

// static
uint_t Numeric::countLeadingZeroes(uint32_t value)
{
  value |= (value >> 1);
  value |= (value >> 2);
  value |= (value >> 4);
  value |= (value >> 8);
  value |= (value >> 16);
  return(32 - countSetBits(value));
}

/*
 */

// static
uint_t Numeric::countLeadingZeroes(uint64_t value)
{
  value |= (value >> 1);
  value |= (value >> 2);
  value |= (value >> 4);
  value |= (value >> 8);
  value |= (value >> 16);
  value |= (value >> 32);
  return(64 - countSetBits(value));
}

/*
 */

// static
uint_t Numeric::countTrailingZeroes(uint32_t value)
{
  return(countSetBits((value & -value) - 1));
}

/*
 */

// static
uint_t Numeric::countTrailingZeroes(uint64_t value)
{
  return(countSetBits((value & -value) - 1));
}


} // namespace ccxx
