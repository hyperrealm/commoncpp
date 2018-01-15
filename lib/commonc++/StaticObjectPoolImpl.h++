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

#ifndef __ccxx_StaticObjectPoolImpl_hxx
#define __ccxx_StaticObjectPoolImpl_hxx

#ifndef __ccxx_StaticObjectPool_hxx
#error "Do not include this header directly from application code!"
#endif

/*
 */

template <typename T> StaticObjectPool<T>::StaticObjectPool(uint_t size)
  : ObjectPool<T>(size)
{
  size_t sz = sizeof(T);
  size_t lsz = sizeof(Link);

  _elemsz = (sz < lsz ? lsz : sz);
  _heap = new byte_t[_elemsz * ObjectPool<T>::_size];

  byte_t* q = (byte_t*)_heap;
  byte_t* p = (byte_t*)q + _elemsz;

  for(int i = ObjectPool<T>::_size - 1; i--; q = p, p += _elemsz)
    reinterpret_cast<Link*>(q)->next = reinterpret_cast<Link*>(p);

  reinterpret_cast<Link*>(q)->next = NULL;

  _freeList = reinterpret_cast<Link*>(_heap);
}

/*
 */

template <typename T> StaticObjectPool<T>::~StaticObjectPool()
{
  delete[] _heap;
}

/*
 */

template <typename T> T* StaticObjectPool<T>::reserve()
{
  if(_freeList == NULL)
    throw ObjectPoolException();

  Link* p = _freeList;
  _freeList = _freeList->next;

  // zero out the memory (helps prevent weird memory bugs)
  ::memset((void*)p, 0, _elemsz);

  // construct a new object in the slot
  T* elem = new((void*)p)T();

  --ObjectPool<T>::_avail;

  // Log_debug("StaticObjectPool<%s> reserve @ %p", typeid(T).name(), p);

  return(elem);
}

/*
 */

template <typename T> void StaticObjectPool<T>::release(T* elem)
{
  if(! elem)
    throw ObjectPoolException();

  // explicitly call object's destructor
  elem->~T();

  // Log_debug("StaticObjectPool<%s> release @ %p", typeid(T).name(), elem);

  Link* p = reinterpret_cast<Link*>(elem);
  p->next = _freeList;
  _freeList = p;

  ++ObjectPool<T>::_avail;
}

#endif // __ccxx_StaticObjectPoolImpl_hxx
