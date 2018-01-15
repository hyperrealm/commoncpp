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

#ifndef __ccxx_Stream_hxx
#define __ccxx_Stream_hxx

#include <commonc++/Common.h++>
#include <commonc++/Buffer.h++>
#include <commonc++/IOException.h++>
#include <commonc++/MemoryBlock.h++>

namespace ccxx {

class AsyncIOTask; // fwd decl

/** %Stream I/O modes. */
enum IOMode {
  /** Read-only mode. */
  IORead,
  /** Write-only mode. */
  IOWrite,
  /** Read/write mode. */
  IOReadWrite
};

/** %Stream seek modes. */
enum SeekMode {
  /** Seek from the beginning of the stream. */
  SeekAbsolute,
  /** Seek from the current stream position. */
  SeekRelative,
  /** Seek backward from the end of the stream. */
  SeekEnd
};

/**
 * An unbuffered I/O stream.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Stream
{
  friend class Process;

 public:

  /**
   * The maximum number of I/O buffers that can be passed to the vector I/O
   * methods.
   */
  static const uint_t MAX_IOBLOCK_COUNT;

 protected:

  /** Construct a new, uninitialized Stream. */
  Stream();

  /**
   * Construct a new stream.
   *
   * @param handle The handle to an open file.
   * @param seekable A flag indicating whether the stream is seekable.
   * @param readable A flag indicating whether the stream is readable.
   * @param writable A flag indicating whether the stream is writable.
   */
  Stream(FileHandle handle, bool seekable = true, bool readable = true,
         bool writable = true);

 public:

  /** Destructor. */
  virtual ~Stream();

  /**
   * Read data from the stream into a ByteBuffer.
   *
   * @param buffer The buffer to read into.
   * @return The number of bytes actually read.
   * @throw IOException If the end-of-file was reached or some other
   * I/O error occurred.
   */
  virtual size_t read(ByteBuffer& buffer);

  /**
   * Read data from the stream into a CharBuffer.
   *
   * @param buffer The buffer to read into.
   * @return The number of characters actually read.
   * @throw IOException If the end-of-file was reached or some other
   * I/O error occurred.
   */
  virtual size_t read(CharBuffer& buffer);

  /**
   * Read data from the stream into a Buffer of arbitrary type.
   *
   * If sizeof(T) is greater than 1 byte, then there can be no
   * guarantee that the number of bytes read before a timeout or
   * error occurs will be evenly divisible by sizeof(T). If the
   * final element was partially read, the number of bytes of the
   * element that were read will be stored in <i>partial</i>;
   * this value should be passed unmodified to the next invocation
   * of this method to continue reading from the appropriate offset
   * within the partially-read element.
   *
   * @param buffer The buffer to read into.
   * @param partial The number of bytes remaining to read for a
   * partially-read element; on return, the number of bytes read
   * of a partially-read element, or 0 if the last element was read
   * completely. Should be set to 0 prior to the first
   * call to this method for a fresh buffer.
   * @return The number of whole elements actually read.
   * @throw IOException If the end-of-file was reached or some other
   * I/O error occurred.
   */
  template<typename T> size_t read(Buffer<T>& buffer, size_t& partial)
  {
    size_t n = _readElem(reinterpret_cast<byte_t *>(buffer.getPointer()),
                         sizeof(T), buffer.getRemaining(), partial);

    if(n > 0)
      buffer.bump(n);

    return(n);
  }

  /**
   * Read data from the stream into a raw buffer.
   *
   * @param buffer The buffer to read into.
   * @param buflen The number of bytes to read.
   * @return The number of bytes actually read.
   * @throw IOException If the end-of-file was reached or some other
   * I/O error occurred.
   */
  virtual size_t read(byte_t* buffer, size_t buflen);

  /**
   * Read data asynchronously from the stream into a raw buffer.
   *
   * @param buffer The buffer to read into.
   * @param buflen The number of bytes to read.
   * @param offset The file offset to read from.
   * @param task The async I/O control block for monitoring the operation.
   * @return The number of bytes actually read, if the operation completed
   * immediately, otherwise 0 to indicate that the operation has begun.
   * @throw IOException If an I/O error occurred.
   */
  size_t read(byte_t* buffer, size_t buflen, int64_t offset,
              AsyncIOTask& task);

