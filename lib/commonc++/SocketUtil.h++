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

#ifndef __ccxx_SocketUtil_hxx
#define __ccxx_SocketUtil_hxx

#include <commonc++/Common.h++>
#include <commonc++/Socket.h++>

namespace ccxx {

/**
 * Internal socket utility routines.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API SocketUtil
{
 public:

  /**
   * Begin an asynchronous socket connect. Returns <b>true</b> on success,
   * or <b>false</b> if the connection is pending (in progress). On error,
   * an exception is thrown.
   *
   * @throw IOException If an I/O error occurs.
   */
  static bool startConnect(SocketHandle socket, struct sockaddr* addr,
                           socklen_t addrlen);

  /**
   * Finish an asynchronous socket connect.
   *
   * @throw IOException If an I/O error occurs.
   */
  static void finishConnect(SocketHandle socket);

  /** Perform a complete asynchronous socket connect operation. */
  static void connectSocket(SocketHandle socket, struct sockaddr* addr,
                            socklen_t addrlen, int timeout);

  /** Wait for a socket to become ready for reading, writing, or both. */
  static void waitForIO(SocketHandle socket, uint_t mode, int timeout);

  /** Close a socket. */
  static void closeSocket(SocketHandle socket);

  static const uint_t WAIT_READ;
  static const uint_t WAIT_WRITE;
};

} // namespace ccxx

#endif // __ccxx_SocketUtil_hxx
