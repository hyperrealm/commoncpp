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

#ifndef __ccxx_ServerStreamPipe_hxx
#define __ccxx_ServerStreamPipe_hxx

#include <commonc++/Common.h++>
#include <commonc++/IOException.h++>
#include <commonc++/Permissions.h++>
#include <commonc++/StreamPipe.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * A server (listening) pipe for StreamPipe connections.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API ServerStreamPipe
{
 public:

  /**
   * Construct a new ServerStreamPipe that will listen at the given path.
   *
   * @param path The path.
   * @param backlog The size of the connection backlog.
   */
  ServerStreamPipe(const String& path, uint_t backlog = 3);

  /** Destructor. Shuts down the pipe. */
  ~ServerStreamPipe();

  /**
   * Begin listening for connections on the pipe.
   *
   * @throw IOException If an error occurs.
   */
  void listen();

  /**
   * Accept a connection on the stream pipe. This method blocks until a new
   * connection is pending.
   *
   * @param pipe A pipe object which will be initialized to represent
   * the newly-established connection.
   * @throw IOException If an error occurs.
   */
  void accept(StreamPipe& pipe);

  /** Shut down the pipe. */
  void shutdown();

 private:

  uint_t _backlog;
  FileHandle _handle;
  String _path;
  bool _listening;

  CCXX_COPY_DECLS(ServerStreamPipe);
};

} // namespace ccxx

#endif // __ccxx_ServerStreamPipe_hxx