  /**
   * Read data asynchronously from the stream into a Buffer.
   *
   * @param buffer The Buffer to read into. This object must not be destroyed
   * before the asynchronous I/O operation is completed or cancelled.
   * @param offset The file offset to read from.
   * @param task The async I/O control block for monitoring the operation.
   * @return The number of bytes actually read, if the operation completed
   * immediately, otherwise 0 to indicate that the operation has begun.
   * @throw IOException If an I/O error occurred.
   */
  size_t read(ByteBuffer& buffer, int64_t offset, AsyncIOTask& task);

  /**
   * Write data asynchronously to the stream from a raw buffer.
   *
   * @param buffer The buffer to write.
   * @param buflen The number of bytes to write.
   * @param offset The file offset to write to.
   * @param task The async I/O control block for monitoring the operation.
   * @return The number of bytes actually written, if the operation completed
   * immediately, otherwise 0 to indicate that the operation has begun.
   * @throw IOException If an I/O error occurred.
   */
  size_t write(const byte_t* buffer, size_t buflen, int64_t offset,
               AsyncIOTask& task);

  /**
   * Write data asynchronously to the stream from a Buffer.
   *
   * @param buffer The Buffer to write. This object must not be destroyed
   * before the asynchronous I/O operation is completed or cancelled.
   * @param offset The file offset to write to.
   * @param task The async I/O control block for monitoring the operation.
   * @return The number of bytes actually written, if the operation completed
   * immediately, otherwise 0 to indicate that the operation has begun.
   * @throw IOException If an I/O error occurred.
   */
  size_t write(ByteBuffer& buffer, int64_t offset, AsyncIOTask& task);

  /**
   * Read data from the stream into a ByteBuffer, until either the
   * buffer is full or an error or timeout occurs.
   *
   * @param buffer The buffer to read into.
   * @param count The number of bytes to read. If 0 or greater than the
   * number of bytes between the position and the limit, reads up to the
   * limit.
   * @return The number of bytes actually read.
   * @throw IOException If the end-of-file was reached or some other
   * I/O error occurred.
   */
  virtual size_t readFully(ByteBuffer& buffer, size_t count = 0);

  /**
   * Read data from the stream into a CharBuffer, until either the
   * buffer is full or an error or timeout occurs.
   *
   * @param buffer The buffer to read into.
   * @param count The number of characters to read. If 0 or greater than the
   * number of bytes between the position and the limit, reads up to the
   * limit.
   * @return The number of bytes actually read.
   * @throw IOException If the end-of-file was reached or some other
   * I/O error occurred.
   */
  virtual size_t readFully(CharBuffer& buffer, size_t count = 0);

  /**
   * Read data from the stream into a Buffer of arbitrary type, until
   * either the buffer is full or an error or timeout occurs.
   *
   * If sizeof(T) is greater than 1 byte, then there can be no
   * guarantee that the number of bytes read before a timeout or
   * error occurs will be evenly divisible by sizeof(T). If the
   * final element was partially read, the number of bytes of the
   * element that were read will be stored in <i>partial</i>;
   * this value should be passed unmodified to the next invocation
   * of this method to continue reading from the appropriate offset
   * within the partially-read element.
   *
   * @param buffer The buffer to read into.
   * @param partial The number of bytes remaining to read for a
   * partially-read element; on return, the number of bytes read
   * of a partially-read element, or 0 if the last element was read
   * completely. Should be set to 0 prior to the first
   * call to this method for a fresh buffer.
   * @return The number of whole elements actually read.
   * @throw IOException If the end-of-file was reached or some other
   * I/O error occurred.
   */
  template<typename T> size_t readFully(Buffer<T>& buffer, size_t& partial)
  {
    size_t n = _readElemFully(reinterpret_cast<byte_t *>(buffer.getPointer()),
                              sizeof(T), buffer.getRemaining(), partial);

    if(n > 0)
      buffer.bump(n);

    return(n);
  }

  /**
   * Read data from the stream into a raw buffer, until either the
   * buffer is full or an error or timeout occurs.
   *
   * @param buffer The buffer to read into.
   * @param buflen The number of bytes to read.
   * @return The number of bytes actually read.
   * @throw IOException If the end-of-file was reached or some other
   * I/O error occurred.
   */
  virtual size_t readFully(byte_t* buffer, size_t buflen);

