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

#ifndef __ccxx_BufferImpl_hxx
#define __ccxx_BufferImpl_hxx

#ifndef __ccxx_Buffer_hxx
#error "Do not include this header directly from application code!"
#endif

#ifdef min
#undef min
#endif

/*
 */

template<typename T>
  Buffer<T>::Buffer(uint_t size)
    : AbstractBuffer<T>(size)
{
  clear();
}

/*
 */

template<typename T>
  Buffer<T>::Buffer(T* data, size_t size, bool takeOwnership /* = true */)
    : AbstractBuffer<T>(data, size, takeOwnership)
{
  clear();
}

/*
 */

template<typename T>
  Buffer<T>::~Buffer()
{
}

/*
 */

template<typename T>
  void Buffer<T>::clear()
{
  AbstractBuffer<T>::clear();

  _limit = this->_size;
  _pos = 0;
}

/*
 */

template<typename T>
  void Buffer<T>::flip()
{
  _limit = _pos;
  _pos = 0;
}

/*
 */

template<typename T>
  void Buffer<T>::rewind()
{
  _pos = 0;
}

/*
 */

template<typename T>
  void Buffer<T>::setLimit(uint_t limit)
{
  _limit = std::min(limit, this->_size);
}

/*
 */

template<typename T>
  void Buffer<T>::setPosition(uint_t pos)
{
  _pos = std::min(pos, _limit);
}

/*
 */

template<typename T>
  uint_t Buffer<T>::bump(uint_t delta)
{
  _pos = std::min(_pos + delta, _limit);
  return(_pos);
}

/*
 */

template<typename T>
  uint_t Buffer<T>::skip(uint_t delta)
{
  uint_t newpos = std::min(_pos + delta, _limit);

  if(newpos > _pos)
  {
    std::memset(getPointer(), 0, ((newpos - _pos) * sizeof(T)));
    _pos = newpos;
  }

  return(_pos);
}

/*
 */

template<typename T>
  T& Buffer<T>::operator[](int index)
{
  if((index < 0) || (static_cast<uint_t>(index) >= _limit))
    throw OutOfBoundsException();

  return(*(this->_data + index));
}

/*
 */

template<typename T>
  T Buffer<T>::operator[](int index) const
{
  if((index < 0) || (static_cast<uint_t>(index) >= _limit))
    throw OutOfBoundsException();

  return(*(this->_data + index));
}

/*
 */

template<typename T>
  bool Buffer<T>::put(const T& item)
{
  if(getRemaining() < 1)
    return(false);

  std::memcpy(getPointer(), &item, sizeof(T));
  bump(1);

  return(true);
}

/*
 */

template<typename T>
  void Buffer<T>::fill(const T& item, uint_t count /* = 0 */)
{
  uint_t newpos = (count == 0 ? _limit : std::min(_pos + count, _limit));

  while(_pos < newpos)
    put(item);
}

/*
 */

template<typename T>
  bool Buffer<T>::get(T *item)
{
  if(getRemaining() < 1)
    return(false);

  std::memcpy(item, getPointer(), sizeof(T));
  bump(1);

  return(true);
}

/*
 */

template<typename T>
  bool Buffer<T>::put(const T * items, uint_t count)
{
  if(getRemaining() < count)
    return(false);

  std::memcpy(getPointer(), items, (sizeof(T) * count));
  bump(count);

  return(true);
}

/*
 */

template<typename T>
  bool Buffer<T>::get(T * items, uint_t count)
{
  if(getRemaining() < count)
    return(false);

  std::memcpy(items, getPointer(), (sizeof(T) * count));
  bump(count);

  return(true);
}

/*
 */

template<typename T>
  int Buffer<T>::peek(const T& item) const
{
  const T *t = (this->_data + this->_pos);
  int max = this->_limit - this->_pos;
  for(int i = 0; i < max; ++i, ++t)
  {
    if(*t == item)
      return(i);
  }

  return(-1);
}

#endif // __ccxx_BufferImpl_hxx
