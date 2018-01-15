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

#ifndef __ccxx_CircularBuffer_hxx
#define __ccxx_CircularBuffer_hxx

#include <commonc++/Common.h++>
#include <commonc++/AbstractBuffer.h++>
#include <commonc++/IOException.h++>
#include <commonc++/MemoryBlock.h++>
#include <commonc++/Stream.h++>

#include <algorithm>
#include <cstdlib>
#include <cstring>

#ifdef CCXX_OS_POSIX
#include <sys/uio.h>
#endif

namespace ccxx {

/**
 * A circular buffer that can be used to efficiently transfer data between
 * buffers and/or streams. The buffer has a <i>read position</i> and a
 * <i>write position</i>, as well as a <i>peek position</i>. When either
 * the read position or the write position reaches the end of the buffer,
 * it wraps around to the beginning, but the read position can never
 * overtake the write position. The peek position is always between the
 * read position and the write position, and is used to "look ahead" for
 * a specific value.
 *
 * @author Mark Lindner
 */
template <typename T> class CircularBuffer : public AbstractBuffer<T>
{
 public:

  /**
   * Construct a new <b>CircularBuffer</b> with the given size.
   *
   * @param size The capacity of the buffer, in elements.
   */
  CircularBuffer(uint_t size);

  /** Destructor. */
  ~CircularBuffer();

  /** Clear the buffer. Resets the buffer to an "empty" state. */
  void clear();

  /**
   * Resize the buffer.
   *
   * @param newSize The new size, in elements.
   */
  void setSize(uint_t newSize);

  /**
   * Write data from an array into the buffer.
   *
   * @param buf The base of the array.
   * @param count The number of elements to read from the array.
   * @return The number of elements actually read.
   */
  uint_t write(const T* buf, uint_t count);

  /**
   * Write data from a <b>Buffer</b> into the buffer.
   *
   * @param buffer The buffer of data to write.
   * @param count The number of elements to write from the buffer, or
   * 0 to write as much as possible.
   * @return The number of elements actually read.
   */
  uint_t write(Buffer<T>& buffer, uint_t count = 0);

  /**
   * Read data from a stream and write it into the buffer.
   *
   * @param stream The stream to read from.
   * @param count The number of elements to write from the stream.
   * @return The number of elements actually written to the buffer.
   * @throw IOException If an I/O error occurs.
   */
  uint_t write(Stream& stream, uint_t count = 0);

  /**
   * Read data from the buffer into an array.
   *
   * @param buf The base of the array.
   * @param count The number of elements to write to the array.
   * @return The number of elements actually written.
   */
  uint_t read(T* buf, uint_t count);

  /**
   * Read data from the buffer into a <b>Buffer</b>.
   *
   * @param buffer The buffer to read into.
   * @param count The number of elements to copy to the buffer, or 0 to
   * write as much as possible.
   * @return The number of elements actually written.
   */
  uint_t read(Buffer<T>& buffer, uint_t count = 0);

  /**
   * Read data from the buffer and write it to a stream.
   *
   * @param stream The stream to write to.
   * @param count The number of bytes to write to the stream, or 0 to write as
   * much as possible.
   * @return The number of bytes actually written.
   * @throw IOException If an I/O error occurs.
   */
  uint_t read(Stream& stream, uint_t count = 0);

  /**
   * Scan forward from the current peek position for an element
   * equal to the given value.
   *
   * @param value The value to scan for.
   * @param maxlen The maximum number of elements to scan.
   * @param found A flag that is set to <b>true</b> if the item was found,
   * and <b>false</b> otherwise.
   * @param resetPeek If <b>true</b>, reset the peek position to the read
   * position before scanning.
   * @return The number of elements between the current read position and
   * the peeked value, inclusive, if found; otherwise, the number of
   * elements scanned (up to <b>maxlen</b>).
   */
  uint_t peek(const T& value, uint_t maxlen, bool& found,
              bool resetPeek = true);

  /**
   * Fill the buffer with the given value. Fills the requested
   * number of items. If the requested count exceeds the number of
   * items available to be written, only the available (free) items
   * are filled.
   *
   * @param value The value to fill with.
   * @param count The number of items to fill.
   * @return The number of items actually filled.
   */
  uint_t fill(const T& value, uint_t count);

  /** Get the read extent.
   *
   * @return The number of contiguous elements that can be read
   * beginning at the current read position, without wrapping to the
   * beginning of the buffer.
   */
  uint_t getReadExtent() const;

  /**
   * Get the write extent.
   *
   * @return The number of contiguous elements that can be written
   * beginning at the current write position, without wrapping to
   * the beginning of the buffer.
   */
  uint_t getWriteExtent() const;

  /** Get the number of elements available to be read from the buffer. */
  inline uint_t getRemaining() const
  { return(_avail); }

  /**
   * Get the number of elements available to be peeked, that is, the
   * number of elements between the peek position and the write position.
   */
  inline uint_t getPeekRemaining() const
  { return(_peekAvail); }

  /** Determine if the buffer is empty. */
  inline bool isEmpty() const
  { return(_avail == 0); }

  /** Determine if the buffer is full. */
  inline bool isFull() const
  { return(this->_size == _avail); }

  /** Get the number of elements available to be written to the buffer. */
  inline uint_t getFree() const
  { return(this->_size - _avail); }

  /** Get the current read position. */
  inline T* getReadPos()
  { return(_readPos); }

  /** Get the current write position. */
  inline T* getWritePos()
  { return(_writePos); }

  /** Get the current peek position. */
  inline T* getPeekPos()
  { return(_peekPos); }

  /**
   * Advance the read position by the given number of elements.
   *
   * @param count The number of elements to advance. If the value is
   * greater than the number of bytes available to be read, the read
   * position is left unchanged.
   * @return The new read position.
   */
  T* advanceReadPos(uint_t count);

  /**
   * Advance the write position by the given number of elements.
   *
   * @param count The number of elements to advance. If the value is
   * greater than the number of elements available to be written, the
   * write position is left unchanged.
   *
   * @return The new write position.
   */
  T* advanceWritePos(uint_t count);

  /**
   * Advance the read position by the number of elements in the
   * read extent, wrapping to the beginning of the buffer if
   * necessary.
   *
   * @return The new read position.
   */
  inline T* advanceReadPos()
  { return(advanceReadPos(getReadExtent())); }

  /**
   * Advance the write position by the number of elements in the
   * write extent, wrapping to the beginning of the buffer if
   * necessary.
   *
   * @return The new write position.
   */
  inline T* advanceWritePos()
  { return(advanceWritePos(getWriteExtent())); }

  /**
   * Advance the peek position by the given number of elements, wrapping
   * to the beginning of the buffer if necessary. If there are not enough
   * elements between the current peek position and the write position, the
   * peek position is not moved.
   *
   * @param count The number of elements to advance.
   * @return The new peek position.
   */
  T* advancePeekPos(uint_t count);

  /** Reset the peek position to the read position. */
  void resetPeekPos();

  /**
   * Determine if a partially-written element is in the buffer.
   * This will always be <b>false</b> when the template parameter is
   * an object of size 1.
   */
  bool isPartialWrite() const
  { return(_writeShift > 0); }

  /**
   * Determine if a partially-read element is in the buffer. This
   * will always be <b>false</b> when the template parameter is an
   * object of size 1.
   */
  bool isPartialRead() const
  { return(_readShift > 0); }

  /**
   * Marks the current read position. As long as no writes are performed on
   * the buffer, the read position can be rewound to the mark by calling
   * rewindReadPos().
   */
  void markReadPos();

  /**
   * Rewinds the read position to the read mark, and clears the read mark.
   * If no read mark is set, the method does nothing.
   */
  void rewindReadPos();

  /**
   * Marks the current write position. As long as no reads are performed on
   * the buffer, the write position can be rewound to the mark by calling
   * rewindWritePos().
   */
  void markWritePos();

  /**
   * Rewinds the write position to the write mark, and clears the write mark.
   * If no write mark is set, the method does nothing.
   */
  void rewindWritePos();

  /**
   * Moves the read position relative to the read mark.
   *
   * @param count The offset to set the read position to, relative to the read
   * mark.
   * @return <code>true</code> on success, or <code>false</code> if the count
   * is invalid or if no read mark is currently set.
   */
  bool setReadPosFromMark(uint_t count);

  /**
   * Moves the write position relative to the write mark.
   *
   * @param count The offset to set the write position to, relative to the
   * write mark.
   * @return <code>true</code> on success, or <code>false</code> if the count
   * is invalid or if no write mark is currently set.
   */
  bool setWritePosFromMark(uint_t count);

 private:

  T* _end;
  T* _readPos;
  T* _writePos;
  T* _peekPos;
  T* _readMark;
  T* _writeMark;
  uint_t _avail;
  uint_t _peekAvail;
  uint_t _readShift;
  uint_t _writeShift;

  CCXX_COPY_DECLS(CircularBuffer);
};

#include <commonc++/CircularBufferImpl.h++>

typedef CircularBuffer<byte_t> CircularByteBuffer;
typedef CircularBuffer<char> CircularCharBuffer;

} // namespace ccxx

#endif // __ccxx_CircularBuffer_hxx
