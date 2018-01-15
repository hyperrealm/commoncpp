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

#ifndef __ccxx_ByteArrayDataWriter_hxx
#define __ccxx_ByteArrayDataWriter_hxx

#include <commonc++/DataWriter.h++>
#include <commonc++/IOException.h++>

namespace ccxx {

/**
 * A DataWriter which writes data to a byte array.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API ByteArrayDataWriter : public DataWriter
{
 public:

  /**
   * Construct a new ByteArrayDataWriter for the given byte buffer.
   *
   * @param array A pointer to the array.
   * @param len The length of the array.
   */
  ByteArrayDataWriter(byte_t* array, size_t len);

  /** Destructor. */
  ~ByteArrayDataWriter();

  void skip(size_t count);

  void skip(size_t count, byte_t fillByte);

  void flush();

  void reset();

  void setOffset(int64_t offset);

 protected:

  size_t write(const byte_t* buf, size_t count);

 private:

  byte_t* _buf;
  byte_t* _end;
  byte_t* _ptr;
  size_t _len;

  CCXX_COPY_DECLS(ByteArrayDataWriter);
};

} // namespace ccxx

#endif // __ccxx_ByteArrayDataWriter_hxx
