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

#ifndef __ccxx_DynamicCache_hxx
#define __ccxx_DynamicCache_hxx

#include <commonc++/AbstractCache.h++>

namespace ccxx {

/**
 * An LRU cache implementation in which cache objects are deleted when they
 * are removed from the cache. For an implementation that recycles existing
 * objects, see StaticCache.
 *
 * The cache maintains a key/value map, and takes ownership of its items. If
 * the cache is at full capacity when a new item is inserted, the least
 * recently accessed item is removed to make room.
 *
 * Whenever an item is removed from the cache as a result of a call to
 * <code>clear()</code> <code>remove()</code>, <code>put()</code>, or the
 * destructor, the <code>onRemove()</code> method is called just before the
 * item is deleted.
 *
 * @author Mark Lindner
 */
template<typename K, typename T> class DynamicCache
  : public AbstractCache<K, T>
{
 public:

  /**
   * Construct a new DynamicCache with the given maximum size.
   *
   * @param maxSize The maximum number of items that the cache will hold.
   * If 0, the value 1 will be used instead.
   */
  DynamicCache(uint_t maxSize);

  /**
   * Destructor. Clears the cache.
   */
  virtual ~DynamicCache();

  /**
   * Put an item in the cache. Takes ownership of the item.
   *
   * @param key The key.
   * @param item The item.
   */
  void put(K key, T* item);

  /**
   * Look up an item in the cache. If the cache does not contain an item
   * with the given key, loadItem() is called to load the item from the
   * external data source.
   *
   * @param key The item key.
   * @return The item associated with the key, or <b>NULL</b> if no item
   * with the specified key was found (and could not be loaded).
   */
  virtual T* get(const K& key);

  /**
   * Remove an item from the cache, and return the item. The
   * <code>onRemove()</code> method is <i>not</i> called.
   *
   * @param key The key.
   * @return The item associated with the key, or <b>NULL</b> if no item
   * with the specified key was found.
   */
  T* take(const K& key);

  /**
   * Remove an item from the cache.
   *
   * @param key The item key.
   * @return <b>true</b> if an item was removed, <b>false</b> if no item with
   * the specified key was found.
   */
  bool remove(const K& key);

  protected:

  /**
   * Called by get() when the cache does not contain an item with the requested
   * key. Subclasses may override this method to load the item from an external
   * data source. The default implementation is a no-op which returns
   * <b>NULL</b>.
   *
   * @param key The key for the item to load.
   * @return The loaded item, or <b>NULL</b> if the item could not be loaded.
   * The cache takes ownership of the item.
   */
  virtual T* loadItem(const K& key);

 private:

  typedef typename AbstractCache<K, T>::Link Link;
  typedef typename AbstractCache<K, T>::MapType MapType;
};

#include <commonc++/DynamicCacheImpl.h++>

} // namespace ccxx

#endif // __ccxx_DynamicCache_hxx
