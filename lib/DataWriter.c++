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

#include "commonc++/DataWriter.h++"
#include "commonc++/Blob.h++"
#include "commonc++/ByteOrder.h++"
#include "commonc++/UTF8Encoder.h++"

#include <cstring>

namespace ccxx {

/*
 */

DataWriter::DataWriter()
{
}

/*
 */

DataWriter::~DataWriter()
{
}

/*
 */

DataWriter& DataWriter::operator<<(bool v)
{
  write(reinterpret_cast<const byte_t *>(&v), sizeof(v));

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(byte_t v)
{
  write(&v, sizeof(v));

  return(*this);
}


/*
 */

DataWriter& DataWriter::operator<<(const byte_t* v)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
    write(v, ctx.length);

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(char v)
{
  write(reinterpret_cast<const byte_t *>(&v), sizeof(v));

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(const char* v)
{
  const Context &ctx = currentContext();

  size_t vlen = std::strlen(v);
  size_t wlen = (ctx.length > 0 ? ctx.length : vlen);

  write(reinterpret_cast<const byte_t *>(v), vlen);

  if((ctx.length > 0) && (vlen < wlen))
    skip(wlen - vlen, 0);

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(int16_t v)
{
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);
  write(reinterpret_cast<const byte_t *>(&v), sizeof(v));

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(const int16_t *v)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    if( isSameEndianness())
      write(reinterpret_cast<const byte_t *>(v), ctx.length * sizeof(int16_t));
    else
    {
      const int16_t *end = v + (ctx.length * sizeof(int16_t));
      for(const int16_t *p = v; v < end; ++v)
      {
        int16_t t = *p;
        ByteOrder::reverseBytes(t);
        write(reinterpret_cast<const byte_t *>(t), sizeof(int16_t));
      }
    }
  }

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(uint16_t v)
{
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);
  write(reinterpret_cast<const byte_t *>(&v), sizeof(v));

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(const uint16_t* v)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    if( isSameEndianness())
      write(reinterpret_cast<const byte_t *>(v),
            ctx.length * sizeof(uint16_t));
    else
    {
      const uint16_t *end = v + (ctx.length * sizeof(uint16_t));
      for(const uint16_t *p = v; v < end; ++v)
      {
        uint16_t t = *p;
        ByteOrder::reverseBytes(t);
        write(reinterpret_cast<const byte_t *>(t), sizeof(uint16_t));
      }
    }
  }

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(int32_t v)
{
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);
  write(reinterpret_cast<const byte_t *>(&v), sizeof(v));

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(const int32_t* v)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    if( isSameEndianness())
      write(reinterpret_cast<const byte_t *>(v), ctx.length * sizeof(int32_t));
    else
    {
      const int32_t *end = v + (ctx.length * sizeof(int32_t));
      for(const int32_t *p = v; v < end; ++v)
      {
        int32_t t = *p;
        ByteOrder::reverseBytes(t);
        write(reinterpret_cast<const byte_t *>(&t), sizeof(int32_t));
      }
    }
  }

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(uint32_t v)
{
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);
  write(reinterpret_cast<const byte_t *>(&v), sizeof(v));

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(const uint32_t* v)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    if( isSameEndianness())
      write(reinterpret_cast<const byte_t *>(v),
            ctx.length * sizeof(uint32_t));
    else
    {
      const uint32_t *end = v + (ctx.length * sizeof(uint32_t));
      for(const uint32_t *p = v; v < end; ++v)
      {
        uint32_t t = *p;
        ByteOrder::reverseBytes(t);
        write(reinterpret_cast<const byte_t *>(&t), sizeof(uint32_t));
      }
    }
  }

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(const int64_t& v)
{
  int64_t tmp = v;

  if(! isSameEndianness())
    ByteOrder::reverseBytes(tmp);
  write(reinterpret_cast<const byte_t *>(&tmp), sizeof(tmp));

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(const int64_t* v)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    if( isSameEndianness())
      write(reinterpret_cast<const byte_t *>(v),
            ctx.length * sizeof(int64_t));
    else
    {
      const int64_t *end = v + (ctx.length * sizeof(int64_t));
      for(const int64_t *p = v; v < end; ++v)
      {
        int64_t t = *p;
        ByteOrder::reverseBytes(t);
        write(reinterpret_cast<const byte_t *>(t), sizeof(int64_t));
      }
    }
  }

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(const uint64_t& v)
{
  uint64_t tmp = v;

  if(! isSameEndianness())
    ByteOrder::reverseBytes(tmp);
  write(reinterpret_cast<const byte_t *>(&tmp), sizeof(tmp));

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(const uint64_t* v)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    if(isSameEndianness())
      write(reinterpret_cast<const byte_t *>(v),
            ctx.length * sizeof(uint64_t));
    else
    {
      const uint64_t *end = v + (ctx.length * sizeof(uint64_t));
      for(const uint64_t *p = v; v < end; ++v)
      {
        uint64_t t = *p;
        ByteOrder::reverseBytes(t);
        write(reinterpret_cast<const byte_t *>(t), sizeof(uint64_t));
      }
    }
  }

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(float v)
{
  // assume IEEE-754 floating point representation
  union
  {
    float f;
    uint32_t i;
  } un;

  un.f = v;

  return(operator<<(un.i));
}

