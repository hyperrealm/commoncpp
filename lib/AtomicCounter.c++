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

#include "commonc++/AtomicCounter.h++"

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#include "atomic.h"

namespace ccxx {

/*
 */

AtomicCounter::AtomicCounter(int32_t value /* = 0 */)
{
  operator=(value);
}

/*
 */

AtomicCounter::~AtomicCounter()
{
}

/*
 */

int32_t AtomicCounter::operator++()
{
  // prefix
  return(atomic_increment(&_atomic));
}

/*
 */

int32_t AtomicCounter::operator++(int)
{
  // postfix
  int32_t r = atomic_increment(&_atomic);
  return(--r);
}

/*
 */

int32_t AtomicCounter::operator--()
{
  // prefix
  return(atomic_decrement(&_atomic));
}

/*
 */

int32_t AtomicCounter::operator--(int)
{
  // postfix
  int32_t r = atomic_decrement(&_atomic);
  return(++r);
}

/*
 */

int32_t AtomicCounter::operator+=(int32_t delta)
{
  return(atomic_add(&_atomic, delta));
}

/*
 */

int32_t AtomicCounter::operator-=(int32_t delta)
{
  return(atomic_add(&_atomic, -delta));
}

/*
 */

int32_t AtomicCounter::operator+(int32_t delta) const
{
  return(get() + delta);
}

/*
 */

int32_t AtomicCounter::operator-(int32_t delta) const
{
  return(get() - delta);
}

/*
 */

int32_t AtomicCounter::operator=(int32_t value)
{
  atomic_set(&_atomic, value);

  return(value);
}

/*
 */

int32_t AtomicCounter::set(int32_t value)
{
  return(atomic_set(&_atomic, value));
}

/*
 */

int32_t AtomicCounter::get() const
{
  return(atomic_add(&_atomic, 0));
}

/*
 */

int32_t AtomicCounter::swap(int32_t value)
{
  return(atomic_swap(&_atomic, value));
}

/*
 */

int32_t AtomicCounter::testAndSet(int32_t value, int32_t comparand)
{
  return(atomic_cas(&_atomic, value, comparand));
}

/*
 */

AtomicCounter::AtomicCounter(const AtomicCounter& other)
{
  set(other.get());
}

/*
 */

AtomicCounter& AtomicCounter::operator=(const AtomicCounter& other)
{
  set(other.get());

  return(*this);
}


} // namespace ccxx
