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

#ifndef __ccxx_BoundedQueue_hxx
#define __ccxx_BoundedQueue_hxx

#include <commonc++/Common.h++>
#include <commonc++/ConditionVar.h++>
#include <commonc++/InterruptedException.h++>
#include <commonc++/Mutex.h++>
#include <commonc++/ScopedLock.h++>
#include <commonc++/System.h++>
#include <commonc++/IOException.h++>

#include <deque>

namespace ccxx {

/**
 * A bounded, threadsafe FIFO processing queue. Items are enqueued
 * by one or more producers and consumed by one or more consumers.
 *
 * @author Mark Lindner
 */
template <typename T> class BoundedQueue
{
 public:

  /**
   * Construct a new BoundedQueue.
   *
   * @param capacity The queue capacity: the maximum number of items
   * that it can hold at any given time.
   */
  BoundedQueue(uint_t capacity);

  /** Destructor. */
  virtual ~BoundedQueue();

  /** Clear the queue. All items are removed from the queue. */
  void clear();

  /**
   * Reset the queue. Clears the queue, and clears the shutdown
   * flag, if it was previously set via a call to shutdown(). This method
   * must be called before a shut down queue can be reused.
   */
  void reset();

  /**
   * Put an item in the queue. If the queue is full, the method blocks
   * until space becomes available.
   *
   * @param item The item to enqueue.
   * @throw InterruptedException If the queue was interrupted via a call
   * to interrupt().
   */
  void put(T item);

  /**
   * Put an item in the queue. If the queue is full, the method blocks
   * until space becomes available or the timeout expires, whichever
   * occurs first.
   *
   * @param item The item to enqueue.
   * @param timeout The timeout, in milliseconds.
   * @throw InterruptedException If the queue was interrupted via a call
   * to interrupt().
   * @throw TimeoutException If the operation timed out.
   */
  void tryPut(T item, timespan_ms_t timeout = 0);

  /**
   * Take an item from the queue. If the queue is empty, the method
   * blocks until an item becomes available.
   *
   * @return The dequeued item.
   * @throw InterruptedException If the queue was interrupted via a call
   * to interrupt().
   */
  T take();

  /**
   * Take an item from the queue. If the queue is empty, the method
   * blocks until an item becomes available or the timeout expires,
   * whichever occurs first.
   *
   * @param timeout The timeout, in milliseconds.
   * @return The dequeued item.
   * @throw InterruptedException If the queue was interrupted via a call
   * to interrupt().
   * @throw TimeoutException If the operation timed out.
   */
  T tryTake(timespan_ms_t timeout = 0);

  /**
   * Get the size of the queue, that is, the number of items currently
   * in the queue.
   */
  uint_t getSize() const;

  /**
   * Get the capacity of the queue, that is, the maximum number of items
   * that the queue can hold.
   */
  inline uint_t getCapacity() const
  { return(_capacity); }

  /**
   * Change the capacity of the queue.
   *
   * @param capacity The new capacity. If the value is less than 1,
   * the request is ignored. It is possible to set the capacity to be less
   * than the number of elements currently in the queue; in this case it
   * will not be possible to add more elements until the size of the queue
   * drops below the new capacity.
   */
  void setCapacity(uint_t capacity);

  /**
   * Interrupt the queue. Unblocks any pending operations, causing the
   * corresponding methods to throw an InterruptedException.
   */
  void interrupt();

  /**
   * Shut down the queue. All subsequent operations will throw an
   * InterruptedException.
   */
  void shutdown();

  /** Test if the queue has been shut down. */
  inline bool isShutdown() const
  { return(_terminated); }

 private:

  uint_t _capacity;
  std::deque<T> _queue;
  mutable Mutex _mutex;
  ConditionVar _condP;
  ConditionVar _condC;
  bool _terminated;
};

#include <commonc++/BoundedQueueImpl.h++>

} // namespace ccxx

#endif // __ccxx_BoundedQueue_hxx
