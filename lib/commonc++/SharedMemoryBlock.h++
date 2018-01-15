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

#ifndef __ccxx_SharedMemoryBlock_hxx
#define __ccxx_SharedMemoryBlock_hxx

#include <commonc++/Common.h++>
#include <commonc++/MemoryBlock.h++>
#include <commonc++/Permissions.h++>
#include <commonc++/String.h++>
#include <commonc++/SystemException.h++>

namespace ccxx {

/**
 * An encapsulation of a block of memory that can be mapped by one or
 * more (not necessarily related) processes; when one process changes
 * the data in the block, the changes are immediately visible to the
 * other processes.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API SharedMemoryBlock : public MemoryBlock
{
 public:

  /**
   * Construct a new SharedMemoryBlock. The actual memory segment is not
   * created and mapped until a call to <b>attach()</b> is made.
   *
   * @param name The name of the memory block. For maximal portability, the
   * name should consist of at most 14 alphanumeric characters and should not
   * contain slashes.
   * @param size The size of the memory block, in bytes. This size will be
   * rounded up to the nearest multiple of the system page size.
   * @param perm The permissions with which to create the memory block, if it
   * does not exist at the time of the <b>attach()</b>.
   */
  SharedMemoryBlock(const String& name, size_t size,
                    const Permissions& perm = Permissions::USER_READ_WRITE);

  /** Destructor. Detaches from the memory block. */
  ~SharedMemoryBlock();

  /**
   * Attach to the memory block, creating it if it does not yet exist.
   *
   * @param readOnly A flag indicating whether the memory block should be
   * mapped for read-only access or read-write access.
   * @throw SystemException If the operation fails.
   */
  void attach(bool readOnly = false);

  /**
   * Detach from the memory block, and destroy it if no other process
   * currently has it mapped.
   */
  void detach();

  /** Get the name of the memory block. */
  inline String getName() const
  { return(_name); }

 private:

  String _name;
  Permissions _perm;

#ifdef CCXX_OS_WINDOWS
  HANDLE _handle;
#else
  int _handle;
#endif

  CCXX_COPY_DECLS(SharedMemoryBlock);
};

} // namespace ccxx

#endif // __ccxx_SharedMemoryBlock_hxx
