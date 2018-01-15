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

#ifndef __ccxx_MemoryMappedFile_hxx
#define __ccxx_MemoryMappedFile_hxx

#include <commonc++/Common.h++>
#include <commonc++/IOException.h++>
#include <commonc++/Permissions.h++>

namespace ccxx {

/**
 * An abstraction of a memory-mapped file.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API MemoryMappedFile
{
 public:

  /**
   * Construct a new MemoryMappedFile for the given path.
   *
   * @param path The path of the file to be mapped.
   */
  MemoryMappedFile(const String& path);

  /** Destructor. Unmaps the file. */
  ~MemoryMappedFile();

  /**
   * Open the file and map it into memory. Zero-length files cannot be
   * mapped.
   *
   * @param size The maximum file size to map; if the file being mapped
   * is larger than this size, the file is truncated to the new size before
   * it is mapped into memory. A value of 0 indicates that the maximum size
   * is the current size of the file.
   * @param readOnly A flag indicating whether the file should be mapped as
   * read-only or read-write. If mapping read-only, the size parameter is
   * ignored, and taken as 0.
   * @throw IOException If the file to be mapped has a length of 0, or if
   * the file could not be opened or the mapping operation failed.
   */
  void open(uint64_t size = 0, bool readOnly = false);

  /** Unmap and close the file. */
  void close();

  /**
   * Synchronize the in-memory state of the file with the file on disk.
   *
   * @param async A flag indicating whether the sync operation should occur
   * asynchronously.
   * @throw IOException If an I/O error occurs.
   */
  void sync(bool async = false);

  /** Get a pointer to the base of the mapped memory segment. */
  inline byte_t* getBase()
  { return(_base); }

  /** Get a pointer to the base of the mapped memory segment. */
  inline const byte_t* getBase() const
  { return(_base); }

  /** Get the size of the memory mapped segment, in bytes. */
  inline uint64_t getSize() const
  { return(_size); }

 private:

  String _path;
  FileHandle _handle;
#ifdef CCXX_OS_WINDOWS
  HANDLE _memHandle;
#endif
  byte_t* _base;
  uint64_t _size;
  bool _open;
};

} // namespace ccxx

#endif // __ccxx_MemoryMappedFile_hxx
