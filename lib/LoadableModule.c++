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

#include "commonc++/LoadableModule.h++"
#include "commonc++/FileName.h++"
#include "commonc++/System.h++"

#if defined CCXX_OS_POSIX
#include <dlfcn.h>
#endif

#include <cstring>
#include <cstdio>
#include <sstream>

namespace ccxx {

/*
 */

LoadableModule::LoadableModule(const String& path /* = String::null */)
  : _path(path),
    _open(false)
{
}

/*
 */

LoadableModule::~LoadableModule()
{
  close();
}

/*
 */

void LoadableModule::open()
{
  if(! _open)
  {
#if defined CCXX_OS_WINDOWS

    if(_path.isEmpty())
    {
      _handle = ::GetModuleHandle(NULL);
      if(! _handle)
        throw IOException(System::getErrorString("GetModuleHandle"));
    }
    else
    {
      _handle = ::LoadLibraryW(_path.data());
      if(! _handle)
        throw IOException(System::getErrorString("LoadLibrary"));
    }

#else

    CString cstr_path = _path.toUTF8();
    _handle = ::dlopen(_path.isEmpty() ? NULL : cstr_path.data(), RTLD_NOW);

    if(! _handle)
      throw IOException(String("dlopen: ") << dlerror());

#endif

    _open = true;
  }
}

/*
 */

void LoadableModule::close()
{
  if(_open)
  {
#if defined CCXX_OS_WINDOWS

    ::FreeLibrary(_handle);

#else

    ::dlclose(_handle);

#endif

    _handle = NULL;
    _open = false;
  }
}

/*
 */

void* LoadableModule::lookup(const char* symbol)
{
  if(! _open)
    return(NULL);

#if defined CCXX_OS_WINDOWS

  return((void *)(::GetProcAddress(_handle, symbol)));

#else

  return(::dlsym(_handle, symbol));

#endif
}

/*
 */

void* LoadableModule::lookup(const String& symbol)
{
  CString cstr_symbol = symbol.toUTF8();

  return(lookup(cstr_symbol.data()));
}

/*
 */

String LoadableModule::getExtension()
{
#if defined(CCXX_OS_WINDOWS)
  return("dll");
#else
  return("so");
#endif
}

/*
 */

String LoadableModule::getPrefix()
{
#ifdef CCXX_OS_WINDOWS
  return("");
#else
  return("lib");
#endif
}

/*
 */

String LoadableModule::fileNameForStem(const String& stem)
{
  String s;
  s << getPrefix() << stem << '.' << getExtension();

  return(s);
}

/*
 */

String LoadableModule::fileNameForStem(const String& stem,
                                        const Version& version)
{
  String s;

#if defined CCXX_OS_WINDOWS

  int v = version.getCurrent() - version.getAge();
  s << getPrefix() << stem << '-' << v << '.' << getExtension();

#else

  s << fileNameForStem(stem) << '.' << version.toString();

#endif

  return(s);
}

/*
 */

bool LoadableModule::isLoadableModuleFile(const String& fileName)
{
  FileName fn(fileName);

  if(! fn.getBaseName().startsWith(getPrefix()))
    return(false);

#if defined(CCXX_OS_MACOSX) || defined(CCXX_OS_WINDOWS)

  String ext = fn.getExtension();

#else

  // on POSIX systems, version number info can follow the filename extension,
  // e.g., .so(\.[0-9]+)*

  String name = fn.getFileName();
  size_t len = name.length();

  bool inNum = false;
  size_t n = len - 1;

  for(; n >= 0; --n)
  {
    Char c = name[n];

    if(c == '.')
    {
      if(inNum)
        inNum = false;
      else
        return(false);
    }
    else if(c.isDigit())
      inNum = true;
    else
      break;
  }

  if(n < 1)
    return(false);

  int m = name.lastIndexOf('.', n - 1);
  if(m < 0)
    return(false);

  String ext = name(m + 1, n + 1);

#endif

  return(ext == getExtension());
}


} // namespace ccx
