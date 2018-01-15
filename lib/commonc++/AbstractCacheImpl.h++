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

#ifndef __ccxx_AbstractCacheImpl_hxx
#define __ccxx_AbstractCacheImpl_hxx

#ifndef __ccxx_AbstractCache_hxx
#error Do not include this file directly!
#endif

template<typename K, typename T>
  AbstractCache<K, T>::AbstractCache(uint_t maxSize)
    : _maxSize(maxSize < 1 ? 1 : maxSize)
    , _head(NULL)
    , _tail(NULL)
{
}

/*
 */

template<typename K, typename T>
  AbstractCache<K, T>::~AbstractCache()
{
  clear();
}

/*
 */

template<typename K, typename T>
  bool AbstractCache<K, T>::contains(const K& key) const
{
  Link keyLink(key);
  typename MapType::const_iterator iter = _map.find(&keyLink);
  return(iter != _map.end());
}

/*
 */

template<typename K, typename T>
  void AbstractCache<K, T>::clear()
{
  for(typename MapType::iterator iter = _map.begin();
      iter != _map.end();
      ++iter)
  {
    Link *link = *iter;
    this->onRemove(link->key, link->data);
    delete link;
  }

  _map.clear();
}

/*
 */

template<typename K, typename T>
  void AbstractCache<K, T>::onRemove(const K& key, T* item)
{
  // no-op by default
}

/*
 */

template<typename K, typename T>
  void AbstractCache<K, T>::_link(Link* link) const
{
  link->prev = NULL;
  link->next = _head;

  if(_head)
    _head->prev = link;
  _head = link;
  if(! _tail)
    _tail = _head;
}

/*
 */

template<typename K, typename T>
  void AbstractCache<K, T>::_unlink(Link* link) const
{
  if(link == _head)
    _head = link->next;
  else if(link == _tail)
    _tail = link->prev;

  if(link->prev)
    link->prev->next = link->next;
  if(link->next)
    link->next->prev = link->prev;
}

#endif // __ccxx_AbstractCacheImpl_hxx
