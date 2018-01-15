/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2011  Mark A Lindner

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

#include "atomic.h"

#include <windows.h>

/*
 */

int32_t atomic_increment(int32_t *ptr)
{
  int32_t r = InterlockedIncrement(ptr);

  return(r);
}

/*
 */

int32_t atomic_decrement(int32_t *ptr)
{
  int32_t r = InterlockedDecrement(ptr);

  return(r);
}

/*
 */

int32_t atomic_set(int32_t *ptr, int32_t val)
{
  *ptr = val;

  return(val);
}

/*
 */

int32_t atomic_add(int32_t *ptr, int32_t val)
{
  int32_t r = InterlockedExchangeAdd(ptr, val);

  return(r + val);
}

/*
 */

int32_t atomic_cas(int32_t *ptr, int32_t val, int32_t cmp)
{
  return(InterlockedCompareExchange(ptr, val, cmp));
}

/*
 */

int32_t atomic_swap(int32_t *ptr, int32_t val)
{
  return(InterlockedExchange(ptr, val));
}
