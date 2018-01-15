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

#include <commonc++/ByteArrayDataWriter.h++>

#include <cstring>

#include <commonc++/Blob.h++>

namespace ccxx {

/*
 */

ByteArrayDataWriter::ByteArrayDataWriter(byte_t* array, size_t len)
  : _buf(array),
    _ptr(array),
    _len(len)
{
  Context &ctx = currentContext();

  ctx.parentLimit = ctx.limit = _len;
}

/*
 */

ByteArrayDataWriter::~ByteArrayDataWriter()
{
}

/*
 */

void ByteArrayDataWriter::reset()
{
  DataWriter::reset();

  _ptr = _buf;
}

/*
 */

void ByteArrayDataWriter::skip(size_t count)
{
  skip(count, 0);
}

/*
 */

void ByteArrayDataWriter::skip(size_t count, byte_t fillByte)
{
  Context &ctx = currentContext();

  checkRemaining(ctx, count);
  std::memset(_ptr, fillByte, count);
  ctx.bumpOffset(count);
  _ptr += count;
}

/*
 */

void ByteArrayDataWriter::flush()
{
  // no-op
}

/*
 */

void ByteArrayDataWriter::setOffset(int64_t offset)
{
  Context &ctx = currentContext();

  if((offset >= 0) && (offset <= ctx.maxOffset))
  {
    // seek to new position

    int64_t diff = offset - ctx.offset;
    _ptr += static_cast<int32_t>(diff);
    ctx.offset = offset;
  }
  else
    throw IOException("out of bounds");
}

/*
 */

size_t ByteArrayDataWriter::write(const byte_t* buf, size_t count)
{
  Context &ctx = currentContext();

  checkRemaining(ctx, count);

  std::memcpy(_ptr, buf, count);

  ctx.bumpOffset(count);
  _ptr += count;

  return(count);
}


} // namespace ccxx
