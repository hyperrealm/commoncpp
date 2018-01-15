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

#ifndef __ccxx_DataEncoder_hxx
#define __ccxx_DataEncoder_hxx

#include <commonc++/IOException.h++>

#include <deque>

namespace ccxx {

class DataEncoder; // fwd decl

/**
 * An abstract class with basic functionality that is common to all
 * data decoders (readers) and data encoders (writers). A
 * DataEncoder operates on an input source or an output destination,
 * such as a stream or buffer. Subclasses provide specific
 * input/output functionality, such as reading and writing primitive
 * data types.
 *
 * The class supports nested encoding contexts. Initially, there is
 * a top-level context, which represents the entire range of bytes
 * being read or written. A nested context represents a sub-range of
 * bytes within its parent context. Each context has an offset and a
 * limit; the offset is the position, relative to the beginning of
 * the context, of the next byte to be read or written, and the
 * limit is the maximum offset within the context beyond which
 * further reading or writing is not allowed.
 *
 * A new context is created by calling pushContext(). The new context
 * inherits the encoder attributes&mdash;such as byte endianness, and
 * value length&mdash;of its parent context, and its offset is
 * initially 0. It represents the range of bytes between the parent
 * context's current offset and limit.
 *
 * A context is destroyed by calling popContext(). The parent context
 * then becomes the current context, and its offset is incremented by the
 * offset of the popped context.
 *
 * Though data is typically read or written sequentially, random
 * access is also possible. The offset in the current context can be
 * changed insofar as it remains within the bounds of that
 * context. When reading, the offset can be set to any value between
 * 0 and the current context's limit. When writing, the offset
 * can be set to any value between 0 and the maximum offset to which
 * data has already been written within the current context.
 *
 * Nested contexts can be used to implement an encoder that reads or
 * writes hierarchical data structures, such as "chunks" in a RIFF file.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API DataEncoder
{
 public:

  /**
   * A lexically scoped DataEncoder context. ScopedContext pushes a new
   * DataEncoder context in its constructor and pops it in its destructor.
   *
   * @author Mark Lindner
   */
  class ScopedContext
  {
    public:

    /**
     * Construct a new ScopedContext for the given DataEncoder. Immediately
     * pushes a new context on the encoder.
     */
    ScopedContext(DataEncoder& encoder)
      : _encoder(encoder)
    { _encoder.pushContext(); }

    /** Destructor. Pops the context off the encoder. */
    ~ScopedContext()
    { _encoder.popContext(); }

    private:

    DataEncoder& _encoder;

    CCXX_COPY_DECLS(ScopedContext);
  };

  /**
   * A base class for DataEncoder manipulators. A manipulator modifies the
   * behavior of the encoder in some way, and can be inlined with other
   * stream operators, in much the same way as the C++ standard library's
   * I/O manipulators work with C++ streams.
   *
   * @author Mark Lindner
   */
  class COMMONCPP_API Manipulator
  {
   public:

    /** Destructor. */
    virtual ~Manipulator() { }

    /**
     * Apply the DataManipulator to a DataEncoder.
     *
     * @param encoder The target DataEncoder.
     * @throw IOException If an I/O error occurs.
     */
    virtual void apply(DataEncoder& encoder) const = 0;
  };

  /**
   * A manipulator that specifies the endianness of integer values to be
   * subsequently encoded/decoded.
   *
   * @author Mark Lindner
   */
  class COMMONCPP_API SetEndianness : public Manipulator
  {
   public:

    /**
     * Construct a new SetEndianness manipulator.
     *
     * @param endianness The endianness.
     */
    SetEndianness(Endianness endianness);

    /** Destructor. */
    ~SetEndianness();

   protected:

    void apply(DataEncoder& encoder) const;

   private:

    Endianness _endianness;
  };

  /**
   * A manipulator that specifies the length of String, Blob, and array
   * values to be subsequently encoded/decoded.
   *
   * @author Mark Lindner
   */
  class COMMONCPP_API SetLength : public Manipulator
  {
   public:

    /**
     * Construct a new SetLength manipulator.
     *
     * @param length The string length.
     */
    SetLength(size_t length);

    /** Destructor. */
    ~SetLength();

   protected:

    void apply(DataEncoder& encoder) const;

   private:

    size_t _length;
  };

  /**
   * A manipulator that skips past bytes in the stream. When reading,
   * the bytes are simply skipped; when writing, the bytes are filled
   * with NULs.
   *
   * @author Mark Lindner
   */
  class COMMONCPP_API Skip : public Manipulator
  {
   public:

    /**
     * Construct a new Skip manipulator.
     *
     * @param count The number of bytes to skip.
     */
    Skip(size_t count);

    /** Destructor. */
    ~Skip();

   protected:

    void apply(DataEncoder& encoder) const;

   private:

    size_t _count;
  };

  /**
   * A manipulator that skips past enough bytes to re-align the
   * data stream at a specified boundary. When reading, the bytes
   * are simply skipped; when writing, the bytes are filled with
   * NULs.
   *
   * @author Mark Lindner
   */
  class COMMONCPP_API Align : public Manipulator
  {
   public:

