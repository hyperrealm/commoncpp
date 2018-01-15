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

#include "commonc++/FileTraverser.h++"
#include "commonc++/Dir.h++"
#include "commonc++/File.h++"

namespace ccxx {

/*
 */

FileTraverser::FileTraverser(const String& basePath,
                             bool depthFirst /* = true */)
  : _baseDir(basePath),
    _depthFirst(depthFirst)
{
  _baseDir.trimEnd(File::validSeparators);
}

/*
 */

FileTraverser::~FileTraverser()
{
}

/*
 */

bool FileTraverser::_traverse(FileName& filename, uint_t depth)
{
  String file, path, filePath;
  bool ok;

  try
  {
    Dir dir(filename.getPathName());

    dir.open(true, false);

    // two passes: one for file/dir processing, one for recursion
    for(int pass = 0; pass <= 1; pass++)
    {
      FileAttributes attrs;
      String name;

      while(dir.nextFile(name, attrs))
      {
        // check if we need to prune

        if(! _prunePattern.isNull() && _prunePattern.match(name))
          continue; // prune

        // build a complete path to file

        String fname = path;
        fname += File::separator;
        fname += name;

        // * if depth first, recurse on directories first, then process all
        //   dirs and files.
        // * if breadth first, process all dirs and files, then recurse on
        //   dirs.

        if(attrs.isDirectory() && (((pass == 0) && _depthFirst)
                                   || ((pass == 1) && ! _depthFirst)))
        {
          filename.setFileName("");
          filename.pushDirectory(name);

          ok = _traverse(filename, depth + 1);

          filename.popDirectory();

          if(! ok)
            return(false);
        }

        if(((pass == 1) && _depthFirst) || ((pass == 0) && ! _depthFirst))
        {
          filename.setFileName(name);

          if(! processFile(filename, attrs.isDirectory(), depth))
            return(false);
        }
      }

      dir.rewind();
    }

    dir.close();
    filename.setFileName("");
  }
  catch(IOException& ex)
  {
    // error condition; directory could not be read
    return(handleError(filename, ex));
  }

  return(true);
}

/*
 */

bool FileTraverser::traverse()
{
  bool dir = File::isDirectory(_baseDir);

  FileName filename(_baseDir);

  if(! _depthFirst)
    if(! processFile(filename, dir, 0))
      return(false);

  if(dir)
  {
    _baseDir += File::separator;
    filename.setPathName(_baseDir);

    if(! _traverse(filename, 1))
      return(false);

    _baseDir.trimEnd(File::validSeparators);
    filename.setPathName(_baseDir);
  }

  if(_depthFirst)
    if(! processFile(filename, dir, 0))
      return(false);

  return(true);
}

/*
 */

bool FileTraverser::handleError(const FileName& name, const IOException& ex)
{
  return(false);
}


} // namespace ccxx
