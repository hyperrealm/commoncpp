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

#ifndef __ccxx_CacheImpl_hxx
#define __ccxx_CacheImpl_hxx

#ifndef __ccxx_Cache_hxx
#error "Do not include this header directly from application code!"
#endif

/*
 */

template<typename K, typename T>
  Cache<K, T>::Cache(uint_t maxSize)
    : _maxSize(maxSize < 1 ? 1 : maxSize),
      _size(0),
      _head(NULL),
      _tail(NULL)
{
}

/*
 */

template<typename K, typename T>
  Cache<K, T>::~Cache()
{
  clear();
}

/*
 */

template<typename K, typename T>
  void Cache<K, T>::put(K key, T *item)
{
  if(_size == _maxSize)
  {
    K tailKey = _tail->key;
    remove(tailKey);
  }

  _link(new Link(key));
  _map.insert(std::make_pair(_head, item));
  ++_size;
}

/*
 */

template<typename K, typename T>
  T *Cache<K, T>::get(K key) const
{
  Link keyLink(key);
  typename MapType::iterator iter = _map.find(&keyLink);
  if(iter == _map.end())
    return(NULL);

  Link *link = iter->first;
  _unlink(link); // unlink
  _link(link); // relink at head

  return(iter->second);
}

/*
 */

template<typename K, typename T>
  T *Cache<K, T>::take(K key)
{
  Link keyLink(key);
  typename MapType::iterator iter = _map.find(&keyLink);
  if(iter == _map.end())
    return(NULL);

  Link *link = iter->first;
  T *data = iter->second;
  _unlink(link);
  delete link;
  _map.erase(iter);
  --_size;

  return(data);
}

/*
 */

template<typename K, typename T>
  bool Cache<K, T>::remove(K key)
{
  T *data = take(key);
  if(data == NULL)
    return(false);

  this->onRemove(key, data);
  delete data;

  return(true);
}

/*
 */

template<typename K, typename T>
  bool Cache<K, T>::contains(K key) const
{
  Link keyLink(key);
  typename MapType::const_iterator iter = _map.find(&keyLink);
  return(iter != _map.end());
}

/*
 */

template<typename K, typename T>
  void Cache<K, T>::clear()
{
  for(typename MapType::iterator iter = _map.begin();
      iter != _map.end();
      ++iter)
  {
    Link *link = iter->first;
    T *data = iter->second;

    this->onRemove(link->key, data);
    delete link;
    delete data;
  }

  _map.clear();
  _size = 0;
}

/*
 */

template<typename K, typename T>
  void Cache<K, T>::onRemove(K key, T *item)
{
  // no-op by default
}

/*
 */

template<typename K, typename T>
  void Cache<K, T>::_link(Link *link) const
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
  void Cache<K, T>::_unlink(Link *link) const
{
  if(link == _head)
    _head = link->next;
  else if(link == _tail)
    _tail = link->prev;

  if(link->prev != NULL)
    link->prev->next = link->next;
  if(link->next != NULL)
    link->next->prev = link->prev;
}

#endif // __ccxx_CacheImpl_hxx