  /**
   * Read data from the stream into a series of I/O buffers. This
   * operation is known as "vector read" or "scatter read." The
   * buffers are read into in order. On systems that support it, the
   * operation is performed with a single system call.
   *
   * @param vec An array of buffers to read into.
   * @param count The number of buffers in the array.
   * @throw IOException If the end-of-file was reached or some other
   * I/O error occurred.
   */
  virtual size_t read(MemoryBlock* vec, uint_t count);

  /**
   * Write data to the stream from a ByteBuffer.
   *
   * @param buffer The buffer to write.
   * @return The number of bytes actually written.
   * @throw IOException If an I/O error occurred.
   */
  virtual size_t write(ByteBuffer& buffer);

  /**
   * Write data to the stream from a CharBuffer.
   *
   * @param buffer The buffer to write.
   * @return The number of characters actually written.
   * @throw IOException If an I/O error occurred.
   */
  virtual size_t write(CharBuffer& buffer);

  /**
   * Write data to the stream from a Buffer of arbitrary type.
   *
   * If sizeof(T) is greater than 1 byte, then there can be no
   * guarantee that the number of bytes written before a timeout or
   * error occurs will be evenly divisible by sizeof(T). If the
   * final element was partially written, the number of bytes of the
   * element that were written will be stored in <i>partial</i>;
   * this value should be passed unmodified to the next invocation
   * of this method to continue writing from the appropriate offset
   * within the partially-written element.
   *
   * @param buffer The buffer to write.
   * @param partial The number of bytes remaining to write for a
   * partially-written element; on return, the number of bytes written
   * of a partially-written element, or 0 if the last element was written
   * completely. Should be set to 0 prior to the first
   * call to this method for a fresh buffer.
   * @return The number of whole elements actually written.
   * @throw IOException If an I/O error occurred.
   */
  template<typename T> size_t write(Buffer<T>& buffer, size_t& partial)
  {
    size_t n = _writeElem(
      reinterpret_cast<const byte_t *>(buffer.getPointer()),
      sizeof(T), buffer.getRemaining(), partial);

    if(n > 0)
      buffer.bump(n);

    return(n);
  }

  /**
   * Write data to the stream from a raw buffer, until either the
   * entire buffer is written or an error or timeout occurs.
   *
   * @param buffer A pointer to the buffer.
   * @param buflen The number of bytes to write.
   * @return The number of bytes actually written.
   * @throw IOException If an I/O error occurred.
   */
  virtual size_t write(const byte_t* buffer, size_t buflen);

  /**
   * Write data to the stream from a ByteBuffer, until either the
   * entire buffer is written or an error or timeout occurs.
   *
   * @param buffer The buffer to write.
   * @return The number of bytes actually written.
   * @throw IOException If and I/O error occurred.
   */
  virtual size_t writeFully(ByteBuffer& buffer);

  /**
   * Write data to the stream from a CharBuffer, until either the
   * entire buffer is written or an error or timeout occurs.
   *
   * @param buffer The buffer to write.
   * @return The number of bytes actually written.
   * @throw IOException If an I/O error occurred.
   */
  virtual size_t writeFully(CharBuffer& buffer);

  /**
   * Write data to the stream from a Buffer of arbitrary type, until
   * either the entire buffer is written or an error or timeout occurs.
   *
   * If sizeof(T) is greater than 1 byte, then there can be no
   * guarantee that the number of bytes written before a timeout or
   * error occurs will be evenly divisible by sizeof(T). If the
   * final element was partially written, the number of bytes of the
   * element that were written will be stored in <i>partial</i>;
   * this value should be passed unmodified to the next invocation
   * of this method to continue writing from the appropriate offset
   * within the partially-written element.
   *
   * @param buffer The buffer to write.
   * @param partial The number of bytes remaining to write for a
   * partially-written element; on return, the number of bytes written
   * of a partially-written element, or 0 if the last element was written
   * completely. Should be set to 0 prior to the first
   * call to this method for a fresh buffer.
   * @return The number of whole elements actually written.
   * @throw IOException If an I/O error occurred.
   */
  template<typename T> size_t writeFully(Buffer<T>& buffer, size_t& partial)
  {
    size_t n = _writeElemFully(
      reinterpret_cast<const byte_t *>(buffer.getPointer()),
      sizeof(T), buffer.getRemaining(), partial);

    if(n > 0)
      buffer.bump(n);

    return(n);
  }

