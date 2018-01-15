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

#ifndef __ccxx_CircularBufferImpl_hxx
#define __ccxx_CircularBufferImpl_hxx

#ifndef __ccxx_CircularBuffer_hxx
#error "Do not include this header directly from application code!"
#endif

/*
 */

template <typename T>
  CircularBuffer<T>::CircularBuffer(uint_t size)
    : AbstractBuffer<T>(size)
{
  _end = this->_data + this->_size;

  clear();
}

/*
 */

template <typename T>
  CircularBuffer<T>::~CircularBuffer()
{
}

/*
 */

template <typename T>
  void CircularBuffer<T>::clear()
{
  AbstractBuffer<T>::clear();

  _readPos = _writePos = _peekPos = this->_data;
  _readMark = _writeMark = NULL;
  _readShift = _writeShift = 0;
  _avail = 0;
  _peekAvail = 0;
}

/*
 */

template <typename T>
  void CircularBuffer<T>::setSize(uint_t newSize)
{
  AbstractBuffer<T>::setSize(newSize);
  _end = this->_data + this->_size;
}

/*
 */

template <typename T>
  uint_t CircularBuffer<T>::write(const T* buf, uint_t len)
{
  if(_writeShift)
    return(0);

  uint_t left = len, n, f, w = 0;

  if((_writePos > _readPos) || (_avail == 0))
  {
    f = (_end - _writePos);
    n = std::min(left, f);

    std::memcpy(static_cast<void *>(_writePos), static_cast<const void *>(buf),
                n * sizeof(T));

    w += n;
    left -= n;
    buf += n;

    advanceWritePos(n);
  }

  if(left > 0)
  {
    f = (_readPos - _writePos);

    if(f > 0)
    {
      n = std::min(left, f);

      std::memcpy(static_cast<void *>(_writePos),
                  static_cast<const void *>(buf),
                  n * sizeof(T));

      w += n;

      advanceWritePos(n);
    }
  }

  return(w);
}

/*
 */

template <typename T>
  uint_t CircularBuffer<T>::write(Buffer<T>& buffer, uint_t count /* = 0 */)
{
  if(_writeShift)
    return(0);

  if(count > buffer.getRemaining())
    return(0);
  else if(count == 0)
    count = buffer.getRemaining();

  const T *buf = buffer.getPointer();

  uint_t r = write(buf, count);
  buffer.bump(r);

  return(r);
}

/*
 */

template <typename T>
  uint_t CircularBuffer<T>::write(Stream& stream, uint_t count /* = 0 */)
{
  if((count == 0) || (count > getFree()))
    count = getFree();

  if(count == 0)
    return(0);

  // First segment extends from the write position to either the read
  // position or, if the read position is behind the write position,
  // to the end of the buffer. In the latter case, second segment
  // extends from beginning of buffer to the read position.

  byte_t *bufStart = reinterpret_cast<byte_t *>(this->getBase());
  byte_t *bufEnd = reinterpret_cast<byte_t *>(_end);
  byte_t *dataStart = reinterpret_cast<byte_t *>(_writePos) + _writeShift;
  byte_t *dataEnd = reinterpret_cast<byte_t *>(_readPos);
  size_t countBytes = (count * sizeof(T)) - _writeShift;

  byte_t *buf1 = NULL, *buf2 = NULL;
  size_t len1 = 0, len2 = 0;
  int iol = 1;

  buf1 = dataStart;
  if(dataStart < dataEnd)
  {
    size_t n = static_cast<size_t>(dataEnd - dataStart);
    len1 = std::min(n, countBytes);
  }
  else
  {
    size_t n = static_cast<size_t>(bufEnd - dataStart);
    len1 = std::min(n, countBytes);
    countBytes -= n;
    if(countBytes > 0)
    {
      buf2 = bufStart;
      len2 = std::min(static_cast<size_t>(dataEnd - bufStart),
                      countBytes);
      ++iol;
    }
  }

  MemoryBlock iov[2] = { MemoryBlock(buf1, len1),
                         MemoryBlock(buf2, len2) };

  uint_t cw = 0;
  uint_t w = stream.read(iov, iol);

  if(_writeShift)
  {
    uint_t part = (sizeof(T) - _writeShift);
    if(w >= part)
    {
      ++cw;
      advanceWritePos(1);
      w -= part;
    }
  }

  if(w > 0)
  {
    div_t dv = div(static_cast<int>(w), static_cast<int>(sizeof(T)));

    if(dv.quot > 0)
    {
      cw += dv.quot;
      advanceWritePos(dv.quot);
    }

    _writeShift = dv.rem;
  }

  return(cw);
}

