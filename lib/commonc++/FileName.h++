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

#ifndef __ccxx_FileName_hxx
#define __ccxx_FileName_hxx

#include <commonc++/Common.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * A class representing a filename. The filename is composed of the
 * following components, each of which can be individually modified:
 * <ul>
 * <li>A volume. This corresponds to the drive letter on Windows systems.
 * It is unused on non-Windows systems.
 * <li>A directory path. This is the path minus the final
 * path component (the filename), unless the path ends in a file
 * separator character, in which case the path is used as the directory,
 * and the filename is blank.
 * <li>A filename (the base name plus the extension).
 * <li>A file base name.
 * <li>A file extension.
 * </ul>
 * <b>Example</b>: The Windows path
 * <tt>C:\\Program Files\\TestProg\\TestProg.exe</tt> breaks down as follows:
 * <br>Volume: <tt>C:</tt>
 * <br>Directory: <tt>\\Program Files\\TestProg\\</tt>
 * <br>Filename: <tt>TestProg.exe</tt>
 * <br>Base name: <tt>TestProg</tt>
 * <br>Extension: <tt>exe</tt>
 *
 * @author Mark Lindner
 */
class COMMONCPP_API FileName
{
 public:

  /** Construct a new, empty FileName. */
  FileName();

  /** Construct a new FileName for the given path. */
  FileName(const String& path);

  /** Construct a new FileName for the given directory and filename. */
  FileName(const String& dir, const String& filename);

  /** Copy constructor. */
  FileName(const FileName& other);

  /** Construct a new FileName for the given path. */
  FileName(const char* path);

  /** Assignment operator. */
  FileName& operator=(const FileName& other);

  /** Assignment operator. */
  FileName& operator=(const String& path);

  /** Assignment operator. */
  FileName& operator=(const char* path);

  /** Destructor. */
  ~FileName();

  /**
   * Get a String representation of the FileName. Equivalent to
   * getPathName().
   */
  inline String toString() const
  { return(getPathName()); }

  /** Get the full path represented by this FileName, as a String. */
  String getPathName() const;

  /** Set the path for this FileName. */
  void setPathName(const String& path);

  /**
   * Get the volume for this FileName. For non-Windows systems, this will
   * be an empty string.
   */
  inline String getVolume() const
  { return(_volume); }

  /**
   * Set the volume for this FileName. On non-Windows systems, this call
   * has no effect.
   */
  void setVolume(const String& volume);

  /** Get the directory for this FileName. */
  inline String getDirectory() const
  { return(_dir); }

  /** Set the directory for this FileName. */
  void setDirectory(const String& dir);

  /**
   * Push a directory. The directory is added to the end of the directory
   * component of the FileName.
   *
   * @param dir The directory to push. May include multiple path components.
   */
  FileName& pushDirectory(const String& dir);

  /**
   * Pop a directory. Removes the rightmost path component from the
   * directory component of the FileName.
   */
  FileName& popDirectory();

  /** Get the base name from the FileName. */
  inline String getBaseName() const
  { return(_basename); }

  /** Set the base name for the FileName. */
  void setBaseName(const String& name);

  /** Get the extension for the FileName. */
  inline String getExtension() const
  { return(_ext); }

  /** Set the extension for the FileName. */
  void setExtension(const String& ext);

  /** Get the filename for the FileName. */
  String getFileName() const;

  /**
   * Set the filename for the FileName.
   *
   * @param name The filename. If <b>name</b> contains no path separators,
   * it simply becomes the new filename. If it begins with a path separator,
   * the call does nothing. Otherwise, it is treated as a relative path: it
   * is appended to the path, and then the entire path is reparsed,
   * with the last component of <b>name</b> becoming the new filename.
   */
  void setFileName(const String& name);

  /**
   * Determine wheter the FileName represents an absolute or relative path.
   *
   * @return <b>true</b> if an absolute path, <b>false</b> otherwise.
   */
  bool isAbsolute() const;

  /**
   * Canonicalize the FileName. All "." components are removed and all
   * ".." components are resolved. If there are more ".." components than
   * literal components, an empty path will result.
   */
  FileName& canonicalize();

 private:

  void _rebuildPathName() const;
  void _rebuildFileName() const;

  String _volume;
  String _dir;
  String _basename;
  String _ext;
  mutable String _filename;
  mutable String _pathname;
  mutable bool _filenameDirty;
  mutable bool _pathnameDirty;
};

} // namespace ccxx

#endif // __ccxx_FileName_hxx
