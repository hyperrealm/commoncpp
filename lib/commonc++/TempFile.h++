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

#ifndef __ccxx_TempFile_hxx
#define __ccxx_TempFile_hxx

#include <commonc++/Common.h++>
#include <commonc++/Stream.h++>
#include <commonc++/IOException.h++>
#include <commonc++/String.h++>

#include <fstream>
#include <ios>

namespace ccxx {

/**
 * A temporary file that may optionally "shadow" another file.
 * <p>
 * If constructed with a path, the temporary file will be created in
 * the same directory as the file specified by that path; when the
 * temporary file is closed, it will be atomically moved onto the original
 * file. This is useful for safely updating an existing file.
 * <p>
 * If constructed without a path, the temporary file will be created
 * in the host platform's designated temporary directory; when the
 * file is closed, it will be deleted.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API TempFile : public Stream
{
 public:

  /**
   * Construct a new TempFile.
   *
   * @param path The path to the file being shadowed, if any.
   */
  TempFile(const String& path = String::null);

  /** Destructor. */
  ~TempFile();

  /**
   * Create and open the TempFile.
   *
   * @throw IOException If the file could not be created and/or opened.
   */
  void open();

  /**
   * Close (and delete) the temporary file. If the temporary file is shadowing
   * another file, its contents are copied to that file before the temporary
   * file is deleted.
   */
  virtual void close();

 private:

  String _path;
  String _tempPath;
  FileHandle _handle;

  CCXX_COPY_DECLS(TempFile);
};

} // namespace ccxx

#endif // __ccxx_TempFile_hxx