    /**
     * Construct a new Align manipulator.
     *
     * @param size The alignment size. Typical values include 2, 4, or 8.
     * Other values are allowed but are of limited usefulness.
     */
    Align(size_t size);

    /** Destructor. */
    ~Align();

   protected:

    void apply(DataEncoder& encoder) const;

   private:

    size_t _size;
  };

  /**
   * A manipulator that specifies the maximum number of bytes that
   * can be read/written. Attempts to read/write past this limit will
   * raise an IOException.
   *
   * @author Mark Lindner
   */
  class COMMONCPP_API SetLimit : public Manipulator
  {
   public:

    /**
     * Construct a new SetLimit manipulator.
     *
     * @param limit The limit, in bytes. A value of 0 indicates no limit.
     */
    SetLimit(int64_t limit);

    /** Destructor. */
    ~SetLimit();

   protected:

    void apply(DataEncoder& encoder) const;

   private:

    int64_t _limit;
  };

  /**
   * Reset the encoder. Discards all but the topmost context, and resets
   * the topmost context to default values.
   *
   * @throw IOException If an I/O error occurs.
   */
  virtual void reset();

  /** Push a new encoder context. */
  void pushContext();

  /**
   * Pop the active encoder context. The previous context becomes the
   * active context, and the offset in the current context is incremented
   * by the offset of the popped context.
   */
  void popContext();

  /** Determine if the active context is the topmost (and only) context. */
  bool isTopContext() const;

  /** Get the current context depth. */
  uint_t getContextDepth() const;

  /**
   * Specify the byte-endianness for reading/writing primitive
   * integer types in the active context.
   */
  void setEndianness(Endianness endianness);

  /** Get the byte-endianness in the active context. */
  Endianness getEndianness() const;

  /**
   * Specify the length of String, Blob, and array values for
   * subsequent reading/writing in the active context.
   */
  void setLength(size_t length);

  /** Get the current length for string values. */
  size_t getLength() const;

  /**
   * Specify the maximum number of bytes that can be
   * read/written in the active context. Attempts to
   * read/write past this limit will raise an IOException.
   *
   * @throw IOException If an I/O error occurs.
   */
  void setLimit(int64_t limit);

  /** Get the current limit. */
  int64_t getLimit() const;

  /** Get the current encoding/decoding offset. */
  int64_t getOffset() const;

  /**
   * Set the current reading/writing offset.
   *
   * @throw IOException If an I/O error occurs.
   */
  virtual void setOffset(int64_t offset) = 0;

  /**
   * Get the cumulative reading/writing offset. This is a sum of
   * the offsets across all contexts.
   */
  int64_t getCumulativeOffset() const;

  /**
   * Get the number of bytes between the current offset and the
   * limit, i.e., the number of bytes remaining to be read or
   * written.
   */
  int64_t getRemaining() const;

  /**
   * %Skip past enough bytes to re-align the data stream at a specified
   * boundary. When reading, the bytes are simply skipped over; when
   * writing, the bytes are filled with NULs.
   *
   * @param size The alignment size. Typical values include 2, 4, or 8.
   * Other values are allowed but are of limited usefulness.
   * @throw IOException If an I/O error occurs.
   */
  void align(size_t size);

  /**
   * %Skip past the given number of bytes in the data stream. When
   * reading, the bytes are simply skipped over; when writing, the
   * bytes are filled with NULs.
   *
   * @param count The number of bytes to skip.
   * @throw IOException If an I/O error occurs.
   */
  virtual void skip(size_t count) = 0;

  /** Destructor. */
  virtual ~DataEncoder();

 protected:

  /** Constructor. */
  DataEncoder();

  /**
   * Test if the endianness of the encoder is the same as the endianness
   * of the host system.
   */
  bool isSameEndianness() const;

  /**
   * A DataEncoder context.
   *
   * @author Mark Lindner
   */

  struct Context
  {
    /** Constructor. */
    Context();

    void reset();

    void bumpOffset(size_t delta);

    /** The current byte endianness. */
    Endianness endianness;
    /** The current value length. */
    size_t length;
    /** The current limit. */
    int64_t limit;
    /** The parent (enclosing) limit. */
    int64_t parentLimit;
    /** The current offset. */
    int64_t offset;
    /** The maximum offset. */
    int64_t maxOffset;
  };

  /** Get a reference to the current context. */
  Context& currentContext();

  /** Get a reference to the current context. */
  const Context& currentContext() const;

  /**
   * Check if at least the given number of bytes are available to be
   * read or written.
   *
   * @param ctx The context to check.
   * @param count The number of bytes.
   * @throw IOException If there are not enough bytes remaining.
   */
  void checkRemaining(const Context& ctx, size_t count) const;

  /** @cond INTERNAL */
  class ContextStack; // fwd decl

  ContextStack* _contexts;
  int64_t _maxOffset;
  /** @endcond */

 private:

  CCXX_COPY_DECLS(DataEncoder);
};

} // namespace ccxx

#endif // __ccxx_DataEncoder_hxx
