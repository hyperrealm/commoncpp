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

#include "commonc++/CString.h++"

#include <cstring>

#include "commonc++/AtomicCounter.h++"
#include "commonc++/OutOfBoundsException.h++"

namespace ccxx {

/*
 */

CString::CString()
  : _str(NULL),
    _refs(new AtomicCounter(1))
{
}

/*
 */

CString::CString(const char* str)
  : _str(str),
    _refs(new AtomicCounter(1))
{
}

/*
 */

CString::CString(const CString& other)
  : _str(other._str),
    _refs(other._refs)
{
  ++(*_refs);
}

/*
 */

CString::~CString()
{
  _release();
}

/*
 */

CString& CString::operator=(const CString& other)
{
  if(other._str != _str)
  {
    _release();
    _str = other._str;
    _refs = other._refs;
    ++(*_refs);
  }

  return(*this);
}

/*
 */

char CString::operator[](int index) const
{
  if((_str == NULL) || (index < 0)
     || (std::strlen(_str) < static_cast<size_t>(index)))
    throw OutOfBoundsException();

  return(*(_str + index));
}

/*
 */

size_t CString::length() const
{
  return(_str ? std::strlen(_str) : 0);
}

/*
 */

void CString::_release()
{
  if(--(*_refs) == 0)
  {
    if(_str != NULL)
      delete[] _str;

    delete _refs;
  }
}


} // namespace ccxx
