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

#ifndef __ccxx_Network_hxx
#define __ccxx_Network_hxx

#include <commonc++/Common.h++>
#include <commonc++/NetworkInterface.h++>
#include <commonc++/String.h++>
#include <commonc++/SystemException.h++>

#ifdef CCXX_OS_POSIX
#include <netdb.h>
#endif

#include <vector>

namespace ccxx {

/** Network protocol types. */
enum NetProtocol {
  /** Any (unspecified) protocol. */
  ProtoAny,
  /** TCP protocol. */
  ProtoTCP,
  /** UDP protocol. */
  ProtoUDP
};

/**
 * Network-related routines.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Network
{
  friend class InetAddress;

 public:

  /**
   * Get a list of all available network interfaces on the host system.
   *
   * @param list The list in which to store the interface information.
   * @return The number of interfaces retrieved.
   * @throw SystemException If an error occurs while retrieving the
   * information.
   */
  static uint_t getInterfaces(std::vector<NetworkInterface>& list);

  /**
   * Look up the port number for the given network service.
   *
   * @param service The name of the service.
   * @param proto The protocol of the service; if <b>ProtoAny</b>, the
   * value is updated to reflect the actual protocol of the service.
   * @return The port number of the service, or 0 if no matching service was
   * found.
   */
  static uint16_t getPortForService(const String& service, NetProtocol& proto);

  /**
   * Look up the network service name for the given port number.
   *
   * @param port The port number.
   * @param proto The protocol of the service; if <b>ProtoAny</b>, the
   * value is updated to reflect the actual protocol of the service.
   * @return The name of the service, or <b>String::null</b> if no matching
   * service was found.
   */
  static String getServiceForPort(uint16_t port, NetProtocol& proto);

 private:

  static int getHostByName(const char* name, struct hostent* ret,
                           char* buffer, int buflen, struct hostent** result,
                           int* h_errnop);

  static int getHostByAddr(const char* addr, int length, int type,
                           struct hostent* ret, char* buffer, int buflen,
                           struct hostent** result, int* h_errnop);

  static int getServByName(const char* name, const char* proto,
                           struct servent* ret, char* buffer, int buflen,
                           struct servent** result);

  static int getServByPort(int port, const char* proto, struct servent* ret,
                           char* buffer, int buflen, struct servent** result);
};

} // namespace ccxx

#endif // __ccxx_Network_hxx
