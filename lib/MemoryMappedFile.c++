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

#include "commonc++/MemoryMappedFile.h++"
#include "commonc++/File.h++"
#include "commonc++/System.h++"

#ifdef CCXX_OS_WINDOWS
#include "commonc++/Windows.h++"
#endif

#ifdef CCXX_OS_POSIX

#include "commonc++/POSIX.h++"
#include "commonc++/System.h++"

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#endif

#include <cerrno>

namespace ccxx {

/*
 */

MemoryMappedFile::MemoryMappedFile(const String& path)
  : _path(path),
    _open(false)
{
}

/*
 */

MemoryMappedFile::~MemoryMappedFile()
{
  close();
}

/*
 */

void MemoryMappedFile::open(uint64_t size /* = 0 */,
                            bool readOnly /* = false */)
{
  if(_open)
    throw IOException("already open");

  if(readOnly)
    size = 0;

  if(File::getSize(_path) == 0)
    throw IOException("cannot map zero-length file");

#ifdef CCXX_OS_WINDOWS

  SECURITY_ATTRIBUTES sa;

  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = NULL;
  sa.bInheritHandle = FALSE;

  _handle = ::CreateFileW(_path.data(), (readOnly ? GENERIC_READ
                                         : GENERIC_READ | GENERIC_WRITE),
                          (FILE_SHARE_READ | FILE_SHARE_WRITE),
                          &sa, OPEN_EXISTING,
                          (FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS),
                          NULL);

  if(_handle == INVALID_HANDLE_VALUE)
  {
    int err = ::GetLastError();
    if((err == ERROR_FILE_NOT_FOUND) || (err == ERROR_PATH_NOT_FOUND))
      throw PathNotFoundException(_path);
    else
      throw IOException(System::getErrorString("CreateFile"));
  }

  DWORD rangeLo = static_cast<DWORD>(size & 0xFFFFFFFF);
  DWORD rangeHi = static_cast<DWORD>((size >> 32) & 0xFFFFFFFF);

  _memHandle = ::CreateFileMapping(_handle, &sa,
                                   (readOnly ? PAGE_READONLY : PAGE_READWRITE),
                                   rangeHi, rangeLo, NULL);
  if(_memHandle == NULL)
  {
    String err = System::getErrorString("CreateFileMapping");
    ::CloseHandle(_handle);
    throw IOException(err);
  }

  _base = reinterpret_cast<byte_t *>
    (::MapViewOfFile(_memHandle,
                     (readOnly ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS),
                     0, 0, 0));

  if(_base == NULL)
  {
    String err = System::getErrorString("MapViewOfFile");
    ::CloseHandle(_handle);
    throw IOException(err);
  }

#else

  CString cstr_path = _path.toUTF8();
  if((_handle = ::open(cstr_path.data(), (readOnly ? O_RDONLY : O_RDWR))) < 0)
  {
    if(errno == ENOENT)
      throw PathNotFoundException(_path);
    else
      throw IOException(System::getErrorString("open"));
  }

  struct stat stbuf;

  if(::stat(cstr_path.data(), &stbuf) != 0)
    throw IOException(System::getErrorString("stat"));

  if(size == 0)
    size = stbuf.st_size;

  if(size != static_cast<uint64_t>(stbuf.st_size))
  {
    if(::ftruncate(_handle, static_cast<off_t>(size)) != 0)
    {
      String err = System::getErrorString("ftruncate");
      ::close(_handle);
      throw IOException(err);
    }
  }

  _base = static_cast<byte_t *>(
    ::mmap(NULL, size, (readOnly ? PROT_READ : (PROT_READ | PROT_WRITE)),
           MAP_SHARED, _handle, 0));

  if(_base == MAP_FAILED)
  {
    String err = System::getErrorString("mmap");
    ::close(_handle);
    throw IOException(err);
  }

#endif

  _size = size;
  _open = true;
}

/*
 */

void MemoryMappedFile::close()
{
  if(! _open)
    return;

  try
  {
    sync();
  }
  catch(IOException) { }

#ifdef CCXX_OS_WINDOWS

  ::UnmapViewOfFile(_base);

  ::CloseHandle(_memHandle);
  ::CloseHandle(_handle);

#else

  ::munmap(_base, _size);
  ::close(_handle);

#endif

  _open = false;
}

/*
 */

void MemoryMappedFile::sync(bool async /* = false */)
{
#ifdef CCXX_OS_WINDOWS

  if(::FlushViewOfFile(_base, 0) == FALSE)
    throw IOException(System::getErrorString("FlushViewOfFile"));

#else

  if(::msync(_base, _size, (async ? MS_ASYNC : MS_SYNC)) != 0)
    throw IOException(System::getErrorString("msync"));

#endif
}


} // namespace ccxx
