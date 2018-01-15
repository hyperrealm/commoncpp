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

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#include "commonc++/Network.h++"
#include "commonc++/ByteOrder.h++"
#include "commonc++/DynamicArray.h++"
#include "commonc++/System.h++"

#ifdef CCXX_OS_WINDOWS
#include <ws2tcpip.h>
#include <iphlpapi.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#ifdef CCXX_OS_MACOSX
#include <net/if_dl.h>
#include <ifaddrs.h>
#endif
#include <sys/ioctl.h>
#endif

#include <cstring>
#include <cerrno>
#include <algorithm>

namespace ccxx {

static const char *__protoNames[] = { NULL, "tcp", "udp" };

#define __MAKE_ADDR(A)                                                  \
  (ByteOrder::networkToHost(static_cast<uint32_t>(                      \
                              ((struct sockaddr_in *)(A))->sin_addr.s_addr)))

/*
 */

uint_t Network::getInterfaces(std::vector<NetworkInterface>& list)
{
  list.clear();

#ifdef CCXX_OS_WINDOWS

  DynamicArray<byte_t> buf(4096);

  IP_ADAPTER_ADDRESSES *addresses = NULL;
  ULONG bufLen = buf.size();
  ULONG flags = (GAA_FLAG_INCLUDE_PREFIX | GAA_SKIP_DNS_SERVER
                 | GAA_FLAG_SKIP_FRIENDLY_NAME);

  do
  {
    addresses = reinterpret_cast<IP_ADAPTER_ADDRESSES *>(buf.data());
    DWORD r = ::GetAdaptersAddresses(AF_INET, flags, NULL, addresses, &bufLen);

    if(r == ERROR_BUFFER_OVERFLOW)
      buf.resize(bufLen);
    else if(r != ERROR_SUCCESS)
      throw SystemExeption(System::getErrorString("GetAdaptersAddresses"));
    else
      break;
  } while(true);

  for(IP_ADAPTER_ADDRESS *address = addresses;
      address != NULL;
      address = address->Next)
  {
    NetworkInterface ixface;

    ixface.setName(String(address->AdapterName));

    ixface.setHardwareAddress(MACAddress(address->PhysicalAddress));

    IP_ADAPTER_UNICAST_ADDRESS *unicastAddr = address->FirstUnicastAddress;
    if(unicastAddr != NULL)
    {
      ixface.setAddress(InetAddress(__MAKE_ADDR(
                                      unicastAddr->Address.lpSockaddr)));
    }

    ixface.setUp((address->OperStatus == IfOperStatusUp)
                 || ((address->OperStatus == IfOperStatusDown)
                     && (address->AdminStatus != NET_IF_ADMIN_STATUS_DOWN)));
    ixface.setLoopback(address->IfType == IF_TYPE_SOFTWARE_LOOPBACK);
    ixface.setPPP(address->IfType == IF_TYPE_PPP);
    ixface.setRunning(address->OperStatus == IfOperStatusUp);
    ixface.setMulticastSupported((address->Flags & IP_ADAPTER_NO_MULTICAST)
                                 == 0);

    // TODO: how to get destination address on Windows?

    ixface.setMTU(address->Mtu);
    ixface.setMetric(address->Ipv4Metric);

    list.push_back(ixface);
  }

  SOCKET sock = ::WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
  if(sock == SOCKET_ERROR)
    throw SystemException(System::getErrorString("WSASocket"));

  INTERFACE_INFO *ixfaces = NULL;
  unsigned long bytesReturned = 0;

  for(DWORD num = 10; ; num += 10)
  {
    ixfaces = new INTERFACE_INFO[num];

    if(::WSAIoctl(sock, SIO_GET_INTERFACE_LIST, NULL, 0, ixfaces,
                  num * sizeof(INTERFACE_INFO), &bytesReturned, 0, 0)
       == SOCKET_ERROR)
    {
      delete[] ixfaces;

      if(::WSAGetLastError() == WSAEFAULT)
      {
        // output buffer too small; try again
        continue;
      }

      String err = System::getErrorString("WSAIoctl");
      ::closesocket(sock);
      throw SystemException(err);
    }

    break; // success
  }

  int numInterfaces = (bytesReturned / sizeof(INTERFACE_INFO));

  for(int i = 0; i < numInterfaces; ++i)
  {
    InetAddress addr(__MAKE_ADDR(&(ixfaces[i].iiAddress)));

    // Find the NetworkInterface with the matching address and add the
    // missing info.
    for(int j = 0; j < list.size(); ++j)
    {
      NetworkInterface& ixface = list[j];
      if(ixface.getAddress() == addr)
      {
        ixface.setBroadcastAddress(
          InetAddress(__MAKE_ADDR(&(ixfaces[i].iiBroadcastAddress))));
        ixface.setNetmask(InetAddress(__MAKE_ADDR(&(ixfaces[i].iiNetmask))));

        u_long flags = ixfaces[i].iiFlags;

        ixface.setBroadcastSupported(flags & IFF_BROADCAST);
        ixface.setLoopback(flags & IFF_LOOPBACK);
        ixface.setPPP(flags & IFF_POINTTOPOINT);
        ixface.setMulticastSupported(flags & IFF_MULTICAST);

        break;
      }
    }
  }

  delete[] ixfaces;
  ::closesocket(sock);

#else

#ifdef CCXX_OS_MACOSX

    struct ifaddrs *ifalist;
    ::getifaddrs(&ifalist);

#endif // CCXX_OS_MACOSX

  int sock, lastlen = 0, len;
  char *buf, *ptr, lastname[IFNAMSIZ];
  struct ifconf ifc;
  struct ifreq *ifr, ifrcopy;

  sock = ::socket(AF_INET, SOCK_DGRAM, 0);

  for(len = 10 * sizeof(struct ifreq);; len += 10 * sizeof(struct ifreq))
  {
    buf = new char[len];
    ifc.ifc_len = len;
    ifc.ifc_buf = buf;

    if(::ioctl(sock, SIOCGIFCONF, &ifc) < 0)
    {
      if((errno != EINVAL) || (lastlen != 0))
      {
        delete[] buf;
        throw SystemException(System::getErrorString("ioctl"));
      }
    }
    else
    {
      if(ifc.ifc_len == lastlen)
        break;

      lastlen = ifc.ifc_len;
    }

    delete[] buf;
  }

  *lastname = NUL;

  sockaddr nullAddr;
  std::memset(&nullAddr, 0, sizeof(nullAddr));
  for(ptr = buf; ptr < buf + ifc.ifc_len; ptr += len)
  {
    NetworkInterface ixface;

    ifr = (struct ifreq *)ptr;

#ifdef HAVE_SOCKADDR_SA_LEN

    len = ifr->ifr_addr.sa_len + sizeof(ifr->ifr_name);
    len = std::max<size_t>(len, sizeof(struct ifreq));

#else

    len = sizeof(struct ifreq);

#endif

    if(ifr->ifr_addr.sa_family != AF_INET)
      continue;

    char *cptr;
    if((cptr = strchr(ifr->ifr_name, ':')))
      *cptr = NUL;

    if(! std::strncmp(lastname, ifr->ifr_name, IFNAMSIZ))
      continue; // skip aliases

    ixface.setName(ifr->ifr_name);

    std::strncpy(lastname, ifr->ifr_name, IFNAMSIZ);

    ifrcopy = *ifr;
    ::ioctl(sock, SIOCGIFFLAGS, &ifrcopy);

    ixface.setUp(ifrcopy.ifr_flags & IFF_UP);
    ixface.setBroadcastSupported(ifrcopy.ifr_flags & IFF_BROADCAST);
    ixface.setLoopback(ifrcopy.ifr_flags & IFF_LOOPBACK);
    ixface.setPPP(ifrcopy.ifr_flags & IFF_POINTOPOINT);
    ixface.setRunning(ifrcopy.ifr_flags & IFF_RUNNING);
    ixface.setMulticastSupported(ifrcopy.ifr_flags & IFF_MULTICAST);
    ixface.setAddress(InetAddress(__MAKE_ADDR(&ifrcopy.ifr_addr)));

    byte_t mac[6] = { 0, 0, 0, 0, 0, 0 };

#if defined(SIOCGIFHWADDR)
    if(::ioctl(sock, SIOCGIFHWADDR, &ifrcopy) == 0)
      std::memcpy(mac, ifrcopy.ifr_hwaddr.sa_data, sizeof(mac));
#endif

#ifdef CCXX_OS_MACOSX

    // look up MAC address

    for(struct ifaddrs *addr = ifalist; addr; addr = addr->ifa_next)
    {
      struct sockaddr *saddr = addr->ifa_addr;
      if((saddr->sa_family == AF_LINK)
         && ! std::strcmp(ifr->ifr_name, addr->ifa_name))
      {
        byte_t *m = (byte_t *)LLADDR((struct sockaddr_dl *)addr->ifa_addr);
        std::memcpy(mac, m, 6);
        break;
      }
    }

#endif

    ixface.setHardwareAddress(MACAddress(mac));

    if(::ioctl(sock, SIOCGIFBRDADDR, &ifrcopy) < 0)
      ixface.setBroadcastAddress(InetAddress::ANY);
    else
      ixface.setBroadcastAddress(InetAddress(__MAKE_ADDR(
                                               &ifrcopy.ifr_broadaddr)));

    if(::ioctl(sock, SIOCGIFDSTADDR, &ifrcopy) < 0)
      ixface.setDestAddress(InetAddress::ANY);
    else
      ixface.setDestAddress(InetAddress(__MAKE_ADDR(&ifrcopy.ifr_dstaddr)));

    if(::ioctl(sock, SIOCGIFNETMASK, &ifrcopy) < 0)
      ixface.setNetmask(InetAddress::ANY);
    else
    {
#ifdef CCXX_OS_MACOSX
      ixface.setNetmask(InetAddress(__MAKE_ADDR(&ifrcopy.ifr_addr)));
#else
      ixface.setNetmask(InetAddress(__MAKE_ADDR(&ifrcopy.ifr_netmask)));
#endif
    }

    if(::ioctl(sock, SIOCGIFMTU, &ifrcopy) < 0)
      ixface.setMTU(0);
    else
      ixface.setMTU(ifrcopy.ifr_mtu);

    // on Linux, the metric field is 0, however ifconfig displays '1'
    // thanks to a hack in its logic.
    // ifconfig.c:190 has the expression  "ptr->metric?ptr->metric:1"

    if(::ioctl(sock, SIOCGIFMETRIC, &ifrcopy) < 0)
    {
      //perror("ioctl");
      ixface.setMetric(0);
    }
    else
      ixface.setMetric(ifrcopy.ifr_metric);

    list.push_back(ixface);
  }

#ifdef CCXX_OS_MACOSX

  ::freeifaddrs(ifalist);

#endif // CCXX_OS_MACOSX

  delete[] buf;
  ::shutdown(sock, SHUT_RDWR);
  ::close(sock);

#endif

  return(list.size());
}


/*
 */

#if !defined(HAVE_GETxxxxBYyyyy_R_SUN) && !defined(HAVE_GETxxxxBYyyyy_R_POSIX)

static int copyNameArray(char **src, char *buf, size_t bufsz)
{
  char *p = buf, **q, **w;
  int alen, left = bufsz, pos = 0, off, pad, hl;

  // compute the length of the pointer array first

  for(alen = 0, q = src; *q; alen++, q++);
  off = sizeof(char *) * ++alen;

  if(left < off)
    return(0);

  // copy strings after the end of the space reserved for the array,
  // while storing pointers to the copied strings in successive slots in
  // that reserved space

  left -= off;
  pos += off;
  p += off;

  for(w = &buf, q = src; *q; q++, w++)
  {
    hl = std::strlen(*q) + 1;

    if(left < hl)
      return(0);

    std::strncpy(p, *q, hl);
    *w = p;
    p += hl;
    left -= hl;
    off += hl;
  }
  *w = NULL;

  // skip enough bytes to word-align next write

  pad = sizeof(void *) - (off % sizeof(void *));

  if(left < pad)
    return(0);

  off += pad;

  return(off);
}

/*
 */

static int copyAddrArray(char **src, char *buf, size_t addrsz, size_t bufsz)
{
  char *p = buf, **q, **w;
  int alen, pos = 0;
  size_t left = bufsz, off, pad;

  // compute the length of the pointer array first

  for(alen = 0, q = src; *q; alen++, q++);
  off = sizeof(char *) * ++alen;

  if(left < off)
    return(0);

  // copy strings after the end of the space reserved for the array,
  // while storing pointers to the copied strings in successive slots in
  // that reserved space

  left -= off;
  pos += off;
  p += off;

  for(w = &buf, q = src; *q; q++, w++)
  {
    if(left < addrsz)
      return(0);

    std::memcpy(p, *q, addrsz);
    *w = p;
    p += addrsz;
    left -= addrsz;
    off += addrsz;
  }
  *w = NULL;

  // skip enough bytes to word-align next write

  pad = sizeof(void *) - (off % sizeof(void *));

  if(left < pad)
    return(0);

  off += pad;

  return(off);
}

/*
 */

static struct hostent *copyStructHostent(struct hostent *rhe,
                                         struct hostent *result,
                                         char *buffer, int buflen)
{
  char *p = buffer;
  int left = buflen, hl, len = 0, r;

  // copy the hostent structure

  std::memcpy((void *)result, (void *)rhe, sizeof(struct hostent));

  // copy the canonical name

  hl = std::strlen(rhe->h_name) + 1;
  if(left < hl)
    return(NULL);

  std::strncpy(p, rhe->h_name, hl);
  len += hl;
  p += hl;
  left -= hl;

  // copy the array of aliases

  r = copyNameArray(rhe->h_aliases, p, left);
  if(r == 0)
    return(NULL);

  len += r;
  p += r;
  left -= r;

  // copy the array of addresses

  r = copyAddrArray(rhe->h_addr_list, p, rhe->h_length, left);
  if(r == 0)
    return(NULL);

  return(result);
}

/*
 */

static struct servent *copyStructServent(struct servent *rse,
                                         struct servent *result,
                                         char *buffer, int buflen)
{
  char *p = buffer;
  int left = buflen, hl, len = 0, r;

  // copy the servent structure

  std::memcpy((void *)result, (void *)rse, sizeof(struct servent));

  // copy the canonical name

  hl = std::strlen(rse->s_name) + 1;
  if(left < hl)
    return(NULL);

  std::strncpy(p, rse->s_name, hl);
  len += hl;
  p += hl;
  left -= hl;

  // copy the array of aliases

  r = copyNameArray(rse->s_aliases, p, left);
  if(r == 0)
    return(NULL);

  len += r;
  p += r;
  left -= r;

  // copy the protocol name

  hl = std::strlen(rse->s_proto) + 1;
  if(left < hl)
    return(NULL);

  std::strncpy(p, rse->s_proto, hl);

  return(result);
}

#endif // !defined(HAVE_GETxxxxBYyyyy_R_SUN)
       //   && !defined(HAVE_GETxxxxBYyyyy_R_POSIX)

/*
 */

uint16_t Network::getPortForService(const String& service, NetProtocol& proto)
{
  struct servent se, *r;
  DynamicArray<char> buf(256);

  CString cstr_service = service.toUTF8();

  for(;;)
  {
    if(getServByName(cstr_service.data(), __protoNames[proto], &se, buf.data(),
                     buf.size(), &r) != 0)
    {
      if(((errno == ERANGE) || (errno == EINVAL)) && (buf.size() < 4096))
      {
        // resize buffer & try again
        buf.resize(buf.size() * 2);
      }
      else
        return(0);
    }
    else
      break;
  }

  if(proto == ProtoAny)
  {
    if(! ::strcmp(se.s_proto, __protoNames[1]))
      proto = ProtoTCP;
    else if(! ::strcmp(se.s_proto, __protoNames[2]))
      proto = ProtoUDP;
  }

  return(static_cast<uint16_t>(ntohs(se.s_port)));
}

/*
 */

String Network::getServiceForPort(uint16_t port, NetProtocol& proto)
{
  struct servent se, *r;
  DynamicArray<char> buf(256);

  for(;;)
  {
    if(getServByPort(htons(port), __protoNames[proto], &se, buf.data(),
                     buf.size(), &r) != 0)
    {
      if(((errno == ERANGE) || (errno == EINVAL)) && (buf.size() < 4096))
      {
        // resize buffer & try again
        buf.resize(buf.size() * 2);
      }
      else
        return(String::null);
    }
    else
      break;
  }

  if(proto == ProtoAny)
  {
    if(! ::strcmp(se.s_proto, __protoNames[1]))
      proto = ProtoTCP;
    else if(! ::strcmp(se.s_proto, __protoNames[2]))
      proto = ProtoUDP;
  }

  String name = se.s_name;
  return(name);
}

/*
 */

int Network::getHostByName(const char *name, struct hostent *ret,
                           char *buffer, int buflen, struct hostent **result,
                           int *h_errnop)
{
#if defined(HAVE_GETxxxxBYyyyy_R_SUN)

  struct hostent *r = ::gethostbyname_r(name, ret, buffer, buflen, h_errnop);

  if(result != NULL)
    *result = r;

  return(r ? 0 : -1);

#elif defined(HAVE_GETxxxxBYyyyy_R_POSIX)

  *h_errnop = 0;
  if(result != NULL)
    *result = NULL;

  int r = ::gethostbyname_r(name, ret, buffer, buflen, result, h_errnop);

  if(r == ERANGE)
  {
    r = -1;
    errno = ERANGE;
  }

  // Some libc's set *h_errnop even on success. Strange.
  if((result != NULL) && (*result != NULL))
    *h_errnop = 0;

  return(r);

#else

  struct hostent *rhe;

#ifndef CCXX_OS_WINDOWS // these are threadsafe on windows
  System::lockGlobalMutex();
#endif

  rhe = ::gethostbyname(name);
  if(! rhe)
  {
    *h_errnop = h_errno;
    ret = NULL;
  }
  else if(! copyStructHostent(rhe, ret, buffer, buflen))
  {
    errno = ERANGE;
    ret = NULL;
  }

  if(result)
    *result = ret;

#ifndef CCXX_OS_WINDOWS // these are threadsafe on windows
  System::unlockGlobalMutex();
#endif

  return(ret ? 0 : -1);

#endif
}

/*
 */

int Network::getHostByAddr(const char *addr, int length, int type,
                           struct hostent *ret, char *buffer, int buflen,
                           struct hostent **result, int *h_errnop)
{
#if defined(HAVE_GETxxxxBYyyyy_R_SUN)

