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

#include "commonc++/StreamPipe.h++"
#include "commonc++/System.h++"

#ifdef CCXX_OS_POSIX
#include "commonc++/POSIX.h++"
#include "commonc++/SocketUtil.h++"

#else
#include "commonc++/Windows.h++"
#endif

#include <sys/stat.h>
#include <cerrno>

namespace ccxx {

#ifndef PIPE_BUF
#define PIPE_BUF 4096
#endif

#ifdef HAVE_STRUCT_MSGHDR_MSG_CONTROL

#ifdef CCXX_OS_MACOSX
#undef CMSG_SPACE
#define CMSG_SPACE(l) (sizeof(struct cmsghdr) + sizeof(l))
#endif // CCXX_OS_MACOSX

union __ccxx_cmsg_un
{
  struct cmsghdr header;
  char control[CMSG_SPACE(sizeof(int))];
};
#endif // HAVE_STRUCT_MSGHDR_MSG_CONTROL

/*
 */

StreamPipe::StreamPipe()
  : _connected(false)
{
}

/*
 */

StreamPipe::~StreamPipe()
{
  close();
}

/*
 */

void StreamPipe::connect(const String& path)
{
  if(_connected)
    throw IOException("already connected");

  FileHandle handle;

#ifdef CCXX_OS_WINDOWS

  String npath = path;
  npath.prepend("\\\\.\\pipe\\");

  DWORD timeout = (_timeout >= 0) ? _timeout : NMPWAIT_WAIT_FOREVER;

  if(! ::WaitNamedPipeW(npath.data(), timeout))
    throw TimeoutException();

  handle = ::CreateFileW(npath.data(), FILE_READ_DATA | FILE_WRITE_DATA,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

  if(handle == INVALID_HANDLE_VALUE)
    throw IOException(System::getErrorString("CreateFile"));

#else

  handle = ::socket(AF_LOCAL, SOCK_STREAM, 0);
  if(handle < 0)
    throw IOException(System::getErrorString("socket"));

  struct sockaddr_un saddr;
  std::memset(&saddr, 0, sizeof(saddr));
  saddr.sun_family = AF_LOCAL;
  CString cstr_path = path.toUTF8();
  std::strcpy(saddr.sun_path, cstr_path.data());
  socklen_t len = SUN_LEN(&saddr);

  SocketUtil::connectSocket(handle, (struct sockaddr *)&saddr, len, _timeout);

#endif

  Stream::_init(handle, false, true, true);
  _connected = true;
}

/*
 */

void StreamPipe::close(IOMode mode /* = IOReadWrite */)
{
#ifdef CCXX_OS_WINDOWS

  if(_handle != NULL)
  {
    if(_server)
      ::DisconnectNamedPipe(_handle);
    else
      ::CloseHandle(_handle);

    _handle = NULL;
  }

#else

  if(_handle != -1)
  {
    ::shutdown(_handle, SHUT_RDWR);

    Stream::close(IOReadWrite);
  }

#endif

  _connected = false;
}

/*
 */

void StreamPipe::setPipeHandle(FileHandle handle)
{
  Stream::_init(handle, false, true, true);

#ifdef CCXX_OS_WINDOWS
  _server = true;
#endif
}

/*
 */

void StreamPipe::sendFileHandle(FileHandle handle)
{
  if(! _connected)
    throw IOException("not connected");

#ifdef CCXX_OS_WINDOWS

  // Receive the PID of the peer.

  DWORD pid;
  readFully(reinterpret_cast<byte_t *>(&pid), sizeof(pid));

  // TODO - this only works on Windows Vista and Windows Server 2008

  /*
  BOOL ok = FALSE;
  if(_server)
    ok = ::GetNamedPipeClientProcessId(_handle, &pid);
  else
    ok = ::GetNamedPipeServerProcessId(_handle, &pid);

  if(! ok)
    throw IOException(System::getErrorString("GetNamedPipe*ProcessId"));
  */

  // Duplicate the handle.

  HANDLE sourceProcess = ::GetCurrentProcess();
  HANDLE targetProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  HANDLE dupHandle;

  BOOL ok = ::DuplicateHandle(sourceProcess, handle, targetProcess, &dupHandle,
                              0, TRUE, DUPLICATE_SAME_ACCESS);

  ::CloseHandle(sourceProcess);
  ::CloseHandle(targetProcess);

  if(! ok)
    throw IOException(System::getErrorString("DuplicateHandle"));

  // Send the duplicated handle over the pipe.

  writeFully(reinterpret_cast<byte_t *>(&dupHandle), sizeof(dupHandle));

#else // ! CCXX_OS_WINDOWS

  char t = 'x';
  struct iovec vec[1];
  struct msghdr msg;

#ifdef HAVE_STRUCT_MSGHDR_MSG_CONTROL
  struct cmsghdr *cmsg;
  union __ccxx_cmsg_un control_un;

  msg.msg_control = control_un.control;
  msg.msg_controllen = sizeof(control_un.control);

  cmsg = CMSG_FIRSTHDR(&msg);
  cmsg->cmsg_len = CMSG_LEN(sizeof(int));
  cmsg->cmsg_level = SOL_SOCKET;
  cmsg->cmsg_type = SCM_RIGHTS;

  std::memcpy(CMSG_DATA(cmsg), &handle, sizeof(handle));
#endif // HAVE_STRUCT_MSGHDR_MSG_CONTROL

#ifdef HAVE_STRUCT_MSGHDR_MSG_ACCRIGHTS
  msg.msg_accrights = (caddr_t)&fd;
  msg.msg_accrightslen = sizeof(int);
#endif // HAVE_STRUCT_MSGHDR_MSG_ACCRIGHTS

  vec[0].iov_base = &t;
  vec[0].iov_len = 1;
  msg.msg_iov = vec;
  msg.msg_iovlen = 1;

  msg.msg_name = NULL;
  msg.msg_namelen = 0;

  if(::sendmsg(_handle, &msg, 0) == -1)
    throw IOException(System::getErrorString("sendmsg"));

#endif // CCXX_OS_WINDOWS
}

/*
 */

FileHandle StreamPipe::receiveFileHandle()
{
#ifdef CCXX_OS_WINDOWS

  // Send our PID to the peer.

  DWORD pid = ::GetCurrentProcessId();
  writeFully(reinterpret_cast<byte_t *>(&pid), sizeof(pid));

  // Receive the duplicated handle over the pipe.

  HANDLE handle;
  readFully(reinterpret_cast<byte_t *>(&handle), sizeof(HANDLE));

#else // ! CCXX_OS_WINDOWS

  int handle = -1;

  struct iovec vec;
  struct msghdr msg;
  ssize_t n;
#ifdef HAVE_STRUCT_MSGHDR_MSG_ACCRIGHTS
  int nfd;
#endif
  char t;
#ifdef HAVE_STRUCT_MSGHDR_MSG_CONTROL
  struct cmsghdr *cmsg;
  union __ccxx_cmsg_un control_un;

  msg.msg_control = control_un.control;
  msg.msg_controllen = sizeof(control_un.control);
#endif // HAVE_STRUCT_MSGHDR_MSG_CONTROL

#ifdef HAVE_STRUCT_MSGHDR_MSG_ACCRIGHTS
  msg.msg_accrights = (caddr_t)&nfd;
  msg.msg_accrightslen = sizeof(int);
#endif // HAVE_STRUCT_MSGHDR_MSG_ACCRIGHTS

  vec.iov_base = &t;
  vec.iov_len = 1;
  msg.msg_iov = &vec;
  msg.msg_iovlen = 1;

  msg.msg_name = NULL;
  msg.msg_namelen = 0;

  if((n = ::recvmsg(_handle, &msg, 0)) != 1)
    throw IOException(System::getErrorString("recvmsg"));

#ifdef HAVE_STRUCT_MSGHDR_MSG_CONTROL
  if(((cmsg = CMSG_FIRSTHDR(&msg)) != NULL)
     && (cmsg->cmsg_len == CMSG_LEN(sizeof(int))))
  {
    if(cmsg->cmsg_level != SOL_SOCKET)
      throw IOException("recvmsg: unexpected control message level");
    if(cmsg->cmsg_type != SCM_RIGHTS)
      throw IOException("recvmsg: unexpected control message type");

    std::memcpy(&handle, CMSG_DATA(cmsg), sizeof(handle));
  }
  else
    throw IOException("recvmsg: no control message present");
#endif // HAVE_STRUCT_MSGHDR_MSG_CONTROL */

#ifdef HAVE_STRUCT_MSGHDR_MSG_ACCRIGHTS
  if(msg.msg_accrightslen != sizeof(int))
    return(FALSE); /* descriptor was not passed */

#endif // HAVE_STRUCT_MSGHDR_MSG_ACCRIGHTS

#endif // CCXX_OS_WINDOWS

  return handle;
}


} // namespace ccxx
