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

#ifndef __ccxx_BTreeImpl_hxx
#define __ccxx_BTreeImpl_hxx

#ifndef __ccxx_BTree_hxx
#error "Do not include this header directly from application code!"
#endif

/*
 */

template <typename K, typename V, int ORDER>
  uint_t BTree<K, V, ORDER>::bsearch(const K x, _Datum **a,
                                     uint_t len) const
{
  uint_t mid, left = 0, right = len - 1;

  if(x <= a[left]->key)
    return(0);

  if(x > a[right]->key)
    return(len);

  while((right - left) > 1)
  {
    mid = (right + left) / 2;
    if(x <= a[mid]->key)
      right = mid;
    else
      left = mid;
  }

  return(right);
}

/*
 */

template <typename K, typename V, int ORDER>
  typename BTree<K, V, ORDER>::_Status
  BTree<K, V, ORDER>::insertNode(_Datum *key, _Node *node,
                                 _Datum **rkey, _Node **rnode)
  {
  _Node *newnode, *_node;
  _Datum *newkey, *_key;
  int i, j;
  _Status s;

  // We're at a leaf, and can't go any deeper. This node will need to be split.

  if(! node)
  {
    *rkey = key;
    *rnode = NULL;
    return(BTreeOverflow);
  }

  // Figure out which subtree of this node should accept the new key,
  // and try to store it in that subtree

  i = bsearch(key->key, node->keys, node->count);
  if((i < node->count) && (node->keys[i]->key == key->key))
    return(BTreeDuplicate);

  s = insertNode(key, node->children[i], &newkey, &newnode);

  if(s != BTreeOverflow)
    return(s);

  // The subtree could not accept the new key, so let's try to store it in
  // this node, if there is room.

  if(node->count < _nkeys)
  {
    i = bsearch(newkey->key, node->keys, node->count);

    // shift keys & pointers to right for insertion

    for(j = node->count; j > i; j--)
    {
      node->keys[j] = node->keys[j - 1];
      node->children[j + 1] = node->children[j];
    }

    // insert new key

    node->keys[i] = newkey;
    node->children[i + 1] = newnode;
    if(newnode != NULL)
      newnode->parent = node;
    node->count++;

    // all done

    return(BTreeOK);
  }

  // There was no room for the key in this node, so split the node.

  if(i == _nkeys)
  {
    _key = newkey;
    _node = newnode;
  }
  else
  {
    _key = node->keys[_nkeys - 1];
    _node = node->children[_nkeys];

    for(j = (_nkeys - 1); j > i; j--)
    {
      node->keys[j] = node->keys[j - 1];
      node->children[j + 1] = node->children[j];
    }

    node->keys[i] = newkey;
    node->children[i + 1] = newnode;
    if(newnode != NULL)
      newnode->parent = node;
  }

  // move right half of keys & child pointers into new node, and float
  // middle item (excess) up to level above

  *rkey = node->keys[_order];
  node->count = _order;
  *rnode = _nodePool.reserve();
  (*rnode)->count = _order;

  for(j = 0; j < _order - 1; j++)
  {
    (*rnode)->keys[j] = node->keys[j + _order + 1];
    (*rnode)->children[j] = node->children[j + _order + 1];
  }

  (*rnode)->keys[_order - 1] = _key;
  (*rnode)->children[_order - 1] = node->children[_nkeys];
  (*rnode)->children[_order] = _node;

  for(j = 0; j <= _order; j++)
  {
    if((*rnode)->children[j] != NULL)
      (*rnode)->children[j]->parent = *rnode;
  }

  return(BTreeOverflow);
  }

/*
 */

