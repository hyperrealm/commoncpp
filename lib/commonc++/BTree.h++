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

#ifndef __ccxx_BTree_hxx
#define __ccxx_BTree_hxx

#include <commonc++/Common.h++>
#include <commonc++/StaticObjectPool.h++>
#include <commonc++/Iterator.h++>


#ifdef CCXX_OS_WINDOWS
#include <search.h>
#endif

#include <iostream>
#include <list>

namespace ccxx {

/**
 * A balanced, n-ary tree data structure for storing key/value
 * relationships. The BTree has a configurable order and maximum
 * capacity.  The items are ordered internally by last access
 * time. When the maximum capacity has been reached, and a new item
 * is added to the tree, the least-recently accessed item is
 * automatically removed.
 *
 * @author Mark Lindner
 */
template <typename K, typename V, int ORDER = 10> class BTree
{
 private:

  /** @cond INTERNAL */
  enum _Status { BTreeOK, BTreeOverflow, BTreeDuplicate, BTreeUnderflow,
                 BTreeNotFound };

  struct _Datum
  {
    K key;
    V value;
    _Datum* next;
    _Datum* prev;
  };

  struct _Node
  {
    _Datum* keys[ORDER * 2];
    _Node* children[ORDER * 2 + 1];
    _Node* parent;
    short count;

    _Node() : parent(NULL), count(0)
    {
      for(int i = 0; i <= ORDER * 2; i++)
        children[i] = NULL;
    }
  };
  /** @endcond */

  _Node* _root;
  int _nkeys;
  int _order;
  uint_t _capacity;
  uint_t _numItems;

  StaticObjectPool<_Node> _nodePool;
  StaticObjectPool<_Datum> _datumPool;

  _Datum* _head;
  _Datum* _tail;

  uint_t bsearch(const K x, _Datum** a, uint_t len) const;
  _Status insertNode(_Datum* key, _Node* node, _Datum** rkey, _Node** rnode);
  _Status deleteNode(K key, _Node* node);
  void _dump(_Node* node, int depth, std::ostream& stream) const;

  void unlink(_Datum* datum);
  void link(_Datum* datum);

 protected:

  /**
   * Called whenever an item is removed from the BTree to make room for
   * another.
   *
   * @param data The value that was removed.
   */
  virtual void itemDropped(V data) const;

 public:

  /**
   * Construct a new BTree with the given maximum capacity.
   *
   * @param capacity The maximum number of items the tree will hold.
   */
  BTree(uint_t capacity);

  /** Destructor. */
  virtual ~BTree();

  /**
   * Put a new item in the tree.
   *
   * @param key The key.
   * @param data The data to associate with the key.
   */
  bool put(const K key, const V data);

  /**
   * Remove an item from the tree.
   *
   * @param key The key.
   */
  bool remove(const K key);

  /**
   * Get an item from the tree.
   *
   * @param key The key.
   * @return The data associated with the key, otherwise a NULL value.
   */
  V get(const K key);

  /**
   * Determine if an item with the given key exists in the tree.
   *
   * @param key The key.
   * @return <b>true</b> if there is a matching key in the tree,
   * <b>false</b> otherwise.
   */
  bool contains(const K key) const;

  /**
   * Dump a representation of the tree (keys only) to a stream.
   *
   * @param stream The stream to write to.
   */
  void dump(std::ostream& stream) const;

  /**
   * Get a list of all of the keys in the tree, in access-order.
   *
   * @param keys The list in which to store the keys.
   */
  void getKeys(std::list<K>& keys) const;

  /**
   * Get a list of all of the values in the tree, in access-order.
   *
   * @param values The list in which to store the values.
   */
  void getValues(std::list<V>& values) const;

  /** Remove all items from the tree. */
  void clear();

  /** Get the tree capacity. */
  inline uint_t getCapacity() const
  { return(_capacity); }

  /**
   * A BTree iterator.
   *
   * @author Mark Lindner.
   */
  class Iterator : public ccxx::Iterator<V>
  {
   private:

    BTree<K, V, ORDER>& _tree;
    _Datum* _prev;
    _Datum* _current;
    _Datum* _next;

   public:

    /** Construct a new Iterator for the given BTree. */
    Iterator(BTree<K, V, ORDER>& tree);
    virtual ~Iterator() {}

    virtual void rewind();
    virtual V next();
    virtual bool hasNext();
    virtual void remove();
  };
};

#include <commonc++/BTreeImpl.h++>

} // namespace ccxx

#endif // __ccxx_BTree_hxx
