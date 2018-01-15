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

#ifndef __ccxx_FileTraverser_hxx
#define __ccxx_FileTraverser_hxx

#include <commonc++/FileName.h++>
#include <commonc++/IOException.h++>
#include <commonc++/RegExp.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * A depth- or breadth-first file traverser. This is an abstract
 * class; subclasses must implement the <b>processFile</b>()
 * function to process each file and directory encountered during
 * the traversal.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API FileTraverser
{
 public:

  /**
   * Construct a new FileTraverser rooted at the given base path.
   *
   * @param basePath The base path at which traversal will begin.
   * @param depthFirst If <b>true</b>, traversal will be depth-first;
   * otherwise it will be breadth-first.
   */
  FileTraverser(const String& basePath, bool depthFirst = true);

  /** Destructor. */
  virtual ~FileTraverser();

  /**
   * Set a prune pattern for the traverser. Any file or directory whose
   * name matches the given pattern will be skipped during traversal.
   * Setting a null pattern disables pruning.
   *
   * @param pattern The prune pattern.
   */
  inline void setPrunePattern(const RegExp& pattern)
  { _prunePattern = pattern; }

  /**
   * Traverse the file tree.
   *
   * @return <b>true</b> if the traversal completed successfully,
   * <b>false</b> otherwise.
   */
  bool traverse();

 protected:

  /**
   * %Process the next file or directory encountered.
   *
   * @param name The filename.
   * @param isDir A flag indicating whether the file is a directory.
   * @param depth The current traversal depth.
   * @return <b>true</b> if traversal should continue, <b>false</b>
   * otherwise.
   */
  virtual bool processFile(const FileName& name, bool isDir, uint_t depth) = 0;

  /**
   * Handle an exception during traversal. The method should return
   * <b>true</b> if traversal should continue, <b>false</b> otherwise.
   * The default implementation is a no-op that simply returns <b>false</b>.
   *
   * @param name The filename for which the error occurred.
   * @param ex The exception indicating the error.
   * @return <b>true</b> if traversal should continue, <b>false</b>
   * otherwise.
   */
  virtual bool handleError(const FileName& name, const IOException& ex);

 private:

  bool _traverse(FileName& filename, uint_t depth);

  String _baseDir;
  bool _depthFirst;
  RegExp _prunePattern;
};

} // namespace ccxx

#endif // __ccxx_FileTraverser_hxx
