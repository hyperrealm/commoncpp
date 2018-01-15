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

#ifndef __ccxx_MulticastSocket_hxx
#define __ccxx_MulticastSocket_hxx

#include <commonc++/Common.h++>
#include <commonc++/DatagramSocket.h++>
#include <commonc++/NetworkInterface.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * A UDP multicast socket. Multicast socket addresses range from
 * 224.0.0.0 to 239.255.255.255, inclusive, with 224.0.0.0 reserved.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API MulticastSocket : public DatagramSocket
{
 public:

  /**
   * Construct a new MulticastSocket bound to the given port.
   *
   * @param port The port on which to listen.
   */
  MulticastSocket(uint16_t port = 0);

  /** Destructor. */
  ~MulticastSocket();

  /**
   * Join a multicast group.
   *
   * @param address The address to join, as a dot-separated IP address.
   * @throw SocketException If the operation fails.
   */
  void join(const String& address);

  /**
   * Join a multicast group.
   *
   * @param address The address to join.
   * @throw SocketException If the operation fails.
   */
  void join(const InetAddress& address);

  /**
   * Join a multicast group on a specific interface.
   *
   * @param address The address to join, as a dot-separated IP address.
   * @param ixface The network interface.
   * @throw SocketException If the operation fails.
   */
  void join(const String& address, const NetworkInterface& ixface);

  /**
   * Join a multicast group on a specific interface.
   *
   * @param address The address to join.
   * @param ixface The network interface.
   * @throw SocketException If the operation fails.
   */
  void join(const InetAddress& address, const NetworkInterface& ixface);

  /**
   * Leave a multicast group.
   *
   * @param address The address to leave, as a dot-separated IP address.
   * @throw SocketException If the operation fails.
   */
  void leave(const String& address);

  /**
   * Leave a multicast group.
   *
   * @param address The address to leave.
   * @throw SocketException If the operation fails.
   */
  void leave(const InetAddress& address);

  /**
   * Leave a multicast group on a specific interface.
   *
   * @param address The address to leave, as a dot-separated IP address.
   * @param ixface The network interface.
   * @throw SocketException If the operation fails.
   */
  void leave(const String& address, const ccxx::NetworkInterface& ixface);

  /**
   * Leave a multicast group on a specific interface.
   *
   * @param address The address to leave.
   * @param ixface The network interface.
   * @throw SocketException If the operation fails.
   */
  void leave(const InetAddress& address,
             const ccxx::NetworkInterface& ixface);

  /**
   * Set the TTL (time-to-live) value for outgoing multicast datagrams.
   * The TTL is the number of "hops" that a packet is allowed to make
   * on the network before being discarded.
   *
   * @param ttl The new TTL value.
   * @throw SocketException If the operation fails.
   */
  void setTTL(uint8_t ttl);

  /**
   * Get the TTL value.
   *
   * @throw SocketException If the operation fails.
   */
  uint8_t getTTL() const;

  /**
   * Enable or disable local loopback of multicast datagrams. When the
   * loopback is enabled, the sender receives copies of all multicast
   * datagrams that it sends.
   *
   * @param enabled <b>true</b> if loopback should be enabled, <b>false</b>
   * if it should be disabled.
   * @throw SocketException If the operation fails.
   */
  void setLoopbackEnabled(bool enabled);

  /**
   * Determine if local loopback is enabled.
   *
   * @return <b>true</b> if loopback is enabled, <b>false</b> otherwise.
   * @throw SocketException If the operation fails.
   */
  bool isLoopbackEnabled() const;

  /** A TTL value representing localhost scope. */
  static uint8_t TTL_HOST;
  /** A TTL value representing subnet scope. */
  static uint8_t TTL_SUBNET;
  /** A TTL value representing site-wide scope. */
  static uint8_t TTL_SITE;
  /** A TTL value representing regional scope. */
  static uint8_t TTL_REGION;
  /** A TTL value representing continental scope. */
  static uint8_t TTL_CONTINENT;
  /** A TTL value representing unlimited scope. */
  static uint8_t TTL_UNRESTRICTED;

 private:

  void verifyAddress(const InetAddress& address);

  uint16_t _port;

  CCXX_COPY_DECLS(MulticastSocket);
};

} // namespace ccxx

#endif // __ccxx_MulticastSocket_hxx
