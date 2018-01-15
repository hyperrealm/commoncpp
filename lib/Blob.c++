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

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#include "commonc++/Blob.h++"
#include "commonc++/Hash.h++"
#include "commonc++/Private.h++"

#include <cstring>
#include <cstdlib>
#include <algorithm>

#ifdef CCXX_OS_WINDOWS
#include "cbits.h"
#endif

namespace ccxx {

const uint_t Blob::END = 0xFFFFFFFF;

const Blob Blob::null((const byte_t *)NULL, 0);

/*
 */

Blob::Blob()
  : _buf(new BlobBuf())
{
}

/*
 */

Blob::Blob(byte_t *data, uint_t length, bool copy /* = true */)
  : _buf(new BlobBuf(data, 0, length, copy))
{
}

/*
 */

Blob::Blob(const byte_t *other, uint_t length)
  : _buf(new BlobBuf(other, 0, length))
{
}

/*
 */

Blob::Blob(const Blob& other)
  : _buf(NULL)
{
  if(&other == &Blob::null)
    _buf = new BlobBuf();
  else if(other._buf->_refs == -1)
    _buf = new BlobBuf(*other._buf, other._buf->_length);
  else
  {
    ++(other._buf->_refs);
    _buf = other._buf;
  }
}

/*
 */

Blob& Blob::operator=(const Blob& other)
{
  if(&other != this)
  {
    _release();

    if(&other == &Blob::null)
      _buf = new BlobBuf();
    else
    {
      if(other._buf->_refs == -1) // unshareable
        _buf = new BlobBuf(*other._buf, other._buf->_length);
      else
      {
        ++(other._buf->_refs);
        _buf = other._buf;
      }
    }
  }

  return(*this);
}

/*
 */

Blob::~Blob()
{
  _release();
}

/*
 */

void Blob::setLength(uint_t length)
{
  _makeCopy(length);
  _buf->_length = length;
}

/*
 */

int Blob::compareTo(const Blob& other) const
{
  if(_buf == other._buf)
    return(0);

  // null is "less than" non-null

  if(isNull())
    return(other.isNull() ? 0 : -1);

  if(other.isNull())
    return(1);

  const byte_t *p = _buf->_data;
  const byte_t *q = other._buf->_data;
  int pl = _buf->_length;
  int ql = other._buf->_length;

  for(; pl > 0 && ql > 0; --pl, --ql, ++p, ++q)
  {
    if(*p < *q)
      return(-1);
    else if(*p > *q)
      return(1);
  }

  if(pl > ql)
    return(1);
  else if(pl < ql)
    return(-1);
  else
    return(0);
}

/*
 */

byte_t& Blob::operator[](int index)
{
  if(isNull() || (index < 0))
    throw OutOfBoundsException();

  uint_t ind = static_cast<uint_t>(index);

  bool extend = false;
  if(ind >= _buf->_length)
  {
    ind = _buf->_length;
    extend = true;
  }

  _makeCopy(std::max(_buf->_length, static_cast<uint_t>(ind + 1)), true);

  if(extend)
    ++_buf->_length;

  return(*(_buf->_data + ind));
}

/*
 */

byte_t Blob::operator[](int index) const
{
  if(isNull() || (index < 0))
    throw OutOfBoundsException();

  uint_t ind = static_cast<uint_t>(index);

  if(ind >= _buf->_length)
    throw OutOfBoundsException();

  return(*(_buf->_data + ind));
}

/*
 */

uint_t Blob::hash(uint_t modulo /* = 256 */) const
{
  uint32_t hash = 0;

  if(! isNull())
    hash = Hash::SDBMHash(_buf->_data, _buf->_length);

  return(hash % modulo);
}

/*
 */

int Blob::indexOf(byte_t b, uint_t fromIndex /* = 0 */) const
{
  if((isNull()) || (fromIndex >= _buf->_length))
    return(-1);

  int index = fromIndex;
  int len = _buf->_length;

  for(const byte_t *p = _buf->_data + fromIndex; len--; ++p, ++index)
    if(*p == b)
      return(index);

  return(-1);
}

/*
 */

int Blob::indexOf(const byte_t *buf, uint_t length, uint_t fromIndex /* = 0 */)
  const
{
  if((isNull()) || (length > (_buf->_length - fromIndex))
     || (fromIndex >= (_buf->_length - length)))
    return(-1);

  int index = fromIndex;
  int n = _buf->_length - length - fromIndex;

  for(const byte_t *p = _buf->_data + fromIndex; n--; ++p, ++index)
  {
    if(! std::memcmp(p, buf, length))
      return(index);
  }

  return(-1);
}

/*
 */

int Blob::lastIndexOf(byte_t b, uint_t fromIndex /* = END */) const
{
  if(isNull() || (_buf->_length == 0))
    return(-1);

  if((fromIndex >= _buf->_length) || (fromIndex == END))
    fromIndex = _buf->_length - 1;

  int index = fromIndex;

  for(const byte_t *p = _buf->_data + fromIndex; index >= 0; --index, --p)
    if(*p == b)
      return(index);

  return(-1);
}

/*
 */

int Blob::lastIndexOf(const byte_t *buf, uint_t length,
                      uint_t fromIndex /* = END */) const
{
  if((isNull() || (length > _buf->_length) || (fromIndex < length)
      || (fromIndex >= _buf->_length)))
    return(-1);

  for(const byte_t *p = _buf->_data + fromIndex - length;
      p >= buf;
      --p)
  {
    if(! std::memcmp(p, buf, length))
      return(p - _buf->_data);
  }

  return(-1);
}

/*
 */

Blob& Blob::append(const byte_t *data, uint_t length)
{
  _makeCopy(_buf->_length + length);

  std::memcpy(_buf->_data + _buf->_length, data, length * sizeof(byte_t));

  _buf->_length += length;

  return(*this);
}

/*
 */

Blob& Blob::append(const char *str, uint_t length /* = 0 */)
{
  uint_t len = std::strlen(str);
  if((length == 0) || (length > len))
    length = len;

  return(append(reinterpret_cast<const byte_t *>(str), length));
}

/*
 */

void Blob::_release()
{
  if(--(_buf->_refs) <= 0)
    delete _buf;
}

/*
 */

void Blob::_makeCopy(uint_t size, bool unshareable /* = false */)
{
  if(_buf->_refs > 1) // and implicitly != -1 (unshareable)
  {
    BlobBuf *buf = new BlobBuf(*_buf, size);

    if(--(_buf->_refs) < 1)
      delete buf; // in case two threads are doing this at the same time
    else
      _buf = buf;
  }
  else
    _buf->reserve(size);

  _buf->_refs = (unshareable ? -1 : 1);
}

/*
 */

Blob::BlobBuf::BlobBuf(const BlobBuf& other, uint_t length)
  : _data(NULL), _capacity(0), _length(0), _refs(1)
{
  reserve(std::max(other._length, length));

  if(other._data != NULL)
    std::memcpy(_data, other._data, other._length * sizeof(byte_t));

  _length = other._length;
}

/*
 */

Blob::BlobBuf::BlobBuf(const byte_t *data, uint_t offset /* = 0 */,
                       uint_t length /* = 0 */)

