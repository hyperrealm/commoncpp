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

#ifndef __ccxx_ByteBufferDataWriter_hxx
#define __ccxx_ByteBufferDataWriter_hxx

#include <commonc++/DataWriter.h++>
#include <commonc++/Buffer.h++>

namespace ccxx {

/**
 * A DataWriter which writes data to a ByteBuffer.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API ByteBufferDataWriter : public DataWriter
{
 public:

  /**
   * Construct a new ByteBufferDataWriter for the given ByteBuffer.
   *
   * @param buffer The buffer.
   */
  ByteBufferDataWriter(ByteBuffer& buffer);

  /** Destructor. */
  ~ByteBufferDataWriter();

  void skip(size_t count);

  void skip(size_t count, byte_t fillByte);

  void flush();

  void reset();

  void setOffset(int64_t offset);

 protected:

  size_t write(const byte_t* buf, size_t count);

 private:

  ByteBuffer& _buffer;
  uint_t _oldPos;

  CCXX_COPY_DECLS(ByteBufferDataWriter);
};

} // namespace ccxx

#endif // __ccxx_ByteBufferDataWriter_hxx
