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

#include "commonc++/DataReader.h++"
#include "commonc++/ByteOrder.h++"
#include "commonc++/UTF8Decoder.h++"

namespace ccxx {

/*
 */

DataReader::DataReader()
{
}

/*
 */

DataReader::~DataReader()
{
}

/*
 */

DataReader& DataReader::operator>>(bool& v)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(byte_t& v)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(byte_t* v)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
    read(v, ctx.length);

  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(char& v)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(char* v)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
    read(reinterpret_cast<byte_t *>(v), ctx.length);

  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(int16_t& v)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);

  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(int16_t* v)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    read(reinterpret_cast<byte_t *>(v), ctx.length * sizeof(int16_t));

    if(! isSameEndianness())
    {
      int16_t *end = v + ctx.length;
      for(int16_t *p = v; p < end; ++p)
        ByteOrder::reverseBytes(*p);
    }
  }

  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(uint16_t& v)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);

  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(uint16_t* v)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    read(reinterpret_cast<byte_t *>(v), ctx.length * sizeof(uint16_t));

    if(! isSameEndianness())
    {
      uint16_t *end = v + ctx.length;
      for(uint16_t *p = v; p < end; ++p)
        ByteOrder::reverseBytes(*p);
    }
  }

  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(int32_t& v)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);

  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(int32_t* v)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    read(reinterpret_cast<byte_t *>(v), ctx.length * sizeof(int32_t));

    if(! isSameEndianness())
    {
      int32_t *end = v + ctx.length;
      for(int32_t *p = v; p < end; ++p)
        ByteOrder::reverseBytes(*p);
    }
  }

  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(uint32_t& v)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);

  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(uint32_t* v)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    read(reinterpret_cast<byte_t *>(v), ctx.length * sizeof(uint32_t));

    if(! isSameEndianness())
    {
      uint32_t *end = v + ctx.length;
      for(uint32_t *p = v; p < end; ++p)
        ByteOrder::reverseBytes(*p);
    }
  }

  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(int64_t& v)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);

  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(int64_t* v)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    read(reinterpret_cast<byte_t *>(v), ctx.length * sizeof(int64_t));

    if(! isSameEndianness())
    {
      int64_t *end = v + ctx.length;
      for(int64_t *p = v; p < end; ++p)
        ByteOrder::reverseBytes(*p);
    }
  }

  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(uint64_t& v)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);

  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(uint64_t* v)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    read(reinterpret_cast<byte_t *>(v), ctx.length * sizeof(uint64_t));

    if(! isSameEndianness())
    {
      uint64_t *end = v + ctx.length;
      for(uint64_t *p = v; p < end; ++p)
        ByteOrder::reverseBytes(*p);
    }
  }

  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(float& v)
{
  // assume IEEE-754 floating point representation
  union
  {
    float f;
    uint32_t i;
  } un;

  operator>>(un.i);
  v = un.f;

  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(float* v)
{
  // assume IEEE-754 floating point representation
  return(operator>>(reinterpret_cast<uint32_t *>(v)));
}

/*
 */

DataReader& DataReader::operator>>(double& v)
{
  // assume IEEE-754 floating point representation
  union
  {
    double d;
    uint64_t i;
  } un;

  operator>>(un.i);
  v = un.d;

  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(double* v)
{
  // assume IEEE-754 floating point representation
  return(operator>>(reinterpret_cast<uint64_t *>(v)));
}

/*
 */

DataReader& DataReader::operator>>(Blob& v)
{
  const Context &ctx = currentContext();

  if(ctx.length == 0)
    v = Blob::null;
  else
  {
    v.setLength(ctx.length);
    read(v.getData(), ctx.length);
  }

  return(*this);
}

/*
 */

DataReader& DataReader::operator>>(String& v)
{
  const Context &ctx = currentContext();

  readString(v, ctx.length);

  return(*this);
}

/*
 */

size_t DataReader::readBytes(byte_t* buf, size_t count)
{
  return(read(buf, count));
}

/*
 */

void DataReader::readFully(byte_t* buf, size_t count)
{
  size_t r = read(buf, count);
  if(r != count)
    throw EOFException();
}

/*
 */

void DataReader::readString(String& v, uint32_t length)
{
  int stringLen = length;

  v = String::null;

  if(stringLen == 0)
  {
    // Read a variable-length string, which is preceded by a 32-bit length.
    uint32_t val;
    operator>>(val);
    if(val > INT32_MAX)
      throw IOException("string decoding error");

    stringLen = val;
  }

  if(stringLen == 0)
    return;

  char inbuf[1024];
  char16_t outbuf[1024];
  const int inputSize = CCXX_LENGTHOF(inbuf);
  const int outputSize = CCXX_LENGTHOF(outbuf);
  int totalLeft = stringLen;

  UTF8Decoder decoder;
  decoder.setStopDecodingAtNulChar(true);

  const char* input;
  int inputLeft = 0;
  char16_t* output = outbuf;
  int outputLeft = outputSize;

  while(totalLeft > 0)
  {
    if(inputLeft == 0)
    {
      // Refill the input buffer.
      input = inbuf;
      inputLeft = std::min(inputSize, totalLeft);
      read(reinterpret_cast<byte_t*>(inbuf), inputLeft);
      totalLeft -= inputLeft;
    }

    int status = decoder.decode(&input, &inputLeft, &output, &outputLeft);
    if((status == UTF8Decoder::STATUS_OK)
       || (status == UTF8Decoder::STATUS_OUTPUT_BUFFER_FULL)
       || ((status == UTF8Decoder::STATUS_NEED_MORE_INPUT) && (totalLeft > 0)))
    {
      // Flush the output buffer.
      int n = (outputSize - outputLeft);
      v.append(outbuf, n);

      output = outbuf;
      outputLeft = outputSize;
    }
    else
      throw IOException("string decoding error");
  }

  if(totalLeft > 0)
    skip(totalLeft);
}

/*
 */

DataReader& DataReader::operator>>(const Manipulator& manip)
{
  manip.apply(*this);

  return(*this);
}


} // namespace ccxx