template <typename K, typename V, int ORDER>
  typename BTree<K, V, ORDER>::_Status
  BTree<K, V, ORDER>::deleteNode(const K key, _Node *node)
{
  bool borrowleft;
  int i, j, nq, z;
  _Status s;
  _Node *left, *right, *q, *q1;
  _Datum *tmp, *_key = NULL;

  if(! node)
    return(BTreeNotFound);

  /* Search for the key in this node. If found, and if the number of items
   * remaining is at least M (or for the root, at least 1), then delete it,
   * and we're done. Otherwise, we have underflow.
   */

  i = bsearch(key, node->keys, node->count);
  if(! node->children[0]) // node is a leaf
  {
    /* is it actually in this node? */

    if((i == node->count) || (node->keys[i]->key > key))
      return(BTreeNotFound);

    // remove it from the chain
    unlink(node->keys[i]);
    _datumPool.release(node->keys[i]);

    /* shift remaining elements over */

    for(j = i + 1; j < node->count; j++)
    {
      node->keys[j - 1] = node->keys[j];
      node->children[j] = node->children[j + 1];
    }

    node->count--;

    if(node->count >= ((node == _root) ? 1 : _order))
      return(BTreeOK);
    else
      return(BTreeUnderflow);
  }

  /* Node is not a leaf. If the key is found in this node, then start at
   * left child and follow rightmost branches until we find a leaf.
   */

  /* it is an interior node (not a leaf): */

  _key = node->keys[i];
  left = node->children[i];

  if((i < node->count) && (node->keys[i]->key == key))
  {
    /* key found in interior node. Go to left child
     * p[i], then follow a path all the way to a leaf, using rightmost
     * branches.
     */

    q = left; // node->children[i];
    nq = q->count;

    while((q1 = q->children[nq]) != NULL)
    {
      q = q1;
      nq = q->count;
    }

    /* exchange k[i] with the rightmost item in leaf. This item is the
     * largest key that is less than the key we are deleting. */

    tmp = q->keys[nq - 1];
    q->keys[nq - 1] = node->keys[i];
    node->keys[i] = tmp;
  }

  /* The key now resides in a leaf node, and we can delete it easily.
   */

  s = deleteNode(key, left);
  if(s != BTreeUnderflow)
    return(s);

  /* Deletion resulted in an underflow. Try to borrow a node from the left.
   * and if necessary, merge
   */

  /* If there is no right sibling OR if there is a left sibling and the
   * left sibling has more than M keys and the right sibling has M keys,
   * the we borrow from the left.
   */

  borrowleft = ((i == node->count)
                || ((i > 0) && (node->children[i + 1]->count == _order)
                    && (node->children[i - 1]->count > _order)));

  if(borrowleft) // p[i] is rightmost pointer in *p
  {
    _key = node->keys[i - 1]; // the item we are borrowing

    left = node->children[i - 1]; // left sibling
    right = node->children[i]; // this node
  }
  else
    right = node->children[i + 1]; //right sibling

  if(borrowleft) // borrowing from left sibling
  {
    // shift everything over to the right one position in this node

    right->children[right->count + 1] = right->children[right->count];

    for(j = right->count; j > 0; j--)
    {
      right->keys[j] = right->keys[j - 1];
      right->children[j] = right->children[j - 1];
    }

    right->count++;

    // leftmost item in this node becomes rightmost item from left sibling

    right->keys[0] = _key;
    right->children[0] = left->children[left->count];
    if(right->children[0] != NULL)
      right->children[0]->parent = right;

    _key = left->keys[left->count - 1];

    left->count--;

    if(left->count >= _order)
      return(BTreeOK); // enough children in left sibling; no merge necessary
  }

  else if(right->count > _order) // borrowing from right
  {
    // rightmost item in this node becomes leftmost item from right sibling

    left->keys[_order - 1] = _key;
    left->children[_order] = right->children[0];
    if(left->children[_order] != NULL)
      left->children[_order]->parent = left;
    _key = right->keys[0];

    // shift everything to the left one position in the right sibling

    left->count++;
    right->count--;

    for(j = 0; j < right->count; j++)
    {
      right->keys[j] = right->keys[j + 1];
      right->children[j] = right->children[j + 1];
    }

    right->children[right->count] = right->children[right->count + 1];
    return(BTreeOK);
  }

  // merge

  left->keys[_order - 1] = _key;
  left->children[_order] = right->children[0];
  if(left->children[_order] != NULL)
    left->children[_order]->parent = left;

  for(j = 0; j < _order; j++)
  {
    z = _order + j;

    left->keys[z] = right->keys[j];
    left->children[z + 1] = right->children[j + 1];
    if(left->children[z + 1] != NULL)
      left->children[z + 1]->parent = left;
  }

  left->count = _nkeys;

  _nodePool.release(right);

  // shift the items on the right of the deleted item to the left

  for(j = i + 1; j < node->count; j++)
  {
    node->keys[j - 1] = node->keys[j];
    node->children[j] = node->children[j + 1];
  }

  node->count--;

  // do we have underflow in the parent now?

  if(node->count < ((node == _root) ? 1 : _order))
    return(BTreeUnderflow);

  return(BTreeOK);
}

/*
 */

template <typename K, typename V, int ORDER>
  BTree<K, V, ORDER>::BTree(uint_t capacity)
    : _root(NULL), _nkeys(ORDER * 2), _order(ORDER), _capacity(capacity),
      _numItems(0), _nodePool(capacity), _datumPool(capacity * ORDER * 2),
      _head(NULL), _tail(NULL)
{
}

/*
 */

template <typename K, typename V, int ORDER>
  BTree<K, V, ORDER>::~BTree()
{
  clear();

  if(_root)
    _nodePool.release(_root);
}

/*
 */

template <typename K, typename V, int ORDER>
  void BTree<K, V, ORDER>::clear()
{
  Iterator iter(*this);

  while(iter.hasNext())
  {
    iter.next();
    iter.remove();
  }
}

/*
 */

template <typename K, typename V, int ORDER>
  bool BTree<K, V, ORDER>::put(const K key, V data)
{
  _Node *newnode, *n;
  _Datum *newkey, *ikey;
  _Status s;

  if(_numItems == _capacity)
  {
    // tree is full; remove the oldest item

    V value = _head->value;
    remove(_head->key);
    itemDropped(value);
  }

  ikey = _datumPool.reserve();
  ikey->key = key;
  ikey->value = data;

  s = insertNode(ikey, _root, &newkey, &newnode);
  if(s == BTreeDuplicate)
    return(false);

  if(s == BTreeOverflow)
  {
    n = _nodePool.reserve();
    n->count = 1;
    n->keys[0] = newkey;
    n->children[0] = _root;
    if(_root != NULL)
      _root->parent = n;
    n->children[1] = newnode;
    if(newnode != NULL)
      newnode->parent = n;

    _root = n;
  }

  link(ikey);
  _numItems++;

  return(true);
}

