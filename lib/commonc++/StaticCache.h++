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

#ifndef __ccxx_StaticCache_hxx
#define __ccxx_StaticCache_hxx

#include <commonc++/AbstractCache.h++>

namespace ccxx {

/**
 * An LRU cache implementation in which cache objects are recycled when they
 * are removed from the cache. For an implementation that does not recycle
 * existing objects, see DynamicCache.
 *
 * The cache maintains a key/value map, and takes ownership of its items. If
 * the cache is at full capacity when a new item is inserted, the least
 * recently accessed item is recycled to become the new item.
 *
 * Whenever an item is removed from the cache as a result of a call to
 * <code>clear()</code> or the destructor, or if it is being recycled,
 * the <code>onRemove()</code> method is called just before the item is
 * deleted or recycled.
 *
 * @author Mark Lindner
 */
template<typename K, typename T> class StaticCache : public AbstractCache<K, T>
{
 public:

  /**
   * Construct a new StaticCache with the given maximum size.
   *
   * @param maxSize The maximum number of items that the cache will hold.
   * If 0, the value 1 will be used instead.
   */
  StaticCache(uint_t maxSize);

  /**
   * Destructor. Clears the cache.
   */
  virtual ~StaticCache();

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

  virtual void clear();

 protected:

  /**
   * Construct a new cache item. As long as the cache is not at full
   * capacity, this method is called to construct a new item whenever one is
   * needed.
   *
   * @return The new, uninitialized item.
   */
  virtual T* constructItem() = 0;

  /**
   * Called by get() when the cache does not contain an item with the requested
   * key. Subclasses may override this method to load the item from an external
   * data source. The default implementation is a no-op which returns
   * <b>NULL</b>.
   *
   * @param key The key for the item to load.
   * @param item The item that is being recycled. The item should be
   * re-initialized with the data for the given <code>key</code>.
   * @return <b>true</b> if the item was loaded successfully, <b>false</b>
   * otherwise. The supplied <code>item</code> should <i>not</i> be modified
   * when returning <b>false</b>.
   */
  virtual bool loadItem(const K& key, T* item) = 0;

 private:

  typedef typename AbstractCache<K, T>::Link Link;
  typedef typename AbstractCache<K, T>::MapType MapType;

  mutable Link* _free;
};

#include <ccxx/StaticCacheImpl.h++>

} // namespace ccxx

#endif // __ccxx_StaticCache_hxx