/*
 */

DataWriter& DataWriter::operator<<(const float* v)
{
  // assume IEEE-754 floating point representation

  return(operator<<(reinterpret_cast<const uint32_t *>(v)));
}

/*
 */

DataWriter& DataWriter::operator<<(const double& v)
{
  // assume IEEE-754 floating point representation
  union
  {
    double d;
    uint64_t i;
  } un;

  un.d = v;

  return(operator<<(un.i));
}

/*
 */

DataWriter& DataWriter::operator<<(const double* v)
{
  // assume IEEE-754 floating point representation

  return(operator<<(reinterpret_cast<const uint64_t *>(v)));
}

/*
 */

DataWriter& DataWriter::operator<<(const Blob& v)
{
  const Context &ctx = currentContext();

  size_t vlen = v.getLength();
  size_t wlen = (ctx.length > 0 ? ctx.length : vlen);

  write(v.getData(), std::min(vlen, wlen));

  if((ctx.length > 0) && (vlen < wlen))
    skip(wlen - vlen, 0);

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(const String& v)
{
  const Context &ctx = currentContext();

  const int maxLen = (ctx.length < 0 ? 0 : ctx.length);

  // Handle null and empty strings.
  if(v.isEmpty())
  {
    if(maxLen != 0)
      skip(maxLen);
    else
      operator<<(uint32_t(0));

    return(*this);
  }

  char buf[1024];
  const int outputSize = CCXX_LENGTHOF(buf);
  const int encodedLen = UTF8Encoder::encodedLength(v.data(), v.length(),
                                                    maxLen);
  if(encodedLen < 0)
    throw IOException("string encoding error");

  UTF8Encoder encoder;
  const char16_t* input = v.data();

  if(maxLen != 0)
  {
    // Write a fixed-length string, truncating or padding with NULs if
    // necessary.

    int totalLeft = encodedLen;

    int inputLeft = maxLen;
    char* output = buf;
    int outputLeft = std::min(outputSize, totalLeft);

    while(totalLeft > 0)
    {
      int status = encoder.encode(&input, &inputLeft, &output, &outputLeft);

      if((status == UTF8Encoder::STATUS_OK)
         || (status == UTF8Encoder::STATUS_OUTPUT_BUFFER_FULL))
      {
        // Flush the output buffer.
        int n = (outputSize - outputLeft);
        write(reinterpret_cast<byte_t *>(output), n);
        totalLeft -= n;
        output = buf;
        outputLeft = std::min(outputSize, totalLeft);
      }
      else
        throw IOException("string encoding error");
    }

    if(encodedLen < maxLen)
      skip(maxLen - encodedLen);
  }
  else
  {
    // Write a variable-length string, preceding it with a 32-bit length.

    operator<<(static_cast<uint32_t>(encodedLen));

    int inputLeft = v.length();
    char* output = buf;
    int outputLeft = outputSize;

    while(inputLeft > 0)
    {
      int status = encoder.encode(&input, &inputLeft, &output, &outputLeft);
      if((status == UTF8Encoder::STATUS_OK)
         || (status == UTF8Encoder::STATUS_OUTPUT_BUFFER_FULL))
      {
        // Flush the output buffer.
        write(reinterpret_cast<byte_t *>(output), (outputSize - outputLeft));
        output = buf;
        outputLeft = outputSize;
      }
      else
        throw IOException("string encoding error");
    }
  }

  return(*this);
}

/*
 */

DataWriter& DataWriter::operator<<(const Manipulator& manip)
{
  manip.apply(*this);

  return(*this);
}


} // namespace ccxx
