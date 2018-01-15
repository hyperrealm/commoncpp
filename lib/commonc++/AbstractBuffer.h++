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

#ifndef __ccxx_AbstractBuffer_hxx
#define __ccxx_AbstractBuffer_hxx

#include <commonc++/Common.h++>
#include <commonc++/UnsupportedOperationException.h++>

#include <cstring>

namespace ccxx {

/**
 * An abstract base class for buffers.
 *
 * @author Mark Lindner
 */
template<typename T> class AbstractBuffer
{
 public:

  /** Destructor. */
  virtual ~AbstractBuffer();

  /**
   * Resize the buffer.
   *
   * @param newSize The new size, in elements.
   * @throw UnsupportedOperationException If this object does not own the
   * underlying byte buffer.
   */
  virtual void setSize(uint_t newSize);

  /** Clear the buffer. */
  virtual void clear();

  /**
   * Get the size of the buffer.
   *
   * @return The size, in elements.
   */
  inline uint_t getSize() const
  { return(_size); }

  /** Get the number of elements remaining in the buffer. */
  virtual uint_t getRemaining() const = 0;

  /** Test if the buffer has elements available to be read or written. */
  inline virtual bool hasRemaining() const
  { return(getRemaining() > 0); }

  /** Get a pointer to the base of the buffer. */
  inline T* getBase()
  { return(_data); }

  /** Get a pointer to the base of the buffer. */
  inline const T* getBase() const
  { return(_data); }

 protected:

  /**
   * Construct a new buffer with the given size.
   *
   * @param size The capacity of the buffer, in elements.
   */
  AbstractBuffer(uint_t size);

  /** Construct a new buffer to wrap a raw byte buffer.
   *
   * @param data The raw data to wrap.
   * @param size The capacity of the buffer, in elements.
   * @param takeOwnership Whether the buffer will take ownership of the raw data
   * and deallocate it in its destructor.
   */
  AbstractBuffer(T* data, size_t size, bool takeOwnership = true);

  /** A pointer to the raw buffer. */
  T* _data;
  /** The size of the buffer. */
  uint_t _size;
  /** Whether this object owns the raw buffer. */
  bool _owner;

 private:

  CCXX_COPY_DECLS(AbstractBuffer);
};

#include <commonc++/AbstractBufferImpl.h++>

} // namespace ccxx

#endif // __ccxx_AbstractBuffer_hxx
