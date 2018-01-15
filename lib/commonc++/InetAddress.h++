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

#ifndef __ccxx_InetAddress_hxx
#define __ccxx_InetAddress_hxx

#include <commonc++/Common.h++>
#include <commonc++/ByteOrder.h++>
#include <commonc++/IOException.h++>
#include <commonc++/String.h++>

#ifdef CCXX_OS_POSIX
#include <sys/types.h>
#include <netinet/in.h>
#endif

namespace ccxx {

/**
 * An exception indicating a hostname resolution failure or malformed
 * IP address string.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API HostNotFoundException : public IOException
{
 public:

  /**
   * Construct a new <b>HostNotFoundException</b> with an optional cause
   * message.
   */
  HostNotFoundException(String message = String::empty);

  inline virtual void write(std::ostream& stream) const
  { _write(stream, "HostNotFoundException"); }

};

/**
 * An IPv4 Internet address.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API InetAddress
{
 public:

  /** Construct a new, wildcard InetAddress. */
  InetAddress();

  /**
   * Construct a new InetAddress for the given host name. The address
   * will not be valid until the hostname is resolved via a call to
   * resolve().
   *
   * @param host The host name; either a DNS name or a dot-separated IP
   * address.
   */
  explicit InetAddress(const String& host);

  /**
   * Construct a new, InetAddress for the given IP address.
   *
   * @param addr The IP address, represented as a 32-bit integer in host
   * byte order.
   */
  explicit InetAddress(uint32_t addr);

  /**
   * Construct a new, InetAddress for the given IP address.
   *
   * @param addr The IP address, represented as a 4-byte tuple.
   */
  explicit InetAddress(byte_t addr[4]);

  /** Destructor. */
  ~InetAddress();

  /**
   * Set the hostname for this address.
   *
   * @param host The host, as a dot-separated IP address or DNS name.
   * @throw HostNotFoundException If the hostname cannot be resolved.
   */
  void setHost(const String& host);

  /**
   * Set the host IP address for this address.
   *
   * @param addr The IP address, packed in a 32-bit integer.
   */
  void setAddress(uint32_t addr);

  /**
   * Get the host IP address for this address.
   *
   * @return The IP address, packed in a 32-bit integer in host byte order.
   */
  inline uint32_t getAddress() const
  { return(_addr); }

  /**
   * Get the network class for the IP address.
   *
   * @return The class, as a character in the range 'A' - 'E', or '?' if
   * the address is not a member of any class (e.g., 0.0.0.0).
   */
  char getNetworkClass() const;

  /**
   * Test if the IP address is a multicast address (range 224.0.0.0
   * through 239.255.255.255, also known as class "D").
   */
  bool isMulticast() const;

  /**
   * Test if the IP address is non-routable (one of 10.x.x.x,
   * 192.168.x.x, or 172.16.x.x - 172.31.x.x).
   */
  bool isNonroutable() const;

  /** Test if the IP address is a link-local address (169.254.x.x). */
  bool isLinkLocal() const;

  /**
   * Get the host name for this address. If necessary, a reverse DNS
   * lookup is done. If that fails, the host is returned in dot-separated
   * form.
   *
   * @throw HostNotFoundException If the hostname could not be resolved.
   */
  String getHost() const;

  /**
   * If the InetAddress was constructed with a host (DNS) name, attempt to
   * resolve the name to an IP address.
   *
   * @throw HostNotFoundException If the host name could not be resolved.
   */
  void resolve();

  /** Test if this InetAddress has been successfully resolved. */
  inline bool isResolved() const
  { return(_resolved); }

  /** Get aString representation for this address. */
  String toString() const;

  /** Get a String representation for this address in dot-separated form. */
  String toIPString() const;

  /** Equality operator. */
  bool operator==(const InetAddress& other) const;

  /** Inequality operator. */
  inline bool operator!=(const InetAddress& other) const
  { return(!operator==(other)); }

  /** Bitwise-OR another address with this address. */
  InetAddress& operator|=(const InetAddress& other);

  /** Bitwise-AND another address with this address. */
  InetAddress& operator&=(const InetAddress& other);

  /** A value representing the wildcard address, 0.0.0.0. */
  static const InetAddress ANY;

 private:

  bool _fromDotSeparated();

  uint32_t _addr;
  mutable String _host;
  mutable bool _resolved;
};

inline std::ostream& operator<<(std::ostream& stream, const InetAddress& a)
{
  CString cstr_a = a.toString().toUTF8();
  return(stream << cstr_a.data());
}

} // namespace ccxx

#endif // __ccxx_InetAddress_hxx