/*
 */

template <typename T>
  uint_t CircularBuffer<T>::read(T* buf, uint_t count)
{
  if(_readShift)
    return(0);

  uint_t left = count, n, f, r = 0;

  if((_readPos > _writePos) || (_avail == this->_size))
  {
    f = (_end - _readPos);
    n = std::min(left, f);

    std::memcpy(static_cast<void *>(buf), static_cast<const void *>(_readPos),
                (n * sizeof(T)));

    r += n;
    left -= n;
    buf += n;

    advanceReadPos(n);
  }

  if(left > 0)
  {
    f = (_writePos - _readPos);

    if(f > 0)
    {
      n = std::min(left, f);

      std::memcpy(static_cast<void *>(buf),
                  static_cast<const void *>(_readPos),
                  n * sizeof(T));

      r += n;

      advanceReadPos(n);
    }
  }

  return(r);
}

/*
 */

template <typename T>
  uint_t CircularBuffer<T>::read(Buffer<T>& buffer, uint_t count /* = 0 */)
{
  if(_readShift)
    return(0);

  if(count >= buffer.getRemaining())
    return(0); // not enough space in buffer
  else if(count == 0)
    count = buffer.getRemaining();

  T *buf = buffer.getPointer();
  uint_t w = read(buf, count);
  buffer.bump(w);

  return(w);
}

/*
 */

template <typename T>
  uint_t CircularBuffer<T>::read(Stream& stream, uint_t count /* = 0 */)
{
  if((count == 0) || (count > getRemaining()))
    count = getRemaining();

  if(count == 0)
    return(0);
  
  byte_t *bufStart = reinterpret_cast<byte_t *>(this->getBase());
  byte_t *bufEnd = reinterpret_cast<byte_t *>(_end);
  byte_t *dataStart = reinterpret_cast<byte_t *>(_readPos) + _readShift;
  byte_t *dataEnd = reinterpret_cast<byte_t *>(_writePos);
  size_t countBytes = (count * sizeof(T)) - _readShift;

  byte_t *buf1 = NULL, *buf2 = NULL;
  size_t len1 = 0, len2 = 0;
  int iol = 1;

  buf1 = dataStart;
  if(dataStart < dataEnd)
  {
    size_t n = static_cast<size_t>(dataEnd - dataStart);
    len1 = std::min(n, countBytes);
  }
  else
  {
    size_t n = static_cast<size_t>(bufEnd - dataStart);
    len1 = std::min(n, countBytes);
    countBytes -= n;
    if(countBytes > 0)
    {
      buf2 = bufStart;
      len2 = std::min(static_cast<size_t>(dataEnd - bufStart), countBytes);
      ++iol;
    }
  }

  MemoryBlock iov[2] = { MemoryBlock(buf1, len1),
                         MemoryBlock(buf2, len2) };

  uint_t cw = 0;
  uint_t r = stream.write(iov, iol);

  if(_readShift)
  {
    uint_t part = (sizeof(T) - _readShift);
    if(r >= part)
    {
      advanceReadPos(1);
      ++cw;
      r -= part;
    }
  }

  div_t dv = div(static_cast<int>(r), static_cast<int>(sizeof(T)));

  if(r > 0)
  {
    advanceReadPos(dv.quot);
    cw += dv.quot;
    _readShift = dv.rem;
  }

  return(cw);
}

/*
 */

template <typename T>
  uint_t CircularBuffer<T>::fill(const T& value, uint_t count)
{
  if(_writeShift)
    return(0);

  if(count > getFree())
    count = getFree();

  if(count == 0)
    return(0);

  T *ptr = _writePos;
  for(uint_t n = 0; n < count; ++n)
  {
    *ptr = value;
    if(++ptr == this->_end)
      ptr = this->_data;
  }

  advanceWritePos(count);

  return(count);
}

/*
 */

