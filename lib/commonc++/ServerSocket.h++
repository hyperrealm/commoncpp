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

#ifndef __ccxx_ServerSocket_hxx
#define __ccxx_ServerSocket_hxx

#include <commonc++/Common.h++>
#include <commonc++/IOException.h++>
#include <commonc++/NetworkInterface.h++>
#include <commonc++/Socket.h++>
#include <commonc++/SocketAddress.h++>
#include <commonc++/StreamSocket.h++>

namespace ccxx {

/**
 * A server (listening) socket for StreamSocket (TCP) connections.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API ServerSocket : public Socket
{
 public:

  /**
   * Construct a new ServerSocket that will listen on the given port.
   *
   * @param port The port number to listen on.
   * @param backlog The size of the connection backlog.
   */
  ServerSocket(uint16_t port, uint_t backlog = 3);

  /**
   * Construct a new ServerSocket that will listen on the given port and
   * network interface.
   *
   * @param port The port number to listen on.
   * @param ixface The network interface to listen on.
   * @param backlog The size of the connection backlog.
   */
  ServerSocket(uint16_t port, const NetworkInterface& ixface,
               uint_t backlog = 3);

  /** Destructor. Shuts down the socket. */
  ~ServerSocket();

  void init();

  void shutdown();

  /**
   * Begin listening for connections on the socket.
   *
   * @throw SocketException If a socket error occurs.
   */
  void listen();

  /** Determine if the socket is in a listening state. */
  inline bool isListening() const
  { return(_listening); }

  /**
   * Accept a connection on the socket. This method blocks until a
   * new connection is pending, unless a timeout has been set on the
   * socket, in which case a TimeoutException is thrown if no
   * connection is pending by the time the timeout expires.
   *
   * @param socket A socket object which will be initialized to represent
   * the newly-established connection.
   * @throw TimeoutException If the operation times out.
   * @throw SocketException If a socket error occurs.
   */
  void accept(StreamSocket& socket);

 private:

  uint_t _backlog;
  bool _listening;

  CCXX_COPY_DECLS(ServerSocket);
};

} // namespace ccxx

#endif // __ccxx_ServerSocket_hxx
