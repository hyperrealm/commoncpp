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

#ifndef __ccxx_BoundedQueueImpl_hxx
#define __ccxx_BoundedQueueImpl_hxx

#ifndef __ccxx_BoundedQueue_hxx
#error "Do not include this header directly from application code!"
#endif

/*
 */

template<typename T> BoundedQueue<T>::BoundedQueue(uint_t capacity)
  : _capacity(capacity),
    _terminated(false)
{
}

/*
 */

template<typename T> BoundedQueue<T>::~BoundedQueue()
{
}

/*
 */

template<typename T> void BoundedQueue<T>::shutdown()
{
  ScopedLock lock(_mutex);

  _terminated = true;

  _condP.notifyAll();
  _condC.notifyAll();
}

/*
 */

template<typename T> void BoundedQueue<T>::reset()
{
  clear();
  _terminated = false;
}

/*
 */

template<typename T> void BoundedQueue<T>::clear()
{
  ScopedLock lock(_mutex);

  _queue.clear();
  _condP.notifyAll(); // notify producers
}

/*
 */

template<typename T> void BoundedQueue<T>::put(T item)
{
  ScopedLock lock(_mutex);

  if(_terminated)
    throw InterruptedException();

  if(_queue.size() == _capacity)
    _condP.wait(_mutex);

  if(_queue.size() == _capacity)
    throw InterruptedException();

  _queue.push_back(item);
  _condC.notify(); // notify a consumer
}

/*
 */

template<typename T> void BoundedQueue<T>::tryPut(
  T item, timespan_ms_t timeout /* = 0 */)
{
  if(timeout < 0)
    timeout = 0;

  time_ms_t expired = System::currentTimeMillis() + timeout;

  ScopedLock lock(_mutex);

  if(_terminated)
    throw InterruptedException();

  if(_queue.size() == _capacity)
  {
    time_ms_t now = System::currentTimeMillis();
    if(now >= expired)
      throw TimeoutException();

    if(! _condP.wait(_mutex, static_cast<uint_t>(expired - now)))
      throw TimeoutException();

    if(_queue.size() == _capacity)
      throw InterruptedException();
  }

  _queue.push_back(item);
  _condC.notify(); // notify a consumer
}

/*
 */

template<typename T> T BoundedQueue<T>::take()
{
  ScopedLock lock(_mutex);

  if(_terminated)
    throw InterruptedException();

  if(_queue.empty())
    _condC.wait(_mutex);

  if(_terminated || _queue.empty())
    throw InterruptedException();

  T item = _queue.front();
  _queue.pop_front();

  _condP.notifyAll(); // notify producers

  return(item);
}

/*
 */

template<typename T> T BoundedQueue<T>::tryTake(
  timespan_ms_t timeout /* = 0 */)
{
  if(timeout < 0)
    timeout = 0;

  time_ms_t expired = System::currentTimeMillis() + timeout;

  ScopedLock lock(_mutex);

  if(_terminated)
    throw InterruptedException();

  if(_queue.empty())
  {
    time_ms_t now = System::currentTimeMillis();
    if(now >= expired)
      throw TimeoutException();

    if(! _condC.wait(_mutex, static_cast<uint_t>(expired - now)))
      throw TimeoutException();

    if(_queue.empty())
      throw TimeoutException();

    if(_terminated)
      throw InterruptedException();
  }

  T item = _queue.front();
  _queue.pop_front();
  _condP.notifyAll(); // notify producers

  return(item);
}

/*
 */

template<typename T> uint_t BoundedQueue<T>::getSize() const
{
  ScopedLock lock(_mutex);

  return(_queue.size());
}

/*
 */

template<typename T> void BoundedQueue<T>::setCapacity(uint_t capacity)
{
  ScopedLock lock(_mutex);

  if(capacity > 0)
    _capacity = capacity;
}

/*
 */

template<typename T> void BoundedQueue<T>::interrupt()
{
  ScopedLock lock(_mutex);

  _condP.notifyAll();
  _condC.notifyAll();
}

#endif // __ccxx_BoundedQueueImpl_hxx
