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

#ifndef __ccxx_StreamSocket_hxx
#define __ccxx_StreamSocket_hxx

#include <commonc++/Common.h++>
#include <commonc++/IOException.h++>
#include <commonc++/Socket.h++>
#include <commonc++/Stream.h++>

namespace ccxx {

/**
 * A reliable, connection-oriented stream (TCP) socket.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API StreamSocket : public Stream, public Socket
{
  friend class ServerSocket;

 public:

  /**
   * Construct a new StreamSocket.
   *
   * @param port The local port to bind to, or 0 to bind to an arbitrary
   * port.
   */
  StreamSocket(uint16_t port = 0);

  /** Destructor. */
  ~StreamSocket();

  void connect(const String& addr, uint16_t port);
  void connect(const SocketAddress& addr);

  size_t read(byte_t* buffer, size_t buflen);
  size_t write(const byte_t* buffer, size_t buflen);

  inline size_t read(ByteBuffer& buffer)
  { return(Stream::read(buffer)); }

  inline size_t write(ByteBuffer& buffer)
  { return(Stream::write(buffer)); }

  inline size_t read(CharBuffer& buffer)
  { return(Stream::read(buffer)); }

  inline size_t write(CharBuffer& buffer)
  { return(Stream::write(buffer)); }

  inline size_t read(byte_t* buffer, size_t buflen, int64_t offset,
                     AsyncIOTask& task)
  { return(Stream::read(buffer, buflen, offset, task)); }

  inline size_t write(const byte_t* buffer, size_t buflen, int64_t offset,
                      AsyncIOTask& task)
  { return(Stream::write(buffer, buflen, offset, task)); }

  inline size_t read(ByteBuffer& buffer, int64_t offset, AsyncIOTask& task)
  { return(Stream::read(buffer, offset, task)); }

  inline size_t write(ByteBuffer& buffer, int64_t offset, AsyncIOTask& task)
  { return(Stream::write(buffer, offset, task)); }

  size_t read(MemoryBlock* vec, uint_t count);
  size_t write(const MemoryBlock* vec, uint_t count);

  /**
   * Read a byte of out-of-band data from the socket.
   *
   * @return The byte of OOB data that was read.
   * @throw IOException If an I/O error occurs.
   */
  byte_t readOOB();

  /**
   * Write a byte of out-of-band data to the socket.
   *
   * @param data The byte of OOB data to write.
   * @throw IOException If an I/O error occurs.
   */
  void writeOOB(byte_t data);

  void close(IOMode mode = IOReadWrite);
  virtual void shutdown();

  void setTimeout(timespan_ms_t timeout);

 private:

  void setSocketHandle(SocketHandle handle);

  CCXX_COPY_DECLS(StreamSocket);
};

} // namespace ccxx

#endif // __ccxx_StreamSocket_hxx
