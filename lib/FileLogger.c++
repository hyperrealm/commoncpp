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

#include "commonc++/FileLogger.h++"

namespace ccxx {

/*
 */

const uint_t FileLogger::MAX_ROTATE_COUNT = 9;

/*
 */

FileLogger::FileLogger(const String &format /* = "[%d %t] %m" */,
                       size_t maxLogSize /* =  2048 */,
                       uint_t rotateCount /* = 1 */)
  : Logger(format),
    _maxFileSize(maxLogSize * 1024),
    _maxLogSize(maxLogSize < 1 ? 1 : maxLogSize),
    _curLogSize(0),
    _rotateCount(rotateCount <= MAX_ROTATE_COUNT ? rotateCount : 0),
    _file(NULL)
{
}

/*
 */

FileLogger::~FileLogger()
{
  delete _file;
}

/*
 */

bool FileLogger::setFile(const String& dir, const String& name)
{
  if(_file)
  {
    _file->close();
    delete _file;
  }

  _dir = File::removeTrailingSeparators(dir);
  _name = name;

  String path;
  path << _dir;

  if(! _dir.endsWith(File::separator))
    path << File::separator;

  path << _name << ".log";

  _file = new File(path);

  try
  {
    openFile();

    return(true);
  }
  catch(IOException &ex)
  {
    delete _file;
    _file = NULL;
    return(false);
  }
}

/*
 */

void FileLogger::setMaxLogSize(size_t maxLogSize)
{
  _maxLogSize = (maxLogSize < 1) ? 1 : maxLogSize;
  _maxFileSize = _maxLogSize * 1024;
}

/*
 */

void FileLogger::setRotateCount(uint_t rotateCount)
{
  if(rotateCount <= MAX_ROTATE_COUNT)
    _rotateCount = rotateCount;
}

/*
 */

void FileLogger::openFile()
{
  _file->open(IOWrite);
}

/*
 */

bool FileLogger::write(CharBuffer& buffer)
{
  if(! _file)
    return(false);

  size_t sz = buffer.getRemaining();

  try
  {
    int64_t offset = _file->seek(0, SeekEnd);

    if(offset + static_cast<int64_t>(sz) > _maxFileSize)
    {
      _file->close();

      if(! _rotate())
        return(false);

      openFile();
    }

    _file->write(buffer);

    return(true);
  }
  catch(IOException &ex)
  {
    _file->close();
    delete _file;
    _file = NULL;

    return(false);
  }
}

/*
 */

bool FileLogger::_rotate()
{
  if(_rotateCount == 0)
  {
    String path;
    path << _dir << File::separator << _name;

    return(File::remove(path));
  }

  for(int i = MAX_ROTATE_COUNT - 1; i >= 0; --i)
  {
    // delete file.i+1
    // rename file.i to file.i+1

    String oldPath, newPath;

    newPath << _dir;

    if(! _dir.endsWith(File::separator))
      newPath << File::separator;

    newPath << _name << '.' << (i + 1) << ".log";

    if(File::exists(newPath))
    {
      if(! File::remove(newPath))
        return(false);
    }

    if(static_cast<uint_t>(i) < _rotateCount)

    oldPath << _dir << File::separator << _name;
    if(i > 0)
      oldPath << '.' << i;
    oldPath << ".log";

    if(File::exists(oldPath))
    {
      if(! File::rename(oldPath, newPath))
        return(false);
    }
  }

  return(true);
}


} // namespace ccxx
