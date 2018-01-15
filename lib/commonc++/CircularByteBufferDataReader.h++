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

#ifndef __ccxx_CircularByteBufferDataReader_hxx
#define __ccxx_CircularByteBufferDataReader_hxx

#include <commonc++/DataReader.h++>
#include <commonc++/CircularBuffer.h++>

namespace ccxx {

/**
 * A DataReader which reads data from a CircularByteBuffer.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API CircularByteBufferDataReader : public DataReader
{
 public:

  /**
   * Construct a new CircularByteBufferDataReader for the given
   * CircularByteBuffer.
   *
   * @param buffer The buffer.
   */
  CircularByteBufferDataReader(CircularByteBuffer& buffer);

  /** Destructor. */
  ~CircularByteBufferDataReader();

  void skip(size_t count);

  void reset();

  void setOffset(int64_t offset);

 protected:

  size_t read(byte_t* buf, size_t count);
  void readString(String& v, uint32_t length);

 private:

  CircularByteBuffer& _buffer;

  CCXX_COPY_DECLS(CircularByteBufferDataReader);
};

} // namespace ccxx

#endif // __ccxx_ByteBufferDataReader_hxx
