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

#ifndef __ccxx_Socket_hxx
#define __ccxx_Socket_hxx

#include <commonc++/Common.h++>
#include <commonc++/SocketException.h++>
#include <commonc++/Network.h++>
#include <commonc++/SocketAddress.h++>

#ifdef CCXX_OS_WINDOWS
typedef int socklen_t;
typedef char FAR* sockbufptr_t;
#endif

#ifdef CCXX_OS_POSIX
#include <netinet/in.h>
#include <sys/socket.h>
typedef void * sockbufptr_t;
#endif

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif // MSG_NOSIGNAL

namespace ccxx {

#ifdef CCXX_OS_WINDOWS
typedef SOCKET SocketHandle;
#define INVALID_SOCKET_HANDLE INVALID_SOCKET
#else
typedef int SocketHandle;
#define INVALID_SOCKET_HANDLE (-1)
#endif

/**
 * A base class for network sockets.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Socket
{
  friend class ServerSocket;
  friend class SocketSelector;

 protected:

  /**
   * Construct a new Socket of the specified type.
   *
   * @param type The socket type.
   */
  Socket(NetProtocol type = ProtoTCP);

 public:

  /** Destructor. Closes and destroys the socket. */
  virtual ~Socket();

  /**
   * Initialize the socket. This method creates the underlying socket
   * object.
   *
   * @throw SocketException If the socket could not be created.
   */
  virtual void init();

  /**
   * Connect the socket to a remote endpoint. Note that if this call fails,
   * the socket will revert to an uninitialized state; call init() again to
   * reuse the socket.
   *
   * @param addr The address, which may either be a dot-separated IP address
   * or a DNS name.
   * @param port The port number.
   * @throw IOException If an error occurs.
   */
  virtual void connect(const String& addr, uint16_t port);

  /**
   * Connect the socket to a remote endpoint. Note that if this call fails,
   * the socket will revert to an uninitialized state; call init() again to
   * reuse the socket.
   *
   * @param addr The address.
   * @throw IOException If an error occurs.
   */
  virtual void connect(const SocketAddress& addr);

  /**
   * Shut down the socket. Closes the connection and destroys the
   * underlying socket object.
   */
  virtual void shutdown();

  /**
   * Set the timeout for the socket. The timeout affects both I/O and
   * connections.
   *
   * @param timeout The timeout, in milliseconds.
   * @throw SocketException If an error occurs.
   */
  void setTimeout(timespan_ms_t timeout);

  /**
   * Set the size of the socket's internal receive buffer.
   *
   * @param size The new size, in bytes.
   * @throw SocketException If the size could not be changed.
   */
  void setReceiveBufSize(size_t size);

  /**
   * Get the size of the socket's internal receive buffer.
   *
   * @return The size, in bytes.
   * @throw SocketException If the size could not be changed.
   */
  size_t getReceiveBufSize() const;

  /**
   * Set the size of the socket's internal send buffer.
   *
   * @param size The new size, in bytes.
   * @throw SocketException If the size could not be changed.
   */
  void setSendBufSize(size_t size);

  /**
   * Get the size of the socket's internal send buffer.
   *
   * @return The size, in bytes.
   * @throw SocketException If the size could not be changed.
   */
  size_t getSendBufSize() const;

  /**
   * Set the linger time for the socket.
   *
   * @param timeout The new linger time, in seconds; a negative value
   * disables the linger feature.
   * @throw SocketException If the linger time could not be changed.
   */
  void setLingerTime(timespan_s_t timeout);

  /**
   * Get the linger time for the socket.
   *
   * @return The linger time, in seconds, or -1 if the feature is
   * disabled.
   * @throw SocketException If the linger time could not be retrieved.
   */
  timespan_s_t getLingerTime() const;

  /**
   * Enable or disable the SO_REUSEADDR option on the socket. This
   * method may only be called before the socket has been initialized. This
   * option is off by default in newly created sockets.
   *
   * @param enable <b>true</b> to enable the option <b>false</b> to
   * disable it.
   * @throw SocketException If a socket error occurs, or if the call was
   * made after the socket was initialized.
   */
  void setReuseAddress(bool enable);

  /**
   * Determine if the SO_REUSEADDR option is enabled or disabled.
   *
   * @return <b>true</b> if the option is enabled, <b>false</b> otherwise.
   * @throw SocketException If a socket error occurs.
   */
  bool getReuseAddress() const;

  /**
   * Enable or disable the SO_KEEPALIVE option on the socket.
   *
   * @param enable <b>true</b> to enable the option <b>false</b> to
   * disable it.
   * @throw SocketException If a socket error occurs.
   */
  void setKeepAlive(bool enable);

  /**
   * Determine if the SO_KEEPALIVE option is enabled or disabled.
   *
   * @return <b>true</b> if the option is enabled, <b>false</b> otherwise.
   * @throw SocketException If a socket error occurs.
   */
  bool getKeepAlive();

  /**
   * Enable or disable the Nagle algorithm for send coalescing.
   *
   * @param enable <b>true</b> to enable the Nagle algorithm
   * (TCP_NODELAY off), <b>false</b> to disable it (TCP_NODELAY on).
   * @throw SocketException If a socket error occurs.
   */
  void setTCPDelay(bool enable);

  /**
   * Determine if the Nagle algorithm is enabled or disabled.
   *
   * @return <b>true</b> if the Nagle algorithm is enabled (TCP_NODELAY off),
   * <b>false</b> otherwise.
   * @throw SocketException If a socket error occurs.
   */
  bool getTCPDelay();

  /** Determine if the socket has been initialized. */
  inline bool isInitialized() const
  { return(_socket != INVALID_SOCKET_HANDLE); }

  /** Determine if the socket is connected. */
  inline bool isConnected() const
  { return(_connected); }

  /** Get the socket type. */
  inline NetProtocol getType() const
  { return(_type); }

  /** Get the address of the local end of the socket. */
  inline const SocketAddress& getLocalAddress() const
  { return(_laddr); }

  /** Get the address of the remote (peer) end of the socket. */
  inline const SocketAddress& getRemoteAddress() const
  { return(_raddr); }

 protected:

  enum IOWaitMode { WaitWrite, WaitRead };

  /** Get the underlying socket handle for this socket. */
  inline SocketHandle getSocketHandle() const
  { return(_socket); }

  /**
   * Wait for the socket to become ready for reading or writing.
   *
   * @param mode The wait mode; one of WaitWrite or WaitRead.
   * @throw IOException On timeout or other error.
   */
  void waitForIO(IOWaitMode mode);

  /** The socket type. */
  NetProtocol _type;
  /** A handle to the socket itself. */
  SocketHandle _socket;
  /** The remote address. */
  SocketAddress _raddr;
  /** The local address. */
  SocketAddress _laddr;
  /** The timeout value. */
  int _sotimeout;

  /** @cond INTERNAL */
  bool _connected;
  bool _reuseAddr;
  /** @endcond */

 private:

  CCXX_COPY_DECLS(Socket);
};

} // namespace ccxx

#endif // __ccxx_Socket_hxx
