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

#include "commonc++/FilePtr.h++"

#include <cstdio>

namespace ccxx {

/*
 */

FilePtr::FilePtr(const String& name, const String& attrs)
  : _fp(NULL)
{
#ifdef CCXX_OS_WINDOW

  _fp = ::_wfopen(name.data(), attrs.data());

#else

  CString cstr_name = name.toUTF8();
  CString cstr_attrs = attrs.toUTF8();

  _fp = std::fopen(cstr_name.data(), cstr_attrs.data());

#endif
}

/*
 */

FilePtr::FilePtr(const char* name, const char* attrs)
  : _fp(NULL)
{
  _fp = std::fopen(name, attrs);
}

/*
 */

FilePtr::FilePtr(FILE* fp)
  : _fp(fp)
{
}

/*
 */

FilePtr::~FilePtr()
{
  if(_fp)
    std::fclose(_fp);
}


} // namespace ccxx
