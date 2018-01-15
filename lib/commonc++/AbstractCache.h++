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

#ifndef __ccxx_AbstractCache_hxx
#define __ccxx_AbstractCache_hxx

#include <set>
#include <utility>

#include <commonc++/Common.h++>

namespace ccxx {

/**
 * An abstract base class for a general-purpose LRU-cache with a maximum
 * capacity. The cache maintains a key/value map, and takes ownership of its
 * items. If the cache is at full capacity when a new item is inserted, the
 * least recently accessed item is removed to make room.
 *
 * Whenever an item is being permanently removed from the cache (e.g., as a
 * result of a call to <code>clear()</code> or the destructor) the
 * <code>onRemove()</code> method is called just before the
 * item is deleted.
 *
 * @author Mark Lindner
 */
template<typename K, typename T> class AbstractCache
{
 public:

  /** Destructor. Clears the cache. */
  virtual ~AbstractCache();

  /**
   * Look up an item in the cache.
   *
   * @param key The item key.
   * @return The item associated with the key, or <b>NULL</b> if no item
   * with the specified key was found.
   */
  virtual T* get(const K& key) = 0;

  /**
   * Index operator. Equivalent to <code>get()</code>.
   */
  inline T *operator[](const K& key)
  { return(this->get(key)); }

  /**
   * Test if the cache contains an item with the specified key.
   *
   * @param key The key.
   * @return <b>true</b> the cache contains an item with the specified key,
   * <b>false</b> otherwise.
   */
  bool contains(const K& key) const;

  /**
   * Remove all items from the cache.
   */
  virtual void clear();

  /**
   * Return the maximum size of the cache, i.e., the maximum number of
   * items that the cache can hold.
   */
  inline uint_t getMaxSize() const
  { return(_maxSize); }

  /**
   * Return the current size of the cache, i.e., the number of items in the
   * cache.
   */
  inline uint_t getSize() const
  { return(_map.size()); }

  /**
   * Test if the cache is empty.
   */
  inline bool isEmpty() const
  { return(_map.empty()); }

  /**
   * Test if the cache is full.
   */
  inline bool isFull() const
  { return(_map.size() == _maxSize); }

 protected:

  /**
   * Construct a new cache with the given maximum size.
   *
   * @param maxSize The maximum number of items that the cache will hold.
   * If 0, the value 1 will be used instead.
   */
  AbstractCache(uint_t maxSize);

  /**
   * A method that is called whenever an item is being removed from
   * the cache, but just before it has been deleted. The default
   * implementation is a no-op. Subclasses may override this method to
   * do any necessary cleanup of the item before it is deleted, but
   * should <i>not</i> delete the item itself.
   *
   * @param key The key for the item being removed.
   * @param item The item being removed.
   */
  virtual void onRemove(const K& key, T* item);

  /** @cond INTERNAL */
  uint_t _maxSize;

  struct Link
  {
    Link(K key, T* data = NULL)
      : key(key)
      , data(data)
      , prev(NULL)
      , next(NULL)
    { }

    ~Link()
    {
      delete data;
    }

    K key;
    T* data;
    Link* prev;
    Link* next;
  };

  typedef Link LinkType;

  struct KeyCompare
  {
    bool operator()(const Link* key1, const Link* key2) const
    { return(key1->key < key2->key); }
  };

  void _link(Link* link) const;
  void _unlink(Link* link) const;

  typedef std::set<Link*, KeyCompare> MapType;
  mutable MapType _map;
  mutable Link* _head;
  mutable Link* _tail;
  /** @endcond */
};

#include <commonc++/AbstractCacheImpl.h++>

} // namespace ccxx

#endif // __ccxx_AbstractCache_hxx
