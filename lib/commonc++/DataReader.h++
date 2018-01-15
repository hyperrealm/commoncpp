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

#ifndef __ccxx_DataReader_hxx
#define __ccxx_DataReader_hxx

#include <commonc++/Blob.h++>
#include <commonc++/DataEncoder.h++>
#include <commonc++/IOException.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * An abstract base class for data readers. A data reader reads raw
 * bytes from an input source and converts them to primitive values
 * such as integers and strings.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API DataReader : public DataEncoder
{
 public:

  /** Destructor */
  virtual ~DataReader();

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(bool& v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(byte_t& v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(byte_t* v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(char& v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(char* v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(int16_t& v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(int16_t* v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(uint16_t& v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(uint16_t* v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(int32_t& v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(int32_t* v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(uint32_t& v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(uint32_t* v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(int64_t& v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(int64_t* v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(uint64_t& v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(uint64_t* v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(float& v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(float* v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(double& v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(double* v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(Blob& v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(String& v);

  /**
   * Read operator.
   *
   * @throw IOException If an I/O error occurs.
   */
  DataReader& operator>>(const Manipulator& manip);

  /** Read raw bytes. The number of bytes read may be less than the number
   * requested, if an end-of-file condition occurs.
   *
   * @param buf The buffer in which to place the bytes.
   * @param len The maximum number of bytes to read.
   * @return The number of bytes actually read.
   * @throw IOException If an I/O error occurs.
   */
  size_t readBytes(byte_t* buf, size_t len);

  /** Read raw bytes, throwing an EOFException if the exact number of bytes
   * requested could not be read.
   *
   * @param buf The buffer in which to place the bytes.
   * @param len The exact number of bytes to read.
   * @throw IOException If an I/O error occurs.
   */
  void readFully(byte_t* buf, size_t len);

 protected:

  /** Constructor. */
  DataReader();

  /**
   * Read raw data from the input source.
   *
   * @param buf The buffer to read into.
   * @param count The number of bytes to read.
   * @return The number of bytes actually read.
   * @throw IOException If an I/O error occurs.
   */
  virtual size_t read(byte_t* buf, size_t count) = 0;

  /**
   * Read a string from the input source.
   *
   * @param v The String to read into.
   * @param length The length of the string to read, or 0 to read a
   * String that is preceded by a 32-bit length.
   * @throw IOException If an I/O error occurs.
   */
  virtual void readString(String& v, uint32_t length);

 private:

  CCXX_COPY_DECLS(DataReader);
};

} // namespace ccxx

#endif // __ccxx_DataReader_hxx
