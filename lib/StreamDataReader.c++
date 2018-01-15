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

#include <commonc++/StreamDataReader.h++>

namespace ccxx {

/*
 */

const size_t StreamDataReader::BUFFER_SIZE = 4096;

/*
 */

StreamDataReader::StreamDataReader(Stream& stream,
                                   size_t bufferSize /* = BUFFER_SIZE */)
  : _stream(stream),
    _buffer(bufferSize)
{
}

/*
 */

StreamDataReader::~StreamDataReader()
{
  _stream.close();
}

/*
 */

void StreamDataReader::skip(size_t count)
{
  Context &ctx = currentContext();

  checkRemaining(ctx, count);

  size_t total = count;

  size_t r = _buffer.getRemaining();

  if(r > count)
    _buffer.advanceReadPos(count);
  else
  {
    count -= r;
    _buffer.clear();

    _stream.seek(static_cast<int64_t>(count), SeekRelative);
  }

  ctx.offset += total;
}

/*
 */

void StreamDataReader::reset()
{
  _buffer.clear();
  _stream.seek(INT64_CONST(0));

  DataReader::reset();
}

/*
 */

void StreamDataReader::setOffset(int64_t offset)
{
  Context &ctx = currentContext();

  if((offset >= 0) && (offset <= ctx.limit))
  {
    _buffer.clear();

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

size_t StreamDataReader::read(byte_t* buf, size_t count)
{
  if(count == 0)
    return(0);

  Context &ctx = currentContext();

  checkRemaining(ctx, count);

  size_t total = 0;
  byte_t *pos = buf;

  while(count > 0)
  {
    size_t r = _buffer.getRemaining();
    if(r == 0)
    {
      _buffer.write(_stream); // refill buffer
      r = _buffer.getRemaining();
    }

    size_t n = std::min(r, count);
    if(_buffer.read(pos, n) == 0)
      throw EOFException();

    count -= n;
    pos += n;
    total += n;
  }

  ctx.offset += total;

  return(total);
}


} // namespace ccxx
