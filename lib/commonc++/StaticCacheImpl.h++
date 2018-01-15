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

#ifndef __ccxx_StaticCacheImpl_hxx
#define __ccxx_StaticCacheImpl_hxx

#ifndef __ccxx_StaticCache_hxx
#error "Do not include this header directly from application code!"
#endif

template<typename K, typename T>
  StaticCache<K, T>::StaticCache(uint_t maxSize)
    : AbstractCache<K, T>(maxSize)
    , _free(NULL)
{
}

/*
 */

template<typename K, typename T>
  StaticCache<K, T>::~StaticCache()
{
  while(_free != NULL)
  {
    Link* link = _free;
    _free = _free->next;
    delete link;
  }
}

/*
 */

template<typename K, typename T>
  T* StaticCache<K, T>::get(const K& key)
{
  Link keyLink(key);
  typename MapType::iterator iter = this->_map.find(&keyLink);
  if(iter != this->_map.end())
  {
    // Item was found.
    Link *link = *iter;
    _unlink(link); // unlink
    _link(link); // relink at head
    return(link->data);
  }

  T* item = NULL;
  typename MapType::iterator titer = this->_map.end();
  Link* link = NULL;

  if(this->isFull())
  {
    item = this->_tail->data;
    this->onRemove(this->_tail->key, item);
    this->_map.erase(this->_tail);
    _unlink(this->_tail);
  }
  else
  {
    if(_free == NULL)
    {
      T* newItem = constructItem();
      if(newItem == NULL)
        return(NULL);

      _free = new Link(key, newItem);
    }

    link = _free;
  }

  if(! loadItem(key, link->data))
    return(NULL);

  if(link == _free)
    _free = _free->next;

  _link(link);
  this->_map.insert(link);
  return(this->_head->data);
}

/*
 */

template<typename K, typename T>
  void StaticCache<K, T>::clear()
{
  for(typename MapType::const_iterator iter = this->_map.begin();
      iter != this->_map.end();
      ++iter)
  {
    Link* link = *iter;
    this->onRemove(link->key, link->data);
    link->next = _free;
    _free = link;
  }

  this->_map.clear();
}

#endif // __ccxx_StaticCacheImpl_hxx