  struct hostent *r = ::gethostbyaddr_r(addr, length, type, ret, buffer,
                                        buflen, h_errnop);

  if(result != NULL)
    *result = r;

  return(r ? 0 : -1);

#elif defined(HAVE_GETxxxxBYyyyy_R_POSIX)

  *h_errnop = 0;
  if(result != NULL)
    *result = NULL;

  int r = ::gethostbyaddr_r(addr, length, type, ret, buffer, buflen, result,
                            h_errnop);

  if(r == ERANGE)
  {
    r = -1;
    errno = ERANGE;
  }

  // Some libc's set *h_errnop even on success. Strange.
  if((result != NULL) && (*result != NULL))
    *h_errnop = 0;

  return(r);

#else

  struct hostent *rhe;

#ifndef CCXX_OS_WINDOWS // these are threadsafe on windows
  System::lockGlobalMutex();
#endif

  rhe = ::gethostbyaddr(addr, length, type);
  if(! rhe)
  {
    *h_errnop = h_errno;
    ret = NULL;
  }
  else if(! copyStructHostent(rhe, ret, buffer, buflen))
  {
    errno = ERANGE;
    ret = NULL;
  }

  if(result)
    *result = ret;

#ifndef CCXX_OS_WINDOWS // these are threadsafe on windows
  System::unlockGlobalMutex();
#endif

