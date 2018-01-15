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

#include "commonc++/FileName.h++"
#include "commonc++/File.h++"

namespace ccxx {

/*
 */

FileName::FileName()
  : _dir(""),
    _basename(""),
    _ext(""),
    _filenameDirty(false),
    _pathnameDirty(false)
{
}

/*
 */

FileName::FileName(const String& path)
  : _dir(""),
    _basename(""),
    _ext(""),
    _filenameDirty(true),
    _pathnameDirty(true)
{
  setPathName(path);
}

/*
 */

FileName::FileName(const String& dir, const String& filename)
  : _dir(dir),
    _pathnameDirty(true)
{
  setFileName(filename);
}

/*
 */

FileName::FileName(const FileName& other)
  : _dir(""),
    _basename(""),
    _ext(""),
    _filenameDirty(true),
    _pathnameDirty(true)
{
  _volume = other._volume;
  _dir = other._dir;
  _basename = other._basename;
  _ext = other._ext;
}

/*
 */

FileName::FileName(const char* path)
  : _dir(""),
    _basename(""),
    _ext(""),
    _filenameDirty(false),
    _pathnameDirty(false)
{
  String spath(path);

  setPathName(spath);
}

/*
 */

FileName& FileName::operator=(const FileName& other)
{
  _volume = other._volume;
  _dir = other._dir;
  _basename = other._basename;
  _ext = other._ext;

  _filenameDirty = true;
  _pathnameDirty = true;

  return(*this);
}

/*
 */

void FileName::setPathName(const String& path)
{
  int si = 0, ei;

  _volume = String::empty;

#ifdef CCXX_OS_WINDOWS

  si = path.indexOf(':');
  if(si >= 0)
    _volume = path.substring(0, ++si).toUpperCase();
  else
    si = 0;

#endif

  ei = path.lastIndexOfAnyOf(File::validSeparators);

  String f;

  if(ei < 0)
  {
    if(path == "." || path == "..")
    {
      _dir = path;
      f = "";
    }
    else
    {
      _dir = "";
      f = path;
    }
  }
  else if(ei == si)
  {
    _dir = "";
    _dir += File::separator;
    f = path.substring(++si);
  }
  else
  {
    _dir = path.substring(si, ei);
    f = path.substring(++ei);
  }

  setFileName(f);
}

/*
 */

FileName& FileName::operator=(const String& path)
{
  setPathName(path);

  return(*this);
}

/*
 */

FileName& FileName::operator=(const char* path)
{
  setPathName(path);

  return(*this);
}

/*
 */

FileName::~FileName()
{
}

/*
 */

void FileName::setVolume(const String& volume)
{
  _volume = volume;
  _volume.toUpperCase();
  _pathnameDirty = true;
}

/*
 */

void FileName::setDirectory(const String& dir)
{
  _dir = dir;
  bool absolute = _dir.startsWithAnyOf(File::validSeparators);
  _dir.trim(File::validSeparators, false, true); // remove any trailing slashes
  if(absolute && _dir.isEmpty())
    _dir += File::separator;

  _pathnameDirty = true;
}

/*
 */

FileName& FileName::pushDirectory(const String& dir)
{
  if(! dir.startsWithAnyOf(File::validSeparators))
  {
    if(! _dir.isEmpty())
      _dir += File::separator;

    _dir += dir;
    _dir.trim(File::validSeparators, false, true); // remove trailing slashes

    _pathnameDirty = true;
  }

  return(*this);
}

/*
 */

FileName& FileName::popDirectory()
{
  int i = _dir.lastIndexOfAnyOf(File::validSeparators);
  if(i >= 0)
    _dir.remove(i);

  return(*this);
}

/*
 */

void FileName::setBaseName(const String& name)
{
  if(name.containsAnyOf(File::validSeparators))
    return;

  _basename = name;
  _filenameDirty = true;
  _pathnameDirty = true;
}

/*
 */

void FileName::setExtension(const String& ext)
{
  if(ext.containsAnyOf(File::validSeparators) || ext.contains('.'))
    return;

  _ext = ext;
  _filenameDirty = true;
  _pathnameDirty = true;
}

/*
 */

void FileName::setFileName(const String& name)
{
  int di = name.indexOfAnyOf(File::validSeparators);
  String fname;

  if(di == 0)
    return;

  else if(di > 0)
  {
    // chop off the path component and append that to the directory

    pushDirectory(name.substring(0, di));
    fname = name.substring(++di);
  }
  else
    fname = name;

  int i = fname.lastIndexOf('.');
  if(i <= 0)
  {
    _basename = fname;
    _ext = "";
  }
  else if(i == 0)
  {
    _basename = "";
    _ext = fname;
  }
  else
  {
    _basename = fname.substring(0, i);
    _ext = fname.substring(++i);
  }

  _filenameDirty = true;
  _pathnameDirty = true;
}

/*
 */

bool FileName::isAbsolute() const
{
#ifdef CCXX_OS_WINDOWS

  if(_volume.isEmpty())
    return(false);

#endif

  return(_dir.isEmpty() ? false : File::isValidSeparator(_dir[0].toChar16()));
}

/*
 */

String FileName::getPathName() const
{
  if(_pathnameDirty)
    _rebuildPathName();

  return(_pathname);
}

/*
 */

String FileName::getFileName() const
{
  if(_filenameDirty)
    _rebuildFileName();

  return(_filename);
}

/*
 */

FileName& FileName::canonicalize()
{
  String dir;
  uint_t index = 0;

  if(_dir.startsWithAnyOf(File::validSeparators))
    dir += File::separator;

  for(;;)
  {
    String tok = _dir.nextToken(index, File::validSeparators);

    if(! tok)
      break;
    else if(tok == ".")
      continue;
    else if(tok == "..")
    {
      int i = dir.lastIndexOfAnyOf(File::validSeparators);
      if(i >= 0)
        dir.remove(i);
    }
    else
    {
      if(! dir.endsWithAnyOf(File::validSeparators))
        dir += File::separator;

      dir += tok;
    }
  }

  _dir = dir;
  _pathnameDirty = true;

  return(*this);
}

/*
 */

void FileName::_rebuildPathName() const
{
  if(! _volume.isEmpty())
    _pathname = _volume;
  else
    _pathname = "";

  if(! _dir.isEmpty())
  {
    _pathname += _dir;
    if(! _dir.endsWithAnyOf(File::validSeparators))
      _pathname += File::separator;
  }

  _pathname += getFileName();
  _pathnameDirty = false;
}

/*
 */

void FileName::_rebuildFileName() const
{
  _filename = _basename;
  if(! _ext.isEmpty())
  {
    _filename += '.';
    _filename += _ext;
  }

  _filenameDirty = false;
}


} // namespace ccxx