  /**
   * Write data to the stream from a raw buffer.
   *
   * @param buffer A pointer to the buffer.
   * @param buflen The number of bytes to write.
   * @return The number of bytes actually written.
   * @throw IOException If an I/O error occurred.
   */
  virtual size_t writeFully(const byte_t* buffer, size_t buflen);

  /**
   * Write data to the stream from a series of I/O buffers. This
   * operation is known as "vector write" or "gather write." The
   * buffers are written in order. On systems that support it, the
   * operation is performed with a single system call.
   *
   * @param vec An array of buffers to write.
   * @param count The number of buffers in the array.
   * @throw IOException If an I/O error occurred.
   */
  virtual size_t write(const MemoryBlock* vec, uint_t count);

  /**
   * Reposition the seek pointer in the stream.
   *
   * @param offset The new offset.
   * @param mode The seek mode.
   * @return The new position of the seek pointer.
   * @throw IOException If the stream does not support seeking, or if some
   * other I/O error occurred.
   */
  virtual int64_t seek(int64_t offset, SeekMode mode = SeekAbsolute);

  /**
   * Get the current (absolute) offset of the seek pointer.
   *
   * @return The current offset, from the beginning of the stream.
   * @throw IOException If the stream does not support seeking, or if some
   * other I/O error occurred.
   */
  virtual int64_t tell();

  /** Test if the stream is open. */
  inline bool isOpen() const
  { return(_canRead || _canWrite); }

  /** Test if the stream supports seeking. */
  inline bool isSeekable() const
  { return(_seekable); }

  /** Test if the stream can be read from. */
  inline bool isReadable() const
  { return(_canRead); }

  /** Test if the stream can be written to. */
  inline bool isWritable() const
  { return(_canWrite); }

  /**
   * Test if the stream is full-duplex (i.e., supports both reading and
   * writing).
   */
  inline bool isFullDuplex() const
  { return(_canRead && _canWrite); }

  /**
   * Test if the stream is half-duplex (i.e., supports either reading or
   * writing, but not both).
   */
  inline bool isHalfDuplex() const
  { return(! _canRead || ! _canWrite); }

  /**
   * Close the stream for reading, writing, or both.
   *
   * @param mode The close mode.
   */
  virtual void close(IOMode mode = IOReadWrite);

  /**
   * Set the stream I/O timeout, in milliseconds. A timeout of 0
   * indicates no timeout, and a timeout of -1 indicates infinite
   * timeout (blocking I/O).
   */
  virtual void setTimeout(timespan_ms_t timeout);

  /** Get the stream I/O timeout, in milliseconds. */
  inline timespan_ms_t getTimeout() const
  { return(_timeout); }

 protected:

  /**
   * Initialize the stream for use with an open file.
   *
   * @param handle The handle to an open file.
   * @param seekable A flag indicating whether the stream is seekable.
   * @param readable A flag indicating whether the stream is readable.
   * @param writable A flag indicating whether the stream is writable.
   */
  void _init(FileHandle handle, bool seekable, bool readable, bool writable);

  /** @cond INTERNAL */
  FileHandle _handle;
  bool _seekable;
  bool _canRead;
  bool _canWrite;
  timespan_ms_t _timeout;
  /** @endcond */

 private:

  size_t _readAsync(byte_t* buffer, size_t buflen, int64_t offset,
                    AsyncIOTask& task);
  size_t _writeAsync(const byte_t* buffer, size_t buflen, int64_t offset,
                     AsyncIOTask& task);

  size_t _readElem(byte_t* buffer, size_t size, size_t nelem, size_t& partial);
  size_t _readElemFully(byte_t* buffer, size_t size, size_t nelem,
                        size_t& partial);

  size_t _writeElem(const byte_t* buffer, size_t size, size_t nelem,
                    size_t& partial);
  size_t _writeElemFully(const byte_t* buffer, size_t size, size_t nelem,
                         size_t& partial);

  CCXX_COPY_DECLS(Stream);
};

} // namespace ccxx

#endif // __ccxx_Stream_hxx
