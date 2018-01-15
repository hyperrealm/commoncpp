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

#ifndef __ccxx_Cache_hxx
#define __ccxx_Cache_hxx

#include <map>
#include <utility>

#include <commonc++/Common.h++>

namespace ccxx {

/**
 * A general-purpose LRU-cache with a maximum capacity. The cache
 * maintains a key/value map, and takes ownership of its items. If the cache
 * is at full capacity when a new item is inserted, the least recently
 * accessed item is removed to make room.
 *
 * Whenever an item is removed from the cache as a result of a call to
 * <code>clear()</code> <code>remove()</code>, <code>put()</code>, or the
 * destructor, the <code>onRemove()</code> method is called just before the
 * item is deleted.
 *
 * @author Mark Lindner
 */
template<typename K, typename T> class Cache
{
 public:

  /**
   * Construct a new cache with the given maximum size.
   *
   * @param maxSize The maximum number of items that the cache will hold.
   * If 0, the value 1 will be used instead.
   */
  Cache(uint_t maxSize);

  /**
   * Destructor. Clears the cache.
   */
  virtual ~Cache();

  /**
   * Put an item in the cache.
   *
   * @param key The key.
   * @param item The item.
   */
  void put(K key, T* item);

  /**
   * Look up an item in the cache.
   *
   * @param key The item key.
   * @return The item associated with the key, or <b>NULL</b> if no item
   * with the specified key was found.
   */
  T* get(K key) const;

  /**
   * Index operator. Equivalent to <code>get()</code>.
   */
  inline T* operator[](K key) const
  { return(this->get(key)); }

  /**
   * Remove an item from the cache, and return the item. The
   * <code>onRemove()</code> method is <i>not</i> called.
   *
   * @param key The key.
   * @return The item associated with the key, or <b>NULL</b> if no item
   * with the specified key was found.
   */
  T* take(K key);

  /**
   * Remove an item from the cache.
   *
   * @param key The item key.
   * @return <b>true</b> if an item was removed, <b>false</b> if no item with
   * the specified key was found.
   */
  bool remove(K key);

  /**
   * Test if the cache contains an item with the specified key.
   *
   * @param key The key.
   * @return <b>true</b> the cache contains an item with the specified key,
   * <b>false</b> otherwise.
   */
  bool contains(K key) const;

  /**
   * Remove all items from the cache.
   */
  void clear();

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
  { return(_size); }

  /**
   * Test if the cache is empty.
   */
  inline bool isEmpty() const
  { return(_size == 0); }

 protected:

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
  virtual void onRemove(K key, T* item);

 private:

  uint_t _maxSize;
  uint_t _size;

  /** @cond INTERNAL */
  struct Link
  {
    Link(K key)
      : key(key),
        prev(NULL),
        next(NULL)
    { }

    K key;
    Link *prev;
    Link *next;
  };

  struct KeyCompare
  {
    bool operator()(const Link* key1, const Link* key2) const
    { return(key1->key < key2->key); }
  };
  /** @endcond */

  void _unlink(Link* link) const;
  void _link(Link* link) const;

  typedef std::map<Link*, T*, KeyCompare> MapType;
  mutable MapType _map;
  mutable Link* _head;
  mutable Link* _tail;
};

#include <commonc++/CacheImpl.h++>

} // namespace ccxx

#endif // __ccxx_Cache_hxx
