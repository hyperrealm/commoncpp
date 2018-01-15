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

#include "commonc++/DataEncoder.h++"
#include "commonc++/ByteOrder.h++"

#include <deque>

namespace ccxx {

/*
 */

class DataEncoder::ContextStack : public std::deque<Context> { };

/*
 */

DataEncoder::SetEndianness::SetEndianness(Endianness endianness)
  : _endianness(endianness)
{
}

/*
 */

DataEncoder::SetEndianness::~SetEndianness()
{
}

/*
 */

void DataEncoder::SetEndianness::apply(DataEncoder& encoder) const
{
  encoder.setEndianness(_endianness);
}

/*
 */

DataEncoder::SetLength::SetLength(size_t length)
  : _length(length)
{
}

/*
 */

DataEncoder::SetLength::~SetLength()
{
}

/*
 */

void DataEncoder::SetLength::apply(DataEncoder& encoder) const
{
  encoder.setLength(_length);
}

/*
 */

DataEncoder::Skip::Skip(size_t count)
  : _count(count)
{
}

/*
 */

DataEncoder::Skip::~Skip()
{
}

/*
 */

void DataEncoder::Skip::apply(DataEncoder& encoder) const
{
  if(_count > 0)
    encoder.skip(_count);
}

/*
 */

DataEncoder::Align::Align(size_t size)
  : _size(size)
{
}

/*
 */

DataEncoder::Align::~Align()
{
}

/*
 */

void DataEncoder::Align::apply(DataEncoder& encoder) const
{
  encoder.align(_size);
}

/*
 */

DataEncoder::SetLimit::SetLimit(int64_t limit)
  : _limit(limit)
{
}

/*
 */

DataEncoder::SetLimit::~SetLimit()
{
}

/*
 */

void DataEncoder::SetLimit::apply(DataEncoder& encoder) const
{
  encoder.setLimit(_limit);
}

/*
 */

DataEncoder::DataEncoder()
{
  _contexts = new ContextStack();

  _contexts->push_back(Context());
}

/*
 */

DataEncoder::~DataEncoder()
{
  delete _contexts;
}

/*
 */

void DataEncoder::pushContext()
{
  Context &parent = currentContext();
  Context ctx;
  ctx.endianness = parent.endianness;

  if(parent.limit != 0)
  {
    ctx.parentLimit = parent.limit - parent.offset;
    ctx.limit = ctx.parentLimit;
  }

  _contexts->push_back(ctx);
}

/*
 */

void DataEncoder::popContext()
{
  if(_contexts->size() > 1)
  {
    int64_t offset = currentContext().offset;
    _contexts->pop_back();

    Context &ctx = currentContext();
    ctx.offset += offset;

    ctx.maxOffset = std::max(ctx.maxOffset, ctx.offset);
  }
}

/*
 */

bool DataEncoder::isTopContext() const
{
  return(_contexts->size() == 1);
}

/*
 */

uint_t DataEncoder::getContextDepth() const
{
  return(_contexts->size());
}

/*
 */

void DataEncoder::setEndianness(Endianness endianness)
{
  Context &ctx = currentContext();
  ctx.endianness = endianness;
}

/*
 */

Endianness DataEncoder::getEndianness() const
{
  const Context &ctx = currentContext();
  return(ctx.endianness);
}

/*
 */

void DataEncoder::setLength(size_t length)
{
  Context &ctx = currentContext();
  ctx.length = length;
}

/*
 */

size_t DataEncoder::getLength() const
{
  const Context &ctx = currentContext();
  return(ctx.length);
}

/*
 */

void DataEncoder::setLimit(int64_t limit)
{
  Context &ctx = currentContext();

  if(ctx.parentLimit == 0)
  {
    if((limit != 0) && (limit < ctx.maxOffset))
      throw IOException("out of bounds");
  }
  else
  {
    if((limit == 0) || (limit < ctx.maxOffset) || (limit > ctx.parentLimit))
      throw IOException(String() << "out of bounds: limit " << limit
                        << " is outside valid range");
  }

  ctx.limit = limit;
}

/*
 */

int64_t DataEncoder::getLimit() const
{
  const Context &ctx = currentContext();
  return(ctx.limit);
}

/*
 */

int64_t DataEncoder::getOffset() const
{
  const Context &ctx = currentContext();
  return(ctx.offset);
}

/*
 */

int64_t DataEncoder::getCumulativeOffset() const
{
  int64_t offset = INT64_CONST(0);

  for(ContextStack::const_iterator iter = _contexts->begin();
      iter != _contexts->end();
      ++iter)
  {
    offset += iter->offset;
  }

  return(offset);
}

/*
 */

void DataEncoder::reset()
{
  while(_contexts->size() > 1)
    _contexts->pop_back();

  Context &ctx = currentContext();
  ctx.reset();
}

/*
 */

void DataEncoder::align(size_t size)
{
  size_t pos = getCumulativeOffset() % size;
  if(pos)
    skip(size - pos);
}

/*
 */

int64_t DataEncoder::getRemaining() const
{
  const Context &ctx = currentContext();

  return((ctx.limit == 0) ? INT64_MAX : (ctx.limit - ctx.offset));
}

/*
 */

void DataEncoder::checkRemaining(const Context &ctx, size_t count) const
{
  if((ctx.limit != 0) && ((ctx.limit - ctx.offset)
                          < static_cast<int64_t>(count)))
    throw IOException("overflow/underflow");
}

/*
 */

bool DataEncoder::isSameEndianness() const
{
  const Context &ctx = currentContext();
  return(ctx.endianness == ByteOrder::getEndianness());
}

/*
 */

DataEncoder::Context &DataEncoder::currentContext()
{
  return(_contexts->back());
}

/*
 */

const DataEncoder::Context &DataEncoder::currentContext() const
{
  return(_contexts->back());
}

/*
 */

DataEncoder::Context::Context()
{
  reset();
}

/*
 */

void DataEncoder::Context::reset()
{
  endianness = ByteOrder::getEndianness();
  length = 0;
  limit = INT64_CONST(0);
  parentLimit = INT64_CONST(0);
  offset = INT64_CONST(0);
  maxOffset = INT64_CONST(0);
}

/*
 */

void DataEncoder::Context::bumpOffset(size_t delta)
{
  offset += delta;
  maxOffset = std::max(maxOffset, offset);
}


} // namespace ccxx
