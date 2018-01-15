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

#ifndef __ccxx_Dir_hxx
#define __ccxx_Dir_hxx

#include <commonc++/Common.h++>
#include <commonc++/File.h++>
#include <commonc++/IOException.h++>
#include <commonc++/String.h++>

#ifdef CCXX_OS_POSIX
#include <dirent.h>
#endif

namespace ccxx {

/**
 * Directory reading support. The class provides a means of enumerating
 * through the files in a directory.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Dir
{
 public:

  /**
   * Construct a new Dir object for the given path.
   *
   * @param path The path to the directory.
   */
  Dir(const String& path);

  /** Destructor. */
  ~Dir();

  /**
   * Open the directory. If the directory is already open, the method has
   * no effect.
   *
   * @param listHidden A flag indicating whether hidden files (as defined
   * for the host platform) should be listed or ignored.
   * @param listDot A flag indicating whether the files '.' and '..' should
   * be listed or ignored.
   * @throw IOException If the directory could not be opened.
   */
  void open(bool listHidden = false, bool listDot = false);

  /**
   * Close the directory. If the directory is already closed, the method
   * has no effect.
   */
  void close();

  /**
   * Rewind the directory. On some platforms this entails closing and
   * re-opening the directory.
   *
   * @throw IOException If the directory could not be rewound.
   */
  void rewind();

  /** Test if the directory is currently open. */
  inline bool isOpen() const
  { return(_open); }

  /**
   * Retrieve the next file from the directory.
   *
   * @param name The String in which to place the file's name.
   * @param attrib The FileAttributes object in which to place the file's
   * attributes.
   * @param fetchPermissions A flag indicating whether the file permissions
   * should be fetched for the file. This is a very slow operation on
   * Windows, hence by default this option is disabled. On POSIX systems,
   * this flag is ignored because fetching the file permissions has no
   * overhead.
   * @return <b>true</b> if the information was successfully retrieved,
   * <b>false</b> if the end of the directory has been reached.
   */
  bool nextFile(String& name, FileAttributes& attrib,
                bool fetchPermissions = false);

 private:

  String _path;
  bool _open;
  bool _hidden;
  bool _dot;

#ifdef CCXX_OS_WINDOWS
  String _pspec;
  HANDLE _dir;
  WIN32_FIND_DATA _findData;
#else
  DIR *_dir;
#endif

  CCXX_COPY_DECLS(Dir);
};

} // namespace ccxx

#endif // __ccxx_Dir_hxx
