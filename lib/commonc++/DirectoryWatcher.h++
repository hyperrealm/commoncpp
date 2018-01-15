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

#ifndef __ccxx_DirectoryWatcher_hxx
#define __ccxx_DirectoryWatcher_hxx

#include <commonc++/String.h++>
#include <commonc++/SystemException.h++>
#include <commonc++/Thread.h++>

namespace ccxx {

/**
 * A class that watches for changes in a given directory. When a file
 * within the watched directory is created, deleted, modified, or
 * renamed, the corresponding handler method is called. This is an
 * abstract class which must be subclassed to implement the handler
 * methods. A watcher must run in its own thread.
 * <p>
 * <b>NOTE</b>: This class is currently not implemented on Mac OS X.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API DirectoryWatcher : public Thread
{
 public:

  /** Destructor. */
  virtual ~DirectoryWatcher();

  /**
   * Initialize the watcher.
   *
   * @throw SystemException If an error occurs.
   */
  void init();

 protected:

  /**
   * Construct a new DirectoryWatcher that will watch the specified
   * directory.
   *
   * @param directory The directory to watch.
   */
  DirectoryWatcher(const String& directory);

  void run();

  /**
   * Called when a file is created in the watched directory.
   *
   * @param path The path (relative to the watched directory) of the new file.
   */
  virtual void fileCreated(const String& path) = 0;

  /**
   * Called when a file is deleted in the watched directory.
   *
   * @param path The path (relative to the watched directory) of the
   * deleted file.
   */
  virtual void fileDeleted(const String& path) = 0;

  /**
   * Called when a file is modified in the watched directory.
   *
   * @param path The path (relative to the watched directory) of the
   * modified file.
   */
  virtual void fileModified(const String& path) = 0;

  /**
   * Called when a file is renamed in the watched directory.
   *
   * @param oldPath The old path (relative to the watched directory) of the
   * renamed file.
   * @param newPath The new path (relative to the watched directory) of the
   * renamed file.
   */
  virtual void fileRenamed(const String& oldPath, const String& newPath) = 0;

 private:

  String _directory;
  bool _initialized;

#if defined(CCXX_OS_WINDOWS)
  HANDLE _handle;
#else
  int _handle;
  int _watch;
#endif

  CCXX_COPY_DECLS(DirectoryWatcher);
};

} // namespace ccxx

#endif // __ccxx_DirectoryWatcher_hxx