template <typename T>
  uint_t CircularBuffer<T>::peek(const T& value, uint_t maxlen, bool& found,
                                 bool resetPeek /* = true */)
{
  if(_avail == 0)
    return(0);

  uint_t count = 0;
  found = false;

  if(resetPeek)
    _peekPos = _readPos;

  uint_t i = maxlen;

  if(_peekPos < _writePos)
  {
    for(; _peekPos < _writePos; ++_peekPos)
    {
      ++count;
      --_peekAvail;

      if(*_peekPos == value)
      {
        found = true;
        break;
      }

      if(i-- == 0)
        return(count); // reached scan limit
    }
  }
  else // wraparound
  {
    for(; _peekPos < _end; ++_peekPos)
    {
      ++count;
      --_peekAvail;

      if(*_peekPos == value)
      {
        found = true;
        break;
      }

      if(i-- == 0)
        return(count);
    }

    if(! found)
    {
      for(_peekPos = this->_data; _peekPos < _writePos; ++_peekPos)
      {
        ++count;
        --_peekAvail;

        if(*_peekPos == value)
        {
          found = true;
          break;
        }

        if(i-- == 0)
          return(count);
      }
    }
  }

  return(count);
}

/*
 */

template <typename T>
  uint_t CircularBuffer<T>::getReadExtent() const
{
  if((_readPos > _writePos) || ((_readPos == _writePos) && _avail))
    return(_end - _readPos);
  else if(_readPos < _writePos)
    return(_writePos - _readPos);
  else
    return(0);
}

/*
 */

template <typename T>
  uint_t CircularBuffer<T>::getWriteExtent() const
{
  if((_writePos > _readPos) || ((_writePos == _readPos) && ! _avail))
    return(_end - _writePos);
  else if(_writePos < _readPos)
    return(_readPos - _writePos);
  else
    return(0);
}

/*
 */

template <typename T>
  T* CircularBuffer<T>::advanceReadPos(uint_t count)
{
  if(count <= _avail)
  {
    _readPos += count;
    if(_readPos >= _end)
      _readPos -= this->_size;

    _avail -= count;
    _readShift = 0;
    _writeMark = NULL;

    if(_peekAvail > _avail)
    {
      // read position overtook peek position

      _peekAvail = _avail;
      _peekPos = _readPos;
    }
  }

  return(_readPos);
}

/*
 */

template <typename T>
  T* CircularBuffer<T>::advanceWritePos(uint_t count)
{
  if(count <= getFree())
  {
    _writePos += count;
    if(_writePos >= _end)
      _writePos -= this->_size;

    _avail += count;
    _peekAvail += count;
    _writeShift = 0;
    _readMark = NULL;
  }

  return(_writePos);
}

/*
 */

template <typename T>
  T* CircularBuffer<T>::advancePeekPos(uint_t count)
{
  // peekPos can't go beyond writePos

  if(count <= _peekAvail)
  {
    _peekPos += count;
    _peekAvail -= count;
    if(_peekPos >= this->_end)
      _peekPos -= this->_size;
  }

  return(_peekPos);
}

/*
 */

template <typename T>
  void CircularBuffer<T>::resetPeekPos()
{
  _peekPos = _readPos;
  _peekAvail = _avail;
}

/*
 */

template <typename T>
  void CircularBuffer<T>::markReadPos()
{
  _readMark = _readPos;
}

/*
 */

template <typename T>
  void CircularBuffer<T>::rewindReadPos()
{
  if(_readMark != NULL)
  {
    _readPos = _readMark;
    _readMark = NULL;
  }
}

/*
 */

template <typename T>
  void CircularBuffer<T>::markWritePos()
{
  _writeMark = _writePos;
}

/*
 */

template <typename T>
  void CircularBuffer<T>::rewindWritePos()
{
  if(_writeMark != NULL)
  {
    _writePos = _writeMark;
    _writeMark = NULL;
  }
}

/*
 */

template <typename T>
  bool CircularBuffer<T>::setReadPosFromMark(uint_t count)
{
  if((_readMark == NULL) || (count > getRemaining()))
    return(false);

  _readPos = _readMark + count;
  if(_readPos >= this->_end)
    _readPos -= this->_size;

  return(true);
}

/*
 */

template <typename T>
  bool CircularBuffer<T>::setWritePosFromMark(uint_t count)
{
  if((_writeMark == NULL) || (count > getFree()))
    return(false);

  _writePos = _writeMark + count;
  if(_writePos >= this->_end)
    _writePos -= this->_size;

  return(true);
}

#endif // __ccxx_CircularBufferImpl_hxx
