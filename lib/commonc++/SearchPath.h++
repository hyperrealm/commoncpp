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

#ifndef __ccxx_SearchPath_hxx
#define __ccxx_SearchPath_hxx

#include <commonc++/Common.h++>
#include <commonc++/FileName.h++>
#include <commonc++/String.h++>

#include <memory>

#ifdef SearchPath
#undef SearchPath
#endif

namespace ccxx {

/**
 * A file search path. A search path is a series of directories
 * (typically specified as absolute paths) that may be searched to
 * locate a file given its filename.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API SearchPath
{
 public:

  /**
   * Construct a new SearchPath for the given path string.
   *
   * @param path The path.
   */
  SearchPath(const String& path = "");

  /** Copy constructor. */
  SearchPath(const SearchPath& other);

  /** Destructor. */
  ~SearchPath();

  /** Assignment operator. */
  SearchPath& operator=(const SearchPath& other);

  /**
   * Append a directory to the end of the path. If the directory is
   * already in the path, the path will be unchanged.
   *
   * @param dir The directory.
   */
  void append(const String& dir);

  /**
   * Prepend a directory to the beginning of the path. If the
   * directory is already in the path, the path will be unchanged.
   *
   * @param dir The directory.
   */
  void prepend(const String& dir);

  /**
   * Remove a directory from the path. If the directory is not in the
   * path, the method does nothing.
   */
  void remove(const String& dir);

  /** Clear the path. Removes all directories from the path. */
  void clear();

  /**
   * Determine if the path contains a directory.
   *
   * @param dir The directory to search for.
   * @return <b>true</b> if the directory is found in the path,
   * <b>false</b> otherwise.
   */
  bool contains(const String& dir) const;

  /**
   * Search the path for a file.
   *
   * @param file The name of the file to search for.
   * @return <b>true</b> if the file was found in one of the directories
   * in the search path, <b>false</b> otherwise. If the file was found, the
   * directory component of the FileName object is updated to reflect the
   * location of the file.
   */
  bool find(FileName& file) const;

  /** Test if the path is empty. */
  inline bool isEmpty() const
  { return(_path.empty()); }

  /**
   * Get the number of directories in the search path. Equivalent to
   * length().
   */
  inline size_t getDirectoryCount() const
  { return(length()); }

  /** Get the number of directories in the search path. */
  inline size_t length() const
  { return(_path.size()); }

  /**
   * Get the directory at the specified index in the search path.
   *
   * @throw OutOfBoundsException If the index is invalid.
   */
  String getDirectory(int index) const;

  /**
   * Get the directory at the specified index in the search path.
   *
   * @throw OutOfBoundsException If the index is invalid.
   */
  inline String operator[](int index) const
  { return(getDirectory(index)); }

  /**
   * Get the path as a String.
   *
   * @return The platform-specific representation of the path.
   */
  String getPath() const;

  /** Parse a SearchPath from a String representation. */
  void fromString(const String& path);

  /** Get a String representation of the SearchPath. Equivalent to getPath().
   */
  inline String toString() const
  { return(getPath()); }

  /** An empty SearchPath. */
  static const SearchPath empty;

  /** The host platform's path separator. */
  static const char* separator;

 private:

  StringVec _path;
  mutable String _pathstr;
};

} // namespace ccxx

#endif // __ccxx_SearchPath_hxx
