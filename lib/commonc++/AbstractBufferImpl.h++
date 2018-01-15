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

#ifndef __ccxx_AbstractBufferImpl_hxx
#define __ccxx_AbstractBufferImpl_hxx

#ifndef __ccxx_AbstractBuffer_hxx
#error "Do not include this header directly from application code!"
#endif

/*
 */

template <typename T>
  AbstractBuffer<T>::AbstractBuffer(uint_t size)
  : _size(size)
  , _owner(true)
{
  _data = new T[_size];
}

/*
 */

template <typename T>
  AbstractBuffer<T>::AbstractBuffer(T *data, size_t size,
                                    bool takeOwnership /* = true */)
    : _data(data)
    , _size(size)
    , _owner(takeOwnership)
{
}

/*
 */

template <typename T>
  AbstractBuffer<T>::~AbstractBuffer()
{
  if(_owner)
    delete[] _data;
}

/*
 */

template <typename T>
  void AbstractBuffer<T>::setSize(uint_t newSize)
{
  if(!_owner || (newSize < 1))
    throw UnsupportedOperationException();

  delete[] _data;
  _data = new T[newSize];
  _size = newSize;

  clear();
}

/*
 */

template <typename T>
  void AbstractBuffer<T>::clear()
{
  std::memset(static_cast<void *>(_data), 0, _size * sizeof(T));
}

#endif // __ccxx_AbstractBufferImpl_hxx
