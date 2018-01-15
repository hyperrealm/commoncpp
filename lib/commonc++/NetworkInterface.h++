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

#ifndef __ccxx_NetworkInterface_hxx
#define __ccxx_NetworkInterface_hxx

#include <commonc++/Common.h++>
#include <commonc++/InetAddress.h++>
#include <commonc++/MACAddress.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * An aggregation of information about a network interface.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API NetworkInterface
{
  friend class Network;

 public:

  /** A flag indicating that an interface is up. */
  static const uint32_t FLAG_UP;

  /** A flag indicating that an interface has broadcast capability. */
  static const uint32_t FLAG_BROADCAST;

  /** A flag identifying an interface as a loopback interface. */
  static const uint32_t FLAG_LOOPBACK;

  /** A flag identifying an interface as a Point-to-Point link interface. */
  static const uint32_t FLAG_PPP;

  /** A flag indicating that an interface is running. */
  static const uint32_t FLAG_RUNNING;

  /** A flag indicating that an interface has multicast capability. */
  static const uint32_t FLAG_MULTICAST;

  /** Flag mask (all flag bits set). */
  static const uint32_t FLAG_MASK;

  /** Destructor. */
  ~NetworkInterface();

  /** Get the interface name. */
  inline String getName() const
  { return(_name); }

  /** Get the unicast address. */
  inline InetAddress getAddress() const
  { return(_addr); }

  /** Get the hardware (MAC) address. */
  inline MACAddress getHardwareAddress() const
  { return(_hwAddr); }

  /** Get the broadcast address. */
  inline InetAddress getBroadcastAddress() const
  { return(_broadcastAddr); }

  /** Get the destination address. */
  inline InetAddress getDestAddress() const
  { return(_destAddr); }

  /** Get the netmask. */
  inline InetAddress getNetMask() const
  { return(_netmask); }

  /** Get the subnet prefix length. */
  inline uint_t getPrefixLength() const
  { return(_prefixLength); }

  /** Get the interface flags. */
  inline uint32_t getFlags() const
  { return(_flags); }

  /** Test if the interface is up. */
  inline bool isUp() const
  { return((_flags & FLAG_UP) != 0); }

  /** Test if the interface has broadcast capability. */
  inline bool isBroadcastSupported() const
  { return((_flags & FLAG_BROADCAST) != 0); }

  /** Test if the interface has multicast capability. */
  inline bool isMulticastSupported() const
  { return((_flags & FLAG_MULTICAST) != 0); }

  /** Test if the interface is a loopback interface. */
  inline bool isLoopback() const
  { return((_flags & FLAG_LOOPBACK) != 0); }

  /** Test if the interface is a PPP interface. */
  inline bool isPPP() const
  { return((_flags & FLAG_PPP) != 0); }

  /** Test if the interface is running. */
  inline bool isRunning() const
  { return((_flags & FLAG_RUNNING) != 0); }

  /** Get the maximum transmission unit (MTU). */
  inline int getMTU() const
  { return(_mtu); }

  /** Get the routing metric. */
  inline int getMetric() const
  { return(_metric); }

 private:

  NetworkInterface();

  inline void setName(const String& name)
  { _name = name; }

  inline void setAddress(const InetAddress& addr)
  { _addr = addr; }

  inline void setHardwareAddress(const MACAddress& addr)
  { _hwAddr = addr; }

  inline void setBroadcastAddress(const InetAddress& addr)
  { _broadcastAddr = addr; }

  inline void setDestAddress(const InetAddress& addr)
  { _destAddr = addr; }

  void setNetmask(const InetAddress& addr);

  inline void setUp(bool flag)
  { flag ? _flags |= FLAG_UP : _flags &= ~FLAG_UP; }

  inline void setBroadcastSupported(bool flag)
  { flag ? _flags |= FLAG_BROADCAST : _flags &= ~FLAG_BROADCAST; }

  inline void setLoopback(bool flag)
  { flag ? _flags |= FLAG_LOOPBACK : _flags &= ~FLAG_LOOPBACK; }

  inline void setPPP(bool flag)
  { flag ? _flags |= FLAG_PPP : _flags &= ~FLAG_PPP; }

  inline void setRunning(bool flag)
  { flag ? _flags |= FLAG_RUNNING : _flags &= ~FLAG_RUNNING; }

  inline void setMulticastSupported(bool flag)
  { flag ? _flags |= FLAG_MULTICAST : _flags &= ~FLAG_MULTICAST; }

  inline void setFlags(int flags)
  { _flags = flags & FLAG_MASK; }

  inline void setMTU(int mtu)
  { _mtu = mtu; }

  inline void setMetric(int metric)
  { _metric = metric; }

  String _name;
  MACAddress _hwAddr;
  InetAddress _addr;
  InetAddress _broadcastAddr;
  InetAddress _destAddr;
  InetAddress _netmask;
  int _mtu;
  int _metric;
  uint_t _prefixLength;
  uint32_t _flags;
};

} // namespace ccxx

#endif // __ccxx_NetworkInterface_hxx