/*
 */

template <typename K, typename V, int ORDER>
  bool BTree<K, V, ORDER>::remove(const K key)
{
  _Status s;
  _Node *newroot;

  s = deleteNode(key, _root);
  if(s == BTreeNotFound)
    return(false);

  if(s == BTreeUnderflow)
  {
    // underflow, so shrink tree

    newroot = _root->children[0];
    _nodePool.release(_root);
    _root = newroot;
  }

  _numItems--;

  return(true);
}

/*
 */

template <typename K, typename V, int ORDER>
  V BTree<K, V, ORDER>::get(const K key)
{
  _Node *node;
  int i;

  node = _root;

  while(node)
  {
    i = bsearch(key, node->keys, node->count);
    if((i < node->count) && (node->keys[i]->key == key))
    {
      unlink(node->keys[i]);
      link(node->keys[i]);

      // found it!
      return(node->keys[i]->value);
    }

    node = node->children[i];
  }

  return(NULL);
}

/*
 */

template <typename K, typename V, int ORDER>
  bool BTree<K, V, ORDER>::contains(const K key) const
{
  _Node *node;
  int i;

  node = _root;

  while(node)
  {
    i = bsearch(key, node->keys, node->count);
    if((i < node->count) && (node->keys[i]->key == key))
    {
      // unlink(node->keys[i]);
      // link(node->keys[i]);

      // found it!
      return(true);
    }

    node = node->children[i];
  }

  return(false);
}

/*
 */

template <typename K, typename V, int ORDER>
  void BTree<K, V, ORDER>::link(_Datum *datum)
{
  // attach to tail (newest)

  datum->next = NULL;
  datum->prev = _tail;

  if(_tail)
    _tail->next = datum;
  else
    _head = datum;

  _tail = datum;
}

/*
 */

template <typename K, typename V, int ORDER>
  void BTree<K, V, ORDER>::unlink(_Datum *datum)
{
  if(datum->prev)
    datum->prev->next = datum->next;
  else
  {
    _head = datum->next;
    if(_head)
      _head->prev = NULL;
  }

  if(datum->next)
    datum->next->prev = datum->prev;
  else
    _tail = datum->prev;
}

/*
 */

template <typename K, typename V, int ORDER>
  void BTree<K, V, ORDER>::_dump(_Node *node, int depth, std::ostream& stream)
  const
{
  int i;

  for(i = 0; i < depth; i++)
    stream << ' ';

  for(i = 0; i < node->count; i++)
    stream << node->keys[i]->key << ' ';
  stream << '\n';

  for(i = 0; i <= node->count; i++)
  {
    if(node->children[i] != NULL)
      _dump(node->children[i], depth + 1, stream);
  }
}

/*
 */

template <typename K, typename V, int ORDER>
  void BTree<K, V, ORDER>::dump(std::ostream& stream) const
{
  stream << "---tree dump---\n";
  if(_root)
    _dump(_root , 0, stream);
  stream << "---         ---" << std::endl;
}

/*
 */

template <typename K, typename V, int ORDER>
  void BTree<K, V, ORDER>::itemDropped(V item) const
{
}

/*
 */

template <typename K, typename V, int ORDER>
  void BTree<K, V, ORDER>::getKeys(std::list<K>& keys) const
{
  keys.clear();

  _Datum *datum;

  for(datum = _head; datum; datum = datum->next)
    keys.push_front(datum->key);
}

/*
 */

template <typename K, typename V, int ORDER>
  void BTree<K, V, ORDER>::getValues(std::list<V>& values) const
{
  values.clear();

  _Datum *datum;

  for(datum = _head; datum; datum = datum->next)
    values.push_front(datum->value);
}

/*
 */

template <typename K, typename V, int ORDER>
  BTree<K, V, ORDER>::Iterator::Iterator(BTree<K, V, ORDER>& tree)
    : _tree(tree)
{
  rewind();
}

/*
 */

template <typename K, typename V, int ORDER>
  void BTree<K, V, ORDER>::Iterator::rewind()
{
  _current = NULL;
  _next = _tree._head;
  _prev = NULL;
}

/*
 */

template <typename K, typename V, int ORDER>
  V BTree<K, V, ORDER>::Iterator::next()
{
  if(_next)
  {
    if(_current)
      _prev = _current;
    _current = _next;
    _next = _current->next;

    return(_current->value);
  }
  else
    throw OutOfBoundsException();
}

/*
 */

template <typename K, typename V, int ORDER>
  bool BTree<K, V, ORDER>::Iterator::hasNext()
{
  return(_next != NULL);
}

/*
 */

template <typename K, typename V, int ORDER>
  void BTree<K, V, ORDER>::Iterator::remove()
{
  if(_current)
  {
    _tree.remove(_current->key);
    _current = NULL;
  }
}

#endif // __ccxx_BTreeImpl_hxx
