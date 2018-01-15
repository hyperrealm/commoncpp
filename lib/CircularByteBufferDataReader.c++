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

#include <commonc++/CircularByteBufferDataReader.h++>
#include <commonc++/UTF8Decoder.h++>

#include <stdio.h>

namespace ccxx {

/*
 */

CircularByteBufferDataReader::CircularByteBufferDataReader(
  CircularByteBuffer& buffer)
  : _buffer(buffer)
{
  Context &ctx = currentContext();
  ctx.parentLimit = ctx.limit = _buffer.getRemaining();

  _buffer.markReadPos();
}

/*
 */

CircularByteBufferDataReader::~CircularByteBufferDataReader()
{
}

/*
 */

void CircularByteBufferDataReader::skip(size_t count)
{
  Context& ctx = currentContext();

  checkRemaining(ctx, count);

  ctx.offset += count;
  _buffer.advanceReadPos(count);
}

/*
 */

void CircularByteBufferDataReader::reset()
{
  DataReader::reset();

  // Reset buffer to original position.
  _buffer.rewindReadPos();
  _buffer.markReadPos();
}

/*
 */

void CircularByteBufferDataReader::setOffset(int64_t offset)
{
  Context& ctx = currentContext();

  if((offset >= 0) && (offset <= ctx.limit))
  {
    // seek to new position

    int64_t diff = offset - ctx.offset;
    _buffer.setReadPosFromMark(static_cast<uint_t>(diff));
    ctx.offset = offset;
  }
  else
    throw IOException("out of bounds");
}

/*
 */

size_t CircularByteBufferDataReader::read(byte_t* buf, size_t count)
{
  Context &ctx = currentContext();

  checkRemaining(ctx, count);
  _buffer.read(buf, count);
  ctx.offset += count;

  return(count);
}

/*
 *  Reimplemented from superclass as an optimization.
 */

void CircularByteBufferDataReader::readString(String& v, uint32_t length)
{
  int stringLen = length;
  v = String::null;
  char16_t outbuf[1024];

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

  uint_t bytesLeft = stringLen;

  if(getRemaining() < bytesLeft)
    throw IOException("string decoding error");

  UTF8Decoder decoder;
  decoder.setStopDecodingAtNulChar(true);
  int r = UTF8Decoder::STATUS_OK;

  while(bytesLeft > 0)
  {
    uint_t n = std::min(bytesLeft, _buffer.getReadExtent());
    const char* input = reinterpret_cast<char*>(_buffer.getReadPos());
    int inputLeft = n;
    char16_t* output = outbuf;
    int outputLeft = sizeof(outbuf);

    int r = decoder.decode(&input, &inputLeft, &output, &outputLeft);
    if((r != UTF8Decoder::STATUS_OK)
       && (r != UTF8Decoder::STATUS_NEED_MORE_INPUT))
      throw IOException("string decoding error");

    v.append(outbuf, sizeof(outbuf) - outputLeft);

    uint_t bytesConsumed = n - inputLeft;

    _buffer.advanceReadPos(bytesConsumed);
    bytesLeft -= bytesConsumed;
  }

  if(r != UTF8Decoder::STATUS_OK)
    throw IOException("string decoding error");
}


} // namespace ccxx
