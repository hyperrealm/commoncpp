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

#include <commonc++/StreamDataWriter.h++>

namespace ccxx {

/*
 */

const size_t StreamDataWriter::BUFFER_SIZE = 4096;

/*
 */

StreamDataWriter::StreamDataWriter(Stream &stream,
                                   size_t bufferSize /* = BUFFER_SIZE */)
  : _stream(stream),
    _buffer(bufferSize)
{
}

/*
 */

StreamDataWriter::~StreamDataWriter()
{
  try
  {
    flush();
  }
  catch(IOException &)
  {
  }

  _stream.close();
}

/*
 */

void StreamDataWriter::skip(size_t count)
{
  skip(count, 0);
}

/*
 */

void StreamDataWriter::skip(size_t count, byte_t fillByte)
{
  Context &ctx = currentContext();

  size_t total = count;

  while(count > 0)
  {
    size_t r = _buffer.getFree();
    size_t n = std::min(r, count);

    _buffer.fill(fillByte, n);
    count -= n;

    if(_buffer.isFull())
      _buffer.read(_stream);
  }

  ctx.bumpOffset(total);
}

/*
 */

void StreamDataWriter::flush()
{
  _buffer.read(_stream);
}

/*
 */

void StreamDataWriter::setOffset(int64_t offset)
{
  Context &ctx = currentContext();

  if((offset >= 0) && (offset <= ctx.maxOffset))
  {
    // flush the buffer

    _buffer.read(_stream);

    // seek to new position

    int64_t diff = offset - ctx.offset;
    _stream.seek(getCumulativeOffset() + diff);
    ctx.offset = offset;
  }
  else
    throw IOException("out of bounds");
}

/*
 */

size_t StreamDataWriter::write(const byte_t* buf, size_t count)
{
  Context &ctx = currentContext();

  const byte_t *pos = buf;
  size_t total = 0;

  while(count > 0)
  {
    size_t f = _buffer.getFree();
    size_t n = std::min(f, count);

    total += _buffer.write(pos, n);
    pos += n;
    count -= n;

    if(_buffer.isFull())
      _buffer.read(_stream);
  }

  ctx.bumpOffset(total);
  ctx.limit = ctx.offset;

  return(total);
}


} // namespace ccxx
