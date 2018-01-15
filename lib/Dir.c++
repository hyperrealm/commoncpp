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

#include "commonc++/Dir.h++"
#include "commonc++/System.h++"

#ifdef CCXX_OS_WINDOWS
#include "commonc++/Windows.h++"
#endif

#ifdef CCXX_OS_POSIX
#include <sys/stat.h>

#include "commonc++/POSIX.h++"
#endif

#include <cerrno>

namespace ccxx {

/*
 */

Dir::Dir(const String& path)
  :
#ifdef CCXX_OS_WINDOWS
    _path(File::removeTrailingSeparators(path)),
#else
    _path(path),
#endif
    _open(false),
    _hidden(false),
    _dot(false)
{
  // trim trailing slashes; if path is all slashes, set it to a single slash

  _path.trim(File::separatorString, false, true);
  _path += File::separator;

#ifdef CCXX_OS_WINDOWS
  _pspec = _path;
  _pspec += '*';
#endif
}

/*
 */

Dir::~Dir()
{
  close();
}

/*
 */

void Dir::open(bool listHidden /* = false */, bool listDot /* = false */)
{
  if(_open)
    return;

  _hidden = listHidden;
  _dot = listDot;

#ifdef CCXX_OS_WINDOWS

  _dir = NULL;

  HANDLE tempDir = ::FindFirstFileW(_pspec.data(), &_findData);

  if(! tempDir)
  {
    if(::GetLastError() == ERROR_PATH_NOT_FOUND)
      throw PathNotFoundException(_path);
    else
      throw IOException(System::getErrorString("FindFirstFile"));
  }

  ::FindClose(tempDir);

#else

  CString cstr_path = _path.toUTF8();
  _dir = ::opendir(cstr_path.data());
  if(! _dir)
  {
    if(errno == ENOENT)
      throw PathNotFoundException(_path);
    else
      throw IOException(System::getErrorString("opendir"));
  }

#endif

  _open = true;
}

/*
 */

void Dir::close()
{
  if(! _open)
    return;

#ifdef CCXX_OS_WINDOWS

  ::FindClose(_dir);

#else

  ::closedir(_dir);

#endif

  _open = false;
}

/*
 */

void Dir::rewind()
{
  if(! _open)
    return;

#ifdef CCXX_OS_WINDOWS

  close();
  open(_hidden, _dot);

#else

  ::rewinddir(_dir);

#endif
}

/*
 */

bool Dir::nextFile(String& name, FileAttributes& attrib,
                   bool fetchPermissions /* = false */)
{
  if(! _open)
    return(false);

  for(;;)
  {

#ifdef CCXX_OS_WINDOWS

    if(_dir == NULL)
    {
      _dir = ::FindFirstFileW(_pspec, &_findData);
      if(_dir == INVALID_HANDLE_VALUE)
        return(false);
    }
    else if(::FindNextFile(_dir, &_findData) == FALSE)
      break;

    if((_findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) && ! _hidden)
      continue; // skip hidden files

    name = (const char *)_findData.cFileName;

    if(((name == ".") || (name == "..")) && ! _dot)
      continue;

    if(_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      attrib.type = TypeDir;
    else
      attrib.type = TypeFile;

    attrib.size = ((_findData.nFileSizeHigh * (MAXDWORD + 1))
                   + _findData.nFileSizeLow);

    attrib.created = Windows::decodeFileTime(_findData.ftCreationTime);
    attrib.modified = Windows::decodeFileTime(_findData.ftLastWriteTime);
    attrib.accessed = Windows::decodeFileTime(_findData.ftLastAccessTime);

    if(fetchPermissions)
    {
      String path = _path;
      path += name;
      File::getPermissions(path, attrib.permissions);
    }

#else

    struct stat stbuf;
    struct dirent *de = readdir(_dir);

    if(! de)
      break;

    if((de->d_name[0] == '.') && ! _hidden)
      continue; // skip hidden files

    if((! ::strcmp(de->d_name, ".") || ! ::strcmp(de->d_name, ".."))
       && ! _dot)
      continue;

    name = (const char *)de->d_name;

    String path = _path;
    path += File::separator;
    path += de->d_name;

    CString cstr_path = path.toUTF8();
    if(::stat(cstr_path.data(), &stbuf) != 0)
    {
      // can't stat the file, so continue to next one
      continue;
    }

    if(S_ISREG(stbuf.st_mode))
      attrib.type = TypeFile;
    else if(S_ISDIR(stbuf.st_mode))
      attrib.type = TypeDir;
    else if(S_ISLNK(stbuf.st_mode))
      attrib.type = TypeSymLink;
    else if(S_ISFIFO(stbuf.st_mode))
      attrib.type = TypePipe;
#ifdef S_ISSOCK
    else if(S_ISSOCK(stbuf.st_mode))
      attrib.type = TypePipe;
#endif // S_ISSOCK
#ifdef S_ISCHR
    else if(S_ISCHR(stbuf.st_mode))
      attrib.type = TypeDevice;
#endif // S_ISCHR
#ifdef S_ISBLK
    else if(S_ISBLK(stbuf.st_mode))
      attrib.type = TypeDevice;
#endif // S_ISBLK
    else
      attrib.type = TypeOther;

    attrib.created = 0; // can't determine on POSIX systems
    attrib.modified = stbuf.st_mtime;
    attrib.accessed = stbuf.st_atime;

    attrib.size = static_cast<uint64_t>(stbuf.st_size);

    POSIX::decodePermissions(stbuf.st_mode, attrib.permissions);

#endif

    return(true);
  }

  return(false);
}


} // namespace ccxx
