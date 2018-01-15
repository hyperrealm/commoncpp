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

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#include "atomic.h"

/*
 */

inline int32_t atomic_add(int32_t *ptr, int32_t val)
{
  int32_t r = 0;

  asm volatile ("lock; xaddl %0,%1"
                : "=r"(r), "=m"(*ptr) // outputs
                : "0"(val), "m"(*ptr)   // inputs
                : "memory", "cc");

  return(r + val);
}

/*
 */

int32_t atomic_increment(int32_t *ptr)
{
  return(atomic_add(ptr, 1));
}

/*
 */

int32_t atomic_decrement(int32_t *ptr)
{
  return(atomic_add(ptr, -1));
}

/*
 */

int32_t atomic_set(int32_t *ptr, int32_t val)
{
  *ptr = val;

  /*

    .text
    .globl atomic_set
    .align 4
atomic_set:
    movl 4(%esp), %ecx
    movl 8(%esp), %eax
    lock
    xchgl %eax, (%ecx)
    ret
  */

  return(val);
}

/*
 */

int32_t atomic_cas(int32_t *ptr, int32_t val, int32_t cmp)
{
  int32_t r;

  asm volatile ("lock; cmpxchgl %1, %2"
                : "=a"(r)                         // outputs
                : "r"(val), "m"(*(ptr)), "0"(cmp) // inputs
                : "memory", "cc");
  return(r);
}

/*
 */

int32_t atomic_swap(int32_t *ptr, int32_t val)
{
  int32_t r = val;

  asm volatile ("lock; xchgl %0, %1"
                : "=r"(r)              // outputs
                : "m"(*(ptr)), "0"(r)  // inputs
                : "memory");
  return(r);
}