  : _data(NULL), _capacity(0), _length(0), _refs(1)
{
  makeCopy(data, offset, length);
}

/*
 */

Blob::BlobBuf::BlobBuf(byte_t *data /* = NULL */, uint_t offset /* = 0 */,
                       uint_t length /* = 0 */, bool copy /* = true */)
  : _data(NULL), _capacity(0), _length(0), _refs(1)
{
  if(! copy || (data == NULL))
  {
    _data = data;
    _length = (data == NULL ? 0 : length);
    _capacity = (data == NULL ? 0 : _length);
  }
  else
    makeCopy(data, offset, length);
}

/*
 */

Blob::BlobBuf::~BlobBuf()
{
  delete[] _data;
}

/*
 */

void Blob::BlobBuf::makeCopy(const byte_t *data, uint_t offset, uint_t length)
{
  if(data != NULL)
  {
    reserve(length);
    std::memcpy(_data, data + offset, length * sizeof(byte_t));
    _length = length;
  }
}

/*
 */

void Blob::BlobBuf::reserve(uint_t size)
{
  if(_capacity < size)
  {
    uint_t needed = std::max<uint_t>(static_cast<uint_t>(_capacity * 1.5),
                                     size);
    uint_t newcap = (needed > 0 ? 4 * ((needed - 1) / 4 + 1) : 0);
    byte_t *newdata = (newcap > 0 ? new byte_t[newcap] : NULL);

    if(newdata != NULL)
    {
      if(_data != NULL)
        std::memcpy(newdata, _data, _length * sizeof(byte_t));

      std::memset(newdata + _length, 0, (newcap - _length));
    }

    if(_data != NULL)
      delete[] _data;

    _data = newdata;
    _capacity = newcap;
  }
}

/*
 */

void Blob::BlobBuf::makeNull()
{
  if(_data)
  {
    delete[] _data;
    _data = NULL;
  }

  _length = 0;
  _capacity = 0;
}


} // namespace ccxx
