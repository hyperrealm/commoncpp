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

#ifndef __ccxx_FlagsImpl_hxx
#define __ccxx_FlagsImpl_hxx

#ifndef __ccxx_Flags_hxx
#error "Do not include this header directly from application code!"
#endif

/*
 */

template<typename T>
  Flags<T>::Flags(const T& flags /* = 0 */)
    : _flags(flags)
{
}

/*
 */

template<typename T>
  Flags<T>::~Flags()
{
}

/*
 */

template<typename T>
  bool Flags<T>::isSet(int index) const
{
  if((index < 0) || (index >= sizeof(T) * 8))
    return(false);

  return(((_flags >> index) & 1) != 0);
}

/*
 */

template<typename T>
  void Flags<T>::set(int index, bool on /* = true */)
{
  if((index < 0) || (index >= sizeof(T) * 8))
    return;

  if(on)
    _flags |= (1 << index);
  else
    _flags &= ~(1 << index);
}

/*
 */

template<typename T>
  bool Flags<T>::anySet(const T& mask) const
{
  return((_flags & mask) != 0);
}

/*
 */

template<typename T>
  bool Flags<T>::allSet(const T& mask) const
{
  return((_flags & mask) == mask);
}

/*
 */

template<typename T>
  bool Flags<T>::operator[](int index) const
{
  return(isSet(index));
}

/*
 */

template<typename T>
  typename Flags<T>::BitRef Flags<T>::operator[](int index)
{
  if((index < 0) || (index >= sizeof(T) * 8))
    throw OutOfBoundsException();

  return(BitRef(this, index));
}

/*
 */

template<typename T>
  void Flags<T>::clear(int index)
{
  set(index, false);
}

/*
 */

template<typename T>
  void Flags<T>::clear()
{
  _flags = 0;
}

/*
 */

template<typename T>
  Flags<T>& Flags<T>::operator=(const T& flags)
{
  _flags = flags;
  return(this);
}

/*
 */

template<typename T>
  Flags<T>::BitRef::BitRef(Flags& flags, int index)
    : _flags(flags)
    , _index(index)
{
}

/*
 */

template<typename T>
  typename Flags<T>::BitRef& Flags<T>::BitRef::operator=(bool value)
{
  _flags.set(_index, value);
  return(*this);
}

#endif // __ccxx_FlagsImpl_hxx
