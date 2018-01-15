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

#ifndef __ccxx_DynamicObjectPoolImpl_hxx
#define __ccxx_DynamicObjectPoolImpl_hxx

#ifndef __ccxx_DynamicObjectPool_hxx
#error "Do not include this header directly from application code!"
#endif

/*
 */

template <class T>
  DynamicObjectPool<T>::DynamicObjectPool(uint_t size,
                                          void (*resetFunc)(T*),
                                          bool lazy /* = false */)
    : ObjectPool<T>(size),
      _resetFunc(resetFunc)
{
  _list = new Link[ObjectPool<T>::_size];
  _freeList = _list;
  _usedList = NULL;

  Link *cur = _list, *prev = NULL;

  for(uint_t i = 0; i < size; ++i)
  {
    cur->data = (lazy ? NULL : new T());

    if(prev)
      prev->next = cur;

    prev = cur;
    ++cur;
  }

  cur->next = NULL;
}

/*
 */

template <class T>
  DynamicObjectPool<T>::~DynamicObjectPool()
{
  Link *p = _list;

  for(uint_t i = 0; i < ObjectPool<T>::_size; ++i)
  {
    delete p->data;
    ++p;
  }

  delete [] _list;
}

/*
 */

template <class T>
  T* DynamicObjectPool<T>::reserve()
{
  if(_freeList == NULL)
    throw ObjectPoolException();

  Link *link = _freeList;

  T *elem = link->data;
  if(! elem)
  {
    elem = new T();
    link->data = elem;
  }

  _freeList = _freeList->next;

  link->next = _usedList;
  _usedList = link;

  --ObjectPool<T>::_avail;

  // Log_debug("DynamicObjectPool<%s> reserve", typeid(T).name());

  return(elem);
}

/*
 */

template <class T>
  void DynamicObjectPool<T>::release(T* elem)
{
  if(!elem || !_usedList)
    throw ObjectPoolException();

  _resetFunc(elem); // reset the object

  Link *item = _usedList;
  _usedList = _usedList->next;

  item->next = _freeList;
  _freeList = item;

  ++ObjectPool<T>::_avail;

  // Log_debug("DynamicObjectPool<%s> release", typeid(T).name());
}

#endif // __ccxx_DynamicObjectPoolImpl_hxx
