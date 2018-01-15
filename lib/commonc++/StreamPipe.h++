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

#ifndef __ccxx_StreamPipe_hxx
#define __ccxx_StreamPipe_hxx

#include <commonc++/Common.h++>
#include <commonc++/IOException.h++>
#include <commonc++/Permissions.h++>
#include <commonc++/Stream.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * A full-duplex stream pipe. A stream pipe is like a socket but may
 * only be used between processes on the same host. The address of a pipe
 * is a file path, which may or may not exist as an actual file in the
 * filesystem.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API StreamPipe : public Stream
{
  friend class ServerStreamPipe;

 public:

  /** Construct a new StreamPipe. */
  StreamPipe();

  /** Destructor. Closes the pipe, if it is open. */
  ~StreamPipe();

  /**
   * Connect the StreamPipe to the specified path.
   *
   * @param path The path.
   * @throw IOException If an error occurs.
   */
  void connect(const String& path);

  /** Close the pipe for reading and writing. The mode parameter is ignored. */
  void close(IOMode mode = IOReadWrite);

  /** Determine if the pipe is connected. */
  inline bool isConnected() const
  { return(_connected); }

  /**
   * Send a FileHandle over the pipe.
   *
   * @param handle The FileHandle to send.
   * @throw IOException If an error occurs.
   */
  void sendFileHandle(FileHandle handle);

  /**
   * Receive a FileHandle over the pipe.
   *
   * @return handle The received FileHandle.
   * @throw IOException If an error occurs.
   */
  FileHandle receiveFileHandle();

 private:

  void setPipeHandle(FileHandle handle);

  bool _connected;
#ifdef CCXX_OS_WINDOWS
  bool _server;
#endif

  CCXX_COPY_DECLS(StreamPipe);
};

} // namespace ccxx

#endif // __ccxx_StreamPipe_hxx
