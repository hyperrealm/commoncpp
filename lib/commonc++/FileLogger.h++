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

#ifndef __ccxx_FileLogger_hxx
#define __ccxx_FileLogger_hxx

#include <commonc++/Common.h++>
#include <commonc++/File.h++>
#include <commonc++/IOException.h++>
#include <commonc++/Logger.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * A logger that writes to a file, and can optionally perform log
 * rotation.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API FileLogger : public Logger
{
 public:

  /**
   * Construct a new FileLogger with the given log message format and
   * logging characteristics.
   *
   * @param format The log message format.
   * @param maxLogSize The maximum log file size, in kilobytes.
   * @param rotateCount The maximum number of backlog files to create. The
   * value must range from 0 to 9, with 0 indicating that log rotation is
   * disabled.
   */
  FileLogger(const String& format = "[%d %t] %m", size_t maxLogSize = 2048,
             uint_t rotateCount = 1);

  /** Destructor. */
  virtual ~FileLogger();

  /**
   * Set the logging directory and log file name.
   *
   * @param dir The directory in which log files will be created.
   * @param name The base name for the log file; log files will be named
   * <i>name</i>.log, <i>name</i>.1.log, etc..
   */
  virtual bool setFile(const String& dir, const String& name);

  /**
   * Set the maximum size for log files.
   *
   * @param maxLogSize The maximum log file size, in kilobytes.
   */
  void setMaxLogSize(size_t maxLogSize);

  /**
   * Set the rotation log file count.
   *
   * @param rotateCount The maximum number of backlog files to create. The
   * value must range from 0 to 9, with 0 indicating that log rotation is
   * disabled.
   */
  void setRotateCount(uint_t rotateCount);

  /** The maximum file rotate count. */
  static const uint_t MAX_ROTATE_COUNT;

 protected:

  /**
   * Open the log file.
   *
   * @throw IOException If an I/O error occurs.
   */
  virtual void openFile();

  /** Write a preformatted log message to the file. */
  virtual bool write(CharBuffer& buffer);

  /** @cond INTERNAL */
  bool _rotate();

  int64_t _maxFileSize;
  size_t _maxLogSize;
  size_t _curLogSize;
  uint_t _rotateCount;

  File* _file;
  String _dir;
  String _name;
  /** @endcond */
};

} // namespace ccxx

#endif // __ccxx_FileLogger_hxx
