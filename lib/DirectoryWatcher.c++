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

#include "commonc++/DirectoryWatcher.h++"

#if defined(CCXX_OS_WINDOWS)
#include <windows.h>
#elif defined(CCXX_OS_MACOSX)
#include <sys/event.h>
#include <sys/fcntl.h>
#else
#include <limits.h>
#ifdef HAVE_SYS_INOTIFY_H
#include <sys/inotify.h>
#endif
#include <sys/select.h>
#include <unistd.h>
#endif

namespace ccxx {

/*
 */

DirectoryWatcher::DirectoryWatcher(const String& directory)
  : _directory(directory)
  , _initialized(false)
#ifdef CCXX_OS_WINDOWS
  , _handle(INVALID_HANDLE_VALUE)
#else
  , _handle(-1)
  , _watch(-1)
#endif
{
}

/*
 */

void DirectoryWatcher::init()
{
  if(_initialized)
    return;

#if defined(CCXX_OS_WINDOWS)

  _handle = ::CreateFileW(c_directory.data(), FILE_LIST_DIRECTORY,
                          (FILE_SHARE_READ | FILE_SHARE_WRITE
                           | FILE_SHARE_DELETE), NULL, OPEN_EXISTING,
                          (FILE_FLAG_BACKUP_SEMANTICS
                           | FILE_FLAG_OVERLAPPED), NULL);
  if(_handle == INVALID_HANDLE_VALUE)
    throw SystemException(System::getErrorString("CreateFile"));

#elif defined(CCXX_OS_MACOSX)

  _handle = kqueue();
  if(_handle < 0)
    throw SystemException(System::getErrorString("kqueue"));

  CString cstr_directory = _directory.toUTF8();
  _watch = ::open(cstr_directory.data(), O_EVTONLY);
  if(_watch == -1)
  {
    String error = System::getErrorString("open");
    ::close(_handle);
    throw SystemException(error);
  }

#elif defined(HAVE_INOTIFY_INIT)

  _handle = ::inotify_init();
  if(_handle == -1)
    throw SystemException(System::getErrorString("inotify_init"));

  CString cstr_directory = _directory.toUTF8();
  _watch = ::inotify_add_watch(_handle, cstr_directory.data(),
                               (IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVE));
  if(_watch == -1)
  {
    String error = System::getErrorString("inotify_add_watch");
    ::close(_handle);
    throw SystemException(error);
  }

#else

  throw SystemException("Not implemented");

#endif

  _initialized = true;
}

/*
 */

DirectoryWatcher::~DirectoryWatcher()
{
#if defined(CCXX_OS_WINDOWS)

  if(_handle != INVALID_HANDLE_VALUE)
    ::CloseHandle(_handle);

#elif defined(CCXX_OS_MACOSX)

  if(_watch != -1)
    ::close(_watch);
  if(_handle != -1)
    ::close(_handle);

#elif defined(HAVE_INOTIFY_INIT)

  if(_watch != -1)
    if(::inotify_rm_watch(_handle, _watch) != -1)
      ::close(_watch);

  if(_handle != -1)
    ::close(_handle);

#endif
}

/*
 */

void DirectoryWatcher::run()
{
  if(!_initialized)
    return;

  String oldPath;

#if defined(CCXX_OS_WINDOWS)

  char buf[4096];
  char tbuf[MAX_PATH + 1];
  HANDLE evt = ::CreateEvent(NULL, TRUE, FALSE, NULL);
  OVERLAPPED ovl = { 0, 0, 0, 0, evt };

  while(! testCancel())
  {
    DWORD bytes = 0;
    char *p = buf;
    if(!::ReadDirectoryChangesW(_handle, buf, sizeof(buf), FALSE,
                                (FILE_NOTIFY_CHANGE_FILE_NAME
                                 | FILE_NOTIFY_CHANGE_DIR_NAME
                                 | FILE_NOTIFY_CHANGE_LAST_WRITE),
                                &bytes, &ovl, NULL))
      continue;

    if(::WaitForSingleObjectEx(evt, 1000, TRUE) != WAIT_OBJECT_0)
      continue;

    ::ResetEvent(evt);
    ::GetOverlappedResult(_handle, &ovl, &bytes, FALSE);

    while(bytes > 0)
    {
      FILE_NOTIFY_INFORMATION *info = (FILE_NOTIFY_INFORMATION *)p;

      int count = ::WideCharToMultiByte(CP_ACP, 0, info->FileName,
                                        info->FileNameLength / sizeof(WCHAR),
                                        tbuf, MAX_PATH, NULL, NULL);
      tbuf[count] = '\0';
      String path = tbuf;

      switch(info->Action)
      {
        case FILE_ACTION_ADDED:
          fileCreated(path);
          break;

        case FILE_ACTION_REMOVED:
          fileDeleted(path);
          break;

        case FILE_ACTION_MODIFIED:
          fileModified(path);
          break;

        case FILE_ACTION_RENAMED_OLD_NAME:
          oldPath = path;
          break;

        case FILE_ACTION_RENAMED_NEW_NAME:
          fileRenamed(oldPath, path);
          oldPath.clear();
          break;

        default:
          break;
      }

      if(info->NextEntryOffset == 0)
        break;

      p += info->NextEntryOffset;
      bytes -= info->NextEntryOffset;
    }
  }

  ::CloseHandle(evt);

#elif defined(CCXX_OS_MACOSX)

  struct kevent evt;
  uint_t events = (NOTE_DELETE |  NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB
                   | NOTE_LINK | NOTE_RENAME | NOTE_REVOKE);

  EV_SET(&evt, _watch, EVFILT_VNODE, EV_ADD | EV_CLEAR, events, 0, NULL);

  while(! testCancel())
  {
    struct timespec timeout = { 1, 0 };
    struct kevent evt_out;

    int r = ::kevent(_handle, &evt, 1, &evt_out, 1, &timeout);
    if(r > 0)
    {
      // TODO: implement for Mac
    }
  }

#elif defined(HAVE_INOTIFY_INIT)

  char buf[sizeof(struct inotify_event) + PATH_MAX];
  fd_set fds;

  while(! testCancel())
  {
    FD_ZERO(&fds);
    FD_SET(_handle, &fds);
    struct timeval tv = { 1, 0 };

  SELECT:
    int r = ::select(FD_SETSIZE, &fds, NULL, NULL, &tv);
    if(r < 0)
    {
      if(errno == EINTR)
        goto SELECT;
      else
        break;
    }
    else if(r > 0)
    {
      char *p = buf;
      r = ::read(_handle, buf, sizeof(buf));

      while(r > 0)
      {
        struct inotify_event *event = (struct inotify_event *)p;
        String path = event->name;

        if(event->mask & IN_CREATE)
          fileCreated(path);
        else if(event->mask & IN_DELETE)
          fileDeleted(path);
        else if(event->mask & IN_MODIFY)
          fileModified(path);
        else if(event->mask & IN_MOVED_FROM)
          oldPath = path;
        else if(event->mask & IN_MOVED_TO)
        {
          fileRenamed(oldPath, path);
          oldPath.clear();
        }

        size_t len = sizeof(struct inotify_event) + event->len;
        p += len;
        r -= len;
      }
    }
  }

#endif
}


} // namespace ccxx