  return(ret ? 0 : -1);

#endif
}

/*
 */

int Network::getServByName(const char *name, const char *proto,
                           struct servent *ret, char *buffer, int buflen,
                           struct servent **result)
{
#if defined(HAVE_GETxxxxBYyyyy_R_SUN)

  struct servent *r = ::getservbyname_r(name, proto, ret, buffer, buflen);

  if(result != NULL)
    *result = r;

  return(r ? 0 : -1);

#elif defined(HAVE_GETxxxxBYyyyy_R_POSIX)

  int r = ::getservbyname_r(name, proto, ret, buffer, buflen, result);

  if(r == ERANGE)
  {
    errno = ERANGE;
    r = -1;
  }

  return(r);

#else

  struct servent *rse;

#ifndef CCXX_OS_WINDOWS // these are threadsafe on windows
  System::lockGlobalMutex();
#endif

  rse = ::getservbyname(name, proto);
  if(! rse)
    ret = NULL;
  else if(! copyStructServent(rse, ret, buffer, buflen))
  {
    errno = ERANGE;
    ret = NULL;
  }

  if(result)
    *result = ret;

#ifndef CCXX_OS_WINDOWS // these are threadsafe on windows
  System::unlockGlobalMutex();
#endif

  return(ret ? 0 : -1);

#endif
}

/*
 */

int Network::getServByPort(int port, const char *proto, struct servent *ret,
                           char *buffer, int buflen, struct servent **result)
{
#if defined(HAVE_GETxxxxBYyyyy_R_SUN)

  struct servent *r = ::getservbyport_r(port, proto, ret, buffer, buflen);

  if(result != NULL)
    *result = r;

  return(r ? 0 : -1);

#elif defined(HAVE_GETxxxxBYyyyy_R_POSIX)

  int r = ::getservbyport_r(port, proto, ret, buffer, buflen, result);

  if(r == ERANGE)
  {
    errno = ERANGE;
    r = -1;
  }

  return(r);

#else

  struct servent *rse;

#ifndef CCXX_OS_WINDOWS // these are threadsafe on windows
  System::lockGlobalMutex();
#endif

  rse = ::getservbyport(port, proto);
  if(! rse)
    ret = NULL;
  else if(! copyStructServent(rse, ret, buffer, buflen))
  {
    errno = ERANGE;
    ret = NULL;
  }

  if(result)
    *result = ret;

#ifndef CCXX_OS_WINDOWS // these are threadsafe on windows
  System::unlockGlobalMutex();
#endif

  return(ret ? 0 : -1);

#endif
}


} // namespace ccxx
