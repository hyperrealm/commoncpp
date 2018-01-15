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

#ifndef __ccxx_Pipe_hxx
#define __ccxx_Pipe_hxx

#include <commonc++/Common.h++>
#include <commonc++/IOException.h++>

namespace ccxx {

/**
 * A unidirectional stream pipe. The pipe has two file descriptors, one for
 * reading, and one for writing.
 *
 * @author Mark Lindner
 */
class Pipe
{
 public:

  /** Constructor. The actual pipe is not created until open() is called. */
  Pipe();

  /** Destructor. Closes the pipe if it is open. */
  ~Pipe();

  /**
   * Create and open the pipe.
   *
   * @throw IOException If an I/O error occurs.
   */
  void open();

  /** Close the pipe. */
  void close();

  /** Test if the pipe is open. */
  bool isOpen() const;

  /**
   * Read raw data from the read end of the pipe.
   *
   * @param buf The buffer to read into.
   * @param count The number of bytes to read.
   * @return The number of bytes actually read, or a negative value if an
   * error occurs.
   */
  ssize_t read(byte_t* buf, size_t count);

  /**
   * Write raw data to the write end of the pipe.
   *
   * @param buf The buffer to write.
   * @param count The number of bytes to write.
   * @return The number of bytes actually written, or a negative value if an
   * error occurs.
   */
  ssize_t write(const byte_t* buf, size_t count);

  /**
   * Set the blocking state of the pipe.
   *
   * @param blocking A flag indicating whether the pipe should be blocking or
   * non-blocking.
   * @throw IOException If an I/O error occurs.
   */
  void setBlocking(bool blocking);

  /** Get the file descriptor for the read end of the pipe. */
  inline FileHandle getReadDescriptor() const
  { return(_fd[0]); }

  /** Get the file descriptor for the write end of the pipe. */
  inline FileHandle getWriteDescriptor() const
  { return(_fd[1]); }

 private:

  FileHandle _fd[2];
};

} // namespace ccxx

#endif // __ccxx_Pipe_hxx
