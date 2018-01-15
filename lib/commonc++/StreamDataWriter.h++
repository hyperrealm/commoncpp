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

#ifndef __ccxx_StreamDataWriter_hxx
#define __ccxx_StreamDataWriter_hxx

#include <commonc++/DataWriter.h++>
#include <commonc++/CircularBuffer.h++>
#include <commonc++/IOException.h++>
#include <commonc++/Stream.h++>

namespace ccxx {

/**
 * A DataWriter which writes data to a Stream.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API StreamDataWriter : public DataWriter
{
 public:

  /**
   * Construct a new StreamDataWriter for the given Stream.
   *
   * @param stream The Stream to write data to.
   * @param bufferSize The stream buffer size.
   */
  StreamDataWriter(Stream& stream, size_t bufferSize = BUFFER_SIZE);

  /** Destructor. */
  ~StreamDataWriter();

  void skip(size_t count);

  void skip(size_t count, byte_t fillByte);

  void flush();

  void setOffset(int64_t offset);

  static const size_t BUFFER_SIZE;

 protected:

  size_t write(const byte_t* buf, size_t count);

 private:

  Stream& _stream;
  CircularByteBuffer _buffer;

  CCXX_COPY_DECLS(StreamDataWriter);
};

} // namespace ccxx

#endif // __ccxx_StreamDataWriter_hxx
