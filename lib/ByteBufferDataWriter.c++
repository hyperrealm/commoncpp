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

#include "commonc++/ByteBufferDataWriter.h++"

namespace ccxx {

/*
 */

ByteBufferDataWriter::ByteBufferDataWriter(ByteBuffer& buffer)
  : _buffer(buffer)
{
  Context &ctx = currentContext();

  ctx.parentLimit = ctx.limit = buffer.getRemaining();

  _oldPos = _buffer.getPosition();
}

/*
 */

ByteBufferDataWriter::~ByteBufferDataWriter()
{
}

/*
 */

void ByteBufferDataWriter::reset()
{
  DataWriter::reset();

  // reset buffer to original position
  _buffer.setPosition(_oldPos);
}

/*
 */

void ByteBufferDataWriter::skip(size_t count)
{
  skip(count, 0);
}

/*
 */

void ByteBufferDataWriter::skip(size_t count, byte_t fillByte)
{
  Context &ctx = currentContext();

  checkRemaining(ctx, count);
  _buffer.fill(fillByte, count);
  ctx.bumpOffset(count);
}

/*
 */

void ByteBufferDataWriter::flush()
{
  // no-op
}

/*
 */

void ByteBufferDataWriter::setOffset(int64_t offset)
{
  Context &ctx = currentContext();

  if((offset >= 0) && (offset <= ctx.maxOffset))
  {
    // seek to new position

    int64_t diff = offset - ctx.offset;
    ctx.offset = offset;
    _buffer.setPosition(_buffer.getPosition() + static_cast<uint_t>(diff));
  }
  else
    throw IOException("out of bounds");
}

/*
 */

size_t ByteBufferDataWriter::write(const byte_t* buf, size_t count)
{
  Context &ctx = currentContext();

  checkRemaining(ctx, count);
  _buffer.put(buf, count);
  ctx.bumpOffset(count);

  return(count);
}


} // namespace ccxx
