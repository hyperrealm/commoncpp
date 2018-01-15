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

#include "commonc++/SearchPath.h++"
#include "commonc++/File.h++"

#include <algorithm>

namespace ccxx {

/*
 */

#ifdef CCXX_OS_WINDOWS
#define PS ";"
#else
#define PS ":"
#endif

const char *SearchPath::separator = PS;

const SearchPath SearchPath::empty;

/*
 */

SearchPath::SearchPath(const String& path /* = "" */)
{
  fromString(path);
}

/*
 */

SearchPath::SearchPath(const SearchPath& rhs)
{
  _path = rhs._path;
}

/*
 */

SearchPath::~SearchPath()
{
  clear();
}

/*
 */

void SearchPath::append(const String& dir)
{
  if(dir.isEmpty())
    return;

  String cdir = File::removeTrailingSeparators(dir);

  if(! contains(cdir))
  {
    _path << cdir;
    _pathstr = String::null;
  }
}

/*
 */

void SearchPath::prepend(const String& dir)
{
  if(dir.isEmpty())
    return;

  String cdir = File::removeTrailingSeparators(dir);

  if(! contains(cdir))
  {
    _path.insert(_path.begin(), cdir);
    _pathstr = String::null;
  }
}

/*
 */

bool SearchPath::contains(const String& dir) const
{
  String cdir = File::removeTrailingSeparators(dir);

  return(std::find(_path.begin(), _path.end(), cdir) != _path.end());
}

/*
 */

void SearchPath::remove(const String& dir)
{
  String cdir = File::removeTrailingSeparators(dir);

  _path.erase(std::find(_path.begin(), _path.end(), cdir));
  _pathstr = String::null;
}

/*
 */

void SearchPath::clear()
{
  _path.clear();
  _pathstr = String::null;
}

/*
 */

bool SearchPath::find(FileName& file) const
{
  for(StringVec::const_iterator iter = _path.begin();
      iter != _path.end();
      iter++)
  {
    String path = (*iter);

    file.setDirectory(path);

    if(File::isFile(file.getPathName()))
      return(true);
  }

  file.setDirectory(String::null);
  return(false);
}

/*
 */

String SearchPath::getDirectory(int index) const
{
  if((index < 0) || (index >= static_cast<int>(_path.size())))
    throw OutOfBoundsException();

  return(_path[index]);
}

/*
 */

String SearchPath::getPath() const
{
  if(! _pathstr)
  {
    bool first = true;
    _pathstr = "";

    for(StringVec::const_iterator iter = _path.begin();
        iter != _path.end();
        iter++)
    {
      if(! first)
        _pathstr += separator;
      else
        first = false;

      _pathstr += *iter;
    }
  }

  return(_pathstr);
}

/*
 */

void SearchPath::fromString(const String& path)
{
  clear();

  for(uint_t index = 0;;)
  {
    String dir = path.nextToken(index, separator);
    if(! dir)
      break;

    append(dir);
  }
}

/*
 */

SearchPath& SearchPath::operator=(const SearchPath& rhs)
{
  _path = rhs._path;
  _pathstr = String::null;

  return(*this);
}


} // namespace ccxx
