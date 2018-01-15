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


#ifndef __ccxx_DynamicCacheImpl_hxx
#define __ccxx_DynamicCacheImpl_hxx

#ifndef __ccxx_DynamicCache_hxx
#error "Do not include this header directly from application code!"
#endif

/*
 */

template<typename K, typename T>
  DynamicCache<K, T>::DynamicCache(uint_t maxSize)
    : AbstractCache<K, T>(maxSize)
{
}

/*
 */

template<typename K, typename T>
  void DynamicCache<K, T>::put(K key, T* item)
{
  if(this->isFull())
  {
    K tailKey = this->_tail->key;
    remove(tailKey);
  }

  Link* link = new Link(key, item);
  _link(link);
  this->_map.insert(link);
}

/*
 */

template<typename K, typename T>
  T* DynamicCache<K, T>::get(const K& key)
{
  Link* link = NULL;
  Link keyLink(key);
  typename MapType::iterator iter = this->_map.find(&keyLink);
  if(iter != this->_map.end())
  {
    link = *iter;
    _unlink(link);
    _link(link);
  }
  else
  {
    T* item = loadItem(key);
    if(item == NULL)
      return(NULL);

    link = this->_tail;
    if(link == NULL)
    {
      link = new Link(key, item);
      _link(link);
    }
    else
    {
      this->onRemove(key, link->data);
      delete link->data;
      link->data = item;
      _unlink(link);
      _link(link);
    }
  }

  return(link->data);
}

/*
 */

template<typename K, typename T>
  T* DynamicCache<K, T>::take(const K& key)
{
  Link keyLink(key);
  typename MapType::iterator iter = this->_map.find(&keyLink);
  if(iter == this->_map.end())
    return(NULL);

  Link *link = *iter;
  T *data = link->data;
  _unlink(link);
  delete link;
  this->_map.erase(iter);

  return(data);
}

/*
 */

template<typename K, typename T>
  bool DynamicCache<K, T>::remove(const K& key)
{
  T *data = take(key);
  if(!data)
    return(false);

  this->onRemove(key, data);
  delete data;

  return(true);
}

/*
 */

template<typename K, typename T>
  T* DynamicCache<K, T>::loadItem(const K& key)
{
  return(NULL);
}

#endif // __ccxx_DynamicCacheImpl_hxx
