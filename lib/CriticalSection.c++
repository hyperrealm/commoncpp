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

#include "commonc++/CriticalSection.h++"
#include "commonc++/Thread.h++"

namespace ccxx {

/*
 */

CriticalSection::CriticalSection()
{
#ifdef CCXX_OS_WINDOWS

  ::InitializeCriticalSection(&_lock);

#endif
}

/*
 */

CriticalSection::~CriticalSection()
{
#ifdef CCXX_OS_WINDOWS

  ::DeleteCriticalSection(&_lock);

#endif
}

/*
 */

void CriticalSection::enter()
{
#ifdef CCXX_OS_WINDOWS

  ::EnterCriticalSection(&_lock);

#else

  int32_t &count = *_counter;
  if(count > 0)
    ++count;
  else
  {
    for(;;)
    {
      if(_lock.testAndSet(1, 0) == 0)
        break;

      uint32_t ms = _rand.nextInt(50);
      Thread::sleep(50 + ms);
    }

    ++count;
  }

#endif
}

/*
 */

bool CriticalSection::tryEnter()
{
#ifdef CCXX_OS_WINDOWS

  return(::TryEnterCriticalSection(&_lock) == TRUE);

#else

  int32_t &count = *_counter;
  if(count > 0)
  {
    ++count;
    return(true);
  }
  else
  {
    if(_lock.testAndSet(1, 0) != 0)
      return(false);

    ++count;
    return(true);
  }

#endif
}

/*
 */

void CriticalSection::leave()
{
#ifdef CCXX_OS_WINDOWS

  ::LeaveCriticalSection(&_lock);

#else

  int32_t &count = *_counter;

  if(count > 0)
  {
    if(--count == 0)
      _lock.set(0);
  }

#endif
}


} // namespace ccxx
