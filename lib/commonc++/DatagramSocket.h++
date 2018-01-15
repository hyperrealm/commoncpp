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

#ifndef __ccxx_DatagramSocket_hxx
#define __ccxx_DatagramSocket_hxx

#include <commonc++/Common.h++>
#include <commonc++/Buffer.h++>
#include <commonc++/IOException.h++>
#include <commonc++/Socket.h++>
#include <commonc++/SocketAddress.h++>

namespace ccxx {

/**
 * A User Datagram (UDP) socket. UDP sockets are connectionless and do not
 * provide reliable delivery. Connecting a UDP socket does not establish an
 * actual network connection, but rather specifies the default address to
 * which datagrams are sent and the only address from which datagrams will
 * be received.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API DatagramSocket : public Socket
{
 public:

  /**
   * Construct a new DatagramSocket.
   *
   * @param port The local port to bind to, or 0 to bind to an arbitrary
   * port.
   */
  DatagramSocket(uint16_t port = 0);

  /** Destructor. */
  ~DatagramSocket();

  /**
   * Connect the datagram socket to a remote endpoint. Connecting a
   * datagram socket to an address does not actually establish a
   * network connection, but rather restricts the socket so that it
   * may only exchange datagrams with that address.
   *
   * @param addr The address, which may either be a dot-separated IP address
   * or a DNS name.
   * @param port The port number.
   * @throw IOException If an error occurs.
   */
  void connect(const String &addr, uint16_t port);

  /**
   * Connect the datagram socket to a remote endpoint. Connecting a
   * datagram socket to an address does not actually establish a
   * network connection, but rather restricts the socket so that it
   * may only exchange datagrams with that address.
   *
   * @param addr The address.
   * @throw IOException If an error occurs.
   */
  void connect(const SocketAddress& addr);

  /**
   * Send a datagram to the remote endpoint. The socket must be
   * connected before this method can be used.
   *
   * @param buffer The raw buffer containing the data to send.
   * @param buflen The length of the buffer.
   * @return The number of bytes sent.
   * @throw IOException If an error occurs.
   */
  size_t send(const byte_t* buffer, size_t buflen);

  /**
   * Send a datagram to the remote endpoint. The socket must be
   * connected before this method can be used.
   *
   * @param buffer The buffer containing the data to send.
   * @return The number of bytes sent.
   * @throw IOException If an error occurs.
   */
  size_t send(ByteBuffer& buffer);

  /**
   * Send a datagram to a given address.
   *
   * @param buffer The raw buffer containing the data to send.
   * @param buflen The length of the buffer.
   * @param dest The destination address.
   * @return The number of bytes sent.
   * @throw IOException If an error occurs.
   */
  size_t send(const byte_t* buffer, size_t buflen, const SocketAddress& dest);

  /**
   * Send a datagram to a given address.
   *
   * @param buffer The buffer containing the data to send.
   * @param dest The destination address.
   * @return The number of bytes sent.
   * @throw IOException If an error occurs.
   */
  size_t send(ByteBuffer& buffer, const SocketAddress& dest);

  /**
   * Receive a datagram from the remote endpoint, and write it to a buffer.
   * The socket must be connected before this method can be used.
   *
   * @param buffer The raw buffer to write the data to.
   * @param buflen The length of the buffer.
   * @return The number of bytes received.
   * @throw IOException If an error occurs.
   */
  size_t receive(byte_t* buffer, size_t buflen);

  /**
   * Receive a datagram from the remote endpoint, and write it to a buffer.
   * The socket must be connected before this method can be used.
   *
   * @param buffer The buffer to write the data to.
   * @return The number of bytes received.
   * @throw IOException If an error occurs.
   */
  size_t receive(ByteBuffer& buffer);

  /**
   * Receive a datagram and write it to a buffer.
   *
   * @param buffer The raw buffer to write the data to.
   * @param buflen The length of the buffer.
   * @param source The object in which to place the source address of the
   * received datagram.
   * @return The number of bytes received.
   * @throw IOException If an error occurs.
   */
  size_t receive(byte_t* buffer, size_t buflen, SocketAddress& source);

  /**
   * Receive a datagram and write it to a buffer.
   *
   * @param buffer The buffer to write the data to.
   * @param source The object in which to place the source address of the
   * received datagram.
   * @return The number of bytes received.
   * @throw IOException If an error occurs.
   */
  size_t receive(ByteBuffer& buffer, SocketAddress& source);

  /**
   * Enable or disable broadcast. When enabled, the socket will
   * receive packets sent to a broadcast address and is allowed to
   * send packets to a broadcast address.
   *
   * @param flag A flag indicating whether the feature should be enabled
   * or disabled.
   * @throw SocketException If an error occurs.
   */
  void setBroadcast(bool flag);

  /**
   * Determine if broadcast is enabled.
   *
   * @return <b>true</b> if broadcast is enabled, <b>false</b> otherwise.
   * @throw SocketException If an error occurs.
   */
  bool getBroadcast() const;

  /** The maximum size of a datagram packet. */
  static const size_t MAX_DATAGRAM_SIZE;

 private:

  CCXX_COPY_DECLS(DatagramSocket);
};

} // namespace ccxx

#endif // __ccxx_DatagramSocket_hxx
